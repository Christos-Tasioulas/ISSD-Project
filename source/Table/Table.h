#ifndef _TABLE_H_
#define _TABLE_H_

#include "List.h"
#include "ColumnStatistics.h"

/* A structure that will be storing the contents of all
 * rows and columns of a relation. Storing "by columns"
 * is approached in this class.
 */
class Table {

private:

    /* The table with all the data stored "by columns" */
    unsigned long long **table;

    /* The amount of rows of the table */
    unsigned long long numTuples;

    /* The amount of columns of the table */
    unsigned long long numColumns;

    /* Statistic data stored for each column of the table */
    ColumnStatistics **columnStatistics;

public:

    /* Constructor & Destructor */
    Table(const char *binary_filename, unsigned int maxBitmapSize = 50000000);
    ~Table();

    /* Getter - Returns the number of rows of the table */
    unsigned long long getNumOfTuples() const;

    /* Getter - Returns the number of columns of the table */
    unsigned long long getNumOfColumns() const;

    /* Getter - Returns a pointer to the table itself */
    unsigned long long **getTable() const;

    /* Getter - Returns the array of statistics for each column */
    ColumnStatistics **getColumnStatistics() const;

    /* Prints the information stored in the table */
    void print() const;
};

#endif
