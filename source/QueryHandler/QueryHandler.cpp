#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "QueryHandler.h"

/********************************************************************
 * An auxiliary global array that will help in various calculations *
 ********************************************************************/

static unsigned int *auxiliaryArray;

/********************************************************************************
 * A counter that will help in the insertion of elements in the auxiliary array *
 ********************************************************************************/

static unsigned int auxiliaryArrayCounter;

/*****************************************************
 * Places an unsigned integer in the auxiliary array *
 *****************************************************/

static void transferItemToAuxiliaryArray(void *item, void *key)
{
    unsigned int my_uint = *((unsigned int *) item);
    auxiliaryArray[auxiliaryArrayCounter++] = my_uint;
}

/*******************************
 * Deletes an unsigned integer *
 *******************************/

static void deleteUnsignedInteger(void *item, void *key)
{
    unsigned int *my_uint = (unsigned int *) item;
    delete my_uint;
}

/******************************
 * Prints the auxiliary array *
 ******************************/

static void printAuxiliaryArray(unsigned int array_size)
{
    std::cout << "aux = [";

    unsigned int i;

    for(i = 0; i < array_size; i++)
        std::cout << auxiliaryArray[i] << ",";

    std::cout << "end]" << std::endl;
}

/*********************************
 * Operations used for the input *
 *            tables             *
 *            ^^^^^^             *
 *********************************/

static void printTable(void *item)
{
	Table *table = (Table *) item;
	table->print();
}

static void deleteTable(void *item)
{
	Table *table = (Table *) item;
	delete table;
}

/*********************************
 * Operations used for the input *
 *    queries of the batches     *
 *    ^^^^^^^        ^^^^^^^     *
 *********************************/

static void printQuery(void *item)
{
	Query *query = (Query *) item;
	query->print();
}

static void deleteQuery(void *item)
{
	Query *query = (Query *) item;
	delete query;
}

/*********************************
 * Operations used for the input *
 *            batches            *
 *            ^^^^^^^            *
 *********************************/

static void printBatch(void *item)
{
	List *batch = (List *) item;
	std::cout << "=============== Next Batch ==============" << std::endl;
	batch->printFromHead(printQuery);
}

static void deleteBatch(void *item)
{
	List *batch = (List *) item;
	batch->traverseFromHead(deleteQuery);
	delete batch;
}

/**********************************
 * Compares two unsigned integers *
 **********************************/

static int compareUnsignedInts(void *item1, void *item2)
{
    unsigned int my_uint_1 = *((unsigned int *) item1);
    unsigned int my_uint_2 = *((unsigned int *) item2);

    if(my_uint_1 > my_uint_2)
        return 1;

    if(my_uint_1 < my_uint_2)
        return -1;

    return 0;
}

/***************
 * Constructor *
 ***************/

QueryHandler::QueryHandler(const char *init_file, const char *work_file,
    const char *config_file)
{
    /* We read the input tables and save them in the 'tables' list */
    tables = FileReader::readInitFile(init_file);

    /* We read the input queries in batches
     * and save them in the 'queryBatches' list
     */
    queryBatches = FileReader::readWorkFile(work_file);

    /* We initialize the configurations of each join that will be performed */
    joinParameters = new PartitionedHashJoinInput(config_file);
}

/**************
 * Destructor *
 **************/

QueryHandler::~QueryHandler()
{
    /* We delete the tables and the list of tables */
    tables->traverseFromHead(deleteTable);
    delete tables;

    /* We delete the batches and the list of batches */
    queryBatches->traverseFromHead(deleteBatch);
    delete queryBatches;

    /* We free the allocated memory for the join configurations */
    delete joinParameters;
}

/******************************************************
 * Addresses a single query and appends the result to *
 *  the file designated by the given file descriptor  *
 ******************************************************/

