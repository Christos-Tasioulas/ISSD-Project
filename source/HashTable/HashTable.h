#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "HashTableEntry.h"

class HashTable {

private:

/* The Hash Table is represented by an array of Hash Table Entries */
	HashTableEntry *table;

/* The amount of occupied entries in the hash table */
	unsigned int elementsNum;

/* The total capacity of the hash table */
	unsigned int bucketsNum;

/* Determines whether the total capacity of items may change */
	bool isResizable;

/* If the 'isResizable' field allows the table to change size,
 * then the table will do so if the quotient elements/buckets
 * becomes greater than the value stored in the load factor.
 */
	double loadFactor;

/* The desired capacity (in bits) of each hop information
 * bitmap that is used to implement Hopscotch Hashing
 */
	unsigned int hopInfoCapacity;

/* Creates a new hash table with double size and hop range
 * and inserts all the items of the current table to the new
 * one. This action creates more capacity for the table and
 * is vital to preserve the properties of the structure when
 * large amounts of input are inserted in the table.
 */
	void rehash(
		unsigned int (*hash_function)(void *),
		int (*compare)(void *, void *)
	);

/* Searches the given key in the hash table. If there exists
 * at least one key in the hash table that is compared equal
 * to the given key by the given compare method, the position
 * in the hash table of the entry containing the key is returned
 */
	unsigned int searchPos(
		void *key,
		unsigned int (*hash_function)(void *),
		int (*compare)(void *, void *)
	);

public:

/* Constructor */
	HashTable(unsigned int startingSize = 101,
		bool isResizable = true,
		double loadFactor = 0.8,
		unsigned int hopInfoCapacity = DEFAULT_BIT_CAPACITY);

/* Destructor */
	~HashTable();

/* Getter - Returns the array representing the hash table */
	HashTableEntry *getTable() const;

/* Getter - Returns the amount of occupied entries in the hash table */
	unsigned int getElementsNum() const;

/* Getter - Returns the number of buckets in the table */
	unsigned int getBucketsNum() const;

/* Getter - Returns the resizability option */
	bool getIsResizable() const;

/* Getter - Returns the load factor of the table */
	double getLoadFactor() const;

/* Getter - Returns the bit capacity of the bitmaps */
	unsigned int getHopInfoCapacity() const;

/* Inserts a new pair of item and key in the hash table */
	void insert(
		void *item,
		void *key,
		unsigned int (*hash_function)(void *),
		int (*compare)(void *, void *)
	);

/* Searches the given key in the hash table
 *
 * Returns a linked list of all the keys inside
 * the hash table that match the given key.
 *
 * The list must be terminated after use with
 * the operation 'terminateBulkSearchList'.
 *
 * Even if the list was empty, it must still be
 * terminated using the same operation.
 */
	List *bulkSearchKeys(
		void *key,
		unsigned int (*hash_function)(void *),
		int (*compare)(void *, void *)
	);

/* Terminates the result returned by 'bulkSearch' */
	static void terminateBulkSearchList(List *bulkSearchResult);

/* Prints all the contents of the hash table */
	void print(
		void (*visitItemAndKey)(void *, void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenDataAndBitmap)() = NULL,
		void (*contextBetweenHashEntries)() = NULL,
		void (*emptyEntryPrinting)() = NULL
	) const;

};

#endif
