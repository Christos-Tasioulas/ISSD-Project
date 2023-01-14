#include <iostream>
#include <cmath>
#include "QueryOptimizer.h"

/******************************************************************
 * An auxiliary static list that stores all the column identities *
 * that need to be filtered due to the initial filter predicates  *
 ******************************************************************/

static List *colIdsForParsing;

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

/**********************************************************************
 * Places all the column identities of a structure in the static list *
 **********************************************************************/

void QueryOptimizer::placeColIdsInList(void *item, void *key)
{
    ColumnIdentity *colId = (ColumnIdentity *) item;
    colIdsForParsing->insertLast(colId);
}

/************************************************************
 *              Compares two column identities              *
 *                                                          *
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
 *                                                          *
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

/*********************************************************************
 *   Compares two column subsets which both consist of 1 column ID   *
 *    The operation uses 'compareColumnIdentities' to compare the    *
 * single column identities and returns the result of the comparison *
 *********************************************************************/

int QueryOptimizer::compareSingleSetsByColumnIds(void *item1, void *item2)
{
    /* First we cast the items to their original type */
    ColumnSubset *colSub1 = (ColumnSubset *) item1;
    ColumnSubset *colSub2 = (ColumnSubset *) item2;

    /* Each subset is supposed to have only 1 column identity.
     *
     * We retrieve that column identity of both subsets.
     */
    ColumnIdentity *colId1 = colSub1->getColumnIdentityInPos(1);
    ColumnIdentity *colId2 = colSub2->getColumnIdentityInPos(1);

    /* We return the result of the comparison of the column identities */
    return compareColumnIdentities(colId1, colId2);
}

/****************************
 * Prints a column identity *
 ****************************/

void QueryOptimizer::printColumnIdentity(void *item)
{
    ColumnIdentity *colId = (ColumnIdentity *) item;
    colId->print();
}

/******************************
 * Prints an unsigned integer *
 ******************************/

void QueryOptimizer::printUnsignedInteger(void *item)
{
    unsigned int uint_item = *((unsigned int *) item);
    std::cout << "Column IDs in Group \"" << uint_item << "\":\n";
    std::cout << "^^^^^^^^^^^^^^^^^^^" << std::endl;
}

/********************************************************************
 * Prints only one identity in the form the inverted index requires *
 ********************************************************************/

