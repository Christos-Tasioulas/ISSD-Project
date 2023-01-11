#include <iostream>
#include "HashTable.h"

/***************
 * Constructor *
 ***************/

HashTable::HashTable(unsigned int startingSize, bool isResizable,
	double loadFactor, unsigned int hopInfoCapacity)
{
	/* We initialize the Hash Table with the given starting size */
	table = new HashTableEntry[startingSize];

	/* Helper variable for counting */
	unsigned int i;

	/* Since we are using an array of hash table entries to represent
	 * the hash table, each entry has been initialized in the default
	 * way (with the default constructor arguments). However, the
	 * default value of the hop information bitmap capacity may not
	 * be the desired one. In this case, we need to change the bit
	 * capacity of the bitmap of each entry.
	 */
	if(hopInfoCapacity != DEFAULT_BIT_CAPACITY)
	{
		for(i = 0; i < startingSize; i++)
			table[i].resetBitmap(hopInfoCapacity);
	}

	/* We store the given bit capacity to the 'hopInfoCapacity' field */
	this->hopInfoCapacity = hopInfoCapacity;

	/* We initialize the number of inserted elements to zero */
	elementsNum = 0;

	/* We initialize the number of buckets to the given size
	 * (here a bucket is in other words a Hash Table Entry)
	 */
	bucketsNum = startingSize;

	/* We store the user's preference on whether the hash table
	 * should be resizable in the 'isResizable' field of the class
	 */
	this->isResizable = isResizable;

	/* We store the given load factor */
	this->loadFactor = loadFactor;
}

/**************
 * Destructor *
 **************/

HashTable::~HashTable()
{
	delete[] table;
}

/**********************************************************
 * Getter - Returns the array representing the hash table *
 **********************************************************/

HashTableEntry *HashTable::getTable() const
{
	return table;
}

/********************************************************
 * Getter - Returns the number of elements in the table *
 ********************************************************/

unsigned int HashTable::getElementsNum() const
{
	return elementsNum;
}

/*******************************************************
 * Getter - Returns the number of buckets in the table *
 *******************************************************/

unsigned int HashTable::getBucketsNum() const
{
	return bucketsNum;
}

/********************************************
 * Getter - Returns the resizability option *
 ********************************************/

bool HashTable::getIsResizable() const
{
	return isResizable;
}

/*************************************************
 * Getter - Returns the load factor of the table *
 *************************************************/

double HashTable::getLoadFactor() const
{
	return loadFactor;
}

/****************************************************
 * Getter - Returns the bit capacity of the bitmaps *
 ****************************************************/

unsigned int HashTable::getHopInfoCapacity() const
{
	return hopInfoCapacity;
}

/*************************************************************
 *  Creates a new hash table with double size and hop range  *
 * and inserts all the items of the current table to the new *
 * one. This action creates more capacity for the table and  *
 * is vital to preserve the properties of the structure when *
 *     large amounts of input are inserted in the table      *
 *************************************************************/

