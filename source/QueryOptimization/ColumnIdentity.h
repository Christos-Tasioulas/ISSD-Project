#ifndef _COLUMN_IDENTITY_H_
#define _COLUMN_IDENTITY_H_

#include "List.h"
#include "Query.h"
#include "ColumnStatistics.h"

class ColumnIdentity {

private:

    /* The name of the table the column belongs to */
    unsigned int tableName;

    /* The name of the column itself */
    unsigned int tableColumn;

    /* The priority of the column in the query relations */
    unsigned int priorityInQuery;

    /* The initial statistics of the column (or after having been filtered) */
    ColumnStatistics *columnStats;

    /* A list of other columns that need to be joined with this one */
    List *neighbors;

    /*  */
    List *predicatePerNeighbor;

public:

    /* Constructor */
    ColumnIdentity(unsigned int tableName, unsigned int tableColumn,
        unsigned int priorityInQuery, ColumnStatistics *columnStats, Query *query);

    /* Destructor */
    ~ColumnIdentity();

    /* Prints the contents of a column identity */
    void print() const;

};

#endif
