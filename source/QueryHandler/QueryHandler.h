#ifndef _QUERY_HANDLER_H_
#define _QUERY_HANDLER_H_

#include "PartitionedHashJoinInput.h"
#include "IntermediateRepresentation.h"
#include "QueryOptimizer.h"

/* A class that stores the input relations (tables) and
 * the input queries split in batches. The class contains
 * operations to answer the above queries on these tables.
 */
class QueryHandler {

private:

    /* The list of input tables */
    List *tables;

    /* The list of input batches of queries */
    List *queryBatches;

    /* Parameters of every join operation that will be performed */
    PartitionedHashJoinInput *joinParameters;

    /* A job scheduler that we will use for parallel
     * execution of the tasks of a join predicate
     */
    JobScheduler *jobScheduler;

    /* Returns the priority of the relation at the specified position */
    unsigned int getPriorityOfRelation(List *queryRelations,
        unsigned int relName, unsigned int relPosInQuery) const;

    /* Addresses a single query and prints the result in the standard output */
    void addressSingleQuery(Query *query);

public:

    /* Constuctor & Destructor */
    QueryHandler(const char *init_file, const char *work_file,
        const char *config_file);
    ~QueryHandler();

    /* Getter - Returns the list of tables */
    List *getTables() const;

    /* Getter - Returns the list of query batches */
    List *getQueryBatches() const;

    /* Addresses the queries from all the batches and prints
     * the results in the standard output
     */
    void addressQueries();

    /* Prints the tables and the batches of queries */
    void print() const;

};

#endif
