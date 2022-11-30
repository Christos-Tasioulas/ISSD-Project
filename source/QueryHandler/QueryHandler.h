#ifndef _QUERY_HANDLER_H_
#define _QUERY_HANDLER_H_

#include "BinaryHeap.h"
#include "RedBlackTree.h"
#include "PartitionedHashJoin.h"
#include "PartitionedHashJoinInput.h"

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

    PartitionedHashJoinInput *joinParameters;

    void addressSingleQuery(Query *query, int fileDescOfResultFile);

public:

    /* Constuctor & Destructor */
    QueryHandler(const char *init_file, const char *work_file,
        const char *config_file);
    ~QueryHandler();

    /* Getter - Returns the list of tables */
    List *getTables() const;

    /* Getter - Returns the list of query batches */
    List *getQueryBatches() const;

    /* Addresses the queries from all the batches and stores
     * the results in the given file (the file may not exist)
     */
    void addressQueries(const char *result_file);

    /* Prints the tables and the batches of queries */
    void print() const;

};

#endif
