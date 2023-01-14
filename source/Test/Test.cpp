#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "acutest.h"
#include "QueryHandler.h"
#include "Query.h"

using namespace std;

// Global variable
int* int_data;

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
    int myInt1, myInt2;
    if(i1 != NULL) myInt1 = *((int *) i1);
    else return -1;
    if(i2 != NULL) myInt2 = *((int *) i2);
    else return 1;
    return myInt1 - myInt2;
}

static void test_compare(void *key, void *value)
{
    int myKey = *((int *) key);
    int myValue = *((int *) value);
    TEST_ASSERT(myValue == int_data[myKey]);
}

int count_char(string s, char mychar) {
  int count = 0;

  for (unsigned int i = 0; i < s.size(); i++)
    if (s[i] == mychar) count++;

  return count;
}

static void deleteTable(void *item)
{
	Table *table = (Table *) item;
	delete table;
}

static void print_table(unsigned long long** table, unsigned long long numTuples, unsigned long long numColumns)
{
    unsigned long long i,j;
    for(j=0; j<numTuples; j++)
    {
        for(i=0; i<numColumns; i++)
        {
            cout<<table[i][j];
            if(i<(numColumns-1)) cout << "|";
        }
        cout << endl;
    }
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

static bool compareTable(Table *table, string filename)
{
    unsigned long long **tbl;
    unsigned long long numTuples = 1;
    
    ifstream counter(filename);

    string tuple;
    getline(counter, tuple);

    // NumColumns = number of times the character '|' appears in the line
    unsigned long long numColumns = (unsigned long long) count_char(tuple, '|');

    // Count tuples
    while(getline(counter, tuple))
    {
        numTuples++;   
    }
    counter.close();

    // The result is wrong if the tuples and the columns number are different between tables
    if(table->getNumOfColumns() != numColumns)
    {
        return false;
    } 
    else if(table->getNumOfTuples() != numTuples)
    {
        return false;
    }

    // Allocating space
    tbl = new unsigned long long*[numColumns];
    for(unsigned long long i = 0; i < numColumns; i++)
    {
        tbl[i] = new unsigned long long[numTuples];
    }

    // Adding the elemets of the file inside the array we made
    ifstream rntbl(filename);
    unsigned long long i,j = 0;
    while(getline(rntbl, tuple))
    {
        
        stringstream tuple_stream(tuple);
        string element;
        i = 0;
        // every element is tokenized by the characters '|', '\n'
        while(getline(tuple_stream, element, '|'))
        {
            stringstream ss(element);
            unsigned long long data;
            ss >> data;
            tbl[i][j] = data; 
            i++;
        }
        j++;
    } 

    unsigned long long ** r_table = table->getTable();
    
    // Comparing each element of the .tbl file with the table we got
    for(unsigned long long j=0; j<numTuples; j++)
    {
        for(unsigned long long i=0; i<numColumns; i++)
        {
            if(tbl[i][j] != r_table[i][j])
            {
                // Delete if there is an error for valgrind's sake
                for(unsigned long long i = 0; i < numColumns; i++)
                {
                    delete[] tbl[i];
                }
                delete[] tbl;
                return false;
            }
        }
    }

    // Deleting table 
    for(unsigned long long i = 0; i < numColumns; i++)
    {
        delete[] tbl[i];
    }
    delete[] tbl;

    // Both tables are the same
    return true;
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
    TEST_ASSERT(bitsNumForHashing == 8);
    TEST_ASSERT(showInitialRelations == false);
    TEST_ASSERT(showAuxiliaryArrays == false);
    TEST_ASSERT(showHashTable == false);
    TEST_ASSERT(showSubrelations == false);
    TEST_ASSERT(showResult == true);
    TEST_ASSERT(hopscotchBuckets == 1001);
    TEST_ASSERT(hopscotchRange == 64);
    TEST_ASSERT(resizableByLoadFactor == false);
    TEST_ASSERT(loadFactor == 0.9);
    TEST_ASSERT(maxAllowedSizeModifier == 0.95);
    TEST_ASSERT(maxPartitionDepth == 2);
}

void read_init_file_test()
{
    List *tables = FileReader::readInitFile("../input/small.init");

    Listnode *current = tables->getHead();
    int i = 0;

    // comparing each table we created with the corresponding .tbl file
    while(current != NULL)
    {
        // Filename: ../input/ri.tbl
        string filename = "../input/r" + to_string(i) + ".tbl";
        Table *item = (Table *) current->getItem();

        TEST_ASSERT(compareTable(item, filename) == true);

        i++;
        current = current->getNext();
    }
    
    // Deleting table
    tables->traverseFromHead(deleteTable);

    delete tables;
}

/**************************************************************************
 *                            Hash Table Tests                            *
 **************************************************************************/
void insertWithoutRehashTest()
{
    HashTable *ht = new HashTable(16, false, 1.0, 4);

    int myKey = 5;
    ht->insert(&myKey, &myKey, hash_int, compare_ints);

    List* items1 = ht->getTable()[5].getItems();
    HashEntryItem *entryItem1 = (HashEntryItem *) items1->getItemInPos(1);
    int item1 = *((int *)entryItem1->getItem());
    TEST_ASSERT(item1 == 5);
    
    // Case: Duplicate
    int myDuplicateKey = 5;
    ht->insert(&myKey, &myKey, hash_int, compare_ints);
    items1 = ht->getTable()[5].getItems();
    entryItem1 = (HashEntryItem *) items1->getItemInPos(2);
    item1 = *((int *)entryItem1->getItem());
    TEST_ASSERT(item1 == 5);
    TEST_ASSERT(items1->getCounter() == 2);

    int newKey = 21;
    ht->insert(&newKey, &newKey, hash_int, compare_ints);

    // This test indicates that a key that has been entered before moves to the next index
    List* items2 = ht->getTable()[6].getItems();
    HashEntryItem *entryItem2 = (HashEntryItem *) items2->getItemInPos(1);
    int item2 = *((int *)entryItem2->getItem());
    TEST_ASSERT(item2 == 21);

    int newnewKey = 6;
    ht->insert(&newnewKey, &newnewKey, hash_int, compare_ints);

    // This test indicates that a key that its original index is occupied moves to the next index
    List* items3 = ht->getTable()[7].getItems();
    HashEntryItem *entryItem3 = (HashEntryItem *) items3->getItemInPos(1);
    int item3 = *((int *)entryItem3->getItem());
    TEST_ASSERT(item3 == 6);

    int key_4 = 37;
    ht->insert(&key_4, &key_4, hash_int, compare_ints);

    List* items4 = ht->getTable()[8].getItems();
    HashEntryItem *entryItem4 = (HashEntryItem *) items4->getItemInPos(1);
    int item4 = *((int *)entryItem4->getItem());
    TEST_ASSERT(item4 == 37);

    int key_5 = 22;
    ht->insert(&key_5, &key_5, hash_int, compare_ints);

    List* items5 = ht->getTable()[9].getItems();
    HashEntryItem *entryItem5 = (HashEntryItem *) items5->getItemInPos(1);
    int item5 = *((int *)entryItem5->getItem());
    TEST_ASSERT(item5 == 22);

    int key_6 = 15;
    ht->insert(&key_6, &key_6, hash_int, compare_ints);

    int key_7 = 31;
    ht->insert(&key_7, &key_7, hash_int, compare_ints);

    // Testing the case the hash table comes full circle
    List* items6 = ht->getTable()[15].getItems();
    HashEntryItem *entryItem6 = (HashEntryItem *) items6->getItemInPos(1);
    int item6 = *((int *)entryItem6->getItem());
    TEST_ASSERT(item6 == 15);

    List* items7 = ht->getTable()[0].getItems();
    HashEntryItem *entryItem7 = (HashEntryItem *) items7->getItemInPos(1);
    int item7 = *((int *)entryItem7->getItem());
    TEST_ASSERT(item7 == 31);

    delete ht;
}

void rehashTest()
{
    HashTable *ht = new HashTable(16, true, 0.7, 4);

    TEST_ASSERT(ht->getBucketsNum() == 16);
    TEST_ASSERT(ht->getHopInfoCapacity() == 4);

    int key_0 = 5;
    ht->insert(&key_0, &key_0, hash_int, compare_ints);

    int key_1 = 21;
    ht->insert(&key_1, &key_1, hash_int, compare_ints);

    int key_2 = 37;
    ht->insert(&key_2, &key_2, hash_int, compare_ints);

    int key_3 = 53;
    ht->insert(&key_3, &key_3, hash_int, compare_ints);

    // A rehash will happen
    int key_4 = 69;
    ht->insert(&key_4, &key_4, hash_int, compare_ints);

    // Testing the effects of the rehash
    TEST_ASSERT(ht->getBucketsNum() == 32);
    TEST_ASSERT(ht->getHopInfoCapacity() == 6);

    int key_5 = 6;
    ht->insert(&key_5, &key_5, hash_int, compare_ints);

    int key_6 = 38;
    ht->insert(&key_6, &key_6, hash_int, compare_ints);

    int key_7 = 70;
    ht->insert(&key_7, &key_7, hash_int, compare_ints);

    int key_8 = 102;
    ht->insert(&key_8, &key_8, hash_int, compare_ints);

    int key_9 = 134;
    ht->insert(&key_9, &key_9, hash_int, compare_ints);

    int key_10 = 146;
    ht->insert(&key_10, &key_10, hash_int, compare_ints);

    int key_11 = 178;
    ht->insert(&key_11, &key_11, hash_int, compare_ints);

    // Testing the effects of the rehash
    TEST_ASSERT(ht->getBucketsNum() == 64);
    TEST_ASSERT(ht->getHopInfoCapacity() == 9);

    // filling the hash table till the rehash is triggered 64*0.7 = 44.8 which is about 45
    // 8 + 37 = 45
    int key[37];
    for(unsigned int i = 0; i < 37; i++)
    {
        key[i] = i;
        ht->insert(&key[i], &key[i], hash_int, compare_ints);
    }
    
    // Testing the effects of the rehash
    TEST_ASSERT(ht->getBucketsNum() == 128);
    TEST_ASSERT(ht->getHopInfoCapacity() == 12);

    delete ht;
}

void bulkSearchTest()
{
    HashTable *ht = new HashTable(16, true, 0.7, 3);

    int key_1 = 5;
    ht->insert(&key_1, &key_1, hash_int, compare_ints);

    int key_2 = 5;
    ht->insert(&key_2, &key_2, hash_int, compare_ints);

    // Testing how many times the item has been found
    List* fives = ht->bulkSearchKeys(&key_1, hash_int, compare_ints);
    TEST_ASSERT(fives->getCounter() == 2);

    // Testing if the correct item has been found at all times
    void* first_item = fives->getItemInPos(0);
    TEST_ASSERT((*(int *) first_item) == 5);

    void* second_item = fives->getItemInPos(1);
    TEST_ASSERT((*(int *) second_item) == 5);

    ht->terminateBulkSearchList(fives);

    delete ht;
}

/**************************************************************************
 *                          Intermediate Array                            *
 **************************************************************************/

void testIntermediateArraySearch()
{
    // Reading initialization file
    List *tables = FileReader::readInitFile("../input/small.init");

    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");

    IntermediateArray *ia = new IntermediateArray(4, 0, 1, 3, 1, 2, tables, phji);

    // Testing the case we have an Intermediate Array with two relations at the start
    TEST_ASSERT(ia->search(4, 1) == true);
    TEST_ASSERT(ia->search(3, 2) == true);
    TEST_ASSERT(ia->search(4, 2) == false);

    // Testing the case we have an Intermediate Array with one relation and a Filter operation
    IntermediateArray *ia1 = new IntermediateArray(4, 1, 2, 3000, '<', tables, phji);

    TEST_ASSERT(ia1->search(4, 1) == false);
    TEST_ASSERT(ia1->search(3, 2) == false);
    TEST_ASSERT(ia1->search(4, 2) == true);

    
    delete phji;
    delete ia;
    delete ia1;
    delete tables;
}

void testExecuteJoinWithForeignRelation()
{
    /*******************************************************************
     *  It is impossible to compare every result of the join operation *
     *  So in these tests we will compare the number of rowIDs before  *
     *                  with the number of rowIDs after                *
     *******************************************************************/
    List *tables = FileReader::readInitFile("../input/small.init");

    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");

    IntermediateArray *ia = new IntermediateArray(4, 0, 1, 3, 1, 2, tables, phji);

    unsigned int prev_rowsNum = ia->getRowsNum();

    ia->executeJoinWithForeignRelation(4, 0, 1, 2, 0, 0);

    unsigned int rowsNum = ia->getRowsNum();

    List *relations = ia->getRelations();
    
    IntermediateRelation *ir2 = (IntermediateRelation *) relations->getItemInPos(3);

    // Testing if the foreign relation is inside that Intermediate Array
    TEST_ASSERT(ir2->getName() == 2);
    TEST_ASSERT(ir2->getPriority() == 0);
    // The new number of rows will be smaller than 
    // or equal to the starting number because of the filter
    TEST_ASSERT(prev_rowsNum >= rowsNum);

    delete tables;
    delete phji;

}

void testExecuteJoinWithTwoRelationsInTheArray()
{
    /*******************************************************************
     *  It is impossible to compare every result of the join operation *
     *  So in these tests we will compare the number of rowIDs before  *
     *                  with the number of rowIDs after                *
     *******************************************************************/
     
    List *tables = FileReader::readInitFile("../input/small.init");

    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");

    IntermediateArray *ia = new IntermediateArray(4, 0, 1, 3, 1, 2, tables, phji);

    unsigned int prev_rowsNum = ia->getRowsNum();

    ia->executeJoinWithTwoRelationsInTheArray(4, 1, 1, 3, 0, 2);

    unsigned int rowsNum = ia->getRowsNum();

    // The new number of rows will be smaller than 
    // or equal to the starting number because of the filter
    TEST_ASSERT(prev_rowsNum >= rowsNum);

    delete tables;
    delete phji;
}

void testExecuteJoinWithRelationOfOtherArray()
{
    /*******************************************************************
     *  It is impossible to compare every result of the join operation *
     *  So in these tests we will compare the number of rowIDs before  *
     *                  with the number of rowIDs after                *
     *******************************************************************/
     
    List *tables = FileReader::readInitFile("../input/small.init");

    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");

    IntermediateArray *ia1 = new IntermediateArray(4, 0, 1, 3, 1, 2, tables, phji);
    IntermediateArray *ia2 = new IntermediateArray(2, 2, 0, 3000, '<', tables, phji);

    unsigned int prev_rowsNum = ia2->getRowsNum();

    ia1->executeJoinWithRelationOfOtherArray(ia2, 4, 0, 1, 2, 2, 0);

    unsigned int rowsNum = ia1->getRowsNum();

    // The new number of rows will be smaller than 
    // or equal to the starting number because of the filter
    TEST_ASSERT(prev_rowsNum >= rowsNum);

    delete tables;
    delete phji;
}

void testExecuteFilter()
{
    /*******************************************************************
     *  It is impossible to compare every result of the join operation *
     *  So in these tests we will compare the number of rowIDs before  *
     *                  with the number of rowIDs after                *
     *******************************************************************/
     
    List *tables = FileReader::readInitFile("../input/small.init");

    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");
    IntermediateArray *ia = new IntermediateArray(2, 2, 0, 3000, '<', tables, phji);

    unsigned int prev_rowsNum = ia->getRowsNum();

    ia->executeFilter(4, 0, 1, 5000, '>');

    unsigned int rowsNum = ia->getRowsNum();

    // The new number of rows will be smaller than 
    // or equal to the starting number because of the filter
    TEST_ASSERT(prev_rowsNum >= rowsNum);

    delete tables;
    delete phji;
}

/**************************************************************************
 *                               List Test                                *
 **************************************************************************/

void testInsertFront()
{
    List* list = new List();

    int i1 = 1;
    list->insertFront(&i1);

    void* item1 = list->getHead()->getItem();
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertFront(&i2);

    void* item2 = list->getHead()->getItem();
    // testing the case there is another node
    TEST_ASSERT((*(int *) item2) == 2);

    delete list;
}

void testInsertLast()
{
    List* list = new List();

    int i1 = 1;
    list->insertLast(&i1);

    void* item1 = list->getTail()->getItem();
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertLast(&i2);
    
    void* item2 = list->getTail()->getItem();
    // testing the case there is another node
    TEST_ASSERT((*(int *) item2) == 2);

    delete list;
}

void testInsertAfterPos()
{
    List* list = new List();

    int i1 = 1;
    list->insertAfterPos(&i1, 0);

    void* item1 = list->getItemInPos(0);
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertAfterPos(&i2, 1);

    void* item2 = list->getItemInPos(2);
    // testing the case the number of entries is less or equal than one
    TEST_ASSERT((*(int *) item2) == 2);

    int i3 = 3;
    list->insertAfterPos(&i3, 1);

    void* item3 = list->getItemInPos(2);
    // testing the normal case
    TEST_ASSERT((*(int *) item3) == 3);

    int i4 = 4;
    list->insertAfterPos(&i4, 3);

    void* item4 = list->getItemInPos(4);
    // testing the case the position given is greater or equal to the number of entries
    TEST_ASSERT((*(int *) item4) == 4);

    delete list;
}

void testInsertBeforePos()
{
    List* list = new List();

    int i1 = 1;
    list->insertBeforePos(&i1, 0);

    void* item1 = list->getItemInPos(0);
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertBeforePos(&i2, 1);

    void* item2 = list->getItemInPos(1);
    // testing the case the number of entries is less or equal than one
    TEST_ASSERT((*(int *) item2) == 2);

    int i3 = 3;
    list->insertBeforePos(&i3, 1);

    void* item3 = list->getItemInPos(1);
    // testing the case the position given is less or equal than one
    TEST_ASSERT((*(int *) item3) == 3);

    int i4 = 4;
    list->insertBeforePos(&i4, 3);

    void* item4 = list->getItemInPos(3);
    // testing the case the position given is greater or equal to the number of entries
    TEST_ASSERT((*(int *) item4) == 4);

    int i5 = 5;
    list->insertBeforePos(&i5, 2);

    void* item5 = list->getItemInPos(2);
    // testing the normal case 
    TEST_ASSERT((*(int *) item5) == 5);

    delete list;
}

void testInsertAfterKeyNode()
{
    List* list = new List();

    int i1 = 1;
    list->insertAfterKeyNode(&i1, NULL, compare_ints);

    void* item1 = list->getItemInPos(1);
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertAfterKeyNode(&i2, NULL, compare_ints);

    void* item2 = list->getItemInPos(2);
    // testing the case the key is NULL
    TEST_ASSERT((*(int *) item2) == 2);

    int i3 = 3;
    list->insertAfterKeyNode(&i3, &i1, compare_ints);

    void* item3 = list->getItemInPos(2);
    // testing the normal case
    TEST_ASSERT((*(int *) item3) == 3);

    int i4 = 4;
    list->insertAfterKeyNode(&i4, NULL, compare_ints);

    void* item4 = list->getItemInPos(4);
    // testing the case we give NULL key
    TEST_ASSERT((*(int *) item4) == 4);

    delete list;
}

void testInsertBeforeKeyNode()
{
    List* list = new List();

    int i1 = 1;
    list->insertBeforeKeyNode(&i1, NULL, compare_ints);

    void* item1 = list->getItemInPos(1);
    // testing the case this is the only node
    TEST_ASSERT((*(int *) item1) == 1);

    int i2 = 2;
    list->insertBeforeKeyNode(&i2, NULL, compare_ints);

    void* item2 = list->getItemInPos(1);
    // testing the case this the second node
    TEST_ASSERT((*(int *) item2) == 2);

    int i3 = 3;
    list->insertBeforeKeyNode(&i3, &i1, compare_ints);

    void* item3 = list->getItemInPos(2);
    // testing the normal case
    TEST_ASSERT((*(int *) item3) == 3);

    int i4 = 4;
    list->insertBeforeKeyNode(&i4, NULL, compare_ints);

    void* item4 = list->getItemInPos(1);
    // testing the case we give NULL key
    TEST_ASSERT((*(int *) item4) == 4);

    delete list;
}

void testRemoveFront()
{
    List* list = new List();

    int i1 = 1;
    list->insertFront(&i1);

    list->removeFront();
    // testing case this is the only node
    TEST_ASSERT(list->getCounter() == 0);

    int i2 = 2;
    list->insertFront(&i1);
    list->insertFront(&i2);

    list->removeFront();
    // testing normal case 
    TEST_ASSERT(list->getCounter() == 1);
    void* item = list->getHead()->getItem();
    TEST_ASSERT((*(int *) item) == 1);

    delete list;
}

void testRemoveLast()
{
    List* list = new List();

    int i1 = 1;
    list->insertLast(&i1);

    list->removeLast();
    // testing case this is the only node
    TEST_ASSERT(list->getCounter() == 0);

    int i2 = 2;
    list->insertLast(&i1);
    list->insertLast(&i2);

    list->removeLast();
    // testing normal case 
    TEST_ASSERT(list->getCounter() == 1);
    void* item = list->getTail()->getItem();
    TEST_ASSERT((*(int *) item) == 1);

    delete list;
}

void testRemovePos()
{
    List* list = new List();

    int i1 = 1;
    list->insertLast(&i1);

    list->removePos(1);
    // testing case this is the only node
    TEST_ASSERT(list->getCounter() == 0);

    int i2 = 2;
    list->insertLast(&i1);
    list->insertLast(&i2);

    list->removePos(1);
    // testing case this is the first node
    TEST_ASSERT(list->getCounter() == 1);
    void* item = list->getHead()->getItem();
    TEST_ASSERT((*(int *) item) == 2);

    // reset and refill
    list->removeLast();
    list->insertLast(&i1);
    list->insertLast(&i2);

    list->removePos(2);
    // testing case this is the last node
    TEST_ASSERT(list->getCounter() == 1);
    void* item1 = list->getTail()->getItem();
    TEST_ASSERT((*(int *) item1) == 1);

    // reset and refill
    list->removeLast();
    int i3 = 3;
    list->insertLast(&i1);
    list->insertLast(&i2);
    list->insertLast(&i3);

    list->removePos(2);
    // testing the normal case
    TEST_ASSERT(list->getCounter() == 2);
    void* item2 = list->getItemInPos(2);
    TEST_ASSERT((*(int *) item2) == 3);

    delete list;
}

void testRemoveKeyNode()
{
    List* list = new List();

    int i1 = 1;
    list->insertLast(&i1);

    list->removeKeyNode(NULL, compare_ints, NULL);
    // testing case we didn't give key
    TEST_ASSERT(list->getCounter() == 1);

    list->removeKeyNode(&i1, compare_ints, NULL);
    // testing case this is the only node
    TEST_ASSERT(list->getCounter() == 0);

    int i2 = 2;
    list->insertLast(&i1);
    list->insertLast(&i2);

    list->removeKeyNode(&i1, compare_ints, NULL);
    // testing case this is the first node
    TEST_ASSERT(list->getCounter() == 1);
    void* item = list->getHead()->getItem();
    TEST_ASSERT((*(int *) item) == 2);

    // reset and refill
    list->removeLast();
    list->insertLast(&i1);
    list->insertLast(&i2);

    list->removeKeyNode(&i2, compare_ints, NULL);
    // testing case this is the last node
    TEST_ASSERT(list->getCounter() == 1);
    void* item1 = list->getTail()->getItem();
    TEST_ASSERT((*(int *) item1) == 1);

    // reset and refill
    list->removeLast();
    int i3 = 3;
    list->insertLast(&i1);
    list->insertLast(&i2);
    list->insertLast(&i3);

    list->removeKeyNode(&i2, compare_ints, NULL);
    // testing the normal case
    TEST_ASSERT(list->getCounter() == 2);
    void* item2 = list->getItemInPos(2);
    TEST_ASSERT((*(int *) item2) == 3);

    delete list;
}

void testSplit()
{
    List* list = new List();
    List* list1;
    List* list2;
    List* list3;
    List* list4;
    List* list5;
    List* list6;

    // Testing the case the initial list is empty
    list->split(&list1, &list2);
    TEST_ASSERT(list1->getCounter() == 0);
    TEST_ASSERT(list2->getCounter() == 0);

    int i1 = 1;
    int i2 = 2;
    list->insertLast(&i1);
    list->insertLast(&i2);
    
    // Testing the case the initial list has an even amount of elements
    list->split(&list3, &list4);
    TEST_ASSERT(list3->getCounter() == list4->getCounter());

    int i3 = 3;
    list->insertLast(&i3);

    // Testing the case the initial list has an even amount of elements
    list->split(&list5, &list6);
    // The second list will have more items than the first one
    TEST_ASSERT(list5->getCounter() <= list6->getCounter());

    delete list1;
    delete list2;
    delete list3;
    delete list4;
    delete list5;
    delete list6;
    delete list;
}

void testAppend()
{
    List* list = new List();
    List* list1 = new List();
    List* list2 = new List();
    List* list3 = new List();

    int i1 = 1;
    list1->insertLast(&i1);

    // Testing the case we append a list to an empty list
    list->append(list1);
    TEST_ASSERT(list->getCounter() == list1->getCounter());

    int prev_counter = list->getCounter();

    // Testing the case we append an empty list to a list
    list->append(list2);
    TEST_ASSERT(list->getCounter() == prev_counter);

    prev_counter = list->getCounter();

    int i2 = 2;
    list3->insertLast(&i2);

    // Testing the normal case
    list->append(list3);
    TEST_ASSERT(list->getCounter() == prev_counter + list3->getCounter());
    // The given list is appended at the end of the current list
    TEST_ASSERT(list->getTail()->getItem() == list3->getTail()->getItem());

    delete list1;
    delete list2;
    delete list3;
    delete list;
}

/**************************************************************************
 *                       Partitioned Hash Join Test                       *
 **************************************************************************/

void partitionedHashJoinTest()
{
    PartitionedHashJoinInput *phji = new PartitionedHashJoinInput("../config.txt");
    Relation *L, *R;
    FileReader::readInputFile("../test.txt", &L, &R);
    PartitionedHashJoin phj = PartitionedHashJoin(L, R, phji);

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
    delete phji;
    
}

/**************************************************************************
 *                                  Query                                 *
 **************************************************************************/

void predicatesParserTest()
{
    char predicate[10] = "2.0 = 3.4";

    PredicatesParser pp(predicate);

    TEST_ASSERT(pp.getLeftArray() == 2);
    TEST_ASSERT(pp.getLeftArrayColumn() == 0);
    TEST_ASSERT(pp.getRightArray() == 3);
    TEST_ASSERT(pp.getRightArrayColumn() == 4);
    TEST_ASSERT(pp.hasConstant() == false);

    char predicate1[9] = "1.1 < 69";
    PredicatesParser pp1(predicate1);

    TEST_ASSERT(pp1.getLeftArray() == 1);
    TEST_ASSERT(pp1.getLeftArrayColumn() == 1);
    TEST_ASSERT(pp1.getFilterOperator() == '<');
    TEST_ASSERT(pp1.getFilterValue() == 69);
    TEST_ASSERT(pp1.hasConstant() == true);

}

void projectionsParserTest()
{
    char projection[4] = "1.5";
    ProjectionsParser pp(projection);

    TEST_ASSERT(pp.getArray() == 1);
    TEST_ASSERT(pp.getColumn() == 5);
}


/***************************************************************************
 *                                 Trees                                   *
 **************************************************************************/

void abTreeInsertTest()
{
    AB_Tree *testTree = new AB_Tree(3,5);
    int_data = new int[20]; 
    int* testArray = new int[20];

    for (int i = 0; i < 20; i++)
    {
        testArray[i] = i;
    }

    for (int i = 0; i < 20; i++)
    {
        bool *insertionWasSuccessful = new bool();
        testTree->insert(&testArray[i], &testArray[i], compare_ints, insertionWasSuccessful);
        TEST_ASSERT(*insertionWasSuccessful == true);
        int_data[i] = i;
        delete insertionWasSuccessful;
    }

    testTree->traverse(Inorder, test_compare);

    delete testTree;
    delete[] int_data;
    delete[] testArray;
}

void abTreeSearchTest()
{
    AB_Tree * testTree = new AB_Tree(3,5);
    int* testArray = new int[20];

    for (int i = 0; i < 20; i++)
    {
        testArray[i] = i;
    }

    for (int i = 0; i < 20; i++)
    {
        testTree->insert(&testArray[i], &testArray[i], compare_ints, NULL);
    }

    for(int i = 0; i < 20; i++)
    {
        TEST_ASSERT(testTree->search(&testArray[i], compare_ints) == true);
    }

    delete testTree;
    delete[] testArray;
}

void abTreeSearchandReturnTest()
{
    AB_Tree * testTree = new AB_Tree(3,5);
    int* testArray = new int[20];

    for (int i = 0; i < 20; i++)
    {
        testArray[i] = i;
    }

    for (int i = 0; i < 20; i++)
    {
        testTree->insert(&testArray[i], &testArray[i], compare_ints, NULL);
    }

    for(int i = 0; i < 20; i++)
    {
        void *retrievedItem, *retrievedKey;
        TEST_ASSERT(testTree->searchAndRetrieve(&testArray[i], compare_ints, &retrievedItem, &retrievedKey) == true);
        int key = *(int*)retrievedKey;
        int value = *(int*)retrievedItem;
        TEST_ASSERT(key == testArray[i]);
        TEST_ASSERT(value == testArray[i]);
    }

    delete testTree;
    delete[] testArray;
}

void abTreeRemoveTest()
{
    AB_Tree * testTree = new AB_Tree(3,5);
    int* testArray = new int[20];

    for (int i = 0; i < 20; i++)
    {
        testArray[i] = i;
    }

    for (int i = 0; i < 20; i++)
    {
        testTree->insert(&testArray[i], &testArray[i], compare_ints, NULL);
    }

    bool *removalWasSuccessful = new bool;
    int removed = 13;
    testTree->remove(&removed, compare_ints, removalWasSuccessful);
    TEST_ASSERT(*removalWasSuccessful == true);
    TEST_ASSERT(testTree->search(&testArray[13], compare_ints) == false);
    TEST_ASSERT(testTree->getCounter() == 19);

    delete removalWasSuccessful;
    delete testTree;
    delete[] testArray;
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
    { "Reading Init File", read_init_file_test},
    // Hash Table Test
    { "Hash Insert",  insertWithoutRehashTest},
    { "Hash Rehash",  rehashTest},
    // { "Hash Bulk Search",  bulkSearchTest},
    // Intermediate Array
    { "Intermediate Array Search", testIntermediateArraySearch},
    { "Intermediate Array Execute Join With Foreign Relation", testExecuteJoinWithForeignRelation},
    { "Intermediate Array Execute Join With Two Relations In The Array", testExecuteJoinWithTwoRelationsInTheArray},
    { "Intermediate Array Execute Join With Relation Of Other Array", testExecuteJoinWithRelationOfOtherArray},
    { "Intermediate Array Execute Filter" , testExecuteFilter},
    // List Test
    { "List Insert Front", testInsertFront},
    { "List Insert Last", testInsertLast},
    { "List Insert After Given Position", testInsertAfterPos},
    { "List Insert Before Given Position", testInsertBeforePos},
    { "List Insert After Given Key", testInsertAfterKeyNode},
    { "List Insert Before Given Key", testInsertBeforeKeyNode},
    { "List Remove Front", testRemoveFront},
    { "List Remove Last", testRemoveLast},
    { "List Remove From Given Position", testRemovePos},
    { "List Remove Given Key", testRemoveKeyNode},
    { "List Split", testSplit},
    { "List Append", testAppend},
    // Partitioned Hash Join Testing
    { "Partitioned Hash Join", partitionedHashJoinTest},
    // Query
    { "Predicates Parser Test", predicatesParserTest},
    { "Projections Parser Test", projectionsParserTest},
    // Trees
    { "AB_Tree Insert Test", abTreeInsertTest},
    { "AB_Tree Search Test", abTreeSearchTest},
    { "AB_Tree Search And Return Test", abTreeSearchandReturnTest},
    { "AB_Tree Remove Test", abTreeRemoveTest},
    { NULL, NULL }
};
