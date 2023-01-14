#ifndef _BINARY_TREE_H_
#define _BINARY_TREE_H_

#include "Treenode.h"
#include "OrderOfTraversal.h"

class BinaryTree {

public:

/* Constructor & Destructor */
	BinaryTree();
	virtual ~BinaryTree();

/* Getters */
	Treenode *getRoot() const;
	unsigned int getCounter() const;

/* Retrieve the item of a treenode */
	void *getItem(Treenode *node) const;

/* Returns 'true' if the tree is empty */
	bool isEmpty() const;

/* Returns the number of nodes in the tree
 * (it is an alternative to 'getCounter')
 */
	unsigned int size() const;

/* Returns the height of the tree */
	unsigned int height() const;

/* Insert a new node in the tree */
	void insertRoot(void *item);
	void insertLeft(Treenode *node, void *item);
	void insertRight(Treenode *node, void *item);

/* Change the value of the item of a node */
	void change(Treenode *node, void *item);

/* Prints the items of the tree. The operation passes through
 * every node in the sequence implied by the given traversal
 * order. A 'visit' function is required to print an item of
 * a node. If the 'contextBetweenNodes' function is a no-null
 * value, it is called to print the context the user desires
 * between the items that are going to be printed. In case the
 * chosen traversal order is 'Levelorder', the user may also
 * provide an operation that prints the desired context between
 * consecutive levels of the tree. Also, again, if the chosen
 * traversal order is 'Levelorder', the context between nodes
 * is also printed at the start and at the end of every level.
 */
	void print(OrderOfTraversal traversal_order,
		void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL) const;

/* Call the given 'actions' operation for each item of
 * the tree as argument in the given traversal order
 */
	void traverse(OrderOfTraversal traversal_order,
		void (*actions)(void *)) const;

/* Remove a leaf node */
	void remove(Treenode *node);

/* Remove every node from the tree */
	virtual void destroy();

protected:

/* The Binary Tree has a
 * 'root' treenode and a
 * counter of treenodes
 */
	Treenode *root;
	unsigned int counter;

private:

/* Finds recursively the size of the subtree
 * that begins from the 'node' argument
 */
	unsigned int sizeRec(Treenode *node) const;

/* Finds recursively the height of the subtree
 * that begins from the 'node' argument
 */
	unsigned int heightRec(Treenode *node) const;

/* Prints recursively the subtree that begins from the 'node'
 * argument with the given traversal order and rest functions
 */
	void printRec(Treenode *node, OrderOfTraversal traversal_order,
		void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL) const;

/* Prints the subtree that begins from the
 * 'node' argument in Levelorder traversal
 */
	void printLevel(Treenode *node, unsigned int level, 
		void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL) const;

/* Traverses recursively the subtree that begins from the 'node'
 * argument with the given traversal order and calls the 'actions'
 * operation for every item in the tree
 */
	void traverseRec(Treenode *node, OrderOfTraversal traversal_order,
		void (*actions)(void *)) const;

/* Traverses the subtree that begins from the
 * 'node' argument in Levelorder traversal
 */
	void traverseLevel(Treenode *node, unsigned int level,
		void (*actions)(void *)) const;

/* Deletes recursively all treenodes of the subtree that
 * begins from the 'node' argument in Postorder traversal
 */
	void destroyRec(Treenode *node);

};

#endif
