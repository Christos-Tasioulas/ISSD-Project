#ifndef _COLUMN_SUBSET_H_
#define _COLUMN_SUBSET_H_

#include "ColumnIdentity.h"

class ColumnSubset {

private:

    /* A column subset is a set of column identities */
    List *columnIdentities;

    /* The order of predicates that depicts the above order of columns */
    List *predicatesOrder;

    /* The statistics of the intermediate array when all
     * the column identities of the subset are joined
     */
    ColumnStatistics *subsetStats;

    /* The number of tuples of the intermediate result */
    unsigned long long totalCost;

public:

    /* Constructor */
    ColumnSubset(
        ColumnIdentity *firstColId,
        ColumnIdentity *neighborOfFirst,
        PredicatesParser *predBetweenTheTwo,
        ColumnStatistics *subsetStats
    );

    /* Constructor */
    ColumnSubset(
        ColumnSubset *existingSubset,
        ColumnIdentity *nextColId,
        PredicatesParser *predBetweenLastAndNext,
        ColumnStatistics *newStats
    );

    /* Destructor */
    ~ColumnSubset();

    /* Getter - Returns the list of column identities */
    List *getColumnsIdentities() const;

    /* Getter - Returns the order of predicates */
    List *getPredicatesOrder() const;

    /* Getter - Returns the current stats of the subset */
    ColumnStatistics *getSubsetStats() const;

    /* Getter - Returns the total cost to reach
     * the intermediate result of this subset
     */
    unsigned long long getTotalCost() const;

    /* Adds a new column identity to the subset */
    void insertColumnIdentity(ColumnIdentity *colId);

    /* Returns the most newly inserted column of the subset */
    ColumnIdentity *getLastColumn() const;

    /* Returns the 'pos'-th identity of the set */
    ColumnIdentity *getColumnIdentityInPos(unsigned int pos) const;

    /* Updates the given 'neighborsList' with all the available column identities
     * that can be joined to the subset and the 'neighborPredsList' with the
     * corresponding join predicates that connect these columns with the set
     */
    void getNeighbors(List **neighborsList, List **neighborPredsList) const;

    /* Frees the allocated memory for the lists built by 'getNeighbors' */
    static void freeNeighbors(List *neighborsList, List *neighborPredsList);

    /* Returns 'true' if the given column identity exists in the subset */
    bool exists(ColumnIdentity *colId) const;

    /* Changes the stats of the subset */
    void changeStats(
        unsigned int newMinElement,
        unsigned int newMaxElement,
        unsigned int newElementsNum,
        unsigned int newDistinctElementsNum
    );

};

#endif
