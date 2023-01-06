#include <iostream>
#include "Queue.h"

/****************************
 * Constructor & Destructor *
 ****************************/

Queue::Queue() : List() {}

Queue::~Queue() {}

/****************************************
 * Get the the oldest item of the queue *
 ****************************************/

void *Queue::getOldestItem() const
{
	return getItemInPos(1);
}

/**********************************
 * Insert an element in the queue *
 **********************************/

void Queue::insert(void *item)
{
	insertLast(item);
}

/************************************
 * Remove an element from the queue *
 ************************************/

void Queue::remove()
{
	removeFront();
}
