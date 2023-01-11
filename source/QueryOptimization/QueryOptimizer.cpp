#include <iostream>
#include "QueryOptimizer.h"

/**********************************************************************
 * Given the list of relations taking part in the query and an alias, *
 *  we return the real name of the relation that has the given alias  *
 **********************************************************************/

unsigned int QueryOptimizer::getRealTableName(List *rels, unsigned int alias)
{
    return *((unsigned int *) rels->getItemInPos(alias + 1));
}

/****************************************************************
 *   Given the list of tables, the real name of a table and a   *
 * column name, we return the initial statistics of that column *
 ****************************************************************/

ColumnStatistics *QueryOptimizer::getInitialStats(List *tables,
    unsigned int realTableName, unsigned int columnName)
{
    Table *table = (Table *) tables->getItemInPos(realTableName + 1);
    return table->getColumnStatistics()[columnName];
}

/************************************************************
 *              Compares two column identities              *
 *
 * - Returns 1 if the first one is greater than the second  *
 * - Returns -1 if the first one is smaller than the second *
 * - Returns 0 if the two column identities are equal       *
 ************************************************************/

int QueryOptimizer::compareColumnIdentities(void *item1, void *item2)
{
    ColumnIdentity *colId1 = (ColumnIdentity *) item1;
    ColumnIdentity *colId2 = (ColumnIdentity *) item2;

    return colId1->compare(colId2);
}

/************************************************************
 *       Compares the values of two unsigned integers       *
 *
 * - Returns 1 if the first one is greater than the second  *
 * - Returns -1 if the first one is smaller than the second *
 * - Returns 0 if the two unsigned integers are equal       *
 ************************************************************/

int QueryOptimizer::compareUnsignedIntegers(void *item1, void *item2)
{
    unsigned int uint1 = *((unsigned int *) item1);
    unsigned int uint2 = *((unsigned int *) item2);

    if(uint1 > uint2)
        return 1;

    if(uint1 < uint2)
        return -1;

    return 0;
}

/****************************************************************
 *  Attempts to insert a relation with the given attributes to  *
 * the B-Tree. Fails if the relation already exists in the tree *
 ****************************************************************/

void QueryOptimizer::insertRelInTree(unsigned int tableAlias,
    unsigned int tableColumn, ColumnIdentity **createdColumnIdentity)
{
    /* We retrieve the list of relations taking part in the query */
    List *relations = query->getRelations();

    /* We find the real name of the given table */
    unsigned int realTable = getRealTableName(relations, tableAlias);

    /* We retrieve the initial statistics of this column */
    ColumnStatistics *columnStats = getInitialStats(
        tables, realTable, tableColumn);

    /* We create a new identity */
    ColumnIdentity *columnId = new ColumnIdentity(
        tableAlias, tableColumn, realTable, columnStats);

    /* This variable will indicate whether or not the column
     * identity was inserted successfully in the tree
     */
    bool insertionWasSuccessful;

    /* We attempt to insert the new column in the B-Tree */
    columnIdentitiesTree->insert(columnId, columnId,
        compareColumnIdentities, &insertionWasSuccessful);

    /* If this column exists already in the tree, it cannot be re-inserted.
     *
     * We want all the columns in the tree to be distinct.
     */
    if(insertionWasSuccessful == false)
    {
        delete columnId;

        if(createdColumnIdentity != NULL)
            (*createdColumnIdentity) = NULL;
    }

    /* Case the column was inserted successfully in the tree */
    else
    {
        if(createdColumnIdentity != NULL)
            (*createdColumnIdentity) = columnId;
    }
}

/****************************
 * Prints a column identity *
 ****************************/

void QueryOptimizer::printColumnIdentity(void *item)
{
    ColumnIdentity *colId = (ColumnIdentity *) item;
    colId->print();
}

/**********************
 * Prints a predicate *
 **********************/

void QueryOptimizer::printPredicate(void *item)
{
    PredicatesParser *predicate = (PredicatesParser *) item;
    predicate->print();
}

/***********************************************
 * Prints a new line (this is the context that *
 *  will be printed between column identities) *
 ***********************************************/

void QueryOptimizer::contextBetweenColumnIdentities()
{
    std::cout << std::endl;
}

/************************************************
 * Prints a comma & space (this is the context  *
 * that will be printed between two predicates) *
 ************************************************/

void QueryOptimizer::contextBetweenPredicates()
{
    std::cout << ", ";
}

/****************************************************
 * Frees the allocated memory for a column identity *
 ****************************************************/

void QueryOptimizer::deleteColumnIdentity(void *item, void *key)
{
    delete (ColumnIdentity *) item;
}

/***************
 * Constructor *
 ***************/

