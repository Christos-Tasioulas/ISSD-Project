#include <iostream>
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

    unsigned int *currentRelations[relationsNum];
    unsigned int i, j;

    // Initialization (implementing the FROM part of the query matching relations with indexes)
    for(i = 0; i < relationsNum; i++)
    {
        unsigned int currentRelationName = query->getRelationInPos(i);
        Table *currentTable = (Table *) tables->getItemInPos(currentRelationName + 1);

        unsigned int tuplesNum = currentTable->getNumOfTuples();
        currentRelations[i] = new unsigned int[tuplesNum];

        for(j = 0; j < tuplesNum; j++)
            currentRelations[i][j] = j;
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
            unsigned int leftTableTuplesNum = leftTable->getNumOfTuples();
            Tuple *leftArrayTuples = new Tuple[leftTableTuplesNum];

            for(i = 0; i < leftTableTuplesNum; i++)
            {
                leftArrayTuples[i].setItem(new unsigned long long(leftTable->getTable()[leftArrayColumn][i]));
                leftArrayTuples[i].setRowId(i);
            }

            Relation *leftRel = new Relation(leftArrayTuples, leftTableTuplesNum);

            // Right
            unsigned int rightArrayNotation = currentPredicate->getRightArray();
            unsigned int rightArray = *((unsigned int *) query->getRelations()->getItemInPos(rightArrayNotation + 1));
            unsigned int rightArrayColumn = currentPredicate->getRightArrayColumn();

            Table *rightTable = (Table *) tables->getItemInPos(rightArray + 1);
            unsigned int rightTableTuplesNum = rightTable->getNumOfTuples();
            Tuple *rightArrayTuples = new Tuple[rightTableTuplesNum];

            for(i = 0; i < rightTableTuplesNum; i++)
            {
                rightArrayTuples[i].setItem(new unsigned long long(rightTable->getTable()[rightArrayColumn][i]));
                rightArrayTuples[i].setRowId(i);
            }

            Relation *rightRel = new Relation(rightArrayTuples, rightTableTuplesNum);

            PartitionedHashJoin phj = PartitionedHashJoin(leftRel, rightRel, joinParameters);

            RowIdRelation *join_result = phj.executeJoin();

            BinaryHeap *leftHeap, *rightHeap;
            leftHeap = new BinaryHeap(MINHEAP);
            rightHeap = new BinaryHeap(MINHEAP);

            unsigned int pairsNum = join_result->getNumOfRowIdPairs();
            RowIdPair *pairs = join_result->getRowIdPairs();

            for(i = 0; i < pairsNum; i++)
            {
                leftHeap->insert(NULL, new unsigned int(pairs[i].getLeftRowId()), compareUnsignedInts);
                rightHeap->insert(NULL, new unsigned int(pairs[i].getRightRowId()), compareUnsignedInts);
            }

            // Replacing left array in main structure
            delete[] currentRelations[leftArrayNotation];
            currentRelations[leftArrayNotation] = new unsigned int[pairsNum];
    
            for(i = 0; i < pairsNum; i++)
            {
                unsigned int *nextHighestPriority = (unsigned int *) leftHeap->getHighestPriorityKey();
                unsigned int currentLowestRowId = *nextHighestPriority;
                leftHeap->remove(compareUnsignedInts);
                delete nextHighestPriority;

                currentRelations[leftArrayNotation][i] = currentLowestRowId;
            }

            delete leftHeap;

            // Replacing right array in main structure
            delete[] currentRelations[rightArrayNotation];
            currentRelations[rightArrayNotation] = new unsigned int[pairsNum];
    
            for(i = 0; i < pairsNum; i++)
            {
                unsigned int *nextHighestPriority = (unsigned int *) rightHeap->getHighestPriorityKey();
                unsigned int currentLowestRowId = *nextHighestPriority;
                rightHeap->remove(compareUnsignedInts);
                delete nextHighestPriority;

                currentRelations[rightArrayNotation][i] = currentLowestRowId;
            }

            delete rightHeap;

            phj.freeJoinResult(join_result);
            delete leftRel;
            delete leftArrayTuples;
            delete rightRel;
            delete rightArrayTuples;
        }

        else
        {

        }

        currentNodeOfPredicate = currentNodeOfPredicate->getNext();
    }
}

/**********************************************************
 * Addresses the queries from all the batches and stores  *
 * the results in the given file (the file may not exist) *
 **********************************************************/

void QueryHandler::addressQueries(const char *result_file)
{

    // we open the result file here...
    int fd = 0;


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
