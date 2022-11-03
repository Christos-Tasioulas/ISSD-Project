#include "acutest.h"
#include "PartitionedHashJoin.h"

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

    TEST_ASSERT(*((int *) ht->getTable()[5].getKey()) == 4);

    delete ht;
}

void partitionTest()
{

}

void buildingTest()
{

}

void probingTest()
{
    
}

TEST_LIST = {
    { "Hopscotch",  hopScotchTest},
    { "Partition", partitionTest},
    { "Building", buildingTest},
    { "Probing", probingTest},
    { NULL, NULL }
};
