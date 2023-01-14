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

Table::Table(const char *binary_filename, unsigned int maxBitmapSize)
{
    /* We open the binary input file */

    int fd = open(binary_filename, O_RDONLY);

    /* We examine if the opening was successful */

    if(fd == -1)
    {
        std::cout << "Error opening" << binary_filename << std::endl;
		perror("open");
		return;
    }

    /* We will use a 'struct stat' object to
     * retrieve information for the file
     */
    struct stat fileInfo;

    /* We examine if the 'fstat' system call was successful */

    if(fstat(fd, &fileInfo) == -1)
    {
        std::cout << "Could not retrieve file information" << std::endl;
        perror("fstat");
        return;
    }

    /* We retrieve the size of the file */
    unsigned int fileSize = fileInfo.st_size;

    /* We create a new mapping for the input binary file */
    char *mapping = (char *) mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0u);

    /* We examine if the mapping was successful */

    if(mapping == MAP_FAILED)
    {
        std::cout << "Could not mmap the file " << binary_filename
            << " of size " << fileSize << std::endl;

        perror("mmap");
        return;
    }

    /* An auxiliary pointer that points to the start of the mapping */
    char *pointerToMapping = mapping;

    /* If the size of the file is less than 16 bytes,
     * that means the file does not have a valid header
     */
    if(fileSize < 16)
    {
        std::cout << "Relation file " << binary_filename
            << " does not contain a valid header" << std::endl;

        return;
    }

    /* The first 8 bytes of the header describe the number of rows */
    this->numTuples = *((unsigned long long *) pointerToMapping);

    /* We proceed to the next data of the header */
    pointerToMapping += sizeof(unsigned long long);

    /* The next 8 bytes describe the number of columns */
    this->numColumns = *((unsigned long long *) pointerToMapping);

    /* Now we proceed to the data of the table */
    pointerToMapping += sizeof(unsigned long long);

    /* We allocate memory for the table */
    table = new unsigned long long *[numColumns];

    /* Auxiliary variable used for counting */
    unsigned long long i;

    /* We allocate memory for the rows of the table */

    for(i = 0; i < numColumns; i++)
        table[i] = new unsigned long long[numTuples];

    /* Now we will fill the table with the rest data from the file */

    for(i = 0; i < numColumns; i++)
    {
        /* We copy the next column from the file to the table */
        memcpy(table[i], pointerToMapping, numTuples * sizeof(unsigned long long));

        /* We proceed to the base address of the next column */
        pointerToMapping += numTuples * sizeof(unsigned long long);
    }

    /* We delete the mapping of the binary file */

    int mapping_deletion = munmap(mapping, fileSize);

    /* We examine if the deletion was successful */

    if(mapping_deletion != 0)
    {
        std::cout << "Error at freeing the mapping of " << binary_filename << std::endl;
		perror("munmap");
    }

    /* Finally, we close the opened binary file */

	int close_result = close(fd);

	/* We examine if the closing of the file was successful */

	if(close_result == -1)
	{
		std::cout << "Error closing " << binary_filename << std::endl;
		perror("close");
	}

    /* We initialize the statistics structure of each column of the table */

    columnStatistics = new ColumnStatistics *[numColumns];

    for(i = 0; i < numColumns; i++)
        columnStatistics[i] = new ColumnStatistics(table[i], numTuples, maxBitmapSize);
}

/**************
 * Destructor *
 **************/

Table::~Table()
{
    /* Auxiliary variable used for counting */
    unsigned long long i;

    /* We delete the column statistics of each column */

    for(i = 0; i < numColumns; i++)
        delete columnStatistics[i];

    /* We delete the array of column statistics for each column */
    delete[] columnStatistics;

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

/***********************************************************
 * Getter - Returns the array of statistics for each colum *
 ***********************************************************/

ColumnStatistics **Table::getColumnStatistics() const
{
    return columnStatistics;
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
        std::cout << "[ ";

        /* We print the contents of that column */

        for(j = 0; j < numTuples; j++)
            std::cout << table[i][j] << " ";

        /* We announce the end of the column with a ']' */
        std::cout << "] ";
    }

    /* Next, we print the number of
    * rows and columns of the array
     */
    std::cout << "(" << numTuples << " rows, "
        << numColumns << " columns)" << std::endl;

    /* Finally, we print the statistics of each column */

    for(i = 0; i < numColumns; i++)
    {
        std::cout << "Column #" << i << " - ";
        columnStatistics[i]->print();
    }
}
