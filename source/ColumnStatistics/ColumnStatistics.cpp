#include <iostream>
#include <climits>
#include "ColumnStatistics.h"

/***************
 * Constructor *
 ***************/

ColumnStatistics::ColumnStatistics( 
    unsigned long long *column,
    unsigned long long size,
    unsigned long long maxBitmapSize)
{
    /* Auxiliary variable (used for counting) */
    unsigned long long i;

    /* We need to keep track of the min & max element value in the column
     *
     * To do that, we will be storing the current minimum & maximum value
     * in two seperate variables. The variable storing the current minimum
     * is initialized with the highest possible value and the variable
     * storing the current maximum is initialized with the lowest possible value
     */
    minElement = ULLONG_MAX; //18446744073709551615U
    maxElement = 0;

    /* We will traverse the column to build our statistic fields */

    for(i = 0; i < size; i++)
    {
        /* We retrieve the element in the current offset */
        unsigned long long currentElement = column[i];

        /* If the current element is smaller than
         * the currently smallest element, the currently
         * smallest element becomes the current element
         */
        if(currentElement < minElement)
            minElement = currentElement;

        /* If the current element is greater than
         * the currently highest element, the currently
         * highest element becomes the current element
         */
        if(currentElement > maxElement)
            maxElement = currentElement;
    }

    /* The number of elements in the column is the given size */
    elementsNum = size;

    /* This is the size of the bitmap that is favoured by the statistics */
    unsigned long long statisticBitmapSize = maxElement - minElement + 1;

    /* If the estimated correct size is smaller than the maximum allowed
     * size for the bitmap, we accept the estimated size and this will
     * be the size of the bitmap. Else, the maximum size of the bitmap
     * will be assigned if the estimated value if greater than the max.
     */
    unsigned long long bitmapSize = (statisticBitmapSize < maxBitmapSize)
        ? statisticBitmapSize : maxBitmapSize;

    /* We create the bitmap that will help us find
     * the amount of distinct values in the column
     */
    Bitmap bitmapForDistincts = Bitmap(bitmapSize);

    /* We initialize the amount of distinct elements to zero */
    distinctElementsNum = 0;

    /* We will traverse the column again to update the bitmap
     * and the amount of distinct elements through the bitmap
     */
    for(i = 0; i < size; i++)
    {
        /* We retrieve the element in the current offset */
        unsigned long long currentElement = column[i];

        /* Now that we retrieved the current element, we will target
         * a position of the bitmap depending on the current element
         * and set the bit of that position to 1 if it has not been
         * set already. The selection of the position in this way is
         * approved by a statistic analysis that has been performed.
         */
        unsigned long long targetPos = (currentElement - minElement) % bitmapSize + 1;

        /* We set the bit at the statistically approved
         * position to 1 if it has not already been set
         */
        if(bitmapForDistincts.isSet(targetPos) == false)
        {
            /* We set the bit at the targeted position to 1 */
            bitmapForDistincts.setBit(targetPos);

            /* We increase the amount of distinct elements by 1 */
            distinctElementsNum++;
        }
    }
}

/*************************
 * Secondary Constructor *
 *************************/

ColumnStatistics::ColumnStatistics(
    unsigned long long minElement,
    unsigned long long maxElement,
    unsigned long long elementsNum,
    unsigned long long distinctElementsNum)
{
    this->minElement = minElement;
    this->maxElement = maxElement;
    this->elementsNum = elementsNum;
    this->distinctElementsNum = distinctElementsNum;
}

/**************
 * Destructor *
 **************/

ColumnStatistics::~ColumnStatistics() {}

/**********************************************************************
 * Getter - Returns the element with the smallest value in the column *
 **********************************************************************/

unsigned long long ColumnStatistics::getMinElement() const
{
    return minElement;
}

/*********************************************************************
 * Getter - Returns the element with the highest value in the column *
 *********************************************************************/

unsigned long long ColumnStatistics::getMaxElement() const
{
    return maxElement;
}

/*********************************************************
 * Getter - Returns the amount of elements in the column *
 *********************************************************/

unsigned long long ColumnStatistics::getElementsNum() const
{
    return elementsNum;
}

/******************************************************************
 * Getter - Returns the amount of distinct elements in the column *
 ******************************************************************/

unsigned long long ColumnStatistics::getDistinctElementsNum() const
{
    return distinctElementsNum;
}

/*******************************************************************
 * Setter - Sets the element with the smallest value in the column *
 *******************************************************************/

void ColumnStatistics::setMinElement(unsigned long long newMinElement)
{
    minElement = newMinElement;
}

/******************************************************************
 * Setter - Sets the element with the highest value in the column *
 ******************************************************************/

void ColumnStatistics::setMaxElement(unsigned long long newMaxElement)
{
    maxElement = newMaxElement;
}

/******************************************************
 * Setter - Sets the amount of elements in the column *
 ******************************************************/

void ColumnStatistics::setElementsNum(unsigned long long newElementsNum)
{
    elementsNum = newElementsNum;
}

/***************************************************************
 * Setter - Sets the amount of distinct elements in the column *
 ***************************************************************/

void ColumnStatistics::setDistinctElementsNum(unsigned long long newDistinctElementsNum)
{
    distinctElementsNum = newDistinctElementsNum;
}

/***************************************
 * Prints the statistics of the column *
 ***************************************/

void ColumnStatistics::print() const
{
    std::cout << "ColumnStatistics{minElement="
        << minElement << ",maxElement=" << maxElement
        << ",elementsNum=" << elementsNum
        << ",distinctElementsNum=" << distinctElementsNum
        << "}";
}
