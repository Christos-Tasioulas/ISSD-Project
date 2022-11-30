#include <iostream>
#include "BinarySearchTree.h"

/***********************************************************
 * Function pointers that store the user's visit functions *
 ***********************************************************/

static void (*bstUserVisit)(void *);
static void (*bstUserVisitBoth)(void *, void *);

/*********************************************
 * The visit functions of the implementation *
 *********************************************/

static void bstImpVisitItems(void *item);
static void bstImpVisitKeys(void *item);
static void bstImpVisitBoth(void *item);

/*************************************************************
 * Function pointer that stores the user's traverse function *
 *************************************************************/

static void (*bstUserTraverse)(void *, void *);

/***********************************************
 * The traverse function of the implementation *
 ***********************************************/

static void bstImpTraverse(void *item);

/****************************
 * Constructor & Destructor *
 ****************************/

BinarySearchTree::BinarySearchTree()
{
	BinaryTree();
}

BinarySearchTree::~BinarySearchTree()
{
	deleteBSTObject(root);
}

/*********************************
 * Insert a new node in the tree *
 *********************************/

void BinarySearchTree::insert(void *item, void *key,
	int (*compare)(void *, void *), Treenode **inserted)
{
	/* If the tree is empty, we insert a root
	 * node and return immediatelly
	 */

	if(isEmpty() == true)
	{
		BSTObject *new_object = new BSTObject(item, key);
		insertRoot(new_object);

		if(inserted != NULL)
			(*inserted) = root;
	}

	/* Else, the recursive insertion operation is called
	 * to add a new node by traversing the appropriate subtrees
	 */

	else
		insertRec(root, item, key, compare, inserted);
}

void BinarySearchTree::insertRec(Treenode *node, void *item, void *key,
	int (*compare)(void *, void *), Treenode **inserted)
{
	BSTObject *new_object = new BSTObject(item, key);

	int c = compare(new_object->getKey(), ((BSTObject *) node->getItem())->getKey());

	/* If the key value is lower than the node's
	 * key value, we traverse the left subtree
	 */

	if(c <= 0)
	{
		/* If the node has no left child, we insert
		 * the new node as its left child
		 */

		if(node->getLeft() == NULL)
		{
			insertLeft(node, new_object);

			if(inserted != NULL)
				(*inserted) = node->getLeft();
		}

		/* Else, if there is a left child, we compare
		 * the key of it to the current key recursively
		 * until the new node is inserted
		 */

		else
		{
			delete new_object;
			insertRec(node->getLeft(), item, key, compare, inserted);
		}
	}

	/* If the key value is greater than the node's
	 * key value, we traverse the right subtree
	 */

	else
	{
		/* If the node has no right child, we insert
		 * the new node as its right child
		 */

		if(node->getRight() == NULL)
		{
			insertRight(node, new_object);

			if(inserted != NULL)
				(*inserted) = node->getRight();
		}

		/* Else, if there is a right child, we compare
		 * the key of it to the current key recursively
		 * until the new node is inserted
		 */

		else
		{
			delete new_object;
			insertRec(node->getRight(), item, key, compare, inserted);
		}
	}
}

/*******************************
 * Remove a node from the tree *
 *******************************/

