#ifndef _COLUMN_SUBSET_H_
#define _COLUMN_SUBSET_H_

#include "ColumnIdentity.h"

class ColumnSubset {

private:

    /* A column subset is a set of column identities */
    List *columnIdentities;

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
        unsigned int minElement,
        unsigned int maxElement,
        unsigned int elementsNum,
        unsigned int distinctElementsNum
    );
    
    // ColumnSubset(ColumnSubset *cs1, ColumnSubset *cs1);
    // ColumnSubset(List* columns);

    /* Destructor */
    ~ColumnSubset();

    /* Getter - Returns the list of column identities */
    List *getColumnsIdentities() const;

    /* Getter - Returns the current stats of the subset */
    ColumnStatistics *getSubsetStats() const;

    /* Getter - Returns the total cost to reach
     * the intermediate result of this subset
     */
    unsigned long long getTotalCost() const;

    /* Adds a new column identity to the subset */
    void insertColumnIdentity(ColumnIdentity *colId);

    /* Returns the 'pos'-th identity of the set */
    ColumnIdentity *getColumnIdentityInPos(unsigned int pos) const;

    /* Changes the stats of the subset */
    void changeStats(
        unsigned int newMinElement,
        unsigned int newMaxElement,
        unsigned int newElementsNum,
        unsigned int newDistinctElementsNum
    );

};

#endif
