#include <iostream>
#include "HashTableEntry.h"

/******************************************************
 * The user's visit function for the item and the key *
 ******************************************************/

static void (*userVisitItemAndKey)(void *item, void *key);

/****************************
 * Prints a 'HashEntryItem' *
 ****************************/

void HashTableEntry::printHashEntryItem(void *item)
{
	/* We cast the item to its original type */
	HashEntryItem *entryItem = (HashEntryItem *) item;

	/* We save the user's function to a seperate function pointer
	 * in order not to lose it forever if another hash entry is
	 * printed within the printing actions of the current entry
	 * (because in that case the user function would be overwritten
	 * by the inner hash entry printing call).
	 */
	void (*tempUserVisitItemAndKey)(void *, void *) = userVisitItemAndKey;

	/* We print the item and the key with the user's function */
	userVisitItemAndKey(entryItem->getItem(), entryItem->getKey());

	/* We assign the initial user function that was given */
	userVisitItemAndKey = tempUserVisitItemAndKey;
}

/*****************************
 * Deletes a 'HashEntryItem' *
 *****************************/

void HashTableEntry::deleteHashEntryItem(void *item)
{
	delete (HashEntryItem *) item;
}

/***************
 * Constructor *
 ***************/

HashTableEntry::HashTableEntry(void *item, void *key,
	unsigned int bitCapacity)
{
	/* We initialize the list of items */
	items = new List();

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
	items->traverseFromHead(deleteHashEntryItem);
	delete items;
	delete hopInformation;
}

/**********************************************************
 * Getter - Returns the list of items stored in the entry *
 **********************************************************/

List *HashTableEntry::getItems() const
{
	return items;
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

void HashTableEntry::insertItem(void *item, void *key)
{
	items->insertLast(new HashEntryItem(item, key));
}

/*****************************************************************
 * Swaps the items of this entry with the items of another entry *
 *****************************************************************/

void HashTableEntry::swap(HashTableEntry *other)
{
	List *otherItems = other->items;
	other->items = this->items;
	this->items = otherItems;
}

/********************************************************************
 * Returns 'true' if this entry is empty (not occupied by any data) *
 ********************************************************************/

bool HashTableEntry::isAvailable() const
{
	return (items->getCounter() == 0);
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

void HashTableEntry::print(
	void (*visitItemAndKey)(void *, void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenItemsAndBitmap)()) const
{
	/* We assign the user's function to the static function pointer */
	userVisitItemAndKey = visitItemAndKey;

	/* We print the list of items from the head */
	items->printFromHead(printHashEntryItem, contextBetweenItems);

	/* If a no-null operation that prints some context
	 * between the user data and the bitmap, we use it
	 */
	if(contextBetweenItemsAndBitmap != NULL)
		contextBetweenItemsAndBitmap();

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
