#include <iostream>
#include "BinaryHeap.h"

/********************************
 * Visit operations of the user *
 *	  and the implementation    *
 ********************************/

void (*bhUserVisit)(void *);
void (*bhUserVisitBoth)(void *, void *);
void bhImpVisitItems(void *item);
void bhImpVisitKeys(void *item);
void bhImpVisitBoth(void *item);

/*******************************
 * Traverse operations of the  *
 * user and the implementation *
 *******************************/

void (*bhUserTraverse)(void *, void *);
void bhImpTraverse(void *item);

/****************************
 * Constructor & Destructor *
 ****************************/

BinaryHeap::BinaryHeap(bool heaptype)
{
	CompleteBinaryTree();
	this->heaptype = heaptype;
}

BinaryHeap::~BinaryHeap()
{
	deleteBHObject(root);
}

/****************************************************
 * Get the item with highest priority from the heap *
 ****************************************************/

void *BinaryHeap::getHighestPriorityItem() const
{
	return ((BHObject *) root->getItem())->getItem();
}

/***************************************************
 * Get the key with highest priority from the heap *
 ***************************************************/

void *BinaryHeap::getHighestPriorityKey() const
{
	return ((BHObject *) root->getItem())->getKey();
}

/*********************************
 * Insert a new node in the heap *
 *********************************/

void BinaryHeap::heapifyUp(Treenode *node, int (*compare)(void *, void *))
{
	/* If the node has not parent (and
	 * thus it is the root), we return
	 */

	if(node->getParent() == NULL)
		return;

	/* If the node is greater than its parent in max heap or lower
	 * than its parent in min heap, it is swapped with its parent
	 * and 'heapifyUp' is called for the parent
	 */

	if(heaptype == MINHEAP)
	{
		if(compare(((BHObject *) node->getItem())->getKey(),
		((BHObject *) node->getParent()->getItem())->getKey()) < 0)
		{
			node->swap(node->getParent());
			heapifyUp(node->getParent(), compare);
		}
	}

	else if(heaptype == MAXHEAP)
	{
		if(compare(((BHObject *) node->getItem())->getKey(),
		((BHObject *) node->getParent()->getItem())->getKey()) > 0)
		{
			node->swap(node->getParent());
			heapifyUp(node->getParent(), compare);
		}
	}

	return;
}

void BinaryHeap::insert(void *item, void *key, int (*compare)(void *, void *))
{
	BHObject *new_object = new BHObject(item, key);
	CompleteBinaryTree::insert(new_object);
	heapifyUp(last, compare);
}

/*******************************************************
 * Remove the item with highest priority from the heap *
 *******************************************************/

void BinaryHeap::heapifyDown(Treenode *node, int (*compare)(void *, void *))
{
	if(node == NULL)
		return;

	/* If the node has no left child, then it has not a right child
	 * either, because of the complete tree formation. So, the node
	 * is a leaf node and cannot go to a lower level with heapify down
	 */

	if(node->getLeft() == NULL)
		return;

	BHObject *node_item = (BHObject *) node->getItem();
	BHObject *left_item = (BHObject *) node->getLeft()->getItem();

	/* If the node has only a left child, we compare the keys of the two
	 * nodes and swap them if needed. Then we return because, since there
	 * is no right child, the level of the left child is the bottom level,
	 * so nodes cannot go deeper than the level of the left child
	 */

	if(node->getRight() == NULL)
	{
		if(heaptype == MINHEAP)
		{
			if(compare(node_item->getKey(), left_item->getKey()) > 0)
				node->swap(node->getLeft());
		}

		else if(heaptype == MAXHEAP)
		{
			if(compare(node_item->getKey(), left_item->getKey()) < 0)
				node->swap(node->getLeft());
		}

		return;
	}

	BHObject *right_item = (BHObject *) node->getRight()->getItem();

	/* If the node is greater than its children in max heap or lower
	 * than its children in min heap, then, because the subtrees that
	 * begin from the left and right children are heaps, we return
	 */

	if(heaptype == MINHEAP)
	{
		if(compare(node_item->getKey(), right_item->getKey()) < 0 &&
	    	compare(node_item->getKey(), left_item->getKey()) < 0)
		{
			return;
		}
	}

	else if(heaptype == MAXHEAP)
	{
		if(compare(node_item->getKey(), right_item->getKey()) > 0 &&
	    	compare(node_item->getKey(), left_item->getKey()) > 0)
		{
			return;
		}
	}

	/* Else if one of the children has the highest priority,
	 * it is swapped with the node and 'heapifyDown'
	 * is called for that child
	 */

	if(heaptype == MINHEAP)
	{
		if(compare(left_item->getKey(), right_item->getKey()) < 0)
		{
			node->swap(node->getLeft());
			heapifyDown(node->getLeft(), compare);
		}

		else
		{
			node->swap(node->getRight());
			heapifyDown(node->getRight(), compare);
		}
	}

	else if(heaptype == MAXHEAP)
	{
		if(compare(left_item->getKey(), right_item->getKey()) > 0)
		{
			node->swap(node->getLeft());
			heapifyDown(node->getLeft(), compare);
		}

		else
		{
			node->swap(node->getRight());
			heapifyDown(node->getRight(), compare);
		}
	}
}

