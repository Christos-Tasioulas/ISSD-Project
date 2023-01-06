#ifndef _QUERY_OPTIMIZER_H_
#define _QUERY_OPTIMIZER_H_

#include "Table.h"
#include "ColumnSubset.h"

class QueryOptimizer {

private:

    /* The list of input tables with all their data */
    List *tables;

    /* The query we want to optimize with this class */
    Query *query;

    /* Returns the most optimal cost of addressing
     * only the predicates in the given linked list
     */
    List *getOptimalPredicatesOrderRec(List *chosenPredicates,
        unsigned long long *cost = NULL) const;

public:

    /* Constructor */
    QueryOptimizer(List *tables, Query *query);

    /* Destructor */
    ~QueryOptimizer();

    /* Returns a list of predicates in the order that is estimated most optimal */
    List *getOptimalPredicatesOrder() const;

    /* Frees the allocated memory for the result of 'getOptimalPredicatesOrder' */
    void deleteOptimalPredicatesOrder(List *optimalPredicatesOrder) const;

};

#endif