void QueryHandler::addressSingleQuery(Query *query, int fileDescOfResultFile)
{
    /* We retrieve the amount of relations taking part in the query */
    unsigned int relationsNum = query->getRelations()->getCounter();

    /* These two arrays below form the intermediate state of the query.
     *                                 ^^^^^^^^^^^^^^^^^^
     * The intermediate state of the query is a sequence of arrays.
     * There are as many arrays as the amount of relations taking part
     * in the query. If, for example, the relations 3, 5 and 7 are
     * taking part in the query, then the intermediate representation
     * will consist of three arrays, the first depicting the row IDs
     * of relation 3 that satisfy the predicates, the second depicting
     * the row IDs of relation 5 that satisfy the predicates and the
     * the third depicting the row IDs of relation 7 that satisfy the
     * predicates.
     *
     * The 'intermediateRelation' array is the above sequence of arrays.
     * The 'rowsNumOfIntermediateRelations' array stores the amount of
     * elements of each array in the sequence 'intermediateRelation'.
     */
    unsigned int *intermediateRelations[relationsNum];
    unsigned int rowsNumOfIntermediateRelations[relationsNum];

    /* Auxiliary variables used for counting */
    unsigned int i, j;

    /* We initialize the intermediate structure with the initial state */

    for(i = 0; i < relationsNum; i++)
    {
        /* We retrieve the name of the current relation */
        unsigned int currentRelationName = query->getRelationInPos(i);

        /* We retrieve the table of that relation */
        Table *currentTable = (Table *) tables->getItemInPos(currentRelationName + 1);

        /* We retrieve the amount of tuples of that table */
        unsigned int tuplesNum = currentTable->getNumOfTuples();

        /* This table will be the next table in the sequence of intermediate arrays */
        intermediateRelations[i] = new unsigned int[tuplesNum];

        /* Initially we store all the row IDs of the table in the intermediate array */

        for(j = 0; j < tuplesNum; j++)
            intermediateRelations[i][j] = j;

        /* We update the intermediate state with the amount of tuples of the array */
        rowsNumOfIntermediateRelations[i] = tuplesNum;
    }

    /* Now we will start traversing the predicates.
     *
     * We retrieve the list of predicates.
     */
    List *predicates = query->getPredicates();

    /* We will start traversing the list of predicates from the head */
    Listnode *currentNodeOfPredicate = predicates->getHead();

    /* As long as we have not finished traversing the list of predicates,
     * we do the following actions inside the 'while' loop below
     */
    while(currentNodeOfPredicate != NULL)
    {
        /* We retrieve the next predicate */
        PredicatesParser *currentPredicate = (PredicatesParser *)
            currentNodeOfPredicate->getItem();

        /* That predicate will either be a 'JOIN' between array columns or
         * a simple filter with a constant value.
         *
         * We identify which of the two categories the predicate belongs to.
         */
        bool needsJoin = !currentPredicate->hasConstant();

        /* Case 1: The predicate suggests a 'JOIN' between two columns
         * ^^^^^^
         */
        if(needsJoin)
        {
            /* This is the position in the query of the left array
             *             ^^^^^^^^^^^^^^^^^^^^^
             */
            unsigned int leftArrayNotation = currentPredicate->getLeftArray();

            /* This is the real position of the left array
             *             ^^^^^^^^^^^^^
             */
            unsigned int leftArray = query->getRelationInPos(leftArrayNotation);

            /* This is the suggested column for 'JOIN' of the left array */
            unsigned int leftArrayColumn = currentPredicate->getLeftArrayColumn();

            /* We retrieve the table with all the data of the left array */
            Table *leftTable = (Table *) tables->getItemInPos(leftArray + 1);

            /* We retrieve the amount of tuples of the array from the intermediate state */
            unsigned int leftTableTuplesNum = rowsNumOfIntermediateRelations[leftArrayNotation];

            /* We will create tuples of <RowId,value> for the left array */
            Tuple *leftArrayTuples = new Tuple[leftTableTuplesNum];

            /* We start creating the tuples */

            for(i = 0; i < leftTableTuplesNum; i++)
            {
                /* We set the value of the tuple */
                leftArrayTuples[i].setItem(new unsigned long long(leftTable->getTable()
                    [leftArrayColumn][intermediateRelations[leftArrayNotation][i]]));

                /* We set the row ID of the tuple */
                leftArrayTuples[i].setRowId(intermediateRelations[leftArrayNotation][i]);
            }

            /* We have created all the tuples.
             *
             * Now we create the relation that will hold them together.
             */
            Relation *leftRel = new Relation(leftArrayTuples, leftTableTuplesNum);

            /* This is the position in the query of the right array
             *             ^^^^^^^^^^^^^^^^^^^^^
             */
            unsigned int rightArrayNotation = currentPredicate->getRightArray();

            /* This is the real position of the right array
             *             ^^^^^^^^^^^^^
             */
            unsigned int rightArray = query->getRelationInPos(rightArrayNotation);

            /* This is the suggested column for 'JOIN' of the right array */
            unsigned int rightArrayColumn = currentPredicate->getRightArrayColumn();

            /* We retrieve the table with all the data of the right array */
            Table *rightTable = (Table *) tables->getItemInPos(rightArray + 1);

            /* We retrieve the amount of tuples of the array from the intermediate state */
            unsigned int rightTableTuplesNum = rowsNumOfIntermediateRelations[rightArrayNotation];

            /* We will create tuples of <RowId,value> for the right array */
            Tuple *rightArrayTuples = new Tuple[rightTableTuplesNum];

            /* We start creating the tuples */

            for(i = 0; i < rightTableTuplesNum; i++)
            {   
                /* We set the value of the tuple */
                rightArrayTuples[i].setItem(new unsigned long long(rightTable->getTable()
                    [rightArrayColumn][intermediateRelations[rightArrayNotation][i]]));

                /* We set the row ID of the tuple */
                rightArrayTuples[i].setRowId(intermediateRelations[rightArrayNotation][i]);
            }

            /* We have created all the tuples.
             *
             * Now we create the relation that will hold them together.
             */
            Relation *rightRel = new Relation(rightArrayTuples, rightTableTuplesNum);

            /* We have built the relations that take part in the 'JOIN' operation.
             *
             * We create an object that will help us perform that operation.
             */
            PartitionedHashJoin phj = PartitionedHashJoin(rightRel, leftRel, joinParameters);

            /* We execute the 'JOIN' operation */
            RowIdRelation *join_result = phj.executeJoin();

            /* The result of the 'JOIN' is an array of items of the form:
             *
             *                  <RowId_1_Left,RowId_1_Right>
             *                  <RowId_2_Left,RowId_2_Right>
             *                  <RowId_3_Left,RowId_3_Right>
             *                              ...
             *                  <RowId_N_Left,RowId_N_Right>
             */

            //BinaryHeap *leftHeap, *rightHeap;
            //leftHeap = new BinaryHeap(MINHEAP);
            //rightHeap = new BinaryHeap(MINHEAP);

            RowIdPair *pairs = join_result->getRowIdPairs();
            unsigned int pairsNum = join_result->getNumOfRowIdPairs();

            RedBlackTree *leftTree, *rightTree;
            leftTree = new RedBlackTree();
            rightTree = new RedBlackTree();

            for(i = 0; i < pairsNum; i++)
            {
                /* We store the left & right row IDs in seperate variables */
                unsigned int leftRowId = pairs[i].getLeftRowId();
                unsigned int rightRowId = pairs[i].getRightRowId();

                /* If the left row ID does not exist in the
                 * left tree already, we insert it in the tree
                 */
                if(leftTree->search(&leftRowId, compareUnsignedInts) == false)
                {
                    unsigned int *new_entry = new unsigned int(leftRowId);
                    leftTree->insert(new_entry, new_entry, compareUnsignedInts);
                }

                /* If the right row ID does not exist in the
                 * right tree already, we insert it in the tree
                 */
                if(rightTree->search(&rightRowId, compareUnsignedInts) == false)
                {
                    unsigned int *new_entry = new unsigned int(rightRowId);
                    rightTree->insert(new_entry, new_entry, compareUnsignedInts);
                }
            }

            /* We retrieve the amount of unique left row IDs */
            unsigned int leftRowIdsNum = leftTree->getCounter();

            /* We retrieve the amount of unique right row IDs */
            unsigned int rightRowIdsNum = rightTree->getCounter();

            /* We will transfer the elements of the tree
             * to the auxiliary array in sorted order
             */
            auxiliaryArray = new unsigned int[leftRowIdsNum];
            auxiliaryArrayCounter = 0;
            leftTree->traverse(Inorder, transferItemToAuxiliaryArray);

            //printAuxiliaryArray(leftRowIdsNum);

            /* We update the intermediate representation with
             * the new sequence of row IDs for the left array
             */
            delete[] intermediateRelations[leftArrayNotation];
            intermediateRelations[leftArrayNotation] = auxiliaryArray;
            rowsNumOfIntermediateRelations[leftArrayNotation] = leftRowIdsNum;

            /* We free the items of the left tree and the left tree itself */
            leftTree->traverse(Inorder, deleteUnsignedInteger);
            delete leftTree;

            /* We will transfer the elements of the tree
             * to the auxiliary array in sorted order
             */
            auxiliaryArray = new unsigned int[rightRowIdsNum];
            auxiliaryArrayCounter = 0;
            rightTree->traverse(Inorder, transferItemToAuxiliaryArray);

            //printAuxiliaryArray(rightRowIdsNum);

            /* We update the intermediate representation with
             * the new sequence of row IDs for the right array
             */
            delete[] intermediateRelations[rightArrayNotation];
            intermediateRelations[rightArrayNotation] = auxiliaryArray;
            rowsNumOfIntermediateRelations[rightArrayNotation] = rightRowIdsNum;

            /* We free the items of the right tree and the right tree itself */
            rightTree->traverse(Inorder, deleteUnsignedInteger);
            delete rightTree;







            // // Replacing left array in main structure
            // delete[] intermediateRelations[leftArrayNotation];
            // intermediateRelations[leftArrayNotation] = new unsigned int[leftRowIdsNum];
            // rowsNumOfIntermediateRelations[leftArrayNotation] = leftRowIdsNum;

            // for(i = 0; i < leftRowIdsNum; i++)
            // {
            //     unsigned int *nextHighestPriority = (unsigned int *) leftHeap->getHighestPriorityKey();
            //     unsigned int currentLowestRowId = *nextHighestPriority;
            //     leftHeap->remove(compareUnsignedInts);
            //     delete nextHighestPriority;

            //     intermediateRelations[leftArrayNotation][i] = currentLowestRowId;
            // }

            // delete leftHeap;

            // // Replacing right array in main structure
            // delete[] intermediateRelations[rightArrayNotation];
            // intermediateRelations[rightArrayNotation] = new unsigned int[rightRowIdsNum];
            // rowsNumOfIntermediateRelations[rightArrayNotation] = rightRowIdsNum;

            // for(i = 0; i < rightRowIdsNum; i++)
            // {
            //     unsigned int *nextHighestPriority = (unsigned int *) rightHeap->getHighestPriorityKey();
            //     unsigned int currentLowestRowId = *nextHighestPriority;
            //     rightHeap->remove(compareUnsignedInts);
            //     delete nextHighestPriority;

            //     intermediateRelations[rightArrayNotation][i] = currentLowestRowId;
            // }

            // delete rightHeap;

            /* We free the result of the join operation */
            phj.freeJoinResult(join_result);

            /* We free the items we used to initialize the tuples of the left array */
            for(i = 0; i < leftTableTuplesNum; i++)
                delete (unsigned long long *) leftArrayTuples[i].getItem();

            /* We free the tuples of the left array */
            delete[] leftArrayTuples;

            /* We free the relation that depicted the suggested column of the left array */
            delete leftRel;

            /* We free the items we used to initialize the tuples of the rightt array */
            for(i = 0; i < rightTableTuplesNum; i++)
                delete (unsigned long long *) rightArrayTuples[i].getItem();

            /* We free the tuples of the right array */
            delete[] rightArrayTuples;

            /* We free the relation that depicted the suggested column of the right array */
            delete rightRel;
        }

        else
        {
            unsigned int leftArrayNotation = currentPredicate->getLeftArray();
            unsigned int leftArray = *((unsigned int *) query->getRelations()->getItemInPos(leftArrayNotation + 1));
            unsigned int leftArrayColumn = currentPredicate->getLeftArrayColumn();

            Table *leftTable = (Table *) tables->getItemInPos(leftArray + 1);
            unsigned int leftTableTuplesNum = rowsNumOfIntermediateRelations[leftArrayNotation];

            unsigned int filterValue = currentPredicate->getFilterValue();
            char filterOperator = currentPredicate->getFilterOperator();

            List *results = new List();

            for(i = 0; i < leftTableTuplesNum; i++)
            {
                unsigned long long currentTableItem = leftTable->getTable()[leftArrayColumn][intermediateRelations[leftArrayNotation][i]];

                bool currentTableItemSatisfiesFilter = false;

                switch(filterOperator)
                {
                    case '<':
                    {
                        if(currentTableItem < filterValue)
                            currentTableItemSatisfiesFilter = true;

                        break;
                    }

                    case '>':
                    {
                        if(currentTableItem > filterValue)
                            currentTableItemSatisfiesFilter = true;

                        break;
                    }

                    case '=':
                    {
                        if(currentTableItem == filterValue)
                            currentTableItemSatisfiesFilter = true;

                        break;
                    }
                }

                if(currentTableItemSatisfiesFilter)
                    results->insertLast(new unsigned int(intermediateRelations[leftArrayNotation][i]));
            }

            unsigned int resultsQuantity = results->getCounter();
            rowsNumOfIntermediateRelations[leftArrayNotation] = resultsQuantity;

            delete[] intermediateRelations[leftArrayNotation];
            intermediateRelations[leftArrayNotation] = new unsigned int[resultsQuantity];

            for(i = 0; i < resultsQuantity; i++)
            {
                unsigned int *addressOfNextRowId = (unsigned int *) results->getItemInPos(1);
                unsigned int nextRowId = *addressOfNextRowId;
                results->removeFront();
                delete addressOfNextRowId;

                intermediateRelations[leftArrayNotation][i] = nextRowId;
            }

            delete results;
        }

        currentNodeOfPredicate = currentNodeOfPredicate->getNext();
    }

    std::cout << "======= Intermediate Array Info =======" << std::endl;
    for(i = 0; i < relationsNum; i++)
        std::cout << "Rows of Intermediate Array \"" << i << "\": " << rowsNumOfIntermediateRelations[i] << std::endl;
    std::cout << "======================================" << std::endl;

    List *projections = query->getProjections();
    Listnode *currentNodeOfProjection = projections->getHead();

    while(currentNodeOfProjection != NULL)
    {
        ProjectionsParser *currentProjection = (ProjectionsParser *) currentNodeOfProjection->getItem();

        // e.g 0.1 => projectionArray = 0, projectionColumn = 1
        unsigned int projectionArray = currentProjection->getArray();
        unsigned int projectionColumn = currentProjection->getColumn();

        unsigned int originalTablePos = *((unsigned int *) query->getRelations()->getItemInPos(projectionArray + 1));
        Table *originalTable = (Table *) tables->getItemInPos(originalTablePos + 1);
        unsigned int tableTuplesNum = rowsNumOfIntermediateRelations[projectionArray];

        if(tableTuplesNum == 0)
        {
            std::cout << "NULL ";
            currentNodeOfProjection = currentNodeOfProjection->getNext();
            continue;
        }

        unsigned long long sum = 0;

        //std::cout << "\n\nFor table \"" << originalTablePos << "\": (" << tableTuplesNum << " tuples)" << std::endl;
        for(i = 0; i < tableTuplesNum; i++)
        {
            //std::cout << "table[" << projectionColumn << "][" << intermediateRelations[projectionArray][i] << "] = " << originalTable->getTable()[projectionColumn][intermediateRelations[projectionArray][i]] << ", ";
            sum += originalTable->getTable()[projectionColumn][intermediateRelations[projectionArray][i]];
        }

        printf("%llu ", sum);

        currentNodeOfProjection = currentNodeOfProjection->getNext();
    }

    std::cout << std::endl;
/*
    for(i = 0; i < relationsNum; i++)
    {
        for(j = 0; j < rowsNumOfIntermediateRelations[i]; j++)
            std::cout << intermediateRelations[i][j] << ",";
        std::cout << "end (" << rowsNumOfIntermediateRelations[i] << " rows)" << std::endl;
    }
*/
    for(i = 0; i < relationsNum; i++)
        delete[] intermediateRelations[i];
}

