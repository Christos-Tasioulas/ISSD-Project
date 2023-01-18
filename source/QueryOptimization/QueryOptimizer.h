#ifndef _QUERY_OPTIMIZER_H_
#define _QUERY_OPTIMIZER_H_

#include "Table.h"
#include "ColumnSubset.h"
#include "InvertedIndex.h"
#include "BinaryHeap.h"
#include "B_Tree.h"

class QueryOptimizer {

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
     * taking part in the query grouped by their real array name
     */
    InvertedIndex *identitiesByRealTable;

    /* An inverted index with groups of binary heaps,
     * each containing the a column subset in every node
     */
    InvertedIndex *subsetsTree;

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
    void filterColumnIdentityAndTheRestInSameTable(
        ColumnIdentity *colId,
        char filterOperator,
        unsigned int filterValue);

    /* Applies the given filter to the given targeted column identity */
    void updateStatsOfTargetedColumnByFilter(
        ColumnIdentity *colId,
        char filterOperator,
        unsigned int filterValue);

    /* Applies the given filter to the given non-targeted column identity */
    void updateStatsOfNonTargetedColumnByFilter(
        ColumnIdentity *colId,
        char filterOperator,
        unsigned int filterValue,
        unsigned int old_fA,
        unsigned int new_fA);

    /* Estimates the new statistics after a join between the two given columns.
     * The indirectly-returned stats must be deleted with 'delete' after use.
     */
    void updateStatsOfColumnsByJoin(
        ColumnIdentity *leftColId,
        ColumnIdentity *rightColId,
        PredicatesParser *connectingPredicate,
        ColumnStatistics **resultStatsAfterJoin
    );

    /* Estimates the new statistics after a join between the given join tree
     * and the given column identity. The indirectly-returned stats must be
     * deleted with 'delete' after use.
     */
    void updateStatsOfJoinTreeAndColumnByJoin(
        ColumnSubset *joinTree,
        ColumnIdentity *colId,
        PredicatesParser *connectingPredicate,
        ColumnStatistics **resultStatsAfterJoin
    );

    /* Appends to the 'result' all the join
     * predicates in the most optimal order
     */
    void getOptimalJoinsOrder(List *result);

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

    /* Used to traverse a group of Binary Heaps of subsets and places the
     * subset in the root of each binary heap in a static helper list
     */
    static void placeSubsetsInList(void *item, void *key);

    /* Compares two lists of predicates and returns 'true' if the two
     * lists have no common predicate, else even if 1 predicate of one
     * list belongs to the other list as well, 'false' is returned
     */
    static bool areForeignSets(List *predsList_1, List *predsList_2);

    /* Places all the duplicate join predicates that exist in
     * the 'joins' list at the end of the given 'result' list
     */
    static void placeDuplicatesAtEnd(List *result, List *joins);

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

    /* Compares two column subsets. By this function, the column
     * subsets are considered equal if they have the same amount
     * of column identities in them and the same column identities
     * themselves. In other words, two subsets are considered
     * "equal by combination" if their columns represent the same
     * combination of columns, no matter their order in the subset.
     *
     * Returns 0 if the subsets are equal by the above rule.
     * Returns 1 if 'item1' is a greater subset than 'item2'.
     * Returns -1 if 'item2' is a greater subset than 'item1'.
     */
    static int compareSubsetsByCombination(void *item1, void *item2);

    /* Compares two column subsets only by their cost
     *
     * Returns -1 if the cost of 'item1' is lower
     * Returns 1 if the cost of 'item1' is greater
     * Returns 0 if the costs of the two subsets are equal
     */
    static int compareSubsetsByCost(void *item1, void *item2);

    /* Prints a column identity */
    static void printColumnIdentity(void *item);

    /* Prints an unsigned integer */
    static void printUnsignedInteger(void *item);

    /* Prints a predicate */
    static void printPredicate(void *item);

    /* Prints only one identity in the form the inverted index requires */
    static void printIdentityAndIdentity(void *item, void *itemKey);

    /* Prints only the ID of a column identity */
    static void printIdOfColumnIdentity(void *item);

    /* Prints only the column identities of a column subset */
    static void printColumnsOfSubset(void *item);

    /* Prints the ID of a group in the inverted index of subsets */
    static void printGroup(void *indexKey);

    /* Prints all the subsets of a Binary Heap of subsets */
    static void printBinaryHeapOfSubsets(void *item, void *key);

    /* Prints a new line (this is the context that
     * will be printed between column identities)
     */
    static void contextBetweenColumnIdentities();

    /* Prints a comma & space (this is the context
     * that will be printed between two predicates)
     */
    static void contextBetweenPredicates();

    /* Prints beautiful context between joined identities */
    static void contextBetweenJoinedIdentities();

    /* Prints some context between two column subsets */
    static void contextBetweenSubsets();

    /* Frees the allocated memory for a column identity */
    static void deleteColumnIdentity(void *item, void *key);

    /* Frees the allocated memory for an unsigned integer */
    static void deleteUnsignedInteger(void *item, void *uInteger);

    /* Frees the allocated memory for the column statistics of an identity */
    static void deleteStatsFromColumnIdentity(void *item);

    /* Frees the allocated memory for a column subset */
    static void deleteColumnSubset(void *item, void *key);

    /* Frees the allocated memory for the heaps and the subsets stored in them */
    static void deleteHeapsOfSubsets(void *item, void *key);

    /* Frees the allocated memory for everything inside the tree of subsets */
    static void deleteInvertedIndexInformation(void *item, void *key);

};

#endif
