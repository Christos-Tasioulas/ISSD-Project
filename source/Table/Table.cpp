#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "Table.h"

/***************
 * Constructor *
 ***************/

Table::Table(const char *binary_filename)
{
    int fd = open(binary_filename, O_RDONLY);

    if(fd == -1)
    {
        std::cout << "Error opening" << binary_filename << std::endl;
		perror("open");
		return;
    }

    // Obtain file size
    struct stat sb;
    if (fstat(fd,&sb)==-1)
    std::cerr << "fstat\n";

    auto length=sb.st_size;

    char *addr=static_cast<char*>(mmap(nullptr,length,PROT_READ,MAP_PRIVATE,fd,0u));
    if(addr == MAP_FAILED)
    {
        std::cerr << "cannot mmap " << binary_filename << " of length " << length << std::endl;
        throw;
    }

    if(length < 16)
    {
        std::cerr << "relation file " << binary_filename << " does not contain a valid header" << std::endl;
        throw;
    }

    this->numTuples=*reinterpret_cast<unsigned long long *>(addr);
    addr+=sizeof(numTuples);
    this->numColumns=*reinterpret_cast<size_t*>(addr);
    addr+=sizeof(size_t);

    table = new unsigned long long *[numColumns];
    unsigned long long i;

    for(i = 0; i < numColumns; i++)
        table[i] = new unsigned long long[numTuples];

    for(unsigned long long i = 0; i < numColumns; i++)
    {
        memcpy(table[i], addr, numTuples * sizeof(unsigned long long));
        addr += numTuples * sizeof(unsigned long long);
    }
}

/**************
 * Destructor *
 **************/

Table::~Table()
{
    /* Auxiliary variable used for counting */
    unsigned long long i;

    /* We delete all the contents of the table */

    for(i = 0; i < numColumns; i++)
        delete[] table[i];

    /* We delete the table itself */
    delete[] table;
}

/****************************************************
 * Getter - Returns the number of rows of the table *
 ****************************************************/

unsigned long long Table::getNumOfTuples() const
{
    return this->numTuples;
}

/*******************************************************
 * Getter - Returns the number of columns of the table *
 *******************************************************/

unsigned long long Table::getNumOfColumns() const
{
    return this->numColumns;
}

/**************************************************
 * Getter - Returns a pointer to the table itself *
 **************************************************/

unsigned long long **Table::getTable() const
{
    return this->table;
}

/**********************************************
 * Prints the information stored in the table *
 **********************************************/

void Table::print() const
{
    /* Auxiliary variables used for counting */
    unsigned long long i, j;

    /* For each column of the table we do the following */

    for(i = 0; i < numColumns; i++)
    {
        /* We announce the start of the column with a '[' */
        std::cout << "[ " << std::endl;

        /* We print the contents of that column */

        for(j = 0; j < numTuples; j++)
            std::cout << table[i][j] << " ";

        /* We announce the end of the column with a ']' */
        std::cout << "]" << std::endl;
    }
}
