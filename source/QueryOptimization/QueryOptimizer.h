#ifndef _QUERY_OPTIMIZER_H_
#define _QUERY_OPTIMIZER_H_

#include "Table.h"
#include "ColumnSubset.h"
#include "B_Tree.h"

class QueryOptimizer {

private:

    /* The list of input tables with all their data */
    List *tables;

    /* The query we want to optimize with this class */
    Query *query;

    /* A list of all the join predicates of the query */
    List *joinPreds;

    /* A list of all the filter predicates of the query */
    List *filterPreds;

    /* A tree storing each initial column taking part in the query */
    B_Tree *columnIdentitiesTree;

    /* Attempts to insert a relation with the given attributes to
     * the B-Tree. Fails if the relation already exists in the tree
     */
    void insertRelInTree(unsigned int tableAlias, unsigned int tableColumn,
        ColumnIdentity **createdColumnIdentity = NULL);

    /* Given the list of relations taking part in the query and an alias,
     * we return the real name of the relation that has the given alias
     */
    static unsigned int getRealTableName(List *rels, unsigned int alias);

    /* Given the list of tables, the real name of a table and a
     * column name, we return the initial statistics of that column
     */
    static ColumnStatistics *getInitialStats(List *tables,
        unsigned int realTableName, unsigned int columnName);

    /* Compares two column identities
     *
     * - Returns 1 if the first one is greater than the second
     * - Returns -1 if the first one is smaller than the second
     * - Returns 0 if the two column identities are equal
     */
    static int compareColumnIdentities(void *item1, void *item2);

    /* Compares the values of two unsigned integers
     *
     * - Returns 1 if the first one is greater than the second
     * - Returns -1 if the first one is smaller than the second
     * - Returns 0 if the two unsigned integers are equal
     */
    static int compareUnsignedIntegers(void *item1, void *item2);

    /* Prints a column identity */
    static void printColumnIdentity(void *item);

    /* Prints a new line (this is the context that
     * will be printed between column identities)
     */
    static void contextBetweenColumnIdentities();

    /* Frees the allocated memory for a column identity */
    static void deleteColumnIdentity(void *item, void *key);

public:

    /* Constructor */
    QueryOptimizer(List *tables, Query *query);

    /* Destructor */
    ~QueryOptimizer();

    /* Returns a list of predicates in the order that is estimated most optimal */
    List *getOptimalPredicatesOrder() const;

    /* Frees the allocated memory for the result of 'getOptimalPredicatesOrder' */
    void deleteOptimalPredicatesOrder(List *optimalPredicatesOrder) const;

    /* Prints all the column identities taking part in the query */
    void printColumnsOfQuery() const;

};

#endif
