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

static void printTableInfo(void *item)
{
	Table *table = (Table *) item;

	unsigned long long numRows = table->getNumOfTuples();
    unsigned long long numCols = table->getNumOfColumns();

    std::cout << "[" << numRows << " rows, "
        << numCols << " columns]" << std::endl;
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
	batch->printFromHead(printQuery);
    std::cout << "============== End of Batch ==============" << std::endl;
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

/*************************************************************************
 * Addresses a single query and prints the result in the standard output *
 *************************************************************************/

void QueryHandler::addressSingleQuery(Query *query)
{
    /* We retrieve the amount of relations taking part in the query */
    unsigned int relationsNum = query->getRelations()->getCounter();

    // /* These two arrays below form the intermediate state of the query.
    //  *                                 ^^^^^^^^^^^^^^^^^^
    //  * The intermediate state of the query is a sequence of arrays.
    //  * There are as many arrays as the amount of relations taking part
    //  * in the query. If, for example, the relations 3, 5 and 7 are
    //  * taking part in the query, then the intermediate representation
    //  * will consist of three arrays, the first depicting the row IDs
    //  * of relation 3 that satisfy the predicates, the second depicting
    //  * the row IDs of relation 5 that satisfy the predicates and the
    //  * the third depicting the row IDs of relation 7 that satisfy the
    //  * predicates.
    //  *
    //  * The 'intermediateRelation' array is the above sequence of arrays.
    //  * The 'rowsNumOfIntermediateRelations' array stores the amount of
    //  * elements of each array in the sequence 'intermediateRelation'.
    //  */
    // unsigned int *intermediateRelations[relationsNum];
    // unsigned int rowsNumOfIntermediateRelations[relationsNum];

    //  Auxiliary variables used for counting 
    // unsigned int i, j;

    // /* We initialize the intermediate structure with the initial state */

    // for(i = 0; i < relationsNum; i++)
    // {
    //     /* We retrieve the name of the current relation */
    //     unsigned int currentRelationName = query->getRelationInPos(i);

    //     /* We retrieve the table of that relation */
    //     Table *currentTable = (Table *) tables->getItemInPos(currentRelationName + 1);

    //     /* We retrieve the amount of tuples of that table */
    //     unsigned int tuplesNum = currentTable->getNumOfTuples();

    //     /* This table will be the next table in the sequence of intermediate arrays */
    //     intermediateRelations[i] = new unsigned int[tuplesNum];

    //     /* Initially we store all the row IDs of the table in the intermediate array */

    //     for(j = 0; j < tuplesNum; j++)
    //         intermediateRelations[i][j] = j;

    //     /* We update the intermediate state with the amount of tuples of the array */
    //     rowsNumOfIntermediateRelations[i] = tuplesNum;
    // }

    IntermediateRepresentation intermediateRepresentation = IntermediateRepresentation(tables, joinParameters);

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

            /* We execute the 'JOIN' between the two relations */
            intermediateRepresentation.executeJoin(leftArray, leftArrayColumn,
                rightArray, rightArrayColumn);
        }

        else
        {
            /* This is the position in the query of the left array
             *             ^^^^^^^^^^^^^^^^^^^^^
             */
            unsigned int leftArrayNotation = currentPredicate->getLeftArray();

            /* This is the real position of the left array
             *             ^^^^^^^^^^^^^
             */
            unsigned int leftArray = query->getRelationInPos(leftArrayNotation);

            /* This is the suggested column of the left array for the predicate */
            unsigned int leftArrayColumn = currentPredicate->getLeftArrayColumn();

            /* We retrieve the constant integer value that will filter the table */
            unsigned int filterValue = currentPredicate->getFilterValue();

            /* We retrieve the operator of the predicate ('<', '>', '=') */
            char filterOperator = currentPredicate->getFilterOperator();

            
        }

        /* We have finished addressing the current predicate.
         *
         * We proceed to the next predicate of the query.
         */
        currentNodeOfPredicate = currentNodeOfPredicate->getNext();
    }
