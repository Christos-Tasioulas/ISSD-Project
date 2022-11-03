#ifndef _HASH_TABLE_ENTRY_H_
#define _HASH_TABLE_ENTRY_H_

#include "Bitmap.h"

/* The default bit capacity of the bitmap of the entry, used
 * in case the user does not give their own bit capacity
 */
#define DEFAULT_BIT_CAPACITY 16

/* The Hash Table Entry (which is the content of a bucket) */

class HashTableEntry {

private:

/* The primary data the user wants to store in the hash table */
	void *item;

/* A key that accompanies the user data, used for hashing */
	void *key;

/* An array of bits used to implement the Hopscotch Hashing
 * Collision Resolution Policy
 */
	Bitmap *hopInformation;

/* The capacity of bits of the hop information bitmap */
	unsigned int bitCapacity;

public:

/* Constructor */
	HashTableEntry(void *item = NULL, void *key = NULL,
		unsigned int bitCapacity = DEFAULT_BIT_CAPACITY);

/* Destructor */
	~HashTableEntry();

/* Getter - Returns the item stored in the entry */
	void *getItem() const;

/* Getter - Returns the key stored in the entry */
	void *getKey() const;

/* Getter - Returns the bitmap of the entry */
	Bitmap *getHopInformation() const;

/* Getter - Returns the capacity of bits of the bitmap */
	unsigned int getBitCapacity() const;

/* Stores the user's item and key in its corresponding fields */
	void updateData(void *item, void *key);

/* Removes the item and the key that are being stored in the entry */
	void removeData();

/* Returns 'true' if this entry is empty (not occupied by any data) */
	bool isAvailable() const;

/* Returns 'true' if every bit of the bitmap of the entry has value 1 */
	bool isFull() const;

/* Prints the contents of the entry */
	void print(void (*visitItemAndKey)(void *, void *),
		void (*contextBetweenDataAndBitmap)() = NULL) const;

/* Destroys the current bitmap and recreates it with a new capacity */
	void resetBitmap(unsigned int newBitCapacity);

};

#endif
