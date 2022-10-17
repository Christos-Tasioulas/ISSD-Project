#include <iostream>
#include "List.h"

/****************************
 * Constructor & Destructor *
 ****************************/

List::List()
{
	head = NULL;
	tail = NULL;
	counter = 0;
}

List::~List()
{
	while(head != NULL)
		removeFront();
}

/***********
 * Getters *
 ***********/

Listnode *List::getHead() const
{
	return head;
}

Listnode *List::getTail() const
{
	return tail;
}

unsigned int List::getCounter() const
{
	return counter;
}

/***********
 * Setters *
 ***********/

void List::setHead(Listnode *new_head)
{
	head = new_head;
}

void List::setTail(Listnode *new_tail)
{
	tail = new_tail;
}

void List::setCounter(unsigned int new_counter)
{
	counter = new_counter;
}

/*********************************************
 * Retrieve the item saved after 'pos' nodes *
 *********************************************/

void *List::getItemInPos(unsigned int pos) const
{
	if(pos <= 1)
		return head->getItem();

	if(pos >= counter)
		return tail->getItem();

	Listnode *current;

	/* If 'pos' is closer to the head (pos < counter / 2),
	 * we reach it by beginning from the head.
	 *
	 * Else, if 'pos' is closer to the tail (pos >= counter / 2),
	 * we reach it by beginning from the tail.
	 */

	if(pos < counter / 2)
	{
		current = head;
		unsigned int i = 1;

		while(i < pos)
		{
			current = current->getNext();
			i++;
		}
	}

	else
	{
		current = tail;
		unsigned int i = counter;

		while(i > pos)
		{
			current = current->getPrevious();
			i--;
		}
	}

	return current->getItem();
}

/***************************************
 * Returns 'true' if the list is empty *
 ***************************************/

bool List::isEmpty() const
{
	return counter == 0;
}

/************
 * Printing *
 ************/

void List::printFromHead(void (*visit)(void *), void (*context)()) const
{
	/* Start printing from the head */

	Listnode *current = head;

	while(current != NULL)
	{
		current->print(visit);
		current = current->getNext();

		if(current != NULL && context != NULL)
			context();
	}
}

void List::printFromTail(void (*visit)(void *), void (*context)()) const
{
	/* Start printing from the tail */

	Listnode *current = tail;

	while(current != NULL)
	{
		current->print(visit);
		current = current->getPrevious();

		if(current != NULL && context != NULL)
			context();
	}
}

/*****************************
 * Insert a node in the list *
 *****************************/

void List::insertFront(void *item)
{
	/* We create a new list node that will store the given item */

	Listnode *new_node = new Listnode(item);

	/* If the list is empty, the 'head' node will be the same
	 * as the 'tail' node and the list will have 1 element
	 */

	if(isEmpty())
	{
		head = new_node;
		tail = new_node;
		counter = 1;

		return;
	}

	/* Else we place the new node in front
	 * of the list and we change the head
	 */

	head->setPrevious(new_node);
	new_node->setNext(head);
	head = new_node;
	counter++;
}

void List::insertLast(void *item)
{
	/* We create a new list node that will store the given item */

	Listnode *new_node = new Listnode(item);

	/* If the list is empty, the 'head' node will be the same
	 * as the 'tail' node and the list will have 1 element
	 */

	if(isEmpty())
	{
		head = new_node;
		tail = new_node;
		counter = 1;

		return;
	}

	/* Else we place the new node after the
	 * 'tail' node and we change the tail
	 */

	tail->setNext(new_node);
	new_node->setPrevious(tail);
	tail = new_node;
	counter++;
}

void List::insertAfterPos(void *item, unsigned int pos)
{
	if(pos >= counter || counter <= 1)
	{
		insertLast(item);
		return;
	}

	Listnode *new_node = new Listnode(item);

	/* If 'pos' is lower than 1 (which is the head's position),
     * we act as if 'pos' was equal to 1. Thus, we add the new
     * node between the head and the next of the head.
	 */

	if(pos <= 1)
	{
		new_node->setNext(head->getNext());
		new_node->setPrevious(head);
		head->setNext(new_node);
		new_node->getNext()->setPrevious(new_node);
		counter++;

		return;
	}

	Listnode *current;

	/* If 'pos' is closer to the head (pos < counter / 2),
	 * we reach it by beginning from the head.
	 *
	 * Else, if 'pos' is closer to the tail (pos >= counter / 2),
	 * we reach it by beginning from the tail.
	 */

	if(pos < counter / 2)
	{
		current = head;
		unsigned int i = 1;

		while(i < pos)
		{
			current = current->getNext();
			i++;
		}
	}

	else
	{
		current = tail;
		unsigned int i = counter;

		while(i > pos)
		{
			current = current->getPrevious();
			i--;
		}
	}

	/* We add the new node between the
	 * current and the next of current
	 */

	new_node->setNext(current->getNext());
	new_node->setPrevious(current);
	current->setNext(new_node);
	new_node->getNext()->setPrevious(new_node);
	counter++;
}