/**********************************************************
 * Addresses the queries from all the batches and stores  *
 * the results in the given file (the file may not exist) *
 **********************************************************/

void QueryHandler::addressQueries(const char *result_file)
{
    // we open the result file here...
    int fd = 0;

    std::cout << "============================== Results ==============================" << std::endl;

    /* We will start traversing the list of batches from the head */
    Listnode *currentNodeOfBatch = queryBatches->getHead();

    /* As long as we have not finished traversing the list of batches */

    while(currentNodeOfBatch != NULL)
    {
        /* We retrieve the batch of the current node */
        List *currentBatch = (List *) currentNodeOfBatch->getItem();

        /* That batch is a list of queries.
         *
         * We traverse the list of queries from the head.
         */
        Listnode *currentNodeOfQuery = currentBatch->getHead();

        /* As long as we have not finished traversing the list of queries */

        while(currentNodeOfQuery != NULL)
        {
            /* We retrieve the next query of the batch */
            Query *currentQuery = (Query *) currentNodeOfQuery->getItem();

            /* We address that query */
            addressSingleQuery(currentQuery, fd);

            /* We proceed to the next query */
            currentNodeOfQuery = currentNodeOfQuery->getNext();
        }

        /* We have finished processing the current batch.
         *
         * We proceed to the next batch.
         */
        currentNodeOfBatch = currentNodeOfBatch->getNext();
    }

    // we close the opened file here...
}

/************************************************
 * Prints the tables and the batches of queries *
 ************************************************/

void QueryHandler::print() const
{
    /* We announce the printing of the input tables */
    std::cout << "================ Tables ================ " << std::endl;

    /* We print the input tables */
    tables->printFromHead(printTable);

    /* We announce the printing of the input query batches */
    std::cout << "============= Query Batches ============= " << std::endl;

    /* We print the input batches of queries */
    queryBatches->printFromHead(printBatch);
}
