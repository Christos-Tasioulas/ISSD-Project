#include <iostream>
#include "RowIdRelation.h"

/***************
 * Constructor *
 ***************/

RowIdRelation::RowIdRelation(RowIdPair *rowIdPairs,
    unsigned int numOfRowIdPairs)
{
    this->rowIdPairs = rowIdPairs;
    this->numOfRowIdPairs = numOfRowIdPairs;
}

/**************
 * Destructor *
 **************/

RowIdRelation::~RowIdRelation() {}

/**********************************************
 * Getter - Returns the array of row ID pairs *
 **********************************************/

RowIdPair *RowIdRelation::getRowIdPairs() const
{
    return rowIdPairs;
}

/************************************************************
 * Getter - Returns the number of row ID pairs in the array *
 ************************************************************/

unsigned int RowIdRelation::getNumOfRowIdPairs() const
{
    return numOfRowIdPairs;
}

/***********************************************
 * Prints all the row ID pairs of the relation *
 ***********************************************/

void RowIdRelation::print(void (*visitRowIdPair)(unsigned int, unsigned int),
    void (*contextBetweenRowIdPairs)()) const
{
    /* Auxiliary variable used for counting */
    unsigned int i;

    /* If a no-null 'contextBetweenRowIdPairs' operation has been
     * given to print context between the pairs, we call it now
     */
    if(contextBetweenRowIdPairs != NULL)
        contextBetweenRowIdPairs();

    /* Here we will print all the pairs of the relation */

    for(i = 0; i < numOfRowIdPairs; i++)
    {
        /* We print the next row ID pair */
        rowIdPairs[i].print(visitRowIdPair);

        /* If a no-null 'contextBetweenRowIdPairs' operation has been
         * given to print context between the pairs, we call it now
         */
        if(contextBetweenRowIdPairs != NULL)
            contextBetweenRowIdPairs();
    }
}