void QueryOptimizer::printIdentityAndIdentity(void *item, void *itemKey)
{
    printColumnIdentity(item);
    std::cout << std::endl;
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

/******************************************************
 * Frees the allocated memory for an unsigned integer *
 ******************************************************/

void QueryOptimizer::deleteUnsignedInteger(void *redBlackTreeItem, void *uInteger)
{
    delete (unsigned int *) uInteger;
}

/***********************************************************************
 * Frees the allocated memory for the column statistics of an identity *
 ***********************************************************************/

void QueryOptimizer::deleteStatsFromColumnIdentity(void *item)
{
    /* First we cast the item to its original type */
    ColumnIdentity *colId = (ColumnIdentity *) item;

    /* We retrieve the column statistics stored in the column identity */
    ColumnStatistics *colStats = colId->getColumnStats();

    /* We free the allocated memory for the those statistics */
    delete colStats;
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

/*****************************************************************
 * Inserts a column identity to the inverted index of the class  *
 * The item will be inserted in the group of items with the same *
 *                          table alias                          *
 *****************************************************************/

void QueryOptimizer::insertRelInInvertedIndex(ColumnIdentity *colId)
{
    /* We retrieve the table alias of the identity */
    unsigned int tableAlias = colId->getTableName();

    /* Case a group with this alias does not
     * exist in the tree inverted index.
     *
     * We will create one and we will insert
     * this column identity to the new group.
     */
    if(identitiesByRealTable->searchIndexKey(
        &tableAlias,
        compareUnsignedIntegers) == NULL)
    {
        /* We store in the heap the ID of the new group
         * because we will need it for future comparisons
         */
        unsigned int *newGroupId = new unsigned int(tableAlias);

        /* We insert the column identity in the inverted index */
        identitiesByRealTable->insert(
            colId,
            newGroupId,
            colId,
            compareUnsignedIntegers,
            compareColumnIdentities
        );
    }

    /* Case the group with ID = 'tableAlias' exists in the inverted index.
     *
     * In this case we just insert the column identity in this group.
     */
    else
    {
        /* We insert the column identity in the inverted index */
        identitiesByRealTable->insert(
            colId,
            &tableAlias,
            colId,
            compareUnsignedIntegers,
            compareColumnIdentities
        );
    }
}

/********************************************************************
 * Makes changes to the initial stats of the given column identity  *
 * according to the given filters. This will also affect the column *
 *    identities that belong to the same table as the given one     *
 *    Consequently, those will have their stats changed as well     *
 ********************************************************************/

void QueryOptimizer::filterColumnIdentityAndTheRestInSameTable(
    ColumnIdentity *colId, char filterOperator, unsigned int filterValue)
{
    /* First we retrieve the alias of the column's table */
    unsigned int tableName = colId->getTableName();

    /* We will use the alias to search the rest
     * column identities that belong to this table
     */
    RedBlackTree *columnsGroupInSameTable = identitiesByRealTable->
        searchIndexKey(&tableName, compareUnsignedIntegers);

    /* We update the stats of the column identity that is targeted by the filter.
     *
     * Here we create the new stats.
     */
    unsigned int prev_fA, new_fA;
    prev_fA = colId->getColumnStats()->getElementsNum();
    updateStatsOfTargetedColumnByFilter(colId, filterOperator, filterValue);
    new_fA = colId->getColumnStats()->getElementsNum();

    /* We insert the identity in the list of identities with renewed stats */
    colIdsWithRenewedStats->insertLast(colId);

    /* We initialize the static helper list */
    colIdsForParsing = new List();

    /* We place all the column identities of the group in the helper list */
    columnsGroupInSameTable->traverse(Inorder, placeColIdsInList);

    /* We will traverse the list from the head to the tail */
    Listnode *currentNode = colIdsForParsing->getHead();

    /* As long as we have not finished traversing the list */
    while(currentNode != NULL)
    {
        /* We retrieve the column identity of the current node */
        ColumnIdentity *currentColId = (ColumnIdentity *) currentNode->getItem();

        /* We make sure to skip the column identity that is
         * directly targeted by the filter predicate, because
         * we place different stats in that column identity
         * from the stats that we place in the rest identities.
         */
        if(currentColId != colId)
        {
            /* We update the stats of the non-targeted identity */
            updateStatsOfNonTargetedColumnByFilter(currentColId,
                filterOperator, filterValue, prev_fA, new_fA);

            /* We insert the identity in the list of identities with renewed stats */
            colIdsWithRenewedStats->insertLast(currentColId);
        }

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    /* We delete the static helper list */
    delete colIdsForParsing;
}

/******************************************************************
 * Applies the given filter to the given targeted column identity *
 ******************************************************************/

void QueryOptimizer::updateStatsOfTargetedColumnByFilter(
    ColumnIdentity *colId,
    char filterOperator,
    unsigned int filterValue)
{
    /* We prepare the four variables that will
     * be used for the creation of the stats
     */
    unsigned int l, u, d, f;

    /* We store some of the current statistics
     * that we will need for the update
     */
    unsigned int prev_f = colId->getColumnStats()->getElementsNum();
    unsigned int prev_d = colId->getColumnStats()->getDistinctElementsNum();

    /* We take different actions depending on the operator type */

    switch(filterOperator)
    {
        case '=':
        {
            /* We update the lower bound as suggested */
            l = filterValue;

            /* We update the greater bound as suggested */
            u = filterValue;

            /* In this case we need to seach if the
             * 'filterValue' exists in the table.
             *
             * We retrieve the table of the column.
             */
            Table *tableOfColId = (Table *) tables->
                getItemInPos(1 + colId->getRealTableName());

            /* We retrieve the number of rows of the table */
            unsigned long long tuplesNum = tableOfColId->
                getNumOfTuples(), i;

            /* We retrieve the column of the identity */
            unsigned int column = colId->getTableColumn();

            /* Initially we consider the filter value does not exist */
            bool exists = false;

            /* We store the address of the table with
             * all the contents to a seperate variable
             */
            unsigned long long **tableContents = tableOfColId->getTable();

            /* We start searching for the element linearly */
            for(i = 0; i < tuplesNum; i++)
            {
                /* Case we found the element in this iteration
                 *
                 * We set the flag to 'true' and break the 'for' loop
                 */
                if(tableContents[column][i] == filterValue)
                {
                    exists = true;
                    break;
                }
            }

            /* We update 'f' and 'd' as suggested (case
             * the filter value exists in the table)
             */
            if(exists)
            {
                d = 1;
                f = prev_f / prev_d;
            }

            /* We update 'f' and 'd' as suggested (case
             * the filter value does not exist in the table)
             */
            else
            {
                d = 0;
                f = 0;
            }

            /* There is nothing else to do in this case */
            break;
        }

        case '<':
        {
            /* There is nothing else to do in this case */
            break;
        }

        case '>':
        {
            /* There is nothing else to do in this case */
            break;
        }
    }

    ColumnStatistics *newStats = new ColumnStatistics(l, u, f, d);
    colId->setColumnStats(newStats);
}

/* Applies the given filter to the given non-targeted column identity */
void QueryOptimizer::updateStatsOfNonTargetedColumnByFilter(
    ColumnIdentity *colId,
    char filterOperator,
    unsigned int filterValue,
    unsigned int old_fA,
    unsigned int new_fA)
{
    /* We prepare the four variables that will
     * be used for the creation of the stats
     */
    unsigned int l, u, d, f;

    /* We store some of the current statistics
     * that we will need for the update
     */
    unsigned int prev_l = colId->getColumnStats()->getMinElement();
    unsigned int prev_u = colId->getColumnStats()->getMaxElement();
    unsigned int prev_f = colId->getColumnStats()->getElementsNum();
    unsigned int prev_d = colId->getColumnStats()->getDistinctElementsNum();

    /* We take different actions depending on the operator type */

    switch(filterOperator)
    {
        case '=':
        {
            /* We update the lower bound as suggested */
            l = prev_l;

            /* We update the greater bound as suggested */
            u = prev_u;

            /* We update the elements num as suggested */
            f = new_fA;

            /* Case the targeted column has no elements
             *
             * Then all the non-targeted columns will not have either
             */
            if(old_fA == 0)
            {
                f = 0;
                d = 0;
            }

            /* Case the targeted column has more than zero elements */
            else
            {
                d = prev_d * round(1 - pow(1 -
                    ((double) new_fA)/((double) old_fA),
                    ((double) prev_f)/((double) prev_d)));
            }

            /* There is nothing else to do in this case */
            break;
        }

        case '<':
        {
            /* There is nothing else to do in this case */
            break;
        }

        case '>':
        {
            /* There is nothing else to do in this case */
            break;
        }
    }

    ColumnStatistics *newStats = new ColumnStatistics(l, u, f, d);
    colId->setColumnStats(newStats);
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

    /* We initialize the inverted index of the column identities */
    identitiesByRealTable = new InvertedIndex();

    /* We initialize the list of column identities with renewed stats */
    colIdsWithRenewedStats = new List();

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

        /* If this is the first time we encounter the left
         * relation, we also add it in the inverted index
         */
        if(leftColId != NULL)
            insertRelInInvertedIndex(leftColId);

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

            /* If this is the first time we encounter the right
             * relation, we also add it in the inverted index
             */
            if(rightColId != NULL)
                insertRelInInvertedIndex(rightColId);

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
    /* We free the allocated memory for the filter & join lists */
    delete filterPreds;
    delete joinPreds;

    /* We free the allocated memory for the renewed stats */
    colIdsWithRenewedStats->traverseFromHead(deleteStatsFromColumnIdentity);
    delete colIdsWithRenewedStats;

    /* We free the allocated memory for the Inverted Index */
    identitiesByRealTable->traverse(Postorder, deleteUnsignedInteger);
    delete identitiesByRealTable;

    /* We free the allocated memory for the B-Tree */
    columnIdentitiesTree->traverse(Postorder, deleteColumnIdentity);
    delete columnIdentitiesTree;
}

/****************************************************************************
 * Returns a list of predicates in the order that is estimated most optimal *
 ****************************************************************************/

List *QueryOptimizer::getOptimalPredicatesOrder()
{
    /* This is the list of the final result that we will return */
    List *result = new List();

    /* We will start traversing the list of filter predicates */
    Listnode *currentNode = filterPreds->getHead();

    /* As long as we have not finished traversing the list */
    while(currentNode != NULL)
    {
        /* We retrieve the predicate stored in the current node */
        PredicatesParser *currentPred = (PredicatesParser *)
            currentNode->getItem();

        /* We retrieve the alias of the left array
         * and the name of the column of the array
         */
        unsigned int leftArray = currentPred->getLeftArray();
        unsigned int leftArrayColumn = currentPred->getLeftArrayColumn();

        /* We create a dummy identity to search the real
         * one with the same attributes in the tree
         */
        ColumnIdentity dummyColId = ColumnIdentity(leftArray, leftArrayColumn);

        /* We will store a pointer to the real identity here */
        ColumnIdentity *colIdForFiltering;

        /* We search and retrieve the real identity from the tree */
        bool searchResult = columnIdentitiesTree->searchAndRetrieve(
            &dummyColId, compareColumnIdentities, (void **) &colIdForFiltering);

        /* The flow should never reach the contents of this 'if' block.
         * However, in case there is this unexpected behaviour, we
         * typically provide a message to the user about the error.
         */
        if(searchResult == false)
        {
            std::cout << "Could not retrieve the column identity " << leftArray
                << "." << leftArrayColumn << " for filtering" << std::endl;
        }

        /* Case the search was successful (normal case) */
        else
        {
            /* We retrieve the filter value & operator of the filter predicate */
            unsigned int filterValue = currentPred->getFilterValue();
            char filterOperator = currentPred->getFilterOperator();

            /* We filter the retrieved column identity with the given filters.
             * This operation will also make changes to the stats of the
             * rest column identities that belong to the same table as the
             * retrieved one, because a filter on the latter affects the stats
             * of all the columns of the same table.
             */
            filterColumnIdentityAndTheRestInSameTable(
                colIdForFiltering, filterOperator, filterValue);
        }

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

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

/****************************************************************
 * Prints all the column identities grouped by their table name *
 ****************************************************************/

void QueryOptimizer::printColumnsGroupedByTableName() const
{
    identitiesByRealTable->print(
        printUnsignedInteger,
        printIdentityAndIdentity
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