void HashTable::rehash(unsigned int (*hash_function)(void *), int (*compare)(void *, void*))
{
	/* We will create a new table with double number of buckets */
	unsigned int newBucketsNum = 2 * bucketsNum;
/*
	std::cout << "Rehashing with " << elementsNum << " elements (buckets: "
		<< bucketsNum << " -> " << newBucketsNum << ", range: " << hopInfoCapacity
		<< " -> " << 3*(hopInfoCapacity/2) << ")" << std::endl;
*/
	/* In the new table the hop range will be also change */
	hopInfoCapacity = 3*(hopInfoCapacity/2);

	/* Here we create the new table in the heap */
	HashTableEntry *newTable = new HashTableEntry[newBucketsNum];

	/* Initially the new table has no occupied entries */
	this->elementsNum = 0;

	/* We store the old number of buckets to a seperate variable */
	unsigned int oldBucketsNum = this->bucketsNum;

	/* We store the new amount of buckets in 'bucketsNum' */
	this->bucketsNum = newBucketsNum;

	/* We create a new variable that points to the old table */
	HashTableEntry *temporaryTable = this->table;

	/* The 'table' field now stores the new table */
	this->table = newTable;

	/* Helper variable used for counting */
	unsigned int i;

	/* Because the hash table stores plain hash table entries
	 * and not pointers of such entries, all of the entries of
	 * the array are initialized with the default arguments of
	 * the entry constructor. But if the default value for the
	 * bit capacity (= hop range) is not the one we want to use,
	 * we need to reset the bitmap of each entry, providing the
	 * desired value for the bit capacity.
	 */
	if(hopInfoCapacity != DEFAULT_BIT_CAPACITY)
	{
		for(i = 0; i < newBucketsNum; i++)
			table[i].resetBitmap(hopInfoCapacity);
	}

	/* We re-insert every element of the old table to the new */

	for(i = 0; i < oldBucketsNum; i++)
	{
		/* The empty entries are not inserted in the table.
		 *
		 * We only insert entries with user data in them.
		 */
		if(!temporaryTable[i].isAvailable())
		{
			/* We retrieve the items of the current entry */
			List *items = temporaryTable[i].getItems();
			Listnode *currentNode = items->getHead();

			while(currentNode != NULL)
			{
				/* We retrieve the item & key stored in the current node */
				HashEntryItem *entryItem = (HashEntryItem *) currentNode->getItem();

				/* We insert the current pair of item & key in the new table */
				insert(entryItem->getItem(), entryItem->getKey(), hash_function, compare);

				/* We proceed to the next node */
				currentNode = currentNode->getNext();
			}
		}
	}

	/* We destroy the old table */
	delete[] temporaryTable;
}

/********************************************************
 * Inserts a new pair of item and key in the hash table *
 ********************************************************/

