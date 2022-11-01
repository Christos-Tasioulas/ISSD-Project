#include "acutest.h"
#include "Test.h"

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

    TEST_ASSERT(*((int *) ht->getTable()->getKey()) == 5);

    delete ht;
}

TEST_LIST = {
    { "hopScotchTest",  hopScotchTest},
    { NULL, NULL }
};