void BinarySearchTree::remove(void *key, int (*compare)(void *, void *),
	bool *removal_true)
{
	Treenode *toberemoved;

	/* If the desired key is not in the tree,
	 * we return immediatelly (we cannot remove)
	 */

	if(search(key, compare, &toberemoved) == true)
	{
		/* If the desired key is stored to a leaf node,
		 * we use the Binary Tree removal operation to
		 * remove the node. We also delete the allocated
		 * 'BSTObject'
		 */

		if(toberemoved->getLeft() == NULL && toberemoved->getRight() == NULL)
		{
			deleteBSTObject(toberemoved);
			BinaryTree::remove(toberemoved);

			/* We set the removal flag to 'true' since the removal was successful */

			if(removal_true != NULL)
				(*removal_true) = true;
		}

		/* If the desired key is stored to an internal node, we find the inorder
		 * predecessor of it. If there is no inorder predecessor, we use the
		 * inorder successor. Then we swap the items of the two nodes and repeat
		 * the procedure for node = [predecessor | successor] until the predecessor
		 * or the successor is a leaf node. Then, after the last swap, the leaf
		 * node can be directly removed (if there is no predecessor & successor at
		 * the same time, that means the tree has only one node - the root - and
		 * this case is resolved above).
		 */

		else
		{
			Treenode *temp = toberemoved;

			/* As long as the node to be removed is not a leaf node, we
			 * swap its content with its inorder predecessor or successor.
			 *
			 * Then the inorder predecessor/successor becomes the node
			 * that must be removed.
			 */

			while(temp->getLeft() != NULL || temp->getRight() != NULL)
			{
				Treenode *helper = inorderPredecessor(temp);

				if(helper == NULL)
					helper = inorderSuccessor(temp);

				helper->swap(temp);
				temp = helper;
			}

			/* Finally, the data that must be removed is stored in the leaf
			 * node 'temp', so we can remove 'temp' with Binary Tree removal
			 */

			deleteBSTObject(temp);
			BinaryTree::remove(temp);

			/* We set the removal flag to 'true' since the removal was successful */

			if(removal_true != NULL)
				(*removal_true) = true;
		}

		return;
	}

	/* At this point the removal was not successful. We set the flag to 'false' */

	if(removal_true != NULL)
		(*removal_true) = false;
}

/*********************************************
 * Returns the inorder predecessor of a node *
 *********************************************/

Treenode *BinarySearchTree::inorderPredecessor(Treenode *node) const
{
	/* The inorder predecessor is the node with highest
	 * value from the left subtree of 'node'
	 */

	Treenode *temp = node->getLeft();

	if(temp == NULL)
		return NULL;

	while(temp->getRight() != NULL)
		temp = temp->getRight();

	return temp;
}

/*******************************************
 * Returns the inorder successor of a node *
 *******************************************/

Treenode *BinarySearchTree::inorderSuccessor(Treenode *node) const
{
	/* The inorder successor is the node with lowest
	 * value from the right subtree of 'node'
	 */

	Treenode *temp = node->getRight();

	if(temp == NULL)
		return NULL;

	while(temp->getLeft() != NULL)
		temp = temp->getLeft();

	return temp;
}

/***********************************
 * Retrieve the item of a treenode *
 ***********************************/

void *BinarySearchTree::getItem(Treenode *node) const
{
	BSTObject *bst_obj = (BSTObject *) BinaryTree::getItem(node);
	return bst_obj->getItem();
}

/**********************************
 * Retrieve the key of a treenode *
 **********************************/

void *BinarySearchTree::getKey(Treenode *node) const
{
	BSTObject *bst_obj = (BSTObject *) BinaryTree::getItem(node);
	return bst_obj->getKey();
}

/***********************************************
 * Printing of items, keys or both of the tree *
 ***********************************************/

void BinarySearchTree::printItems(OrderOfTraversal traversal_order,
	void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	bstUserVisit = visit;

	BinaryTree::print(traversal_order,
		bstImpVisitItems,
		contextBetweenNodes,
		contextBetweenLevels);
}

void BinarySearchTree::printKeys(OrderOfTraversal traversal_order,
	void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	bstUserVisit = visit;

	BinaryTree::print(traversal_order,
		bstImpVisitKeys,
		contextBetweenNodes,
		contextBetweenLevels);
}

