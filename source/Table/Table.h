#ifndef _TABLE_H_
#define _TABLE_H_

#include "List.h"

class Table {

private:

    unsigned long long **table;
    unsigned long long numTuples;
    unsigned long long numColumns;

public:

    Table(const char *binary_filename);
    ~Table();

    unsigned long long getNumOfTuples() const;
    unsigned long long getNumOfColumns() const;
    unsigned long long **getColumns() const;
    void print() const;
};

#endif
