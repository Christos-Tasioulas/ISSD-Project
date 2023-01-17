#include <iostream>
#include <cmath>
#include "QueryOptimizer.h"

/******************************************************************
 * An auxiliary static list that stores all the column identities *
 * that need to be filtered due to the initial filter predicates  *
 ******************************************************************/

static List *colIdsForParsing;

/******************************************************************
 *  An auxiliary static list that stores the best column subsets  *
 * of a specific group of Binary Heaps for building the rest tree *
 ******************************************************************/

static List *subsetsForParsing;

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

/**********************************************************************
 * Used to traverse a group of Binary Heaps of subsets and places the *
 *   subset in the root of each binary heap in a static helper list   *
 **********************************************************************/

void QueryOptimizer::placeSubsetsInList(void *item, void *key)
{
    /* First we cast the item to its original type */
    BinaryHeap *subsetsHeap = (BinaryHeap *) item;

    /* We retrieve the best order of all the orders in the heap */
    ColumnSubset *bestOrder = (ColumnSubset *) subsetsHeap->getHighestPriorityItem();

    /* We insert the above best order in the list of subsets */
    subsetsForParsing->insertLast(bestOrder);
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

/***************************************************************
 *  Compares two column subsets. By this function, the column  *
 *  subsets are considered equal if they have the same amount  *
 * of column identities in them and the same column identities *
 *   themselves. In other words, two subsets are considered    *
 * "equal by combination" if their columns represent the same  *
 * combination of columns, no matter their order in the subset *
 *                                                             *
 *    Returns 0 if the subsets are equal by the above rule     *
 *   Returns  1 if 'item1' is a greater subset than 'item2'    *
 *   Returns -1 if 'item2' is a greater subset than 'item1'    *
 ***************************************************************/

int QueryOptimizer::compareSubsetsByCombination(void *item1, void *item2)
{
    /* We cast the two given items to their original type */
    ColumnSubset *subset_1 = (ColumnSubset *) item1;
    ColumnSubset *subset_2 = (ColumnSubset *) item2;

    /* We will traverse both lists of columns */
    Listnode *current_1 = subset_1->getColumnsIdentities()->getHead();
    Listnode *current_2 = subset_2->getColumnsIdentities()->getHead();

    /* We will compute the sum of powers of 2 to each of the column IDs.
     *
     * For example, if one subset is [2 3 5], then its sum will be
     * 2^2 + 2^3 + 2^5 = 44. We produce this sum for both subsets.
     */
    unsigned int sum_1 = 0;
    unsigned int sum_2 = 0;
    unsigned int temp = 1;

    /* As long as we have not finished traversing the lists */
    while((current_1 != NULL) && (current_2 != NULL))
    {
        /* We retrieve the columns of the current pair of nodes */
        ColumnIdentity *colId_1 = (ColumnIdentity *) current_1->getItem();
        ColumnIdentity *colId_2 = (ColumnIdentity *) current_2->getItem();

        /* We create the next term of the first sum and add it to the sum */
        unsigned int id_1 = colId_1->getId();
        temp <<= id_1;
        sum_1 += temp;
        temp = 1;

        /* We create the next term of the second sum and add it to the sum */
        unsigned int id_2 = colId_2->getId();
        temp <<= id_2;
        sum_2 += temp;
        temp = 1;

        /* We proceed to the next pair of columns */
        current_1 = current_1->getNext();
        current_2 = current_2->getNext();
    }

    /* We return the result of the comparison between the two sums */
    return compareUnsignedIntegers(&sum_1, &sum_2);
}

/*******************************************************
 *   Compares two column subsets only by their cost    *
 *                                                     *
 *     Returns -1 if the cost of 'item1' is lower      *
 *     Returns 1 if the cost of 'item1' is greater     *
 * Returns 0 if the costs of the two subsets are equal *
 *******************************************************/

int QueryOptimizer::compareSubsetsByCost(void *item1, void *item2)
{
    /* We cast the two given items to their original type */
    ColumnSubset *subset_1 = (ColumnSubset *) item1;
    ColumnSubset *subset_2 = (ColumnSubset *) item2;

    /* We retrieve the costs of both subsets */
    unsigned long long cost_1 = subset_1->getTotalCost();
    unsigned long long cost_2 = subset_2->getTotalCost();

    /* Case the cost of subset '1' is greater.
     *
     * Then subset '1' is greater. We return 1.
     */
    if(cost_1 > cost_2)
        return 1;

    /* Case the cost of subset '2' is greater.
     *
     * Then subset '2' is greater. We return -1.
     */
    if(cost_1 < cost_2)
        return -1;

    /* Case the costs of the two subsets are equal.
     *
     * Then the subsets are equal. We return 0.
     */
    return 0;
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

/*******************************************
 * Prints only the ID of a column identity *
 *******************************************/

void QueryOptimizer::printIdOfColumnIdentity(void *item)
{
    ColumnIdentity *colId = (ColumnIdentity *) item;
    std::cout << colId->getId();
}

/********************************************************
 * Prints only the column identities of a column subset *
 ********************************************************/

void QueryOptimizer::printColumnsOfSubset(void *item)
{
    ColumnSubset *subset = (ColumnSubset *) item;
    List *columnIdsOfSubset = subset->getColumnsIdentities();

    columnIdsOfSubset->printFromHead(
        printIdOfColumnIdentity, contextBetweenJoinedIdentities);

    std::cout << std::endl;
}

/*************************************************************
 * Prints the ID of a group in the inverted index of subsets *
 *************************************************************/

void QueryOptimizer::printGroup(void *indexKey)
{
    unsigned int groupId = *((unsigned int *) indexKey);
    std::cout << "Information of group #" << groupId << ":\n";
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
}

/******************************************************
 * Prints all the subsets of a Binary Heap of subsets *
 ******************************************************/

void QueryOptimizer::printBinaryHeapOfSubsets(void *item, void *key)
{
    BinaryHeap *subsetsHeap = (BinaryHeap *) item;
    subsetsHeap->printItems(printColumnsOfSubset);

    std::cout << "============ End of Heap ("
        << subsetsHeap->getCounter() << " items) ============" << std::endl;
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

/******************************************************
 * Prints beautiful context between joined identities *
 ******************************************************/

void QueryOptimizer::contextBetweenJoinedIdentities()
{
    std::cout << " i><i ";
}

/**************************************************
 * Prints some context between two column subsets *
 **************************************************/

void QueryOptimizer::contextBetweenSubsets()
{
    std::cout << "," << std::endl;
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

void QueryOptimizer::deleteUnsignedInteger(void *item, void *uInteger)
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

/**************************************************
 * Frees the allocated memory for a column subset *
 **************************************************/

void QueryOptimizer::deleteColumnSubset(void *item, void *key)
{
    delete (ColumnSubset *) item;
}

/***************************************************************************
 * Frees the allocated memory for the heaps and the subsets stored in them *
 ***************************************************************************/

void QueryOptimizer::deleteHeapsOfSubsets(void *item, void *key)
{
    /* We cast the item to its original type - It's a heap of subsets */
    BinaryHeap *subsetsHeap = (BinaryHeap *) item;

    /* We traverse the heap to delete all the subsets stored in it */
    subsetsHeap->traverse(Postorder, deleteColumnSubset);

    /* Then we delete the heap itself */
    delete subsetsHeap;
}

/************************************************************************
 * Frees the allocated memory for everything inside the tree of subsets *
 ************************************************************************/

 void QueryOptimizer::deleteInvertedIndexInformation(void *item, void *key)
 {
    /* We delete the ID of the group */
    delete (unsigned int *) key;

    /* We cast the group to its original type */
    RedBlackTree *group = (RedBlackTree *) item;

    /* We traverse the inner Red Black Tree and free all
     * the memory for each Binary Heap stored in it and
     * for the subsets stored in the Binary Heap
     */
    group->traverse(Postorder, deleteHeapsOfSubsets);
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
    ColumnStatistics *stats = colId->getColumnStats();
    unsigned int prev_l = stats->getMinElement();
    unsigned int prev_u = stats->getMaxElement();
    unsigned int prev_f = stats->getElementsNum();
    unsigned int prev_d = stats->getDistinctElementsNum();

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
            /* Statistical numbers we will use for this case */
            unsigned int k1 = prev_l;
            unsigned int k2 = (filterValue > prev_u) ? prev_u : filterValue;

            /* Case the whole column is greater-equal
             * than the maximum value of the filter.
             *
             * Then all elements will be removed
             * with the execution of the filter.
             */
            if(k1 >= k2)
            {
                l = 0;
                u = 0;
                d = 0;
                f = 0;
            }

            /* Case k1 < k2 */
            else
            {
                /* In this case the lowest bound remains the same */
                l = k1;

                /* The highest bound becomes the filter value,
                * because everything is less or equal to it
                */
                u = k2;

                /* Case the column has only 1 distinct element */
                if(prev_u == prev_l)
                {
                    d = 0;
                    f = 0;
                }

                /* Case the column has more than 1 distinct elements */
                else
                {
                    d = round((((double) (k2 - k1))/((double) (prev_u - prev_l))) * prev_d);
                    f = round((((double) (k2 - k1))/((double) (prev_u - prev_l))) * prev_f);
                }
            }

            /* There is nothing else to do in this case */
            break;
        }

        case '>':
        {
            /* Statistical numbers we will use for this case */
            unsigned int k1 = (prev_l > filterValue) ? prev_l : filterValue;
            unsigned int k2 = prev_u;

            /* Case the whole column is lower-equal
             * than the minimum value of the filter.
             *
             * Then all elements will be removed
             * with the execution of the filter.
             */
            if(k1 >= k2)
            {
                l = 0;
                u = 0;
                d = 0;
                f = 0;
            }

            /* Case k1 < k2 */
            else
            {
                /* In this case the lowest bound remains the same */
                l = k1;

                /* The highest bound becomes the filter value,
                * because everything is less or equal to it
                */
                u = k2;

                /* Case the column has only 1 distinct element */
                if(prev_u == prev_l)
                {
                    d = 0;
                    f = 0;
                }

                /* Case the column has more than 1 distinct elements */
                else
                {
                    d = round((((double) (k2 - k1))/((double) (prev_u - prev_l))) * prev_d);
                    f = round((((double) (k2 - k1))/((double) (prev_u - prev_l))) * prev_f);
                }
            }

            /* There is nothing else to do in this case */
            break;
        }
    }

    ColumnStatistics *newStats = new ColumnStatistics(l, u, f, d);
    colId->setColumnStats(newStats);
}

/**********************************************************************
 * Applies the given filter to the given non-targeted column identity *
 **********************************************************************/

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

    /* In the case of non-targeted relations by filter, the actions
     * we should take are the same no matter the operator of the
     * filter ('<', '>', '='). We take these actions right below.
     *
     * We update the lower bound as suggested
     */
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

    ColumnStatistics *newStats = new ColumnStatistics(l, u, f, d);
    colId->setColumnStats(newStats);
}

/***************************************************************************
 * Estimates the new statistics after a join between the two given columns *
 *  The indirectly-returned stats must be deleted with 'delete' after use  *
 ***************************************************************************/

void QueryOptimizer::updateStatsOfColumnsByJoin(
    ColumnIdentity *leftColId,
    ColumnIdentity *rightColId,
    PredicatesParser *connectingPredicate,
    ColumnStatistics **resultStatsAfterJoin)
{
    (*resultStatsAfterJoin) = new ColumnStatistics(0, 0, 0, 0);
}

/*************************************************************************
 * Estimates the new statistics after a join between the given join tree *
 * and the given column identity. The indirectly-returned stats must be  *
 *                    deleted with 'delete' after use                    *
 *************************************************************************/

void QueryOptimizer::updateStatsOfJoinTreeAndColumnByJoin(
    ColumnSubset *joinTree,
    ColumnIdentity *colId,
    PredicatesParser *connectingPredicate,
    ColumnStatistics **resultStatsAfterJoin)
{
    (*resultStatsAfterJoin) = new ColumnStatistics(0, 0, 0, 0);
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

    /* We reset the counter of IDs for the next query */
    ColumnIdentity::resetIdCounter();
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

    /* We place in the result list all the filter predicates first.
     * It's most optimal to have those executed first rather than
     * any join predicate. The order of the filter predicates does
     * not matter so much. We take their default order in the query.
     */
    result->append(filterPreds);

    /* Then we append to the result list all the join predicates in
     * the most optimal order with the 'getOptimalJoinsOrder' operation.
     */
    getOptimalJoinsOrder(joinPreds, result);

    /* We return the final result */
    return result;
}

/******************************************************
 * Appends to the 'result' all the join predicates in *
 *   the 'joinPreds' list in the most optimal order   *
 ******************************************************/

void QueryOptimizer::getOptimalJoinsOrder(List *joinPreds, List *result)
{
    /* If there are no join predicates, we simply return */
    if(joinPreds->getCounter() == 0)
        return;

    /* We initialize the tree of the subsets we will create */
    subsetsTree = new InvertedIndex();

    /* We place all the column identities in a
     * list temporarily for a linear traversal
     */
    colIdsForParsing = new List();
    columnIdentitiesTree->traverse(Preorder, placeColIdsInList);

    /* We will traverse the list of columns from the head */
    Listnode *currentNode = colIdsForParsing->getHead();

    /* As long as we have not finished traversing the list */
    while(currentNode != NULL)
    {
        /* We retrieve the column stored in the current node */
        ColumnIdentity *colId = (ColumnIdentity *) currentNode->getItem();

        /* We retrieve the neighbors of the column */
        List *neighbors = colId->getNeighbors();

        /* We also retrieve the list of neighbor predicates */
        List *neighborPreds = colId->getNeighborPredicates();

        /* We will traverse the list of neighbors from the head */
        Listnode *currentNeighborNode = neighbors->getHead();

        /* We will also traverse the list of neighbor preds from the head */
        Listnode *currentNeighborPredicateNode = neighborPreds->getHead();

        /* The two lists have the same number of nodes
         *
         * As long as we have not finished traversing the list
         */
        while(currentNeighborNode != NULL)
        {
            /* We retrieve the neighbor stored in the current node */
            ColumnIdentity *neighborColId = (ColumnIdentity *)
                currentNeighborNode->getItem();

            /* We retrieve the neighbor pred stored in the current node */
            PredicatesParser *neighborPred = (PredicatesParser *)
                currentNeighborPredicateNode->getItem();

            /* This variable will point to the result stats after join */
            ColumnStatistics *statsAfterJoin;

            /* We find the new stats after join */
            updateStatsOfColumnsByJoin(colId, neighborColId,
                neighborPred, &statsAfterJoin);

            /* We create a new subset */
            ColumnSubset *newSubset = new ColumnSubset(
                colId, neighborColId, neighborPred, statsAfterJoin
            );

            /* We search for group #2 in the tree, since
             * the subset we created has 2 columns
             */
            unsigned int group_2_id = 2;

            RedBlackTree *group_2 = subsetsTree->searchIndexKey(
                &group_2_id, compareUnsignedIntegers);

            /* Case 1: Group #2 does not exist.
             * ^^^^^^
             * We create the group and insert it in the tree.
             */
            if(group_2 == NULL)
            {
                /* We create the key for group 2 */
                unsigned int *new_group_2_id = new unsigned int(2);

                /* We place our first heap in the group */
                BinaryHeap *new_2_combination = new BinaryHeap(MINHEAP);

                /* We insert the new subset in the heap */
                new_2_combination->insert(newSubset, newSubset,
                    compareSubsetsByCost);

                /* We insert the heap in the group */
                subsetsTree->insert(
                    new_2_combination,
                    new_group_2_id,
                    newSubset,
                    compareUnsignedIntegers,
                    compareSubsetsByCombination
                );
            }

            /* Case 2: Group #2 exists. 
             * ^^^^^^
             * We search for the proper heap and place the new subset there.
             * If the heap with combinations of this subset does not exist,
             * we create a new heap and insert it in group #2.
             */
            else
            {
                /* We search for the heap of this combination */
                BinaryHeap *combinationOfTheseTwoCols =
                    (BinaryHeap *) subsetsTree->searchItemKey(
                    &group_2_id,
                    newSubset,
                    compareUnsignedIntegers,
                    compareSubsetsByCombination
                );

                /* If it does not exist, we create it */
                if(combinationOfTheseTwoCols == NULL)
                {
                    combinationOfTheseTwoCols = new BinaryHeap(MINHEAP);

                    /* We insert the heap in the group */
                    subsetsTree->insert(
                        combinationOfTheseTwoCols,
                        &group_2_id,
                        newSubset,
                        compareUnsignedIntegers,
                        compareSubsetsByCombination
                    );
                }

                /* We insert the new ordering of the two columns in the heap */
                combinationOfTheseTwoCols->insert(newSubset,
                    newSubset, compareSubsetsByCost);
            }

            /* We proceed to the next pair of nodes */
            currentNeighborNode = currentNeighborNode->getNext();
            currentNeighborPredicateNode = currentNeighborPredicateNode->getNext();
        }

        /* We proceed to the next node */
        currentNode = currentNode->getNext();
    }

    /* We free the allocated memory for the list of column identities */
    delete colIdsForParsing;

    /* Now we will start building the rest groups */
    unsigned int nextGroupId = 2;

    /* As long as a new group is created by the
     * following actions, we keep repeating them
     */
    while(1)
    {
        /* We search for the next group */
        RedBlackTree *nextGroup = subsetsTree->searchIndexKey(
            &nextGroupId, compareUnsignedIntegers);

        /* If no group was created in the previous loop, we exit */
        if(nextGroup == NULL)
            break;

        /* We place the best ordering of each combination of the group in a list */
        subsetsForParsing = new List();
        nextGroup->traverse(Inorder, placeSubsetsInList);

        /* We will traverse the list of subsets from the head */
        Listnode *currentNode = subsetsForParsing->getHead();

        /* As long as we have not finished traversing the list */
        while(currentNode != NULL)
        {
            /* We retrieve the subset (best ordering) stored in the current node */
            ColumnSubset *currentSubset = (ColumnSubset *)
                currentNode->getItem();

            /* We retrieve the last column that joined the set */
            //ColumnIdentity *lastColumn = currentSubset->getLastColumn();

            /* We retrieve the neighbors of the column */
            //List *neighbors = lastColumn->getNeighbors();

            /* We also retrieve the list of neighbor predicates */
            //List *neighborPreds = lastColumn->getNeighborPredicates();

            List *neighbors, *neighborPreds;
            currentSubset->getNeighbors(&neighbors, &neighborPreds);

            /* We will traverse the list of neighbors from the head */
            Listnode *currentNeighborNode = neighbors->getHead();

            /* We will also traverse the list of neighbor preds from the head */
            Listnode *currentNeighborPredicateNode = neighborPreds->getHead();

            /* The two lists have the same number of nodes
             *
             * As long as we have not finished traversing the list
             */
            while(currentNeighborNode != NULL)
            {
                /* We retrieve the neighbor stored in the current node */
                ColumnIdentity *neighborColId = (ColumnIdentity *)
                    currentNeighborNode->getItem();

                /* We retrieve the neighbor pred stored in the current node */
                PredicatesParser *neighborPred = (PredicatesParser *)
                    currentNeighborPredicateNode->getItem();

                /* This variable will point to the result stats after join */
                ColumnStatistics *statsAfterJoin;

                /* We find the new stats after join */
                updateStatsOfJoinTreeAndColumnByJoin(currentSubset,
                    neighborColId, neighborPred, &statsAfterJoin);

                /* We create a new subset by placing the neighbor at the
                 * end, while also passing the new stats to the contructor
                 */
                ColumnSubset *newSubset = new ColumnSubset(
                    currentSubset, neighborColId, neighborPred, statsAfterJoin
                );

                /* We search for the next group in the tree */
                unsigned int nextId = nextGroupId + 1;

                RedBlackTree *group_i = subsetsTree->searchIndexKey(
                    &nextId, compareUnsignedIntegers);

                /* Case 1: Group #i does not exist.
                 * ^^^^^^
                 * We create the group and insert it in the tree.
                 */
                if(group_i == NULL)
                {
                    /* We create the key for group 2 */
                    unsigned int *new_group_i_id = new unsigned int(nextId);

                    /* We place our first heap in the group */
                    BinaryHeap *new_i_combination = new BinaryHeap(MINHEAP);

                    /* We insert the new subset in the heap */
                    new_i_combination->insert(newSubset, newSubset,
                        compareSubsetsByCost);

                    /* We insert the heap in the group */
                    subsetsTree->insert(
                        new_i_combination,
                        new_group_i_id,
                        newSubset,
                        compareUnsignedIntegers,
                        compareSubsetsByCombination
                    );
                }

                /* Case 2: Group #i exists. 
                 * ^^^^^^
                 * We search for the proper heap and place the new subset there.
                 * If the heap with combinations of this subset does not exist,
                 * we create a new heap and insert it in group #i.
                 */
                else
                {
                    /* We search for the heap of this combination */
                    BinaryHeap *combinationOfTheseCols =
                        (BinaryHeap *) subsetsTree->searchItemKey(
                        &nextId,
                        newSubset,
                        compareUnsignedIntegers,
                        compareSubsetsByCombination
                    );

                    /* If it does not exist, we create it */
                    if(combinationOfTheseCols == NULL)
                    {
                        combinationOfTheseCols = new BinaryHeap(MINHEAP);

                        /* We insert the heap in the group */
                        subsetsTree->insert(
                            combinationOfTheseCols,
                            &nextId,
                            newSubset,
                            compareUnsignedIntegers,
                            compareSubsetsByCombination
                        );
                    }

                    /* We insert the new ordering of the two columns in the heap */
                    combinationOfTheseCols->insert(newSubset,
                        newSubset, compareSubsetsByCost);
                }

                /* We proceed to the next neighbor */
                currentNeighborNode = currentNeighborNode->getNext();

                /* We proceed to the next neighbor predicate */
                currentNeighborPredicateNode =
                    currentNeighborPredicateNode->getNext();
            }

            /* We free the allocated memory for the neighbors
             * list and the neighbor predicates list
             */
            ColumnSubset::freeNeighbors(neighbors, neighborPreds);

            /* We proceed to the next node */
            currentNode = currentNode->getNext();
        }

        /* We free the allocated memory for the list of subsets */
        delete subsetsForParsing;

        /* We will traverse the next group in the next loop */
        nextGroupId++;
    }

    subsetsTree->print(printGroup, printBinaryHeapOfSubsets);

    nextGroupId--;

    RedBlackTree *largestCombinationsGroup = subsetsTree->
        searchIndexKey(&nextGroupId, compareUnsignedIntegers);

    ComplexItem *rootItem = (ComplexItem *)
        largestCombinationsGroup->getRoot()->getItem();

    BinaryHeap *largestSubsetsHeap = (BinaryHeap *) rootItem->getItem();

    ColumnSubset *bestOrder = (ColumnSubset *)
        largestSubsetsHeap->getHighestPriorityItem();

    List *resultPredicatesOrder = bestOrder->getPredicatesOrder();

    std::cout << "Best order: ";
    resultPredicatesOrder->printFromHead(printPredicate);
    std::cout << std::endl;

    // List *remainingPreds = new List();

    // searchRemainingPredsAndPlaceDuplicates(
    //     resultPredicatesOrder, joinPreds, remainingPreds
    // );

    // result->append(resultPredicatesOrder);

    // getOptimalPredicatesOrder(remainingPreds, result);

    // delete remainingPreds;

    result->append(joinPreds);

    /* We free the allocated memory for the inverted index */
    subsetsTree->traverse(Postorder, deleteInvertedIndexInformation);
    delete subsetsTree;
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
