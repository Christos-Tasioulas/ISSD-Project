#ifndef _QUERY_HANDLER_H_
#define _QUERY_HANDLER_H_

#include "FileReader.h"
#include "BinaryHeap.h"
#include "PartitionedHashJoin.h"

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

public:

    /* Constuctor & Destructor */
    QueryHandler(const char *init_file, const char *work_file);
    ~QueryHandler();

    /* Getter - Returns the list of tables */
    List *getTables() const;

    /* Getter - Returns the list of query batches */
    List *getQueryBatches() const;

    /* Prints the tables and the batches of queries */
    void print() const;

};

#endif
