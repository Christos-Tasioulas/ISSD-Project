#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "List.h"

class Queue : public List {

public:

/* Constructor & Destructor */
	Queue();
	~Queue();

/* Get the the oldest item of the queue */
	void *getOldestItem() const;

/* Insert an element in the queue */
	void insert(void *item);

/* Remove an element from the queue */
	void remove();

};

#endif
