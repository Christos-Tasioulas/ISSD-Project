#ifndef _ROW_ID_RELATION_H_
#define _ROW_ID_RELATION_H_

#include "RowIdPair.h"

class RowIdRelation {

private:

/* The array of row ID pairs (always in the heap) */
    RowIdPair *rowIdPairs;

/* The number of row ID pairs in the array */
    unsigned int numOfRowIdPairs;

public:

/* Constructor */
    RowIdRelation(RowIdPair *rowIdPairs = NULL,
        unsigned int numOfRowIdPairs = 0);

/* Destructor */
    ~RowIdRelation();

/* Getter - Returns the array of row ID pairs */
    RowIdPair *getRowIdPairs() const;

/* Getter - Returns the number of row ID pairs in the array */
    unsigned int getNumOfRowIdPairs() const;

/* Prints all the row ID pairs of the relation */
	void print(void (*visitRowIdPair)(unsigned int, unsigned int),
		void (*contextBetweenRowIdPairs)() = NULL) const;

};

#endif