void BinaryHeap::remove(int (*compare)(void *, void *))
{
	root->swap(last);
	deleteBHObject(last);
	CompleteBinaryTree::removeLast();
	heapifyDown(root, compare);
}

/**********************************************
 * Printing variations (always in Levelorder) *
 **********************************************/

void BinaryHeap::printItems(void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)()) const
{
	bhUserVisit = visit;

	BinaryTree::print(Levelorder,
		bhImpVisitItems,
		contextBetweenNodes,
		contextBetweenLevels);
}

void BinaryHeap::printKeys(void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)()) const
{
	bhUserVisit = visit;

	BinaryTree::print(Levelorder,
		bhImpVisitKeys,
		contextBetweenNodes,
		contextBetweenLevels);
}

void BinaryHeap::printBoth(void (*visit)(void *, void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)()) const
{
	bhUserVisitBoth = visit;

	BinaryTree::print(Levelorder,
		bhImpVisitBoth,
		contextBetweenNodes,
		contextBetweenLevels);
}

/**************************************************
 * Implementation visit type functions that break *
 * the Complex Item to its parts and use the user *
 * functions to visit the desired data. Before we *
 * call 'bhUserVisit(Both)' we make sure to save  *
 * the address of it in another function pointer  *
 * (in case to retrieve it back if 'bhUserVisit'  *
 * overwrites itself to traverse another B. Heap  *
 **************************************************/

void bhImpVisitItems(void *item)
{
	BHObject *bh_object = (BHObject *) item;
	void (*tempUserVisit)(void *) = bhUserVisit;
	bhUserVisit(bh_object->getItem());
	bhUserVisit = tempUserVisit;
}

void bhImpVisitKeys(void *item)
{
	BHObject *bh_object = (BHObject *) item;
	void (*tempUserVisit)(void *) = bhUserVisit;
	bhUserVisit(bh_object->getKey());
	bhUserVisit = tempUserVisit;
}

void bhImpVisitBoth(void *item)
{
	BHObject *bh_object = (BHObject *) item;
	void (*tempUserVisit)(void *, void *) = bhUserVisitBoth;
	bhUserVisitBoth(bh_object->getItem(), bh_object->getKey());
	bhUserVisitBoth = tempUserVisit;
}

/********************
 * Heapify the tree *
 ********************/

void BinaryHeap::heapifyTree(int (*compare)(void *, void *))
{
	heapifyTreeRec(root, compare);
}

void BinaryHeap::heapifyTreeRec(Treenode *node, int (*compare)(void *, void *))
{
	if(node == NULL)
		return;

	heapifyTreeRec(node->getLeft(), compare);
	heapifyTreeRec(node->getRight(), compare);
	heapifyDown(node, compare);
}

/*************************************************************
 * Call the 'actions' operation for every pair of item & key *
 *   in the nodes of the tree in the given traversal order   *
 *************************************************************/

void BinaryHeap::traverse(OrderOfTraversal traversal_order,
	void (*actions)(void *, void *))
{
	bhUserTraverse = actions;
	BinaryTree::traverse(traversal_order, bhImpTraverse);
}

/*******************************************************
 * Traverse operation that breaks the Complex Item to  *
 * its parts and then calls the user traverse function *
 *   Before we call 'bhUserTraverse' we make sure to   *
 * save the address of it in another function pointer  *
 * (so as to retrieve it back in case 'bhUserTraverse' *
 *  overwrites itself to traverse another Binary Heap) *
 *******************************************************/

void bhImpTraverse(void *item)
{
	BHObject *bh_obj = (BHObject *) item;
	void (*tempUserTraverse)(void *, void *) = bhUserTraverse;
	bhUserTraverse(bh_obj->getItem(), bh_obj->getKey());
	bhUserTraverse = tempUserTraverse;
}

/**************************************************
 * Delete the allocated 'BHObject' from all nodes *
 **************************************************/

void BinaryHeap::deleteBHObject(Treenode *node)
{
	if(node != NULL)
	{
		delete (BHObject *) node->getItem();
		deleteBHObject(node->getLeft());
		deleteBHObject(node->getRight());
	}

	return;
}

/***********************************
 * Remove every node from the tree *
 ***********************************/

void BinaryHeap::destroy()
{
	deleteBHObject(root);
	BinaryTree::destroy();
}
