#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "PartitionedHashJoin.h"

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

/*************************************
 * Reads the input relational arrays *
 *************************************/

static void readInput(Relation **relR, Relation **relS)
{

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

	RowIdPair *result = partitionedHashJoin(relR, relS);
}
