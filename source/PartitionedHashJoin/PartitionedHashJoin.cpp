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

/*******************************************************************
 * Used to call the printing operation of the Hopscotch Hash Table *
 *******************************************************************/

static void printTupleAndTuple(void *item1, void *item2)
{
    /* The item and the key are identical in every hash entry.
     *
     * We just print one of them.
     */
    Tuple *tuple = (Tuple *) item1;

    /* We retrieve the user data and the row ID of the tuple */
    int dataValue = *((int *) tuple->getItem());
    unsigned int rowId = tuple->getRowId();

    /* We print the tuple */
    printf("|%11u|%11d|", rowId, dataValue);
}

/*******************************************************************
 * Used to call the printing operation of the Hopscotch Hash Table *
 *******************************************************************/

static void colonContext()
{
    std::cout << " : ";
}

/*******************************************************************
 * Used to call the printing operation of the Hopscotch Hash Table *
 *******************************************************************/

static void lineContextWithNewLine()
{
    std::cout << "\n+-----------+-----------+" << std::endl;
}

/***************************************************************
 * Used to print the empty entries of the Hopscotch Hash Table *
 ***************************************************************/

static void emptyHashEntryPrinting()
{
    std::cout << "|           |           |";
}

/***************************************************************************
 *                      Compares two signed integers                       *
 *                                                                         *
 * Returns positive result if the first integer is greater than the second *
 * Returns negative result if the second integer is greater than the first *
 *                Returns zero if the two integers are equal               *
 ***************************************************************************/

static int compareInts(void *item_1, void *item_2)
{
    /* We retrieve the values of both integers */
    int integer_1 = *((int *) item_1);
    int integer_2 = *((int *) item_2);

    /* We return the difference of the integers */
    return integer_1 - integer_2;
}

/*************************************************************
 * Compares only the user data of two tuples. That means the *
 *  comparison doesn't depend on the row ID of either tuple  *
 *       We just compare the user items stored in them       *
 *************************************************************/

