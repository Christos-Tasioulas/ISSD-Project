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

}

TEST_LIST = {
    { "Hopscotch",  hopScotchTest},
    { "PartitionedHashJoin", partitionedHashJoinTest},
    { NULL, NULL }
};
