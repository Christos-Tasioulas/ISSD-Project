#ifndef _COLUMN_STATISTICS_H_
#define _COLUMN_STATISTICS_H_

#include "Bitmap.h"

class ColumnStatistics {

private:

    /* The element with the smallest value in the column */
    unsigned long long minElement;

    /* The element with the highest value in the column */
    unsigned long long maxElement;

    /* The amount of elements in the column */
    unsigned long long elementsNum;

    /* The amount of distinct elements in the column */
    unsigned long long distinctElementsNum;

public:

    /* Constructor */
    ColumnStatistics(unsigned long long *column, unsigned long long size,
        unsigned long long maxBitmapSize);

    /* Secondary Constructor */
    ColumnStatistics(unsigned long long minElement = 0, unsigned long long maxElement = 0,
        unsigned long long elementsNum = 0, unsigned long long distinctElementsNum = 0);

    /* Destructor */
    ~ColumnStatistics();

    /* Getter - Returns the element with the smallest value in the column */
    unsigned long long getMinElement() const;

    /* Getter - Returns the element with the highest value in the column */
    unsigned long long getMaxElement() const;

    /* Getter - Returns the amount of elements in the column */
    unsigned long long getElementsNum() const;

    /* Getter - Returns the amount of distinct elements in the column */
    unsigned long long getDistinctElementsNum() const;

    /* Setter - Sets the element with the smallest value in the column */
    void setMinElement(unsigned long long newMinElement);

    /* Setter - Sets the element with the highest value in the column */
    void setMaxElement(unsigned long long newMaxElement);

    /* Setter - Sets the amount of elements in the column */
    void setElementsNum(unsigned long long newElementsNum);

    /* Setter - Sets the amount of distinct elements in the column */
    void setDistinctElementsNum(unsigned long long newDistinctElementsNum);

    /* Prints the statistics of the column */
    void print() const;

};

#endif
