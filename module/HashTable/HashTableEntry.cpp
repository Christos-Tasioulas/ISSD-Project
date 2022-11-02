#include <iostream>
#include "HashTableEntry.h"

/***************
 * Constructor *
 ***************/

HashTableEntry::HashTableEntry(void *item, void *key,
	unsigned int bitCapacity)
{
	/* We set the given item to the 'item' field */
	this->item = item;

	/* We set the given key to the 'key' field */
	this->key = key;

	/* We initialize the bit capacity of the bitmap */
	this->bitCapacity = bitCapacity;

	/* We initialize the hop information bitmap */
	hopInformation = new Bitmap(bitCapacity);
}

/**************
 * Destructor *
 **************/

HashTableEntry::~HashTableEntry()
{
	delete hopInformation;
}

/*************************************************
 * Getter - Returns the item stored in the entry *
 *************************************************/

void *HashTableEntry::getItem() const
{
	return item;
}

/************************************************
 * Getter - Returns the key stored in the entry *
 ************************************************/

void *HashTableEntry::getKey() const
{
	return key;
}

/********************************************
 * Getter - Returns the bitmap of the entry *
 ********************************************/

Bitmap *HashTableEntry::getHopInformation() const
{
	return hopInformation;
}

/*******************************************************
 * Getter - Returns the capacity of bits of the bitmap *
 *******************************************************/

unsigned int HashTableEntry::getBitCapacity() const
{
	return bitCapacity;
}

/**************************************************************
 * Stores the user's item and key in its corresponding fields *
 **************************************************************/

void HashTableEntry::updateData(void *item, void *key)
{
	/* We store the given user's item to the 'item' field */
	this->item = item;

	/* We store the given user's key to the 'key' field */
	this->key = key;
}

/*******************************************************************
 * Removes the item and the key that are being stored in the entry *
 *******************************************************************/

void HashTableEntry::removeData()
{
	/* We detach the current item by setting the 'item' field to 'NULL' */
	item = NULL;

	/* We detach the current key by setting the 'key' field to 'NULL' */
	key = NULL;
}

/********************************************************************
 * Returns 'true' if this entry is empty (not occupied by any data) *
 ********************************************************************/

bool HashTableEntry::isAvailable() const
{
	return (item == NULL) && (key == NULL);
}

/**********************************************************************
 * Returns 'true' if every bit of the bitmap of the entry has value 1 *
 **********************************************************************/

bool HashTableEntry::isFull() const
{
	return hopInformation->isFull();
}

/************************************
 * Prints the contents of the entry *
 ************************************/

void HashTableEntry::print(void (*visitItemAndKey)(void *, void *),
	void (*contextBetweenDataAndBitmap)()) const
{
	/* We visit the item and the key stored in the entry */
	visitItemAndKey(item, key);

	/* If a no-null operation that prints some context
	 * between the user data and the bitmap, we use it
	 */
	if(contextBetweenDataAndBitmap != NULL)
		contextBetweenDataAndBitmap();

	/* Finally, we print the bitmap of this entry */
	hopInformation->print();
}

/********************************************************************
 * Destroys the current bitmap and recreates it with a new capacity *
 ********************************************************************/

void HashTableEntry::resetBitmap(unsigned int newBitCapacity)
{
	/* We destroy the bitmap and recreate it with the new size */
	hopInformation->resetAndChangeSize(newBitCapacity);

	/* We update the bit capacity field of the class */
	bitCapacity = newBitCapacity;
}
