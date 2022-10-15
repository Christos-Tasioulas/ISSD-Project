#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "PartitionedHashJoin.h"

static unsigned int n = 3;

/*****************************************
 * Used to print the contents of a tuple *
 *****************************************/

static void printTuple(void *data, unsigned int rowId)
{
    int dataValue = *((int *) data);

    printf("|%11u|%11d|\n", rowId, dataValue);
}

/*********************************************
 * Used to print a pair of unsigned integers *
 *********************************************/

static void printUnsignedPair(unsigned int l, unsigned int r)
{
    printf("|%11u|%11u|\n", l, r);
}

/***********************************************************************
 * Used as context that seperates the printed tuples or unsigned pairs *
 ***********************************************************************/

static void lineContext()
{
    std::cout << "+-----------+-----------+" << std::endl;
}

/*********************************************************************
 * Returns the size of the LVL2 cache or -1 in case an error occured *
 *********************************************************************/

static long get_Lvl2_Cache_Size()
{
    /* We use 'sysconf' with the '_SC_LEVEL2_CACHE_SIZE'
     * parameter to read the size of the level-2 cache
     */
    long lvl_2_cache_size = sysconf(_SC_LEVEL2_CACHE_SIZE);

    /* If an error has occured and the cache size cannot
     * be determined, we print an informative message on
     * the screen about the reason why the error occured
     */
    if(lvl_2_cache_size == -1)
    {
        std::cout << "Could not determine the size of LVL2 Cache" << std::endl;
        perror("Reason");
    }

    /* We return the final result */
    return lvl_2_cache_size;
}

/**********
 * Hash 1 *
 **********/

unsigned int hash_1(int integer)
{
    unsigned int result = 0;
    unsigned int i = 0;
    int mask = 1;

    for(i = 0; i < n; i++)
    {
        if((integer & mask) == mask)
            result += mask;

        mask <<= 1;
    }

    return result;
}

/*************************************
 * Reads the input relational arrays *
 *************************************/

static void readInput(Relation **relR, Relation **relS)
{
    Tuple *tuples_array_1 = new Tuple[3];
    Tuple *tuples_array_2 = new Tuple[3];

    tuples_array_1[0] = Tuple(new int(1), 1);
    tuples_array_1[1] = Tuple(new int(5), 2);
    tuples_array_1[2] = Tuple(new int(8), 3);

    tuples_array_2[0] = Tuple(new int(4), 1);
    tuples_array_2[1] = Tuple(new int(9), 2);
    tuples_array_2[2] = Tuple(new int(5), 3);

    (*relR) = new Relation(tuples_array_1, 3);
    (*relS) = new Relation(tuples_array_2, 3);
}

/************************************************
 * Executes the Partitioned Hash Join Algorithm *
 ************************************************/

static RowIdPair *partitionedHashJoin(Relation *relR, Relation *relS)
{
    long lvl2CacheSize = get_Lvl2_Cache_Size();

    if(lvl2CacheSize != -1)
    {
        std::cout << "LVL2 Cache Size: " << lvl2CacheSize << std::endl;
    }

    /* This is the size (in bytes) of relation 'R' */
    unsigned int relR_size = relR->getSize(sizeof(int));

    /* This is the size (in bytes) of relation 'S' */
    unsigned int relS_size = relS->getSize(sizeof(int));

    unsigned int i, R_numOfTuples, S_numOfTuples;
    R_numOfTuples = relR->getNumOfTuples();
    S_numOfTuples = relS->getNumOfTuples();
    Tuple *R_table = relR->getTuples();
    Tuple *S_table = relS->getTuples();

    unsigned int R_histogramSize = 1;
    R_histogramSize <<= n;
    unsigned int R_histogram[R_histogramSize];

    for(i = 0; i < R_histogramSize; i++)
    {
        R_histogram[i] = 0;
    }

    for(i = 0; i < R_numOfTuples; i++)
    {
        int currentItem = *((int *) R_table[i].getItem());
        unsigned int hash_value = hash_1(currentItem);
        R_histogram[hash_value]++;
        std::cout << "Hash value of " << currentItem << ": " << hash_value << std::endl;
    }

    unsigned int S_histogramSize = 1;
    S_histogramSize <<= n;
    unsigned int S_histogram[S_histogramSize];

    for(i = 0; i < S_histogramSize; i++)
    {
        S_histogram[i] = 0;
    }

    std::cout << "-----------------------------" << std::endl;

    for(i = 0; i < S_numOfTuples; i++)
    {
        int currentItem = *((int *) S_table[i].getItem());
        unsigned int hash_value = hash_1(currentItem);
        S_histogram[hash_value]++;
        std::cout << "Hash value of " << currentItem << ": " << hash_value << std::endl;
    }

    std::cout << "-----------------------------" << std::endl;

    for(i = 0; i < R_histogramSize; i++)
    {
        std::cout << "R_histogram[" << i << "] = " << R_histogram[i] << std::endl;
    }

    std::cout << "-----------------------------" << std::endl;

    for(i = 0; i < S_histogramSize; i++)
    {
        std::cout << "S_histogram[" << i << "] = " << S_histogram[i] << std::endl;
    }

	return NULL;
}

/**************************************************
 * Reads the input relational arrays and executes *
 * the Partitioned Hash Join Algorithm to perform *
 *        the Join operation between them         *
 **************************************************/

void execute_PHJ()
{
    Relation *relR = NULL, *relS = NULL;
    readInput(&relR, &relS);


    std::cout << "Relations in the beginning\n" << std::endl;
    std::cout << "            R" << std::endl;
    relR->print(printTuple, lineContext);
    std::cout << std::endl;
    std::cout << "            S" << std::endl;
    relS->print(printTuple, lineContext);
    std::cout << std::endl;






	RowIdPair *result = partitionedHashJoin(relR, relS);






    delete ((int *) (relR->getTuples()[0]).getItem());
    delete ((int *) (relR->getTuples()[1]).getItem());
    delete ((int *) (relR->getTuples()[2]).getItem());
    delete ((int *) (relS->getTuples()[0]).getItem());
    delete ((int *) (relS->getTuples()[1]).getItem());
    delete ((int *) (relS->getTuples()[2]).getItem());

    delete[] relR->getTuples();
    delete[] relS->getTuples();

    delete relR;
    delete relS;
}
