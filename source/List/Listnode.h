#ifndef _LISTNODE_H_
#define _LISTNODE_H_

class Listnode {

private:

/* A listnode stores an address
 * and has pointers to the next
 * and the previous listnode
 */
	void *item;
	Listnode *next;
	Listnode *previous;

public:

/* Constructor & Destructor */
	Listnode(void *item);
	~Listnode();

/* Getters */
	void *getItem() const;
	Listnode *getNext() const;
	Listnode *getPrevious() const;

/* Setters */
	void setItem(void *new_item);
	void setNext(Listnode *new_next);
	void setPrevious(Listnode *new_previous);

/* Printing */
	void print(void (*visit)(void *)) const;

};

#endif