#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "QueryHandler.h"

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
    unsigned int relationsNum = query->getRelations()->getCounter();

    unsigned int *intermediateRelations[relationsNum];
    unsigned int rowsNumOfIntermediateRelations[relationsNum];
    unsigned int i, j;

    // Initialization (implementing the FROM part of the query matching relations with indexes)
    for(i = 0; i < relationsNum; i++)
    {
        unsigned int currentRelationName = query->getRelationInPos(i);
        Table *currentTable = (Table *) tables->getItemInPos(currentRelationName + 1);

        unsigned int tuplesNum = currentTable->getNumOfTuples();
        intermediateRelations[i] = new unsigned int[tuplesNum];

        for(j = 0; j < tuplesNum; j++)
            intermediateRelations[i][j] = j;

        rowsNumOfIntermediateRelations[i] = tuplesNum;
    }

    List *predicates = query->getPredicates();
    Listnode *currentNodeOfPredicate = predicates->getHead();

    while(currentNodeOfPredicate != NULL)
    {
        PredicatesParser *currentPredicate = (PredicatesParser *) currentNodeOfPredicate->getItem();

        bool needsJoin = !currentPredicate->hasConstant();

        if(needsJoin)
        {
            // Left
            unsigned int leftArrayNotation = currentPredicate->getLeftArray();
            unsigned int leftArray = *((unsigned int *) query->getRelations()->getItemInPos(leftArrayNotation + 1));
            unsigned int leftArrayColumn = currentPredicate->getLeftArrayColumn();

            Table *leftTable = (Table *) tables->getItemInPos(leftArray + 1);
            unsigned int leftTableTuplesNum = rowsNumOfIntermediateRelations[leftArrayNotation];
            Tuple *leftArrayTuples = new Tuple[leftTableTuplesNum];

            for(i = 0; i < leftTableTuplesNum; i++)
            {
                // Tuple = <RowIDa , A.C>
                leftArrayTuples[i].setItem(new unsigned long long(leftTable->getTable()[leftArrayColumn][intermediateRelations[leftArrayNotation][i]]));
                leftArrayTuples[i].setRowId(intermediateRelations[leftArrayNotation][i]);
            }

            Relation *leftRel = new Relation(leftArrayTuples, leftTableTuplesNum);

            // Right
            unsigned int rightArrayNotation = currentPredicate->getRightArray();
            unsigned int rightArray = *((unsigned int *) query->getRelations()->getItemInPos(rightArrayNotation + 1));
            unsigned int rightArrayColumn = currentPredicate->getRightArrayColumn();

            Table *rightTable = (Table *) tables->getItemInPos(rightArray + 1);
            unsigned int rightTableTuplesNum = rowsNumOfIntermediateRelations[rightArrayNotation];
            Tuple *rightArrayTuples = new Tuple[rightTableTuplesNum];

            for(i = 0; i < rightTableTuplesNum; i++)
            {   
                // Tuple = <RowIDb , B.C>
                rightArrayTuples[i].setItem(new unsigned long long(rightTable->getTable()[rightArrayColumn][intermediateRelations[rightArrayNotation][i]]));
                rightArrayTuples[i].setRowId(intermediateRelations[rightArrayNotation][i]);
            }

            Relation *rightRel = new Relation(rightArrayTuples, rightTableTuplesNum);

            PartitionedHashJoin phj = PartitionedHashJoin(rightRel, leftRel, joinParameters);

            RowIdRelation *join_result = phj.executeJoin();

            BinaryHeap *leftHeap, *rightHeap;
            leftHeap = new BinaryHeap(MINHEAP);
            rightHeap = new BinaryHeap(MINHEAP);

            RedBlackTree *leftTree, *rightTree;
            leftTree = new RedBlackTree();
            rightTree = new RedBlackTree();

            unsigned int pairsNum = join_result->getNumOfRowIdPairs();
            RowIdPair *pairs = join_result->getRowIdPairs();

            for(i = 0; i < pairsNum; i++)
            {
                unsigned int leftRowId = pairs[i].getLeftRowId();
                unsigned int rightRowId = pairs[i].getRightRowId();

                if(!leftTree->search(&leftRowId, compareUnsignedInts))
                {
                    unsigned int *new_entry = new unsigned int(leftRowId);
                    leftHeap->insert(new_entry, new_entry, compareUnsignedInts);
                    leftTree->insert(new_entry, new_entry, compareUnsignedInts);
                }

                if(!rightTree->search(&rightRowId, compareUnsignedInts))
                {
                    unsigned int *new_entry = new unsigned int(rightRowId);
                    rightHeap->insert(new_entry, new_entry, compareUnsignedInts);
                    rightTree->insert(new_entry, new_entry, compareUnsignedInts);
                }
            }

            unsigned int leftPairsNum = leftTree->getCounter();
            delete leftTree;

            unsigned int rightPairsNum = rightTree->getCounter();
            delete rightTree;

            // Replacing left array in main structure
            delete[] intermediateRelations[leftArrayNotation];
            intermediateRelations[leftArrayNotation] = new unsigned int[leftPairsNum];
            rowsNumOfIntermediateRelations[leftArrayNotation] = leftPairsNum;

            for(i = 0; i < leftPairsNum; i++)
            {
                unsigned int *nextHighestPriority = (unsigned int *) leftHeap->getHighestPriorityKey();
                unsigned int currentLowestRowId = *nextHighestPriority;
                leftHeap->remove(compareUnsignedInts);
                delete nextHighestPriority;

                intermediateRelations[leftArrayNotation][i] = currentLowestRowId;
            }

            delete leftHeap;

            // Replacing right array in main structure
            delete[] intermediateRelations[rightArrayNotation];
            intermediateRelations[rightArrayNotation] = new unsigned int[rightPairsNum];
            rowsNumOfIntermediateRelations[rightArrayNotation] = rightPairsNum;

            for(i = 0; i < rightPairsNum; i++)
            {
                unsigned int *nextHighestPriority = (unsigned int *) rightHeap->getHighestPriorityKey();
                unsigned int currentLowestRowId = *nextHighestPriority;
                rightHeap->remove(compareUnsignedInts);
                delete nextHighestPriority;

                intermediateRelations[rightArrayNotation][i] = currentLowestRowId;
            }

            delete rightHeap;

            phj.freeJoinResult(join_result);

            for(i = 0; i < leftTableTuplesNum; i++)
                delete (unsigned long long *) leftArrayTuples[i].getItem();

            delete[] leftArrayTuples;

            delete leftRel;

            for(i = 0; i < rightTableTuplesNum; i++)
                delete (unsigned long long *) rightArrayTuples[i].getItem();

            delete[] rightArrayTuples;

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

    Listnode *currentNodeOfBatch = queryBatches->getHead();

    while(currentNodeOfBatch != NULL)
    {
        List *currentBatch = (List *) currentNodeOfBatch->getItem();

        Listnode *currentNodeOfQuery = currentBatch->getHead();

        while(currentNodeOfQuery != NULL)
        {
            Query *currentQuery = (Query *) currentNodeOfQuery->getItem();

            addressSingleQuery(currentQuery, fd);

            currentNodeOfQuery = currentNodeOfQuery->getNext();
        }

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
