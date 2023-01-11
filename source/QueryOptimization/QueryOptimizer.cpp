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

/***********************************************
 * Prints a new line (this is the context that *
 *  will be printed between column identities) *
 ***********************************************/

void QueryOptimizer::contextBetweenColumnIdentities()
{
    std::cout << std::endl;
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
                ColumnIdentity *retrievedColId;

                /* Since we couldn't insert the column identity in the tree
                 * (because it already exists), the search of it should be successful.
                 */
                if(columnIdentitiesTree->searchAndRetrieve(leftColId,
                    compareColumnIdentities, (void **) &retrievedColId))
                {
                    /* We delete the temporary dummy column identity */
                    delete leftColId;

                    /* We store in the outer variable
                     * the retrieved one from the tree
                     */
                    leftColId = retrievedColId;
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
                ColumnIdentity *retrievedColId;

                /* Since we couldn't insert the column identity in the tree
                 * (because it already exists), the search of it should be successful.
                 */
                if(columnIdentitiesTree->searchAndRetrieve(rightColId,
                    compareColumnIdentities, (void **) &retrievedColId))
                {
                    /* We delete the temporary dummy column identity */
                    delete rightColId;

                    /* We store in the outer variable
                     * the retrieved one from the tree
                     */
                    rightColId = retrievedColId;
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

    /* The list of predicates of the query contains both join and
     * filter queries. Our first job is to place all the filter
     * queries at the begining of the result list (those have the
     * highest priority of execution). Then, we need to place all
     * the join predicates in a seperate list and find the optimal
     * path of execution among those. In the end, we will concat
     * the optimal path of the join predicates to the result list.
     */
    List *joinPredicatesOnly = new List();

    /* We will start traversing the list of predicates from the head */
    Listnode *currentNode = query->getPredicates()->getHead();

    /* As long as we have not finished traversing the list */

    while(currentNode != NULL)
    {
        /* We retrieve the predicate stored in the current node */

        PredicatesParser *currentPredicate = (PredicatesParser *)
            currentNode->getItem();

        /* Case the current predicate is a filter predicate.
         *                                 ^^^^^^
         * We insert it instantly in the final list.
         */
        if(currentPredicate->hasConstant())
            result->insertLast(currentPredicate);

        /* Case the current predicate is a join predicate.
         *                                 ^^^^
         * We insert it in the new list containing only join predicates.
         */     
            joinPredicatesOnly->insertLast(currentPredicate);

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    /* We concatenate that order to the filter predicates */
    result->append(joinPredicatesOnly);

    /* We free the allocated memory for the temporary list */
    delete joinPredicatesOnly;

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
