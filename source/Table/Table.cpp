#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "Table.h"

Table::Table(const char *binary_filename)
{
    int fd = open(binary_filename, O_RDONLY);
    if (fd == -1) {
        std::cerr << "cannot open " << binary_filename << std::endl;
        throw;
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

Table::~Table()
{
    unsigned long long i;

    for(i = 0; i < numColumns; i++)
        delete[] table[i];

    delete[] table;
}

unsigned long long Table::getNumOfTuples() const
{
    return this->numTuples;
}

unsigned long long Table::getNumOfColumns() const
{
    return this->numColumns;
}

unsigned long long **Table::getColumns() const
{
    return this->table;
}

void Table::print() const
{
    unsigned long long i, j;

    for(i = 0; i < numColumns; i++)
    {
        std::cout << "[ " << std::endl;

        for(j = 0; j < numTuples; j++)
            std::cout << table[i][j] << " ";

        std::cout << "]" << std::endl;
    }
}