void List::insertBeforePos(void *item, unsigned int pos)
{
	if(pos <= 1 || counter <= 1)
	{
		insertFront(item);
		return;
	}

	Listnode *new_node = new Listnode(item);

	/* If 'pos' is greater than 'counter' (which is the tail's
	 * position), we act as if 'pos' was equal to 'counter'. Thus, we
	 * add the new node between the tail and the previous of the tail.
	 */

	if(pos >= counter)
	{
		new_node->setNext(tail);
		new_node->setPrevious(tail->getPrevious());
		tail->setPrevious(new_node);
		new_node->getPrevious()->setNext(new_node);
		counter++;

		return;
	}

	Listnode *current;

	/* If 'pos' is closer to the head (pos < counter / 2),
	 * we reach it by beginning from the head.
	 *
	 * Else, if 'pos' is closer to the tail (pos >= counter / 2),
	 * we reach it by beginning from the tail.
	 */

	if(pos < counter / 2)
	{
		current = head;
		unsigned int i = 1;

		while(i < pos)
		{
			current = current->getNext();
			i++;
		}
	}

	else
	{
		current = tail;
		unsigned int i = counter;

		while(i > pos)
		{
			current = current->getPrevious();
			i--;
		}
	}

	/* We add the new node between the
	 * current and the previous of current
	 */

	new_node->setNext(current);
	new_node->setPrevious(current->getPrevious());
	current->setPrevious(new_node);
	new_node->getPrevious()->setNext(new_node);
	counter++;
}

void List::insertAfterKeyNode(void *item, void *key, int (*compare)(void *, void *))
{
	Listnode *current = head;

	/* If the key is not found in the list, the
	 * item is stored after the current tail.
	 */

	while(1)
	{
		if(current == NULL || current == tail)
		{
			insertLast(item);
			return;
		}

		if(!compare(current->getItem(), key))
			break;

		current = current->getNext();
	}	

	Listnode *new_node = new Listnode(item);

	/* We place the node after the desired key node
	 * (The desired key node is saved in 'current')
	 */

	new_node->setNext(current->getNext());
	new_node->setPrevious(current);
	current->setNext(new_node);
	new_node->getNext()->setPrevious(new_node);
	counter++;
}

void List::insertBeforeKeyNode(void *item, void *key, int (*compare)(void *, void *))
{
	Listnode *current = tail;

	/* If the key is not found in the list, the
	 * item is stored before the current head.
	 */

	while(1)
	{
		if(current == NULL || current == head)
		{
			insertFront(item);
			return;
		}

		if(!compare(current->getItem(), key))
			break;

		current = current->getPrevious();
	}	

	Listnode *new_node = new Listnode(item);

	/* We place the node after the desired key node
	 * (The desired key node is saved in 'current')
	 */

	new_node->setNext(current);
	new_node->setPrevious(current->getPrevious());
	current->setPrevious(new_node);
	new_node->getPrevious()->setNext(new_node);
	counter++;
}

/*******************************
 * Remove a node from the list *
 *******************************/

void List::removeFront()
{
	if(isEmpty())
		return;

	/* If the list has 1 element, both the head
	 * and the tail of the list will be set to
	 * 'NULL' and the list will have no elements
	 */

	if(counter == 1)
	{
		delete head;

		head = NULL;
		tail = NULL;
		counter = 0;

		return;
	}

	/* Else we change the head of the list */

	Listnode *current = head;
	head = head->getNext();
	head->setPrevious(NULL);
	counter--;

	delete current;
}

void List::removeLast()
{
	if(isEmpty())
		return;

	/* If the list has 1 element, both the head
	 * and the tail of the list will be set to
	 * 'NULL' and the list will have no elements
	 */

	if(counter == 1)
	{
		delete head;

		head = NULL;
		tail = NULL;
		counter = 0;

		return;
	}

	/* Else we change the tail of the list */

	Listnode *current = tail;
	tail = tail->getPrevious();
	tail->setNext(NULL);
	counter--;

	delete current;
}

void List::removePos(unsigned int pos)
{
	if(pos <= 1)
	{
		removeFront();
		return;
	}

	if(pos >= counter)
	{
		removeLast();
		return;
	}

	Listnode *current;

	/* If 'pos' is closer to the head (pos < counter / 2),
	 * we reach it by beginning from the head.
	 *
	 * Else, if 'pos' is closer to the tail (pos >= counter / 2),
	 * we reach it by beginning from the tail.
	 */

	if(pos < counter / 2)
	{
		current = head;
		unsigned int i = 1;

		while(i < pos)
		{
			current = current->getNext();
			i++;
		}
	}

	else
	{
		current = tail;
		unsigned int i = counter;

		while(i > pos)
		{
			current = current->getPrevious();
			i--;
		}
	}

	/* The nodes 'current' is between, instead of
	 * pointing to 'current', point to each other
	 */

	current->getPrevious()->setNext(current->getNext());
	current->getNext()->setPrevious(current->getPrevious());
	counter--;

	delete current;
}

