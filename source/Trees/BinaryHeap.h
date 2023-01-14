#ifndef _BINARY_HEAP_H_
#define _BINARY_HEAP_H_

#include "CompleteBinaryTree.h"
#include "ComplexItem.h"

#define MAXHEAP true
#define MINHEAP false

typedef ComplexItem BHObject;

class BinaryHeap : public CompleteBinaryTree {

public:

/* Constructor & Destructor */
	BinaryHeap(bool heaptype);
	~BinaryHeap();

/* Get the item with highest priority from the heap */
	void *getHighestPriorityItem() const;

/* Get the key with highest priority from the heap */
	void *getHighestPriorityKey() const;

/* Insert a new node in the heap */
	void insert(void *item, void *key, int (*compare)(void *, void *));

/* Remove the item with highest priority from the heap */
	void remove(int (*compare)(void *, void *));

/* Print the items of the tree (always in Levelorder)
 *
 * A 'visit' function is required to print an item.
 * If the given 'contextBetweenNodes' is a no-null
 * value, it is called to print the context the user
 * desires between the items that are going to be
 * printed. If the given 'contextBetweenLevels' is a
 * no-null value, it is called to print the desired
 * context between consecutive levels of the tree.
 */
	void printItems(void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL) const;

/* Print the keys of the tree (always in Levelorder)
 *
 * A 'visit' function is required to print an item.
 * If the given 'contextBetweenNodes' is a no-null
 * value, it is called to print the context the user
 * desires between the items that are going to be
 * printed. If the given 'contextBetweenLevels' is a
 * no-null value, it is called to print the desired
 * context between consecutive levels of the tree.
 */
	void printKeys(void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL) const;

/* Print both the items & the keys of the tree (always
 * in Levelorder)
 *
 * A 'visit' function is required to print an item.
 * If the given 'contextBetweenNodes' is a no-null
 * value, it is called to print the context the user
 * desires between the items that are going to be
 * printed. If the given 'contextBetweenLevels' is a
 * no-null value, it is called to print the desired
 * context between consecutive levels of the tree.
 */
	void printBoth(void (*visit)(void *, void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL) const;

/* Heapify the tree (used when a key
 * in the heap is updated by the user)
 */
	void heapifyTree(int (*compare)(void *, void *));

/* Call the 'actions' operation for every pair of item & key
 * in the nodes of the tree in the given traversal order
 */
	void traverse(OrderOfTraversal traversal_order,
		void (*actions)(void *, void *));

/* Remove every node from the tree */
	void destroy();

private:

/* Type of heap: Maxheap - Minheap */
	bool heaptype;

/* Operation to sift down the data of a tree node */
	void heapifyDown(Treenode *node, int (*compare)(void *, void *));

/* Operation to sift up the data of a tree node */
	void heapifyUp(Treenode *node, int (*compare)(void *, void *));

/* Used to delete the allocated memory of the treenodes */
	void deleteBHObject(Treenode *node);

/* Heapifies the tree calling heapify down for every node in Postorder */
	void heapifyTreeRec(Treenode *node, int (*compare)(void *, void *));

};

#endif