void HashTable::insert(void *item, void *key,
	unsigned int (*hash_function)(void *), int (*compare)(void *, void *))
{
	/* First we hash the key to find out the bucket where
	 * the pair is supposed to go. We always take care so
	 * as the hash value is lower than the num of buckets
	 */
	unsigned int hash_value = hash_function(key) % bucketsNum;

	/* We search if there is already an existing key in the
	 * table which is compared as equal with the given key.
	 *
	 * The amount of buckets is returned in case of unsuccessful search.
	 */
	unsigned int identicalElementsPos = searchPos(key, hash_function, compare);

	/* Case the search was successful */

	if(identicalElementsPos != bucketsNum)
	{
		/* We insert the item in the same entry as those with identical key */
		table[identicalElementsPos].insertItem(item, key);

		/* There is nothing else to do in this case */
		return;
	}

	/* If the hop information bitmap of the targeted entry
	 * indicates that the neighborhood is full, we need
	 * to grow the table and the neighborhood range and
	 * rehash every key before we can continue with the
	 * insertion of the new element
	 */
	if(table[hash_value].isFull())
	{
		/* We grow the table and rehash every key */
		rehash(hash_function, compare);

		/* We try to insert the element in the new table */
		insert(item, key, hash_function, compare);

		/* When the insertion ends, we return immediatelly */
		return;
	}

	/* Helper variable used for counting */
	unsigned int i = 0;

	/* Helper variable used to point to the index of an entry */
	unsigned int j = hash_value;

	/* We will find the first empty entry, starting from the
	 * targeted entry, by serially traversing the entries.
	 */
	for(i = 0; i < bucketsNum; i++)
	{
		/* If the current entry is available, that means it
		 * has no user data in it, which means it is empty
		 *
		 * Consequently, we just found the first empty entry.
		 */
		if(table[j].isAvailable())
			break;

		/* This part is reached if the entry is not empty.
		 *
		 * We make 'j' point to the next entry. If 'j' is
		 * currently pointing to the last entry, it will
		 * be set to point to the first. Essentially, the
		 * hash table is considered to be a circular table.
		 */
		j = (j+1) % bucketsNum;
	}

	/* Case no available entry was found in the whole table.
	 *
	 * We need to rehash and try the insertion again.
	 */
	if(i == bucketsNum)
	{
		/* We grow the table and rehash every key */
		rehash(hash_function, compare);

		/* We try to insert the element in the new table */
		insert(item, key, hash_function, compare);

		/* When the insertion ends, we return immediatelly */
		return;
	}

	/* This is the distance between 'j' and the bucket
	 * where the new key was initially hashed to
	 */
	unsigned int distanceFromTargetedBucket;

	/* In this implementation we are using unsigned variables.
	 * Consequently, we cannot handle negative values with
	 * these variables. The distance here must generally be
	 * 'j - hash_value', but if the result of this subtraction
	 * is negative, we will get wrong results. Therefore, we
	 * use the 'if' block below to handle this case and assign
	 * the correct value of the distance.
	 */
	if(hash_value > j)
	{
		/* We find how much larger than 'j' the hash value is */
		unsigned int positive_difference = hash_value - j;

		/* Then the desired index is found by subtracting
		 * the above difference from the number of buckets
		 */
		distanceFromTargetedBucket = bucketsNum - positive_difference;
	}

	/* Case the result of 'j - hash_value' is non-negative
	 * (this is the normal case)
	 */
	else
		distanceFromTargetedBucket = j - hash_value;

	/* Now we have found the first empty entry in the table
	 * starting from the bucket where the key was hashed.
	 *
	 * This empty entry is called 'j'.
	 *
	 * If the distance of 'j' from the bucket where the key
	 * was hashed is greater than the neighbourhood of that
	 * bucket, we need to perform some actions to bring the
	 * empty entry closer to the bucket. All these actions
	 * are taken in the 'while' loop below.
	 *
	 * We perform these actions as long as 'j' is not in the
	 * range of the neighbourhood of the targeted bucket.
	 */
	while((distanceFromTargetedBucket % bucketsNum) >= hopInfoCapacity)
	{
		/* We will "move" the empty entry closer to 'j' by
		 * displacing another item in a previous position.
		 *
		 * This is the max range of previous entries we can
		 * search to choose one for displacement
		 */
		unsigned int maxRange = hopInfoCapacity - 1;

		/* This is the index of the entry which will be
		 * chosen for displacement. Initially we assign
		 * an index out of bounds to the displacement
		 * target. After the end of the procedure, if
		 * an item for displacement has been found, the
		 * displacement target will be overwritten with
		 * a legal value in bounds. If this has not
		 * happened until then, that means there was no
		 * valid item for displacement and consequently
		 * we need to perform rehashing.
		 */
		unsigned int displacementTarget = bucketsNum;

		/* This is the index of the entry where the
		 * displacement target was originally hashed
		 * to. We will need to access the bitmap of
		 * this entry later.
		 */
		unsigned int displacedKeyHashValue;

		/* This variable indicates the position of the
		 * first bit with value 1 in the bitmap of the
		 * entry where the displaced key was originally
		 * hashed to. The position of the first ace of
		 * that bitmap indicates the entry we can displace
		 */
		unsigned int posOfFirstAceInMap;

		/* In the loop below we will try to find an entry inside
		 * the max range of 'j' whose content can be displaced
		 * to 'j'. We will begin examining the farthest entry in
		 * range of 'j', because the farther we can choose, the
		 * better it is. By choosing a far position we go closer
		 * to the neighborhood of the bucket where the new key
		 * was originally hashed to. If the farthest entry cannot
		 * be chosen, we will examine the 2nd farthest and so on.
		 */
		for(i = 0; i < maxRange; i++)
		{
			/* This is the index of the entry that we will examine
			 * if this loop. We are examining if this entry or one
			 * one in its neighborhood before 'j' can be displaced
			 * to 'j'
			 */
			unsigned int nextTarget;

			/* We need to take care when choosing the next target.
			 *
			 * The table is circular and we are traversing it backwards.
			 * We are also using unsigned variables for greater positive
			 * range, so we cannot represent negative values. If we are
			 * currently on entry zero and want to go backwards, simply
			 * subtracting 1 would cause an underflow, which would bring
			 * wrong results. In this case we need to go to the end of
			 * the table and continue traversing from there.
			 *
			 * In the below if-else block we handle the selection of the target.
			 *
			 * Case the range has greater value than 'j + i'.
			 *
			 * In this case the result would underflow, because we are
			 * using unsigned integers for greater positive range.
			 *
			 * We handle this special case in the 'if' block below.
			 */
			if(maxRange > j + i)
			{
				/* We find how much larger the max range is */
				unsigned int diff = maxRange - j - i;

				/* Then the desired index is found by subtracting
				 * the above difference from the number of buckets
				 */
				nextTarget = bucketsNum - diff;
			}

			/* Case the result of the subtraction is non-negative
			 * (this is the normal case)
			 */
			else
				nextTarget = j + i - maxRange;

			/* Now we have chosen the next target. We examine
			 * if we can displace its content or the content of
			 * one of its neighborhoods before 'j'
			 *
			 * We retrieve the bitmap of the newly targeted entry
			 */
			Bitmap *nextTargetBitmap = table[nextTarget].getHopInformation();

			/* We find the first ace in that bitmap. It represents
			 * the place of the farthest entry we can choose for
			 * displacement.
			 */
			posOfFirstAceInMap = nextTargetBitmap->posOfFirstAce();

			/* If the first ace exists and is in range,
			 * we found the target we will displace
			 */
			if((posOfFirstAceInMap != 0) && (posOfFirstAceInMap <= maxRange - i))
			{
				/* The displacement target is 'posOfFirstAceInMap' entries
				 * after 'nextTarget'. We decrease by 1 because the bitmap
				 * positions start from 1 but the first position indicates
				 * the 'nextTarget' itself. Also, the table is always circular.
				 */
				displacementTarget = (nextTarget + posOfFirstAceInMap - 1) % bucketsNum;

				/* That target's hash value was 'nextTarget' */
				displacedKeyHashValue = nextTarget;

				/* Since we found the displacement target, we escape */
				break;
			}
		}

		/* Case we did not find any target for displacement */

		if(displacementTarget == bucketsNum)
		{
			/* We grow the table and rehash every key */
			rehash(hash_function, compare);

			/* We try to insert the element in the new table */
			insert(item, key, hash_function, compare);

			/* When the insertion ends, we return immediatelly */
			return;
		}

		/* We transfer the data of the displacement target to 'j' */

		table[j].swap(&table[displacementTarget]);

		/* This is the distance between 'j' and the displacement
		 * target. We will use it to make 'j' point to that entry
		 * from now on.
		 */
		unsigned int distance;

		/* We handle specially the limits of the array and
		 * the limits posed by the unsigned integers
		 *
		 * Generally: distance = 'j - displacementTarget'
		 *
		 * But if 'j' < 'displacementTarget' -> underflow
		 *
		 * Case 'j' is smaller than 'displacementTarget'
		 */
		if(displacementTarget > j)
		{
			/* We find how much larger the the target is */
			unsigned int diff = displacementTarget - j;

			/* Then the desired distance is found by subtracting
			 * the above difference from the number of buckets
			 */
			distance = bucketsNum - diff;
		}

		/* Case the result of the subtraction is non-negative
		 * (this is the normal case)
		 */
		else
			distance = j - displacementTarget;

		/* We retrieve the bitmap of the bucket where the
		 * data of the next target was originally hashed to
		 */
		Bitmap *hashHopInfo = table[displacedKeyHashValue].getHopInformation();

		/* We set to zero the first ace in the bitmap we found,
		 * because the element responsible for that 1 has now
		 * been transfered and an empty entry is in its potision
		 */
		hashHopInfo->unsetBit(posOfFirstAceInMap);

		/* The element was transfered 'distance' entries next
		 * So the bit in that position is now turned on to 1.
		 */
		hashHopInfo->setBit(posOfFirstAceInMap + distance);

		/* The glorious 'j' now becomes the entry
		 * whose element was just displaced
		 */
		j = displacementTarget;

		/* We find the distance of the new 'j' from the bucket
		 * where the new key was originally hashed to
		 */
		if(hash_value > j)
		{
			/* We find how much larger than 'j' the hash value is */
			unsigned int positive_difference = hash_value - j;

			/* Then the desired index is found by subtracting
			 * the above difference from the number of buckets
			 */
			distanceFromTargetedBucket = bucketsNum - positive_difference;
		}

		/* Case the result of 'j - hash_value' is non-negative
		 * (this is the normal case)
		 */
		else
			distanceFromTargetedBucket = j - hash_value;
	}

	/* We insert the new element in the entry 'j' */
	table[j].insertItem(item, key);

	/* We retrieve the bitmap of the bucket where the
	 * new key was originally hashed to and we set
	 * to 1 the bit of the bitmap that points to 'j'
	 */
	table[hash_value].getHopInformation()->setBit(distanceFromTargetedBucket + 1);

	/* We just inserted a new element in the hash table
	 * We increase the number of elements by 1
	 */
	elementsNum++;

	/* If the 'isResizable' option is enabled, that means the table will
	 * not only grow under the Hopscotch intention, but also when the
	 * amount of elements in the table divided by the number of buckets
	 * surpasses the load factor of the table
	 */
	if(isResizable)
	{
		if(((double) elementsNum) / ((double) bucketsNum) >= loadFactor)
			rehash(hash_function, compare);
	}
}

