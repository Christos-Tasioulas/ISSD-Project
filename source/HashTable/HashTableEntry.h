#ifndef _HASH_TABLE_ENTRY_H_
#define _HASH_TABLE_ENTRY_H_

#include "Bitmap.h"
#include "List.h"
#include "ComplexItem.h"

/* The default bit capacity of the bitmap of the entry, used
 * in case the user does not give their own bit capacity
 */
#define DEFAULT_BIT_CAPACITY 64

typedef ComplexItem HashEntryItem;

/* The Hash Table Entry (which is the content of a bucket) */

class HashTableEntry {

private:

/* A list of all the items inserted in this entry.
 * Items with equal keys are inserted in the same entry.
 */
	List *items;

/* An array of bits used to implement the Hopscotch Hashing
 * Collision Resolution Policy
 */
	Bitmap *hopInformation;

/* The capacity of bits of the hop information bitmap */
	unsigned int bitCapacity;

/* Prints a 'HashEntryItem' */
	static void printHashEntryItem(void *item);

/* Deletes a 'HashEntryItem' */
	static void deleteHashEntryItem(void *item);

public:

/* Constructor */
	HashTableEntry(void *item = NULL, void *key = NULL,
		unsigned int bitCapacity = DEFAULT_BIT_CAPACITY);

/* Destructor */
	~HashTableEntry();

/* Getter - Returns the list of items stored in the entry */
	List *getItems() const;

/* Getter - Returns the bitmap of the entry */
	Bitmap *getHopInformation() const;

/* Getter - Returns the capacity of bits of the bitmap */
	unsigned int getBitCapacity() const;

/* Inserts a new pair of item and key in the entry */
	void insertItem(void *item, void *key);

/* Swaps the items of this entry with the items of another entry */
	void swap(HashTableEntry *other);

/* Returns 'true' if this entry is empty (not occupied by any data) */
	bool isAvailable() const;

/* Returns 'true' if every bit of the bitmap of the entry has value 1 */
	bool isFull() const;

/* Prints the contents of the entry */
	void print(
		void (*visitItemAndKey)(void *, void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenItemsAndBitmap)() = NULL
	) const;

/* Destroys the current bitmap and recreates it with a new capacity */
	void resetBitmap(unsigned int newBitCapacity);

};

#endif
