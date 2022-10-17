#ifndef _LIST_H_
#define _LIST_H_

#include "Listnode.h"

class List {

protected:

/* head - the starting node of the list
 * tail - the ending node of the list
 * counter - the quantity of nodes in the list
 */
	Listnode *head;
	Listnode *tail;
	unsigned int counter;

public:

/* Constructor & Destructor */
	List();
	~List();

/* Getters */
	Listnode *getHead() const;
	Listnode *getTail() const;
	unsigned int getCounter() const;

/* Setters */
	void setHead(Listnode *new_head);
	void setTail(Listnode *new_tail);
	void setCounter(unsigned int new_counter);

/* Retrieve the item saved after 'pos' nodes */
	void *getItemInPos(unsigned int pos) const;

/* Returns 'true' if the list is empty */
	bool isEmpty() const;

/* Printing */
	void printFromHead(void (*visit)(void *), void (*context)() = NULL) const;
	void printFromTail(void (*visit)(void *), void (*context)() = NULL) const;

/* Insert a node in the list */
	void insertFront(void *item);
	void insertLast(void *item);
	void insertAfterPos(void *item, unsigned int pos);
	void insertBeforePos(void *item, unsigned int pos);
	void insertAfterKeyNode(void *item, void *key, int (*compare)(void *, void *));
	void insertBeforeKeyNode(void *item, void *key, int (*compare)(void *, void *));

/* Remove a node from the list */
	void removeFront();
	void removeLast();
	void removePos(unsigned int pos);
	void removeKeyNode(void *key, int (*compare)(void *, void *), bool *removal_true = NULL);

/* Calls the 'actions' operation for each item in the list
 * (Starting from the head and ending in the tail)
 */
	void traverseFromHead(void (*actions)(void *));

/* Calls the 'actions' operation for each item in the list
 * (Starting from the tail and ending in the head)
 */
	void traverseFromTail(void (*actions)(void *));

/* Splits the list in two equal parts. The first part will be
 * stored in the list 'firstPart' and the second will be stored
 * in the list 'secondPart'.
 *
 * The two lists must not be initialized with 'new' by the user,
 * but must be deleted when not needed anymore with 'delete'.
 */
	void split(List **firstPart, List **secondPart);

/* Appends the given list 'other' to the list */
	void append(List *other);

};

#endif