/************************************************
 *   Searches the given key in the hash table   *
 *                                              *
 * Returns a linked list of all the keys inside *
 *   the hash table that match the given key.   *
 *                                              *
 *  The list must be terminated after use with  *
 *   the operation 'terminateBulkSearchList'.   *
 *                                              *
 * Even if the list was empty, it must still be *
 *     terminated using the same operation.     *
 ************************************************/

List *HashTable::bulkSearchKeys(void *key, unsigned int (*hash_function)(void *),
	int (*compare)(void *, void *))
{
	/* We initialize the list with the results (the matching keys) */
	List *result = new List();

	/* We hash the key to find its neighborhood */
	unsigned int hash_value = hash_function(key) % bucketsNum;

	/* We retrieve the bitmap of the bucket
	 * where the given key was hashed to
	 */
	Bitmap *bitmap = table[hash_value].getHopInformation();

	/* Since the given key was hashed to the bucket with
	 * index 'hash_value', the bitmap of this bucket
	 * indicates the position of the given key for search,
	 * if the key exists. So, we only need to look at the
	 * positions of the table where the bitmap implies
	 * the given key could be. These positions are
	 * represented by the bits of the bitmap with value 1
	 */
	unsigned int posOfNextAce = 0;

	while(1)
	{
		/* We find the position of the next ace */
		posOfNextAce = bitmap->posOfFirstAceFromPos(posOfNextAce + 1);

		/* If there are no more aces, the given key does not exist */

		if(posOfNextAce == 0)
			break;

		/* We retrieve the key of the next candidate entry where the given key
		 * could be. If the key is equal to the key of the first element, this
		 * is enough for us to understand the key exists. All the elements of
		 * a single entry have keys that are compared as equal.
		 */
		List *entryItems = table[(hash_value + posOfNextAce - 1) % bucketsNum].getItems();
		HashEntryItem *firstItem = (HashEntryItem *) entryItems->getItemInPos(1);
		void *keyOfNextBucket = firstItem->getKey();

		/* If that key is equal to the given one, the key exists */

		if(compare(key, keyOfNextBucket) == 0)
		{
			/* We will start traversing the list of items of that entry */
			Listnode *currentNode = entryItems->getHead();

			/* As long as we have not finished traversing the list */
			while(currentNode != NULL)
			{
				/* We retrieve the hash entry item in the current node */
				HashEntryItem *nextEntryItem = (HashEntryItem *) currentNode->getItem();

				/* We insert the key of the hash entry in the result list */
				result->insertLast(nextEntryItem->getKey());

				/* We proceed to the next node */
				currentNode = currentNode->getNext();
			}
		}
	}

	/* Finally, we return the list of all matching keys */
	return result;
}