static int compareTupleUserData(void *item_1, void *item_2)
{
    /* First, we cast both items to the 'Tuple' type */
    Tuple *tuple_1 = (Tuple *) item_1;
    Tuple *tuple_2 = (Tuple *) item_2;

    /* We retrieve the items of both tuples */
    void *tuple_1_item = tuple_1->getItem();
    void *tuple_2_item = tuple_2->getItem();

    /* The items stored in the tuples are integers
     *
     * We return the result of the comparison between them.
     * This function ignores the row ID stored in the tuples.
     * It just compares the two tuples by their user items.
     */
    return compareInts(tuple_1_item, tuple_2_item);
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

/***********************************************************************
 * Uses the level-2 cache size to return the maximum capacity in bytes *
 *   that a relational array may have in order to need no partition    *
 ***********************************************************************/

static long capacity_limit(long lvl_2_cache_size, double max_percent_of_size = 1.0)
{
    //double max_allowed_size = ((double) lvl_2_cache_size) * max_percent_of_size;
    //return (long) max_allowed_size;
    return 32;
}

/***********************************************************************
 * When a partition needs to be done, the amount of bits that will be  *
 * used to determine the hash code of each element must increase. This *
 *  function, given the current amount of bits that are used, decides  *
 *    the new amount of bits that must be used to hash the elements    *
 ***********************************************************************/

static unsigned int alterBitsNum(unsigned int currentBitsNumForHashing)
{
    return currentBitsNumForHashing + 2;
}

/***************
 * Constructor *
 ***************/

PartitionedHashJoin::PartitionedHashJoin(const char *input_file,
    const char *config_file)
{
    /* We read the input file to access the user's relations */

    FileReader::readInputFile(input_file, &relR, &relS);

    /* We read the configuration file to store the user's options */

    FileReader::readConfigFile(config_file,
        &bitsNumForHashing,
        &showInitialRelations,
        &showAuxiliaryArrays,
        &showHashTable,
        &showSubrelations,
        &showResult,
        &hopscotchBuckets,
        &hopscotchRange,
        &resizableByLoadFactor,
        &loadFactor,
        &maxAllowedSizeModifier,
        &maxPartitionDepth);

    /* Since this object was created through an input and configuration
     * file, it is the object that the user created and not an object
     * created by another 'PartitionedHashJoin' object. That means
     * the current object is not depicting subrelations, but the whole
     * relations instead.
     */
    hasSubrelations = false;

    /* We have not yet reordered any relational tables */
    alteredTables = false;
}

/*************************
 * Secondary Constructor *
 *************************/

PartitionedHashJoin::PartitionedHashJoin(
    Relation *relR,
    Relation *relS,
    unsigned int bitsNumForHashing,
    bool showInitialRelations,
    bool showAuxiliaryArrays,
    bool showHashTable,
    bool showSubrelations,
    bool showResult,
    unsigned int hopscotchBuckets,
    unsigned int hopscotchRange,
    bool resizableByLoadFactor,
    double loadFactor,
    double maxAllowedSizeModifier,
    unsigned int maxPartitionDepth)
{
    /* We set the value of every variable field to
     * the value provided by the constructor arguments
     */
    this->relR = relR;
    this->relS = relS;
    this->bitsNumForHashing = bitsNumForHashing;
    this->showInitialRelations = showInitialRelations;
    this->showAuxiliaryArrays = showAuxiliaryArrays;
    this->showHashTable = showHashTable;
    this->showSubrelations = showSubrelations;
    this->showResult = showResult;
    this->hopscotchBuckets = hopscotchBuckets;
    this->hopscotchRange = hopscotchRange;
    this->resizableByLoadFactor = resizableByLoadFactor;
    this->loadFactor = loadFactor;
    this->maxAllowedSizeModifier = maxAllowedSizeModifier;
    this->maxPartitionDepth = maxPartitionDepth;

    /* An object initialized by this constructor
     * always depicts subrelations. Consequently,
     * we set the value of 'hasSubrelations' to 'true'
     */
    hasSubrelations = true;

    /* We have not yet reordered any relational tables */
    alteredTables = false;
}

/**************
 * Destructor *
 **************/

PartitionedHashJoin::~PartitionedHashJoin()
{
    /* Actions if the object is an "inner" helper object */

    if(hasSubrelations)
    {
        if(alteredTables == true)
        {
            delete[] relR->getTuples();
            delete[] relS->getTuples();
        }

        return;
    }

    /* We retrieve the amount of tuples of 'relR' and 'relS' */
    unsigned int R_numOfTuples = relR->getNumOfTuples();
    unsigned int S_numOfTuples = relS->getNumOfTuples();

    /* Helper variable for counting */
    unsigned int i;

    /* We delete the user data of every tuple of 'relR' */

    for(i = 0; i < R_numOfTuples; i++)
        delete ((int *) (relR->getTuples()[i]).getItem());

    /* We delete the user data of every tuple of 'relS' */

    for(i = 0; i < S_numOfTuples; i++)
        delete ((int *) (relS->getTuples()[i]).getItem());

    /* We delete the allocated memory for the relational
     * arrays of both 'relR' and 'relS'
     */
    delete[] relR->getTuples();
    delete[] relS->getTuples();

    /* We delete the relations themselves */
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

/*************************************************************
 * Returns 'true' if both relational arrays are small enough *
 *     to fit in the level-2 cache and 'false' otherwise     *
 *************************************************************/

bool PartitionedHashJoin::noPartitionRequired(long lvl_2_cache_size) const
{
    /* This is the size (in bytes) of relation 'relR' */
    unsigned int relR_size = relR->getSize();

    /* This is the size (in bytes) of relation 'relS' */
    unsigned int relS_size = relS->getSize();

    /* We retrieve the maximum capacity in bytes that a relational
     * array can have in order to fit the cache with no partition
     */
    long max_allowed_capacity = capacity_limit(
        lvl_2_cache_size, maxAllowedSizeModifier);

    /* If both relations can fit the cache, no partition is needed */
    return (relR_size <= max_allowed_capacity)
        && (relS_size <= max_allowed_capacity);
}

/***************************************************************
 * Determines wheter a bucket of the relation 'rerR' or 'relS' *
 *    needs to be further partitioned to additional buckets    *
 ***************************************************************/

bool PartitionedHashJoin::multiplePartitionRequired(
    unsigned int R_numOfItemsInBucket,
    unsigned int S_numOfItemsInBucket,
    unsigned int item_size,
    unsigned int lvl_2_cache_size) const
{
    /* We retrieve the maximum capacity in bytes that a relational
     * array can have in order to fit the cache with no partition
     */
    long max_allowed_capacity = capacity_limit(
        lvl_2_cache_size, maxAllowedSizeModifier);

    /* We examine if the given bucket of 'relR' fits the cache.
     *
     * If the bucket has no elements, it definetely fits the cache.
     *
     * We examine if the bucket fits the caches as long as it has more
     * than zero elements (the item of the end index is not included).
     */
    if(R_numOfItemsInBucket > 0)
    {
        /* We find the total size in bytes of the bucket */
        unsigned int totalSize = R_numOfItemsInBucket * item_size;

        /* If the total size surpasses the cache size,
         * the bucket needs to be further partitioned
         */
        if(totalSize > max_allowed_capacity)
            return true;
    }

    /* We examine if the given bucket of 'relS' fits the cache.
     *
     * If the bucket has no elements, it definetely fits the cache.
     *
     * We examine if the bucket fits the caches as long as it has more
     * than zero elements (the item of the end index is not included).
     */
    if(S_numOfItemsInBucket > 0)
    {
        /* We find the total size in bytes of the bucket */
        unsigned int totalSize = S_numOfItemsInBucket * item_size;

        /* If the total size surpasses the cache size,
         * the bucket needs to be further partitioned
         */
        if(totalSize > max_allowed_capacity)
            return true;
    }

    /* If this part is reached, both buckets fit the cache */
    return false;
}

/****************************************************************
 * Displays in the screen the contents of the initial relations *
 ****************************************************************/

void PartitionedHashJoin::displayInitialRelations(const char *message) const
{
    /* We print an informative message about the initial relations */
    std::cout << "\n" << message << "\n" << std::endl;

    /* We print the contents of the relational array 'relR' */
    std::cout << "\t    R" << std::endl;
    relR->print(printTuple, lineContext);
    std::cout << std::endl;

    /* We print the contents of the relational array 'relS' */
    std::cout << "\t    S" << std::endl;
    relS->print(printTuple, lineContext);
    std::cout << std::endl;
}

/***************************************************************
 * Displays in the screen the contents of the auxiliary arrays *
 ***************************************************************/

void PartitionedHashJoin::displayAuxiliaryArrays(unsigned int size,
    unsigned int *R_hist, unsigned int *S_hist,
    unsigned int *R_psum, unsigned int *S_psum) const
{
    if(!hasSubrelations)
        std::cout << "\nContents of Auxiliary Arrays\n" << std::endl;

    else
        std::cout << "\nContents of Auxiliary Arrays of Subrelations\n" << std::endl;

    std::cout << "+------------+------------+------------+------------+" << std::endl;
    std::cout << "|   R Hist   |   R Psum   |   S Hist   |   S Psum   |" << std::endl;
    std::cout << "+------------+------------+------------+------------+" << std::endl;

    unsigned int i;

    for(i = 0; i < size; i++)
    {
        printf("|%12u|%12u|%12u|%12u|\n", R_hist[i], R_psum[i], S_hist[i], S_psum[i]);
        std::cout << "+------------+------------+------------+------------+" << std::endl;
    }

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

/***********************************************
 * (Hash Function)                             *
 * Given the address of a tuple, it hashes its *
 *  contents to a non-negative integer value   *
 ***********************************************/

unsigned int PartitionedHashJoin::hashTuple(void *item)
{
    /* We cast the given address to its original type */
    Tuple *tuple = (Tuple *) item;

    /* We retrieve the item stored in the tuple */
    void *tuple_item = tuple->getItem();

    /* The item stored in the tuple is an integer */
    int integer_item = *((int *) tuple_item);

    /* The hash value of the integer is the integer itself,
     * but only after being cast to the unsigned integer type
     */
    return (unsigned int) integer_item;
}

/****************************************************************
 *  Executes Building and Probing for a pair of buckets of the  *
 * relations 'S' and 'R'. The four indexes determine the start  *
 *  and the end of the buckets. The end index is not included   *
 *                                                              *
 * The method places the row ID pairs that exist in the buckets *
 * and satisfy the join operation in the provided list 'result' *
 ****************************************************************/

void PartitionedHashJoin::probeRelations(
    unsigned int R_start_index,
    unsigned int R_end_index,
    unsigned int S_start_index,
    unsigned int S_end_index,
    List *result) const
{
    /* If one of the two buckets is empty, the join
     * operation produces no result for these buckets
     */
    if(R_start_index >= R_end_index
    || S_start_index >= S_end_index)
    {
        return;
    }

    /* We will start building the index of relation 'R' */

    HashTable *R_tuples_table = new HashTable(hopscotchBuckets,
        resizableByLoadFactor, loadFactor, hopscotchRange);

    /* We retrieve the relational tables of 'relR' and 'relS' */
    Tuple *R_table = relR->getTuples();
    Tuple *S_table = relS->getTuples();

    /* Helper variable for counting */
    unsigned int i;

    /* Starting from the given starting index of 'R', we place
     * every tuple of the 'R' table to the hash table. We keep
     * inserting items until we reach the given end index of 'R'
     */
    for(i = R_start_index; i < R_end_index; i++)
    {
        R_tuples_table->insert(&R_table[i], &R_table[i],
            PartitionedHashJoin::hashTuple);
    }

    /* We print the contents of the hash table if we need to */

    if(hasSubrelations)
    {
        if(showHashTable && showSubrelations)
        {
            std::cout << "Hash Table of Subrelation" << std::endl;

            R_tuples_table->print(
                printTupleAndTuple,
                colonContext,
                lineContextWithNewLine,
                emptyHashEntryPrinting
            );
        }
    }

    else
    {
        if(showHashTable)
        {
            std::cout << "Hash Table of Relation" << std::endl;

            R_tuples_table->print(
                printTupleAndTuple,
                colonContext,
                lineContextWithNewLine,
                emptyHashEntryPrinting
            );
        }
    }

    /* Starting from the given starting index of 'S', we search
     * every tuple of the 'S' table in the hash table. If we
     * find an item with the same value in the hash table, we
     * include it in the final result of the 'join' operation
     *
     * We keep searching the tuples of 'S' in the hash
     * table until we reach the given end index of 'S'
     */
    for(i = S_start_index; i < S_end_index; i++)
    {
        /* We search the current tuple of 'S' in the hash table */

        List *matchingKeys = R_tuples_table->bulkSearch(&S_table[i],
            PartitionedHashJoin::hashTuple, compareTupleUserData);

        /* As long as the list is not empty, we do the following */

        while(!matchingKeys->isEmpty())
        {
            /* We retrieve the current content of the head of the list */
            Tuple *current_tuple = (Tuple *) matchingKeys->getItemInPos(1);

            /* The current content of the head is removed from the list
             *
             * The next node of the head becomes the new head
             */
            matchingKeys->removeFront();

            /* Now we have found a tuple of 'S' and a tuple of 'R'
             * that have the same user data. We retrieve the row
             * IDs of these two tuples
             */
            unsigned int R_tuple_rowId = current_tuple->getRowId();
            unsigned int S_tuple_rowId = S_table[i].getRowId();

            /* We create an item that stores the pair of the above
            * two row IDs and we insert that item in the list
            */
            result->insertLast(new RowIdPair(R_tuple_rowId, S_tuple_rowId));
        }

        /* Finally, we terminate the list of matching keys */
        HashTable::terminateBulkSearchList(matchingKeys);
    }

    /* We free the allocated memory for the hash table */
    delete R_tuples_table;
}

/************************************************
 * Executes the Partitioned Hash Join Algorithm *
 ************************************************/

RowIdRelation *PartitionedHashJoin::executeJoin()
{
    /* We print the initial contents of the relations */

    if(hasSubrelations)
    {
        if(showInitialRelations && showSubrelations)
            displayInitialRelations("Subrelations in the beginning");
    }

    else
    {
        if(showInitialRelations)
            displayInitialRelations("Relations in the beginning");
    }

    /* We retrieve the size of the level-2 cache */
    long lvl2CacheSize = get_Lvl2_Cache_Size();

    /* If the cache size could not be retrieved,
     * we cancel the join operation and return 'NULL'
     */
    if(lvl2CacheSize == -1)
        return NULL;

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

    /* Case both relation tables are below the size of level 2 cache
     * or the (sub)relations have reached the maximum depth limit
     */
    if(noPartitionRequired(lvl2CacheSize) || maxPartitionDepth == 0)
    {
        /* We create the list that will be storing all the contents
         * of the result. We are using a list because we do not know
         * the size of the result beforehand (we don't know the num
         * of tuples that the result will have)
         */
        List *resultAsList = new List();

        /* Since the relations can fit in the
         * cache, we probe the whole arrays
         */
        probeRelations(0, R_numOfTuples, 0, S_numOfTuples, resultAsList);

        /* This is the number of row ID pairs of the result */
        unsigned int numOfItemsInList = resultAsList->getCounter();

        /* We create a new array of row ID pairs with size equal to the
         * size of the list (which is the num of row ID pairs of the result)
         */
        RowIdPair *resultArray = new RowIdPair[numOfItemsInList];

        /* We are going to transfer every row
         * ID pair of the list to the array
         */
        Listnode *current = resultAsList->getHead();

        /* Helper variable for counting */
        unsigned int i;

        /* As long as we have not transfered all the items from
         * the list to the array, we do the following actions
         */
        for(i = 0; i < numOfItemsInList; i++)
        {
            /* We retrieve the current row ID pair for transer */
            RowIdPair *current_pair = (RowIdPair *) current->getItem();

            /* We transfer the information of the pair we retrieved
             * to the pair of the current position in the array
             */
            resultArray[i].setLeftRowId(current_pair->getLeftRowId());
            resultArray[i].setRightRowId(current_pair->getRightRowId());

            /* We delete the pair of the list */
            delete current_pair;

            /* We proceed to the next pair of the list */
            current = current->getNext();
        }

        /* We initialize the final item we will return.
         * It holds the array of row ID pairs and its size.
         */
        RowIdRelation *result = new RowIdRelation(resultArray, numOfItemsInList);

        /* We free the allocated memory for the auxiliary list */
        delete resultAsList;

        /* We return the final result */
        return result;
    }

    /* If this part is reached, we are about to perform a partition */
    maxPartitionDepth--;

    /* We are going to reorder the tables. For some technical things,
     * we need to set the variable 'alteredTables' to 'true' if the
     * current object consists of subrelations.
     */
    if(hasSubrelations)
        alteredTables = true;

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

    /* We display both histograms and prefix sums in the screen */

    if(hasSubrelations)
    {
        if(showAuxiliaryArrays && showSubrelations)
        {
            displayAuxiliaryArrays(S_histogramSize, R_histogram,
                S_histogram, prefixSum_R, prefixSum_S);
        }
    }

    else
    {
        if(showAuxiliaryArrays)
        {
            displayAuxiliaryArrays(S_histogramSize, R_histogram,
                S_histogram, prefixSum_R, prefixSum_S);
        }
    }

    /* Now we have everything we need to reorder the contents
     * of the relational arrays 'relR' and 'relS'.
     *
     * First, we will allocate two new arrays with the same
     * size as the relational arrays in the heap.
     */
    Tuple *reordered_R = new Tuple[R_numOfTuples];
    Tuple *reordered_S = new Tuple[S_numOfTuples];

    /* When an item is hashed to a bucket (with the help of the prefix
     * sum auxiliary array), then another item that will be hashed
     * to the same bucket cannot take the position of the first one.
     * It has to be inserted in the next index of the first item.
     *
     * We will use the 'elementsCounter' array to decide properly to
     * which position in the new array an item must be inserted, given
     * that other items may have been hashed to the same bucket in the
     * past. We keep track of how many items have been inserted to
     * every bucket with the 'elementsCounter' auxiliary array.
     */
    unsigned int *elementsCounter = new unsigned int[R_histogramSize];

    /* Initially, there are no elements in any bucket.
     *
     * We initialize all the contents of 'elementsCounter' to zero.
     */
    for(i = 0; i < R_histogramSize; i++)
        elementsCounter[i] = 0;

    /* We start reordering the relational array 'relR' */

    for(i = 0; i < R_numOfTuples; i++)
    {
        /* We retrieve the value of the current tuple */
        int currentItem = *((int *) R_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        /* According to its hash value and the amount of previous
         * items that have been hashed to the same bucket, we
         * insert the current item to the reordered array of 'relR'
         */
        reordered_R[prefixSum_R[hash_value] + elementsCounter[hash_value]] = R_table[i];

        /* We increase the amount of inserted items in this bucket by 1 */
        elementsCounter[hash_value]++;
    }

    /* We assign the reordered array to 'relR' and discard the previous array */
    relR->setTuples(reordered_R);

    /* All the 'PartitionedHashJoin' objects that are assigned with sub-relations
     * use a part of the array of the whole relation. The object depicting the
     * whole relation will delete these arrays in this part, but the objects that
     * depict sub-relations must never delete the array, because they may delete
     * elements that will be needed later by other 'PartitionedHashJoin' objects.
     */
    if(hasSubrelations == false)
        delete[] R_table;

    /* We reset the contents of 'elementCounter' to zero,
     * because we want to repeat the process for 'relS'
     */
    for(i = 0; i < R_histogramSize; i++)
        elementsCounter[i] = 0;

    /* We start reordering the relational array 'relS' */

    for(i = 0; i < S_numOfTuples; i++)
    {
        /* We retrieve the value of the current tuple */
        int currentItem = *((int *) S_table[i].getItem());

        /* We hash that value with bit reduction hashing */
        unsigned int hash_value = bitReductionHash(currentItem);

        /* According to its hash value and the amount of previous
         * items that have been hashed to the same bucket, we
         * insert the current item to the reordered array of 'relS'
         */
        reordered_S[prefixSum_S[hash_value] + elementsCounter[hash_value]] = S_table[i];

        /* We increase the amount of inserted items in this bucket by 1 */
        elementsCounter[hash_value]++;
    }

    /* We assign the reordered array to 'relS' and discard the previous array */
    relS->setTuples(reordered_S);

    /* All the 'PartitionedHashJoin' objects that are assigned with sub-relations
     * use a part of the array of the whole relation. The object depicting the
     * whole relation will delete these arrays in this part, but the objects that
     * depict sub-relations must never delete the array, because they may delete
     * elements that will be needed later by other 'PartitionedHashJoin' objects.
     */
    if(hasSubrelations == false)
        delete[] S_table;

    /* We print the reordered arrays of the initial relations */

    if(hasSubrelations)
    {
        if(showInitialRelations && showSubrelations)
            displayInitialRelations("Subrelations with reordered contents");
    }

    else
    {
        if(showInitialRelations)
            displayInitialRelations("Relations with reordered contents");
    }

    /* We create the list that will be storing all the contents
     * of the result. We are using a list because we do not know
     * the size of the result beforehand (we don't know the num
     * of tuples that the result will have)
     */
    List *resultAsList = new List();

    /* A bitmap with bit capacity equal to the number of buckets
     * (both relations have the same number of buckets). The 1s
     * of this bitmap indicate buckets that have already been
     * processed by the multipartition algorithm and the result
     * of 'join' that comes from these buckets has already been
     * included to the result list.
     */
    Bitmap processedBuckets = Bitmap(R_histogramSize);

    /* For each bucket we will examine if it needs to be partitioned */

    if(maxPartitionDepth > 0)
    {
        for(i = 0; i < R_histogramSize; i++)
        {
            /* If the current bucket must be partitioned,
             * we proceed to the actions of this 'if' block
             */
            if(multiplePartitionRequired(R_histogram[i], S_histogram[i],
                sizeof(Tuple), lvl2CacheSize))
            {
                /* The 'multiplePartitionRequired' function just examines
                 * whether at least one of the two buckets has greater
                 * size than the cache and if this is true, the function
                 * returns 'true'. However, if one bucket has size equal
                 * to zero and the other has size greater than the cache,
                 * the result of 'join' between the two buckets has zero
                 * elements, but the 'multiplePartitionRequired' function
                 * will suggest that there is the need for partitioning,
                 * which is not needed in this case. That's why we examine
                 * here if one of the two buckets has zero elements.
                 * Because it is not handled properly inside the function.
                 *
                 * No partitioning is needed if one bucket has zero items.
                 */
                if((R_histogram[i] == 0) || (S_histogram[i] == 0))
                {
                    /* We don't need to examine further these two buckets
                     * at all, so we just set the bit of the bitmap that
                     * indicates the position of these buckets to 1, as we
                     * do not need to process them later either.
                     */
                    processedBuckets.setBit(i+1);

                    /* We proceed to the next pair of buckets */
                    continue;
                }

                /* We retrieve the relational arrays of 'relR' and 'relS' */
                Tuple *bucket_R = relR->getTuples();
                Tuple *bucket_S = relS->getTuples();

                /* We will move the pointer far from the base address of
                 * the whole relation up to the offset where the current
                 * bucket starts. We will do this for both relations.
                 */
                bucket_R += prefixSum_R[i];
                bucket_S += prefixSum_S[i];

                /* We create the two sub-relations that only
                 * contain the elements of the current buckets
                 */
                Relation subrelR = Relation(bucket_R, R_histogram[i]);
                Relation subrelS = Relation(bucket_S, S_histogram[i]);

                /* We will create a new 'PartitionedHashJoin' object that
                 * will perform the 'join' operation between the buckets,
                 */
                PartitionedHashJoin *subjoin = new PartitionedHashJoin(
                    &subrelR,
                    &subrelS,
                    alterBitsNum(bitsNumForHashing),
                    showInitialRelations,
                    showAuxiliaryArrays,
                    showHashTable,
                    showSubrelations,
                    showResult,
                    hopscotchBuckets,
                    hopscotchRange,
                    resizableByLoadFactor,
                    loadFactor,
                    maxAllowedSizeModifier,
                    maxPartitionDepth
                );

                /* Here we perform the 'join' operation between the buckets */
                RowIdRelation *subjoin_result = subjoin->executeJoin();

                /* Case something went wrong with the 'join' operation above */

                if(subjoin_result == NULL)
                {
                    std::cout << "A unexpected problem occurred" << std::endl;
                    std::cout << "A bucket could not be processed" << std::endl;

                    delete subjoin;
                    continue;
                }

                /* If this part is reached, the 'join' operation was performed
                 * successfully. We retrieve the result array of the 'join'
                 */
                RowIdPair *subjoin_array = subjoin_result->getRowIdPairs();

                /* We retrieve the number of elements of the above array */
                unsigned int subjoin_items = subjoin_result->getNumOfRowIdPairs();

                /* Helper variable for counting */
                unsigned int j;

                /* We add all the elements of the result to the list */

                for(j = 0; j < subjoin_items; j++)
                {
                    resultAsList->insertLast(new RowIdPair(subjoin_array[j].
                        getLeftRowId(), subjoin_array[j].getRightRowId()));
                }

                /* We free the allocated memory for the result of 'join' */
                subjoin->freeJoinResult(subjoin_result);

                /* We free the 'PartitionedHashJoin' object we created
                 * to perform the 'join' operation between the buckets
                 */
                delete subjoin;

                /* These two buckets have now been processed and the result
                 * of 'join' that comes from them has been included in the
                 * final result. We do not need to process these two buckets
                 * further. Therefore, we set the bit that indicates their
                 * position in the whole arrays to 1.
                 */
                processedBuckets.setBit(i+1);
            }
        }
    }

    /* We start probing the buckets of the reordered array 'R'
     * to the buckets of the reordered array 'S' that have the
     * same hashing ID.
     *
     * We skip processing the buckets that have already been
     * processed by the mutlipartitioning algorithm just above.
     */
    for(i = 0; i < R_histogramSize - 1; i++)
    {
        if(!processedBuckets.isSet(i+1))
        {
            probeRelations(prefixSum_R[i], prefixSum_R[i+1],
                prefixSum_S[i], prefixSum_S[i+1], resultAsList);
        }
    }

    /* We compare the last pair of buckets (only if it has not
     * already been processed by the multipartitioning algorithm)
     */
    if(!processedBuckets.isSet(R_histogramSize))
    {
        probeRelations(prefixSum_R[i], R_numOfTuples,
            prefixSum_S[i], S_numOfTuples, resultAsList);
    }

    /* This is the number of row ID pairs of the result */
    unsigned int numOfItemsInList = resultAsList->getCounter();

    /* We create a new array of row ID pairs with size equal to the
     * size of the list (which is the num of row ID pairs of the result)
     */
    RowIdPair *resultArray = new RowIdPair[numOfItemsInList];

    /* We are going to transfer every row
     * ID pair of the list to the array
     */
    Listnode *current = resultAsList->getHead();

    /* As long as we have not transfered all the items from
     * the list to the array, we do the following actions
     */
    for(i = 0; i < numOfItemsInList; i++)
    {
        /* We retrieve the current row ID pair for transer */
        RowIdPair *current_pair = (RowIdPair *) current->getItem();

        /* We transfer the information of the pair we retrieved
         * to the pair of the current position in the array
         */
        resultArray[i].setLeftRowId(current_pair->getLeftRowId());
        resultArray[i].setRightRowId(current_pair->getRightRowId());

        /* We delete the pair of the list */
        delete current_pair;

        /* We proceed to the next pair of the list */
        current = current->getNext();
    }

    /* We initialize the final item we will return.
     * It holds the array of row ID pairs and its size.
     */
    RowIdRelation *result = new RowIdRelation(resultArray, numOfItemsInList);

    /* We free the allocated memory for the auxiliary list */
    delete resultAsList;

    /* We free the allocated memory for the auxiliary arrays */
    delete[] elementsCounter;
    delete[] R_histogram;
    delete[] S_histogram;
    delete[] prefixSum_R;
    delete[] prefixSum_S;

    /* The 'join' operation has been performed. We restore the
     * initial maximum depth of partitions for this operation.
     *
     * Above we had just decreased it by 1. Now we revert it
     * to its original value.
     */
    maxPartitionDepth++;

    /* We return the final result */
	return result;
}

/***************************************************************
 * Displays in the screen the result returned by 'executeJoin' *
 ***************************************************************/

void PartitionedHashJoin::printJoinResult(RowIdRelation *resultOfExecuteJoin)
{
    /* If the user does not desire to see the result, we just return */
    if(showResult == false)
        return;

    /* Case the result is valid */

    if(resultOfExecuteJoin != NULL)
    {
        std::cout << "\nResult of Join Operation\n\n\tR |>-<| S" << std::endl;
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
