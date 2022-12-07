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

/******************************************************************
 * Returns the priority of the relation at the specified position *
 ******************************************************************/

unsigned int QueryHandler::getPriorityOfRelation(List *queryRelations,
    unsigned int relName, unsigned int relPosInQuery) const
{
    /* An auxiliary counter used to count how many nodes
     * we have traversed in the list of query relations
     */
    unsigned int posCounter = 0;

    /* We initialize the priority of the given relation with 1 */
    unsigned int priority = 1;

    /* We will start traversing the list of query relations from the head */
    Listnode *currentNode = queryRelations->getHead();

    while(posCounter < relPosInQuery)
    {
        /* We retrieve the relation name stored in the current node */
        unsigned int currentRelName = *((unsigned int *) currentNode->getItem());

        /* If we encountered the same relation in a previous
         * position in the list, we increase the priority by 1
         */
        if(currentRelName == relName)
            priority++;

        /* We increase the counter by 1 and proceed to the next node */
        posCounter++;
        currentNode = currentNode->getNext();
    }

    /* Finally we return the priority of the given relation */
    return priority;
}

/*************************************************************************
 * Addresses a single query and prints the result in the standard output *
 *************************************************************************/

void QueryHandler::addressSingleQuery(Query *query)
{
    /* We create the intermediate representation of this query */
    IntermediateRepresentation intermediateRepresentation =
    IntermediateRepresentation(tables, joinParameters);

    /* We retrieve the list of the relations taking part in the query */
    List *queryRels = query->getRelations();

    /* Now we will start traversing the predicates.
     *
     * We retrieve the list of predicates.
     */
    List *predicates = query->getPredicates();

    /* We will start traversing the list of predicates from the tail */
    Listnode *currentNodeOfPredicate = predicates->getTail();

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
            intermediateRepresentation.executeJoin(
                leftArray,
                leftArrayColumn,
                getPriorityOfRelation(queryRels, leftArray, leftArrayNotation),
                rightArray,
                rightArrayColumn,
                getPriorityOfRelation(queryRels, rightArray, rightArrayNotation));
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

            /* We apply the filter on the given relation */
            intermediateRepresentation.executeFilter(
                leftArray,
                leftArrayColumn,
                getPriorityOfRelation(queryRels, leftArray, leftArrayNotation),
                filterValue,
                filterOperator);
        }

        /* We have finished addressing the current predicate.
         *
         * We proceed to the next predicate of the query.
         */
        currentNodeOfPredicate = currentNodeOfPredicate->getPrevious();
    }

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
        unsigned int originalRelationPos = query->getRelationInPos(projectionArray);

        /* We print the requested sum in the standard output */
        intermediateRepresentation.produceSum(
            originalRelationPos,
            projectionColumn,
            getPriorityOfRelation(queryRels, originalRelationPos, projectionArray));

        /* We proceed to the next projection */
        currentNodeOfProjection = currentNodeOfProjection->getNext();
    }

    /* We have finished addressing the query.
     *
     * We print a new line to escape the line of the printed results.
     */
    std::cout << std::endl;
}

/**********************************************************
 * Addresses the queries from all the batches and prints *
 *          the results in the standard output           *
 **********************************************************/

void QueryHandler::addressQueries()
{
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