/*
    std::cout << "======= Intermediate Array Info =======" << std::endl;
    for(i = 0; i < relationsNum; i++)
        std::cout << "Rows of Intermediate Array \"" << i << "\": " << rowsNumOfIntermediateRelations[i] << std::endl;
    std::cout << "======================================" << std::endl;
*/
    /* Now we will traverse the projections of
     * the query to compute the suggested sums
     */
    List *projections = query->getProjections();

    /* We will start traversing the projections list from the head */
    Listnode *currentNodeOfProjection = projections->getHead();

    /* As long as we have not finished traversing the list of projections,
     * we do the following actions inside the 'while' loop below
     */
    while(currentNodeOfProjection != NULL)
    {
        /* We retrieve the projection in the current node */
        ProjectionsParser *currentProjection = (ProjectionsParser *) currentNodeOfProjection->getItem();

        /* We retrieve the suggested relation and column of that relation */
        unsigned int projectionArray = currentProjection->getArray();
        unsigned int projectionColumn = currentProjection->getColumn();

        /* We retrieve the real position of the suggested relation */
        unsigned int originalTablePos = query->getRelationInPos(projectionArray);

        // /* We retrieve the table with all the data of the suggested relation */    
        // Table *originalTable = (Table *) tables->getItemInPos(originalTablePos + 1);

        // /* We retrieve the amount of rows of the intermediate table */
        // unsigned int tableTuplesNum = rowsNumOfIntermediateRelations[projectionArray];

        // /* Case the intermediate table has no valid rows (sum = 0) */

        // if(tableTuplesNum == 0)
        // {
        //     /* In this case we just print 'NULL' */
        //     std::cout << "NULL ";

        //     /* We proceed to the next projection */
        //     currentNodeOfProjection = currentNodeOfProjection->getNext();

        //     /* There is nothing else to do in this case */
        //     continue;
        // }

        // /* Case the intermediate table has non-zero rows
        //  *
        //  * We initialize the sum to zero.
        //  */
        // unsigned long long sum = 0;

        // /* We sum all the integers in the requested position of the original
        //  * table designated by the row IDs of the intermediate table
        //  */
        // //std::cout << "\n\nFor table \"" << originalTablePos << "\": (" << tableTuplesNum << " tuples)" << std::endl;
        // for(i = 0; i < tableTuplesNum; i++)
        // {
        //     //std::cout << "table[" << projectionColumn << "][" << intermediateRelations[projectionArray][i] << "] = " << originalTable->getTable()[projectionColumn][intermediateRelations[projectionArray][i]] << ", ";
        //     sum += originalTable->getTable()[projectionColumn]
        //         [intermediateRelations[projectionArray][i]];
        // }

        // /* We print the sum */
        // std::cout << sum << " ";

        /* We proceed to the next projection */
        currentNodeOfProjection = currentNodeOfProjection->getNext();
    }

    /* We have finished addressing the query.
     *
     * We print a new line to escape the line of the printed results.
     */
    std::cout << std::endl;
/*
    for(i = 0; i < relationsNum; i++)
    {
        for(j = 0; j < rowsNumOfIntermediateRelations[i]; j++)
            std::cout << intermediateRelations[i][j] << ",";
        std::cout << "end (" << rowsNumOfIntermediateRelations[i] << " rows)" << std::endl;
    }
*/
    /* Finally we free the allocated memory for the intermediate state */

    // for(i = 0; i < relationsNum; i++)
    //     delete[] intermediateRelations[i];
}

/**********************************************************
 * Addresses the queries from all the batches and prints *
 *          the results in the standard output           *
 **********************************************************/

void QueryHandler::addressQueries()
{
    std::cout << "================ Results =================" << std::endl;

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
            addressSingleQuery(currentQuery);

            /* We proceed to the next query */
            currentNodeOfQuery = currentNodeOfQuery->getNext();
        }

        /* We have finished processing the current batch.
         *
         * We proceed to the next batch.
         */
        currentNodeOfBatch = currentNodeOfBatch->getNext();
    }
}

/************************************************
 * Prints the tables and the batches of queries *
 ************************************************/

void QueryHandler::print() const
{
    /* We announce the printing of the input tables */
    std::cout << "\n================ Tables ================ " << std::endl;

    /* We print the input tables */
    tables->printFromHead(printTable);

    /* We announce the printing of the input tables info */
    std::cout << "\n============== Table Info ============== " << std::endl;

    /* We print the input tables (num rows, num columns) */
    tables->printFromHead(printTableInfo);

    /* We announce the printing of the input query batches */
    std::cout << "\n============ Query Batches ============= " << std::endl;

    /* We print the input batches of queries */
    queryBatches->printFromHead(printBatch);

    /* We print the join parameters given of the handler */
    joinParameters->print();
}
