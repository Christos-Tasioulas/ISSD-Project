#ifndef _QUERY_OPTIMIZER_H_
#define _QUERY_OPTIMIZER_H_

#include "Table.h"
#include "ColumnSubset.h"
#include "InvertedIndex.h"
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

    /* A list of all the column identities which have had their
     * stats renewed due to being affected by initial filters
     */
    List *colIdsWithRenewedStats;

    /* An inverted index storing all the column identities
     * taking part in the query by their real array name
     */
    InvertedIndex *identitiesByRealTable;

    /* Attempts to insert a relation with the given attributes to
     * the B-Tree. Fails if the relation already exists in the tree
     */
    void insertRelInTree(unsigned int tableAlias, unsigned int tableColumn,
        ColumnIdentity **createdColumnIdentity = NULL);

    /* Inserts a column identity to the inverted index of the class.
     * The item will be inserted in the group of items with the same
     * table alias.
     */
    void insertRelInInvertedIndex(ColumnIdentity *colId);

    /* Makes changes to the initial stats of the given column identity
     * according to the given filters. This will also affect the column
     * identities that belong to the same table as the given one.
     * Consequently, those will have their stats changed as well.
     */
    void filterColumnIdentityAndTheRestInSameTable(ColumnIdentity *colId,
        char filterOperator, unsigned int filterValue);

    /* Applies the given filter to the given targeted column identity */
    void updateStatsOfTargetedColumnByFilter(ColumnIdentity *colId,
        char filterOperator, unsigned int filterValue);

    /* Applies the given filter to the given non-targeted column identity */
    void updateStatsOfNonTargetedColumnByFilter(ColumnIdentity *colId,
        char filterOperator, unsigned int filterValue,
        unsigned int old_fA, unsigned int new_fA);

    /* Given the list of relations taking part in the query and an alias,
     * we return the real name of the relation that has the given alias
     */
    static unsigned int getRealTableName(List *rels, unsigned int alias);

    /* Given the list of tables, the real name of a table and a
     * column name, we return the initial statistics of that column
     */
    static ColumnStatistics *getInitialStats(List *tables,
        unsigned int realTableName, unsigned int columnName);

    /* Places all the column identities of a structure in the static list */
    static void placeColIdsInList(void *item, void *key);

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

    /* Compares two column subsets which both consist of 1 column ID
     * The operation uses 'compareColumnIdentities' to compare the
     * single column identities and returns the result of the comparison
     */
    static int compareSingleSetsByColumnIds(void *item1, void *item2);

    /* Prints a column identity */
    static void printColumnIdentity(void *item);

    /* Prints an unsigned integer */
    static void printUnsignedInteger(void *item);

    /* Prints a predicate */
    static void printPredicate(void *item);

    /* Prints only one identity in the form the inverted index requires */
    static void printIdentityAndIdentity(void *item, void *itemKey);

    /* Prints a new line (this is the context that
     * will be printed between column identities)
     */
    static void contextBetweenColumnIdentities();

    /* Prints a comma & space (this is the context
     * that will be printed between two predicates)
     */
    static void contextBetweenPredicates();

    /* Frees the allocated memory for a column identity */
    static void deleteColumnIdentity(void *item, void *key);

    /* Frees the allocated memory for an unsigned integer */
    static void deleteUnsignedInteger(void *redBlackTreeItem, void *uInteger);

    /* Frees the allocated memory for the column statistics of an identity */
    static void deleteStatsFromColumnIdentity(void *item);

public:

    /* Constructor */
    QueryOptimizer(List *tables, Query *query);

    /* Destructor */
    ~QueryOptimizer();

    /* Returns a list of predicates in the order that is estimated most optimal */
    List *getOptimalPredicatesOrder();

    /* Frees the allocated memory for the result of 'getOptimalPredicatesOrder' */
    void deleteOptimalPredicatesOrder(List *optimalPredicatesOrder) const;

    /* Prints all the column identities taking part in the query */
    void printColumnsOfQuery() const;

    /* Prints all the column identities grouped by their table name */
    void printColumnsGroupedByTableName() const;

    /* Prints all the filter predicates and then
     * all the join predicates of the query
     */
    void printFilterAndJoinPredicates() const;

};

#endif