QueryOptimizer::QueryOptimizer(List *tables, Query *query)
{
    /* We assign the given tables and query to the fields of the class */
    this->tables = tables;
    this->query = query;

    /* We initialize the tree as a (2,3)-Tree */
    columnIdentitiesTree = new B_Tree(3);

    /* We initialize the list that will be storing only the join predicates
     * and the list that will be storing only the filter predicates
     */
    joinPreds = new List();
    filterPreds = new List();

    /* We retrieve the list of predicates of the query */
    List *predicates = query->getPredicates();

    /* We will traverse the list from the head to the tail */
    Listnode *currentNode = predicates->getHead();

    /* As long as we have not reached the end of the list */
    while(currentNode != NULL)
    {
        /* We retrieve the predicate in the current node */
        PredicatesParser *currentPred = (PredicatesParser *)
            currentNode->getItem();

        /* We retrieve the alias of the left array
         * and the name of the column of the array
         */
        unsigned int leftArray = currentPred->getLeftArray();
        unsigned int leftArrayColumn = currentPred->getLeftArrayColumn();

        /* We attempt to insert the relation we retrieved in the B-Tree.
         *
         * It will not be inserted successfully if it already exists.
         */
        ColumnIdentity *leftColId;
        insertRelInTree(leftArray, leftArrayColumn, &leftColId);

        /* We find out if the query is a join or a filter query */
        bool needsJoin = !currentPred->hasConstant();

        /* Case the current predicate is a 'JOIN' predicate */

        if(needsJoin)
        {
            /* We retrieve the alias of the right array
             * and the name of the column of the array
             */
            unsigned int rightArray = currentPred->getRightArray();
            unsigned int rightArrayColumn = currentPred->getRightArrayColumn();

            /* We attempt to insert the relation we retrieved in the B-Tree.
             *
             * It will not be inserted successfully if it already exists.
             */
            ColumnIdentity *rightColId;
            insertRelInTree(rightArray, rightArrayColumn, &rightColId);

            if(leftColId == NULL)
            {
                /* We create a dummy column identity for searching */
                leftColId = new ColumnIdentity(leftArray, leftArrayColumn);

                /* The indirect result of searching will be stored in this variable */
                ColumnIdentity *leftRetrievedColId;

                /* Since we couldn't insert the column identity in the tree
                 * (because it already exists), the search of it should be successful.
                 */
                if(columnIdentitiesTree->searchAndRetrieve(leftColId,
                    compareColumnIdentities, (void **) &leftRetrievedColId))
                {
                    /* We delete the temporary dummy column identity */
                    delete leftColId;

                    /* We store in the outer variable
                     * the retrieved one from the tree
                     */
                    leftColId = (ColumnIdentity *) leftRetrievedColId;
                }

                /* This part should never be reached, but typically
                 * we print an error message in the screen to inform
                 * the user about the unexpected behaviour.
                 */
                else
                {
                    std::cout << "Could not find the relation {" << leftArray
                        << "." << leftArrayColumn << "} in the tree" << std::endl;
                }
            }

            if(rightColId == NULL)
            {
                /* We create a dummy column identity for searching */
                rightColId = new ColumnIdentity(rightArray, rightArrayColumn);

                /* The indirect result of searching will be stored in this variable */
                ColumnIdentity *rightRetrievedColId;

                /* Since we couldn't insert the column identity in the tree
                 * (because it already exists), the search of it should be successful.
                 */
                if(columnIdentitiesTree->searchAndRetrieve(rightColId,
                    compareColumnIdentities, (void **) &rightRetrievedColId))
                {
                    /* We delete the temporary dummy column identity */
                    delete rightColId;

                    /* We store in the outer variable
                     * the retrieved one from the tree
                     */
                    rightColId = (ColumnIdentity *) rightRetrievedColId;
                }

                /* This part should never be reached, but typically
                 * we print an error message in the screen to inform
                 * the user about the unexpected behaviour.
                 */
                else
                {
                    std::cout << "Could not find the relation {" << rightArray
                        << "." << rightArrayColumn << "} in the tree" << std::endl;
                }
            }

            /* Now that we have retrieved both columns, we
             * can insert them as neighbors of each other.
             *
             * Since they are connected with a 'JOIN'
             * predicate, they are considered neighbors.
             */
            leftColId->insertNeighbor(rightColId, currentPred);
            rightColId->insertNeighbor(leftColId, currentPred);

            /* Also, we insert the current predicate in the
             * list that is storing only the join predicates
             */
            joinPreds->insertLast(currentPred);
        }

        /* Case the current predicate is a filter predicate */

        else
        {
            /* We insert the current predicate in the list
             * that is storing only the filter predicates
             */
            filterPreds->insertLast(currentPred);
        }

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }
}

/**************
 * Destructor *
 **************/

QueryOptimizer::~QueryOptimizer()
{
    columnIdentitiesTree->traverse(Postorder, deleteColumnIdentity);
    delete columnIdentitiesTree;
}

/****************************************************************************
 * Returns a list of predicates in the order that is estimated most optimal *
 ****************************************************************************/

List *QueryOptimizer::getOptimalPredicatesOrder() const
{
    /* This is the list of the final result that we will return */
    List *result = new List();

    result->append(filterPreds);
    result->append(joinPreds);

    /* We return the final result */
    return result;
}

/****************************************************************************
 * Frees the allocated memory for the result of 'getOptimalPredicatesOrder' *
 ****************************************************************************/

void QueryOptimizer::deleteOptimalPredicatesOrder(List *optimalPredicatesOrder) const
{
    delete optimalPredicatesOrder;
}

/*************************************************************
 * Prints all the column identities taking part in the query *
 *************************************************************/

void QueryOptimizer::printColumnsOfQuery() const
{
    columnIdentitiesTree->printItems(
        Inorder,
        printColumnIdentity,
        contextBetweenColumnIdentities
    );
}

/*********************************************
 * Prints all the filter predicates and then *
 *   all the join predicates of the query    *
 *********************************************/

void QueryOptimizer::printFilterAndJoinPredicates() const
{
    std::cout << "Filter Predicates:" << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^" << std::endl;
    filterPreds->printFromHead(printPredicate, contextBetweenPredicates);
    std::cout << std::endl;

    std::cout << "Join Predicates:" << std::endl;
    std::cout << "^^^^^^^^^^^^^^^" << std::endl;
    joinPreds->printFromHead(printPredicate, contextBetweenPredicates);
    std::cout << std::endl;
}
