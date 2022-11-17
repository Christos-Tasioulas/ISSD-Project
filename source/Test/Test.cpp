#include <iostream>
#include <string>
#include "acutest.h"
#include "Bitmap.h"
#include "PartitionedHashJoin.h"

using namespace std;

/**************************************************************************
 *                          Auxiliary Functions                           *
 **************************************************************************/

static unsigned int hash_int(void *i)
{
    int myInt = *((int *) i);
    return (unsigned int) myInt;
}

static int compare_ints(void *i1, void* i2)
{
    int myInt1 = *((int *) i1);
    int myInt2 = *((int *) i2);
    return myInt1 - myInt2;
}

/*******************************************************************
 * Used to call the printing operation of the Hopscotch Hash Table *
 *******************************************************************/

static void printIntAndInt(void *item1, void *item2)
{
   int my_int_1 = *((int *) item1);
   int my_int_2 = *((int *) item2);

    printf("|%11u|%11d|", my_int_1, my_int_2);
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

static int compare_results(RowIdRelation* r1, RowIdRelation* r2)
{
    if(r1->getNumOfRowIdPairs() != r2->getNumOfRowIdPairs()) return 0;
    else
    {
        unsigned int number_of_pairs = r1->getNumOfRowIdPairs();
        RowIdPair* array1 = r1->getRowIdPairs();
        RowIdPair* array2 = r2->getRowIdPairs();

        for(unsigned int i = 0; i < number_of_pairs; i++)
        {
            RowIdPair p1 = array1[i];
            RowIdPair p2 = array2[i];
            if((p1.getLeftRowId() != p2.getLeftRowId()) || (p1.getRightRowId() != p2.getRightRowId())) 
            {
                return 0;  
            }
            
        }

    }
    return 1;
}

/**************************************************************************
 *                              Bitmap Tests                              *
 **************************************************************************/

void test_posOfFirstAce()
{
    Bitmap* b = new Bitmap(16);

    b->setBit(12);
    b->setBit(8);
    b->setBit(14);

    // the first one is in 8th position
    TEST_ASSERT(b->posOfFirstAce() == 8);

    b->setBit(6);

    // the first one is in 6th position
    TEST_ASSERT(b->posOfFirstAce() == 6);

    delete b;
}

void test_posOfFirstAce_from_Pos()
{
    Bitmap* b = new Bitmap(16);

    b->setBit(12);
    b->setBit(8);
    b->setBit(14);

    // the first one from the 9th position is the 12th one
    TEST_ASSERT(b->posOfFirstAceFromPos(9) == 12);
    // the first one from the 13th position is the 14th one
    TEST_ASSERT(b->posOfFirstAceFromPos(13) == 14);
    // the first one from the 1st position is the 8th one
    TEST_ASSERT(b->posOfFirstAceFromPos(1) == 8);

    b->setBit(6);

    // the first one from the 7th position is the 8th one
    TEST_ASSERT(b->posOfFirstAceFromPos(7) == 8);
    // the first one from the 1st position is the 6th one
    TEST_ASSERT(b->posOfFirstAceFromPos(1) == 6);

    delete b;
}

void test_reset_and_resize()
{
    Bitmap* b = new Bitmap(16);

    unsigned int i;
    // Setting all bits to 1
    for(i = 1; i <= 16; i++)
    {
        b->setBit(i);
    }

    // Resetting and Resizing to 32 
    b->resetAndChangeSize(32);

    // Reset and Change size resets all bits to zero.
    for(i = 1; i <= 32; i++)
    {
        TEST_ASSERT(b->isSet(i) == false);
    }

    delete b;
}

/**************************************************************************
 *                            Filereader Test                             *
 **************************************************************************/

void read_config_test()
{
    char config_file[14] = "../config.txt";
    unsigned int bitsNumForHashing;
    bool showInitialRelations;
    bool showAuxiliaryArrays;
    bool showHashTable;
    bool showSubrelations;
    bool showResult;
    unsigned int hopscotchBuckets;
    unsigned int hopscotchRange;
    bool resizableByLoadFactor;
    double loadFactor;
    double maxAllowedSizeModifier;
    unsigned int maxPartitionDepth;

    FileReader::readConfigFile(
        config_file,
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
        &maxPartitionDepth
    );

    // Tests to see if the function can read the CURRENT configuration settings
    // The test must be updated along with the configuration file 
    TEST_ASSERT(bitsNumForHashing == 1);
    TEST_ASSERT(showInitialRelations == true);
    TEST_ASSERT(showAuxiliaryArrays == true);
    TEST_ASSERT(showHashTable == false);
    TEST_ASSERT(showSubrelations == true);
    TEST_ASSERT(showResult == true);
    TEST_ASSERT(hopscotchBuckets == 101);
    TEST_ASSERT(hopscotchRange == 16);
    TEST_ASSERT(resizableByLoadFactor == true);
    TEST_ASSERT(loadFactor == 0.8);
    TEST_ASSERT(maxAllowedSizeModifier == 0.85);
    TEST_ASSERT(maxPartitionDepth == 2);
}

/**************************************************************************
 *                            Hash Table Tests                            *
 **************************************************************************/

void insertWithoutRehashTest()
{
    HashTable *ht = new HashTable(16, false, 1.0, 4);

    int myKey = 5;
    ht->insert(&myKey, &myKey, hash_int);

    TEST_ASSERT(*((int *) ht->getTable()[5].getKey()) == 5);

    int newKey = 5;
    ht->insert(&newKey, &newKey, hash_int);

    // This test indicates that a key that has been entered before moves to the next index
    TEST_ASSERT(*((int *) ht->getTable()[6].getKey()) == 5);

    int newnewKey = 6;
    ht->insert(&newnewKey, &newnewKey, hash_int);

    // This test indicates that a key that its original index is occupied moves to the next index
    TEST_ASSERT(*((int *) ht->getTable()[7].getKey()) == 6);

    int key_4 = 6;
    ht->insert(&key_4, &key_4, hash_int);

    int key_5 = 5;
    ht->insert(&key_5, &key_5, hash_int);

    // The new key has been pushed back
    TEST_ASSERT(*((int *) ht->getTable()[7].getKey()) == 5);
    TEST_ASSERT(*((int *) ht->getTable()[8].getKey()) == 6);
    TEST_ASSERT(*((int *) ht->getTable()[9].getKey()) == 6);

    int key_6 = 15;
    ht->insert(&key_6, &key_6, hash_int);

    int key_7 = 15;
    ht->insert(&key_7, &key_7, hash_int);

    // Testing the case the hash table comes full circle
    TEST_ASSERT(*((int *) ht->getTable()[15].getKey()) == 15);
    TEST_ASSERT(*((int *) ht->getTable()[0].getKey()) == 15);

    ht->print(
        printIntAndInt,
        colonContext,
        lineContextWithNewLine,
        emptyHashEntryPrinting
    );

    delete ht;
}

void rehashTest()
{
    HashTable *ht = new HashTable(16, true, 0.7, 3);

    TEST_ASSERT(ht->getBucketsNum() == 16);
    TEST_ASSERT(ht->getHopInfoCapacity() == 3);

    int key_1 = 5;
    ht->insert(&key_1, &key_1, hash_int);

    int key_2 = 5;
    ht->insert(&key_2, &key_2, hash_int);

    int key_3 = 5;
    ht->insert(&key_3, &key_3, hash_int);

    // A rehash will happen
    int key_4 = 5;
    ht->insert(&key_4, &key_4, hash_int);

    TEST_ASSERT(ht->getBucketsNum() == 32);
    TEST_ASSERT(ht->getHopInfoCapacity() == 6);
    TEST_ASSERT(*((int *) ht->getTable()[8].getKey()) == 5);

    int key_5 = 6;
    ht->insert(&key_5, &key_5, hash_int);

    int key_6 = 6;
    ht->insert(&key_6, &key_6, hash_int);

    int key_7 = 6;
    ht->insert(&key_7, &key_7, hash_int);

    // A rehash will happen
    int key_8 = 6;
    ht->insert(&key_8, &key_8, hash_int);

    TEST_ASSERT(ht->getBucketsNum() == 64);
    TEST_ASSERT(ht->getHopInfoCapacity() == 12);
    TEST_ASSERT(*((int *) ht->getTable()[12].getKey()) == 6);

    int key[37];
    for(unsigned int i = 0; i < 37; i++)
    {
        key[i] = i;
        ht->insert(&key[i], &key[i], hash_int);
    }

    ht->print(
        printIntAndInt,
        colonContext,
        lineContextWithNewLine,
        emptyHashEntryPrinting
    );

    TEST_ASSERT(ht->getBucketsNum() == 128);
    TEST_ASSERT(ht->getHopInfoCapacity() == 24);

    delete ht;
}

void searchTest()
{
    HashTable *ht = new HashTable(16, true, 0.7, 3);

    int key_1 = 5;
    ht->insert(&key_1, &key_1, hash_int);
    TEST_ASSERT(ht->search(&key_1, hash_int, compare_ints) == true);

    void *searched_item = ht->searchItem(&key_1, hash_int, compare_ints);
    TEST_ASSERT(*((int *) searched_item) == 5); 

    int key_2 = 5;
    ht->insert(&key_2, &key_2, hash_int);

    List* fives = ht->bulkSearch(&key_1, hash_int, compare_ints);



    delete ht;
}


/**************************************************************************
 *                       Partitioned Hash Join Test                       *
 **************************************************************************/

void partitionedHashJoinTest()
{
    PartitionedHashJoin phj = PartitionedHashJoin("../test.txt", "../config.txt");

    // Replicating the result of our tested input's execution
    RowIdPair rp1(1, 1);
    RowIdPair rp2(2, 3);

    RowIdPair array[2];
    array[0] = rp1;
    array[1] = rp2;

    RowIdRelation* expected = new RowIdRelation(array, 2);

    RowIdRelation* result = phj.executeJoin();
    
    TEST_ASSERT(compare_results(result, expected) == 1);

    phj.freeJoinResult(result);
    delete expected;
    
}

/**************************************************************************
 *                                  Main                                  *
 **************************************************************************/

TEST_LIST = {
    // Bitmap testing
    { "Position of first ace in Bitmap", test_posOfFirstAce},
    { "Position of first ace from given position in Bitmap", test_posOfFirstAce_from_Pos},
    { "Reset and resize Bitmap", test_reset_and_resize},
    // Filereader testing
    { "Reading Configuration File", read_config_test},
    // Hash Table Test
    { "Insert",  insertWithoutRehashTest},
    { "Rehash",  rehashTest},
    { "Search",  searchTest},
    // Partitioned Hash Join Testing
    { "Partitioned Hash Join", partitionedHashJoinTest},
    { NULL, NULL }
};