/*****************************************************************
 *   Searches the given key in the hash table. If there exists   *
 *   at least one key in the hash table that is compared equal   *
 *   to the given key by the given compare method, the position  *
 * in the hash table of the entry containing the key is returned *
 *****************************************************************/

unsigned int HashTable::searchPos(void *key,
	unsigned int (*hash_function)(void *),
	int (*compare)(void *, void *))
{
	/* First we hash the key to find its neighborhood */
	unsigned int hash_value = hash_function(key) % bucketsNum;

	/* We retrieve the bitmap of the bucket
	 * where the given key was hashed to
	 */
	Bitmap *bitmap = table[hash_value].getHopInformation();

	/* Since the given key was hashed to the bucket with
	 * index 'hash_value', the bitmap of this bucket
	 * indicates the position of the given key for search,
	 * if the key exists. So, we only need to look at the
	 * positions of the table where the bitmap implies
	 * the given key could be. These positions are
	 * represented by the bits of the bitmap with value 1
	 */
	unsigned int posOfNextAce = 0;

	while(1)
	{
		/* We find the position of the next ace */
		posOfNextAce = bitmap->posOfFirstAceFromPos(posOfNextAce + 1);

		/* If there are no more aces, the given key does not exist
		 * In this case we return the number of buckets of the table
		 */
		if(posOfNextAce == 0)
			return bucketsNum;

		/* We retrieve the key of the next candidate entry where the given key
		 * could be. If the key is equal to the key of the first element, this
		 * is enough for us to understand the key exists. All the elements of
		 * a single entry have keys that are compared as equal.
		 */
		List *entryItems = table[(hash_value + posOfNextAce - 1) % bucketsNum].getItems();
		HashEntryItem *firstItem = (HashEntryItem *) entryItems->getItemInPos(1);
		void *keyOfNextBucket = firstItem->getKey();

		/* If that key is equal to the given one, the key exists */

		if(compare(key, keyOfNextBucket) == 0)
			return (hash_value + posOfNextAce - 1) % bucketsNum;
	}

	/* The control should not reach this place.
	 *
	 * However, we just return arbitrarily the buckets num,
	 * which is the return value of an unsuccessful search.
	 */
	return bucketsNum;
}

