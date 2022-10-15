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

/***************
 * Constructor *
 ***************/

PartitionedHashJoin::PartitionedHashJoin(char *input_file, char *config_file)
{
    Tuple *tuples_array_1 = new Tuple[3];
    Tuple *tuples_array_2 = new Tuple[3];

    tuples_array_1[0] = Tuple(new int(1), 1);
    tuples_array_1[1] = Tuple(new int(5), 2);
    tuples_array_1[2] = Tuple(new int(8), 3);

    tuples_array_2[0] = Tuple(new int(4), 1);
    tuples_array_2[1] = Tuple(new int(9), 2);
    tuples_array_2[2] = Tuple(new int(5), 3);

    relR = new Relation(tuples_array_1, 3);
    relS = new Relation(tuples_array_2, 3);

    bitsNumForHashing = 3;
}

/**************
 * Destructor *
 **************/

PartitionedHashJoin::~PartitionedHashJoin()
{
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

/************************************************
 * Getter - Returns the relational array 'relR' *
 ************************************************/

Relation *PartitionedHashJoin::getRelR() const
{
    return relR;
}

/************************************************
 * Getter - Returns the relational array 'relS' *
 ************************************************/

Relation *PartitionedHashJoin::getRelS() const
{
    return relS;
}

/************************************************************
 * Getter - Returns the number of included bits for hashing *
 ************************************************************/

unsigned int PartitionedHashJoin::getBitsNumForHashing() const
{
    return bitsNumForHashing;
}

/************************************************
 * Hashes a given integer into the value of its *
 *      rightmost 'bitsNumForHashing' bits      *
 ************************************************/

unsigned int PartitionedHashJoin::bitReductionHash(int integer) const
{
    /* This is the final result of hashing */
    unsigned int result = 0;

    /* Helper variable used for counting */
    unsigned int i = 0;

    /* A mask that will help detecting whether a specific
     * bit of the given integer has value 1 or 0
     */
    int mask = 1;

    /* Starting from the rightmost bit of the given
     * integer, we will examine whether the bit under
     * examination has the value 1 or 0. We will do
     * this for the rightmost 'bitsNumForHashing' bits.
     */
    for(i = 0; i < bitsNumForHashing; i++)
    {
        /* If the result of the bitwise 'AND' between
         * the integer and the mask is the mask itself,
         * then the value of the current bit is 1.
         *
         * If the value of the bit is 1, we have to add
         * the value of the mask to the result so as to
         * turn that bit of the result to 1.
         */
        if((integer & mask) == mask)
            result += mask;

        /* We multiply the mask by 2, so as to turn the
         * next bit of the mask from 0 to 1 and set the
         * current one to 0. We will use this new value
         * of the mask to check the value of the next bit.
         */
        mask <<= 1;
    }

    /* We return the hashing result */
    return result;
}

/************************************************
 * Executes the Partitioned Hash Join Algorithm *
 ************************************************/

RowIdPair *PartitionedHashJoin::executeJoin()
{
    std::cout << "Relations in the beginning\n" << std::endl;
    std::cout << "            R" << std::endl;
    relR->print(printTuple, lineContext);
    std::cout << std::endl;
    std::cout << "            S" << std::endl;
    relS->print(printTuple, lineContext);
    std::cout << std::endl;

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
    R_histogramSize <<= bitsNumForHashing;
    unsigned int R_histogram[R_histogramSize];

    for(i = 0; i < R_histogramSize; i++)
    {
        R_histogram[i] = 0;
    }

    for(i = 0; i < R_numOfTuples; i++)
    {
        int currentItem = *((int *) R_table[i].getItem());
        unsigned int hash_value = bitReductionHash(currentItem);
        R_histogram[hash_value]++;
        std::cout << "Hash value of " << currentItem << ": " << hash_value << std::endl;
    }

    unsigned int S_histogramSize = 1;
    S_histogramSize <<= bitsNumForHashing;
    unsigned int S_histogram[S_histogramSize];

    for(i = 0; i < S_histogramSize; i++)
    {
        S_histogram[i] = 0;
    }

    std::cout << "-----------------------------" << std::endl;

    for(i = 0; i < S_numOfTuples; i++)
    {
        int currentItem = *((int *) S_table[i].getItem());
        unsigned int hash_value = bitReductionHash(currentItem);
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
