#include "acutest.h"
#include "PartitionedHashJoin.h"
#include <iostream>
#include <string>

using namespace std;

static unsigned int hash_int(void *i)
{
    int myInt = *((int *) i);
    return (unsigned int) myInt;
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

void hopScotchTest()
{
    HashTable *ht = new HashTable();

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

    // A rehash will happen 
    ht->insert(&key_5, &key_5, hash_int);

    // The bitmap of each index is larger now
    TEST_ASSERT(*((int *) ht->getTable()[9].getKey()) == 5);

    delete ht;
}

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

TEST_LIST = {
    { "Hopscotch",  hopScotchTest},
    { "PartitionedHashJoin", partitionedHashJoinTest},
    { NULL, NULL }
};