void BinarySearchTree::printBoth(OrderOfTraversal traversal_order,
	void (*visit)(void *, void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	bstUserVisitBoth = visit;

	BinaryTree::print(traversal_order,
		bstImpVisitBoth,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*******************
 * Sorted Printing *
 *******************/

void BinarySearchTree::printSortedItems(void (*visit)(void *),
	void (*contextBetweenNodes)())
{
	bstUserVisit = visit;
	BinaryTree::print(Inorder, bstImpVisitItems, contextBetweenNodes);
}

void BinarySearchTree::printSortedKeys(void (*visit)(void *),
	void (*contextBetweenNodes)())
{
	bstUserVisit = visit;
	BinaryTree::print(Inorder, bstImpVisitKeys, contextBetweenNodes);
}

void BinarySearchTree::printSortedBoth(void (*visit)(void *, void *),
	void (*contextBetweenNodes)())
{
	bstUserVisitBoth = visit;
	BinaryTree::print(Inorder, bstImpVisitBoth, contextBetweenNodes);
}

/**************************************************
 * Implementation visit type functions that break *
 * the Complex Item to its parts and use the user *
 * functions to visit the desired data. Before we *
 * call 'bstUserVisit(Both)' we make sure to save *
 * the address of it in another function pointer  *
 * (in case to retrieve it back if 'bstUserVisit' *
 * overwrites itself to traverse another B.S.Tree *
 **************************************************/

void bstImpVisitItems(void *item)
{
	/* The item in the node is always a 'BSTObject' */

	BSTObject *bst_object = (BSTObject *) item;

	/* Each 'BSTObject' consists of an 'item' and a 'key'.
	 * In this function we want to visit the item of it.
	 *                                       ^^^^
	 * (That's why it is called 'bstImpVisitItems')
	 *                                      ^^^^^
	 * The 'item' & 'key' values represent the item and
	 * the key that the user inserted when they called
	 *
	 *        insert('item', 'key', 'compare')
	 *
	 * First we store the visit function of the user in a
	 * function pointer. We do this action because the
	 * item or the key of the node may be another Binary
	 * Search Tree. So, the 'bstUserVisit' function would
	 * change in that case and we would lose the current one.
	 */

	void (*tempUserVisit)(void *) = bstUserVisit;
	bstUserVisit(bst_object->getItem());
	bstUserVisit = tempUserVisit;
}

void bstImpVisitKeys(void *item)
{
	BSTObject *bst_object = (BSTObject *) item;
	void (*tempUserVisit)(void *) = bstUserVisit;
	bstUserVisit(bst_object->getKey());
	bstUserVisit = tempUserVisit;
}

void bstImpVisitBoth(void *item)
{
	BSTObject *bst_object = (BSTObject *) item;
	void (*tempUserVisit)(void *, void *) = bstUserVisitBoth;
	bstUserVisitBoth(bst_object->getItem(), bst_object->getKey());
	bstUserVisitBoth = tempUserVisit;
}

/***************************************************************
 * Search a key in the tree - Returns 'true' if the key exists *
 ***************************************************************/

bool BinarySearchTree::search(void *key, int (*compare)(void *, void *),
	Treenode **searched) const
{
	return searchRec(root, key, compare, searched);
}

bool BinarySearchTree::searchRec(Treenode *node, void *key,
	int (*compare)(void *, void *), Treenode **searched) const
{
	/* If the node is 'NULL', we return 'false' */

	if(node == NULL)
	{
		if(searched != NULL)
			(*searched) = NULL;

		return false;
	}

	/* We compare the given key to the node key */

	int c = compare(key, ((BSTObject *) node->getItem())->getKey());

	/* If the keys are same, the desired node
	 * is found and we return the value 'true'
	 */

	if(c == 0)
	{
		if(searched != NULL)
			(*searched) = node;

		return true;
	}

	/* Else, if the node key is greater than the given key,
	 * we search the left subtree of the node. In the same way,
	 * if the node key is lower than the given key, we search
	 * its right subtree
	 */

	if(c < 0)
		return searchRec(node->getLeft(), key, compare, searched);

	else
		return searchRec(node->getRight(), key, compare, searched);
}

/***************************************************************
 * Search a key in the tree - Item of desired node is returned *
 ***************************************************************/

void *BinarySearchTree::searchItem(void *key, int (*compare)(void *, void *),
	Treenode **searched) const
{
	return searchItemRec(root, key, compare, searched);
}

void *BinarySearchTree::searchItemRec(Treenode *node, void *key,
	int (*compare)(void *, void *), Treenode **searched) const
{
	/* If the node is 'NULL', we return 'NULL' */

	if(node == NULL)
	{
		if(searched != NULL)
			(*searched) = NULL;

		return NULL;
	}

	/* We compare the given key to the node key */

	int c = compare(key, ((BSTObject *) node->getItem())->getKey());

	/* If the keys are same, the desired node
	 * is found and we return the item of it
	 */

	if(c == 0)
	{
		if(searched != NULL)
			(*searched) = node;

		return ((BSTObject *) node->getItem())->getItem();
	}

	/* Else, if the node key is greater than the given key,
	 * we search the left subtree of the node. In the same way,
	 * if the node key is lower than the given key, we search
	 * its right subtree
	 */

	if(c < 0)
		return searchItemRec(node->getLeft(), key, compare, searched);

	else
		return searchItemRec(node->getRight(), key, compare, searched);
}

/**************************************************************
 * Search a key in the tree - Key of desired node is returned *
 **************************************************************/

void *BinarySearchTree::searchKey(void *key, int (*compare)(void *, void *),
	Treenode **searched) const
{
	return searchKeyRec(root, key, compare, searched);
}

void *BinarySearchTree::searchKeyRec(Treenode *node, void *key,
	int (*compare)(void *, void *), Treenode **searched) const
{
	/* If the node is 'NULL', we return 'NULL' */

	if(node == NULL)
	{
		if(searched != NULL)
			(*searched) = NULL;

		return NULL;
	}

	/* We compare the given key to the node key */

	int c = compare(key, ((BSTObject *) node->getItem())->getKey());

	/* If the keys are same, the desired node
	 * is found and we return the item of it
	 */

	if(c == 0)
	{
		if(searched != NULL)
			(*searched) = node;

		return ((BSTObject *) node->getItem())->getKey();
	}

	/* Else, if the node key is greater than the given key,
	 * we search the left subtree of the node. In the same way,
	 * if the node key is lower than the given key, we search
	 * its right subtree
	 */

	if(c < 0)
		return searchKeyRec(node->getLeft(), key, compare, searched);

	else
		return searchKeyRec(node->getRight(), key, compare, searched);
}

/*************************************************************
 * Call the 'actions' operation for every pair of item & key *
 *   in the nodes of the tree in the given traversal order   *
 *************************************************************/

void BinarySearchTree::traverse(OrderOfTraversal traversal_order,
	void (*actions)(void *, void *))
{
	bstUserTraverse = actions;
	BinaryTree::traverse(traversal_order, bstImpTraverse);
}

/*******************************************************
 * Traverse operation that breaks the Complex Item to  *
 * its parts and then calls the user traverse function *
 *  Before we call 'bstUserTraverse' we make sure to   *
 * save the address of it in another function pointer  *
 *  (in case to retrieve it back if 'bstUserTraverse'  *
 *   overwrites itself to traverse another B.S.Tree    *
 *******************************************************/

void bstImpTraverse(void *item)
{
	BSTObject *bst_obj = (BSTObject *) item;
	void (*tempUserTraverse)(void *, void *) = bstUserTraverse;
	bstUserTraverse(bst_obj->getItem(), bst_obj->getKey());
	bstUserTraverse = tempUserTraverse;
}

/***************************************************
 * Delete the allocated 'BSTObject' from all nodes *
 ***************************************************/

void BinarySearchTree::deleteBSTObject(Treenode *node)
{
	if(node != NULL)
	{
		delete (BSTObject *) node->getItem();
		deleteBSTObject(node->getLeft());
		deleteBSTObject(node->getRight());
	}

	return;
}

/***********************************
 * Remove every node from the tree *
 ***********************************/

void BinarySearchTree::destroy()
{
	deleteBSTObject(root);
	BinaryTree::destroy();
}