void List::removeKeyNode(void *key, int (*compare)(void *, void *), bool *removal_true)
{
	/* We initialize the removal flag with 'true' (successful removal) */

	if(removal_true != NULL)
		*removal_true = true;

	Listnode *current = head;

	while(1)
	{
		if(current == NULL)
			break;

		/* Here we compare the node's item with the given key. If
		 * we found the key, we get inside the following 'if' block,
		 * where the function will also return after the removal
		 */

		if(!compare(current->getItem(), key))
		{
			/* If the key was found in the head, we remove the front element */

			if(current == head)
				removeFront();

			/* If the key was found in the tail, we remove the last element */

			else if(current == tail)
				removeLast();

			/* Else we have to remove an intermediate element
			 *
			 * The nodes 'current' is between, instead of
			 * pointing to 'current', point to each other
			 */

			else
			{
				current->getPrevious()->setNext(current->getNext());
				current->getNext()->setPrevious(current->getPrevious());
				counter--;

				delete current;
			}

			return;
		}

		/* If we didn't find the key, we proceed to the next list node */

		current = current->getNext();
	}

	/* If this part is reached, the removal was not successful
	 * because the desired key was not found. We update
	 * the removal flag with 'false' in case it is given
	 */

	if(removal_true != NULL)
		*removal_true = false;
}

/***********************************************************
 * Calls the 'actions' operation for each item in the list *
 *     (Starting from the head and ending in the tail)     *
 ***********************************************************/

void List::traverseFromHead(void (*actions)(void *))
{
	Listnode *current = head;

	while(current != NULL)
	{
		actions(current->getItem());
		current = current->getNext();
	}
}

/***********************************************************
 * Calls the 'actions' operation for each item in the list *
 *     (Starting from the tail and ending in the head)     *
 ***********************************************************/

void List::traverseFromTail(void (*actions)(void *))
{
	Listnode *current = tail;

	while(current != NULL)
	{
		actions(current->getItem());
		current = current->getPrevious();
	}
}

/****************************************************************
 *  Splits the list in two equal parts. The first part will be  *
 * stored in the list 'firstPart' and the second will be stored *
 *                   in the list 'secondPart'                   *
 *                                                              *
 * The two lists must not be initialized with 'new' by the user *
 *   but must be deleted when not needed anymore with 'delete'  *
 ****************************************************************/

void List::split(List **firstPart, List **secondPart)
{
	/* We initialize the lists that will store the two halves */
	(*firstPart) = new List();
	(*secondPart) = new List();

	/* If there are no nodes in the the
	 * list, we return the two empty lists
	 */
	if(counter == 0)
		return;

	/* If there is only one node in the list, we place that
	 * node in the list storing the first part and we return
	 */
	if(counter == 1)
	{
		(*firstPart)->insertLast(head->getItem());
		return;
	}

	/* Else we start traversing the list from the head */
	Listnode *current = head;

	/* We keep a counter on how many nodes we have copied */
	unsigned int pos = 1;

	/* We find the index of the middle position in the list */
	unsigned int middle_pos = (counter + 1) / 2;

	/* As long as we have not reached the middle position,
	 * we copy each node to the list storing the left part
	 */
	while(pos < middle_pos)
	{
		/* Here we insert the item of the current
		 * node to the list storing the left part
		 */
		(*firstPart)->insertLast(current->getItem());

		/* We increase our current position in the list by 1 */
		pos++;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* If the number of items in the list is even, we
	 * need to place the middle item in the left part. 
	 */
	if(counter % 2 == 0)
	{
		/* We place the item of the middle node
		 * to the list storing the left part
		 */
		(*firstPart)->insertLast(current->getItem());

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* We copy the rest nodes to the right part */
	while(current != NULL)
	{
		/* Here we insert the item of the current
		 * node to the list storing the right part
		 */
		(*secondPart)->insertLast(current->getItem());

		/* We proceed to the next node */
		current = current->getNext();
	}
}

/**************************************************
 * Appends the given list 'other' to the the list *
 **************************************************/

void List::append(List *other)
{
	/* If the given list is 'NULL' or empty,
	 * there is nothing to append
	 */
	if(other == NULL || other->isEmpty())
		return;

	/* We will traverse the other list from the head*/
	Listnode *current = other->getHead();

	/* As long as we have not reached the end of the other list */
	while(current != NULL)
	{
		/* We retrieve the item stored in the current node */
		void *currentItem = current->getItem();

		/* We insert that item at the end of our list */
		this->insertLast(currentItem);

		/* We proceed to the next node */
		current = current->getNext();
	}
}