/**************************************************
 * Terminates the result returned by 'bulkSearch' *
 **************************************************/

void HashTable::terminateBulkSearchList(List *bulkSearchResult)
{
	delete bulkSearchResult;
}

/*********************************************
 * Prints all the contents of the hash table *
 *********************************************/

void HashTable::print(void (*visitItemAndKey)(void *, void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenDataAndBitmap)(),
	void (*contextBetweenHashEntries)(),
	void (*emptyEntryPrinting)()) const
{
	/* Helper variable used for counting */
	unsigned int i;

	/* The 'contextBetweenEntries' operation is
	 * an optional parameter that the user may
	 * provide. This function prints some context
	 * between the printed entries in the screen,
	 * so as the printing has better aesthetic.
	 */
	if(contextBetweenHashEntries != NULL)
		contextBetweenHashEntries();

	for(i = 0; i < bucketsNum; i++)
	{
		/* Case the current entry is not empty
		 *
		 * We print the contents of the entry
		 */
		if(!table[i].isAvailable())
		{
			table[i].print(
				visitItemAndKey,
				contextBetweenItems,
				contextBetweenDataAndBitmap
			);
		}

		/* Case the current entry is empty
		 *
		 * In this case we will call the user's function
		 * that prints an empty entry in the way the user
		 * wants. The user may as well not provide such a
		 * function. In that case we do nothing.
		 */
		else
		{
			if(emptyEntryPrinting != NULL)
				emptyEntryPrinting();
		}

		/* If a no-null 'contextBetweenEntries' has been
		 * given, we call it now to split the currently
		 * printed entry from the next one that will be
		 * printed
		 */
		if(contextBetweenHashEntries != NULL)
			contextBetweenHashEntries();
	}
}
