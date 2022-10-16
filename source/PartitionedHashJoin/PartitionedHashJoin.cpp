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

    tuples_array_2[0] = Tuple(new int(1), 1);
    tuples_array_2[1] = Tuple(new int(9), 2);
    tuples_array_2[2] = Tuple(new int(5), 3);

    relR = new Relation(tuples_array_1, 3);
    relS = new Relation(tuples_array_2, 3);

    bitsNumForHashing = 1;
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

/****************************************************************
 * Displays in the screen the contents of the initial relations *
 ****************************************************************/

void PartitionedHashJoin::displayInitialRelations() const
{
    /* We print an informative message about the initial relations */
    std::cout << "\nRelations in the beginning\n" << std::endl;

    /* We print the contents of the relational array 'relR' */
    std::cout << "\t    R" << std::endl;
    relR->print(printTuple, lineContext);
    std::cout << std::endl;

    /* We print the contents of the relational array 'relS' */
    std::cout << "\t    S" << std::endl;
    relS->print(printTuple, lineContext);
    std::cout << std::endl;
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

RowIdRelation *PartitionedHashJoin::executeJoin()
{
    /* We print the contents of the initial relations */
    displayInitialRelations();

    /* We retrieve the size of the level-2 cache */
    long lvl2CacheSize = get_Lvl2_Cache_Size();

    /* If the cache size could not be retrieved,
     * we cancel the join operation and return 'NULL'
     */
    if(lvl2CacheSize == -1)
        return NULL;

    /* This is the size (in bytes) of relation 'relR' */
    unsigned int relR_size = relR->getSize(sizeof(int));

    /* This is the size (in bytes) of relation 'relS' */
    unsigned int relS_size = relS->getSize(sizeof(int));

    /* Variables we will need later in the algorithm */
    unsigned int i, R_numOfTuples, S_numOfTuples;

    /* This is the number of tuples of the relation 'relR' */
    R_numOfTuples = relR->getNumOfTuples();

    /* This is the number of tuples of the relation 'relS' */
    S_numOfTuples = relS->getNumOfTuples();

    /* This is the array itself of the relation 'relR' */
    Tuple *R_table = relR->getTuples();

    /* This is the array itself of the relation 'relS' */
    Tuple *S_table = relS->getTuples();

    /* We will build the histogram of the relation 'relR'
     *
     * First we initialize the histogram size to 1
     */
    unsigned int R_histogramSize = 1;

    /* Then we shift the size as many position to the left
     * as the amount of bits that were used to hash the
     * elements of the relation, which is 'bitsNumForHashing'
     *
     * This will produce the power (2 ^ 'bitsNumForHashing'),
     * which is the desired size for the histogram of 'relR'
     */
    R_histogramSize <<= bitsNumForHashing;

    /* We define the histogram of 'relR' */
    unsigned int *R_histogram = new unsigned int[R_histogramSize];

    /* We initialize every element of the histogram to zero */

    for(i = 0; i < R_histogramSize; i++)
        R_histogram[i] = 0;

    /* Then we fill the histogram with the desired content
     *
     * The value of the element of index 'i' in the histogram
     * indicates the amount of elements of the relational
     * array 'relR' that were hashed to the bucket 'i'
     *
     * For each element of the array we do the following
     */
    for(i = 0; i < R_numOfTuples; i++)
    {
        /* We retrieve the value of the current element */
        int currentItem = *((int *) R_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        /* The value of the histogram's element of the index
         * that matches the hash value is now increased by 1,
         * since one more element of the relational array was
         * hashed to this value
         */
        R_histogram[hash_value]++;
    }

    /* We will build the histogram of the relation 'relS'
     *
     * First we initialize the histogram size to 1
     */
    unsigned int S_histogramSize = 1;

    /* Then we shift the size as many position to the left
     * as the amount of bits that were used to hash the
     * elements of the relation, which is 'bitsNumForHashing'
     *
     * This will produce the power (2 ^ 'bitsNumForHashing'),
     * which is the desired size for the histogram of 'relS'
     */
    S_histogramSize <<= bitsNumForHashing;

    /* We define the histogram of 'relS' */
    unsigned int *S_histogram = new unsigned int[S_histogramSize];

    /* We initialize every element of the histogram to zero */

    for(i = 0; i < S_histogramSize; i++)
        S_histogram[i] = 0;

    /* Then we fill the histogram with the desired content
     *
     * The value of the element of index 'i' in the histogram
     * indicates the amount of elements of the relational
     * array 'relS' that were hashed to the bucket 'i'
     *
     * For each element of the array we do the following
     */
    for(i = 0; i < S_numOfTuples; i++)
    {
        /* We retrieve the value of the current element */
        int currentItem = *((int *) S_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        /* The value of the histogram's element of the index
         * that matches the hash value is now increased by 1,
         * since one more element of the relational array was
         * hashed to this value
         */
        S_histogram[hash_value]++;
    }

    /* Now we are going to build the prefix sum arrays
     *
     * We initialize the prefix sum to zero
     */
    unsigned int prefixSum = 0;

    /* This is the prefix sum array of the relation 'relR'
     * It has the same size as the histogram of 'relR'
     */
    unsigned int *prefixSum_R = new unsigned int[R_histogramSize];

    /* We build the prefix sum of 'relR' */

    for(i = 0; i < R_histogramSize; i++)
    {
        /* We store the current prefix sum to index 'i' */
        prefixSum_R[i] = prefixSum;

        /* We retrieve the value of the current element */
        unsigned int currentItem = R_histogram[i];
     
        /* We add the next element of the
         * histogram to the prefix sum
         */
        prefixSum += currentItem;
    }

    /* We reset the prefix sum to do the same with 'relS' */
    prefixSum = 0;

    /* This is the prefix sum array of the relation 'relS'
     * It has the same size as the histogram of 'relS'
     */
    unsigned int *prefixSum_S = new unsigned int[S_histogramSize];

    /* We build the prefix sum of 'relS' */

    for(i = 0; i < S_histogramSize; i++)
    {
        /* We store the current prefix sum to index 'i' */
        prefixSum_S[i] = prefixSum;

        /* We retrieve the value of the current element */
        unsigned int currentItem = S_histogram[i];
     
        /* We add the next element of the
         * histogram to the prefix sum
         */
        prefixSum += currentItem;
    }

    std::cout << "+------------+------------+------------+------------+" << std::endl;
    std::cout << "|   R Hist   |   R Psum   |   S Hist   |   S Psum   |" << std::endl;
    std::cout << "+------------+------------+------------+------------+" << std::endl;

    for(i = 0; i < S_histogramSize; i++)
    {
        printf("|%12u|%12u|%12u|%12u|\n", R_histogram[i],
            prefixSum_R[i], S_histogram[i], prefixSum_S[i]);

        std::cout << "+------------+------------+------------+------------+" << std::endl;
    }

    std::cout << std::endl;

    Tuple *reordered_R = new Tuple[R_numOfTuples];
    Tuple *reordered_S = new Tuple[S_numOfTuples];
    unsigned int *elementsCounter = new unsigned int[R_histogramSize];

    for(i = 0; i < R_histogramSize; i++)
        elementsCounter[i] = 0;

    for(i = 0; i < R_numOfTuples; i++)
    {
        /* We retrieve the value of the current tuple */
        int currentItem = *((int *) R_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        reordered_R[prefixSum_R[hash_value] + elementsCounter[hash_value]] = R_table[i];
        elementsCounter[hash_value]++;
    }

    relR->setTuples(reordered_R);
    delete[] R_table;

    for(i = 0; i < R_histogramSize; i++)
        elementsCounter[i] = 0;

    for(i = 0; i < S_numOfTuples; i++)
    {
        /* We retrieve the value of the current tuple */
        int currentItem = *((int *) S_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        reordered_S[prefixSum_S[hash_value] + elementsCounter[hash_value]] = S_table[i];
        elementsCounter[hash_value]++;
    }

    relS->setTuples(reordered_S);
    delete[] S_table;

    displayInitialRelations();

    delete[] elementsCounter;
    delete[] R_histogram;
    delete[] S_histogram;
    delete[] prefixSum_R;
    delete[] prefixSum_S;

















    /* Hardcoded result just to check the output printing */
    RowIdPair *a = new RowIdPair[2];
    a[0].setLeftRowId(1);
    a[0].setRightRowId(1);
    a[1].setLeftRowId(2);
    a[1].setRightRowId(3);

    RowIdRelation *result = new RowIdRelation(a, 2);

	return result;
}

/***************************************************************
 * Displays in the screen the result returned by 'executeJoin' *
 ***************************************************************/

void PartitionedHashJoin::printJoinResult(RowIdRelation *resultOfExecuteJoin)
{
    /* Case the result is valid */

    if(resultOfExecuteJoin != NULL)
    {
        std::cout << "Result of Join Operation\n\n\tR |>-<| S" << std::endl;
        resultOfExecuteJoin->print(printUnsignedPair, lineContext);
        std::cout << std::endl;
    }

    /* Case the result is invalid because of an unexpected error */

    else
    {
        std::cout << "Due to an unexpected problem, no valid result was produced"
            << std::endl;
    }
}

/*******************************************************
 * Frees the result that was returned by 'executeJoin' *
 *******************************************************/

void PartitionedHashJoin::freeJoinResult(RowIdRelation *resultOfExecuteJoin)
{
    if(resultOfExecuteJoin != NULL)
    {
        delete[] resultOfExecuteJoin->getRowIdPairs();
        delete resultOfExecuteJoin;
    }
}
