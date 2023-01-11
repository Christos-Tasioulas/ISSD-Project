#include <iostream>
#include "AB_Tree.h"

/****************************************************************
 * A function pointer used to store the user's compare function *
 ****************************************************************/

static int (*userCompare)(void *, void *);

/**************************************************************
 * Function pointers used to store the user's visit functions *
 **************************************************************/

static void (*userVisitItems)(void *);
static void (*userVisitKeys)(void *);
static void (*userVisitBoth)(void *, void *);

/*******************************************************
 * A function pointer used to store the user's actions *
 *******************************************************/

static void (*userActions)(void *, void *);

/******************************************
 * Compare function of the implementation *
 ******************************************/

static int abImpCompare(void *item1, void *item2)
{
	AB_Item *ab_item1 = (AB_Item *) item1;
	AB_Item *ab_item2 = (AB_Item *) item2;

	return userCompare(ab_item1->getKey(), ab_item2->getKey());
}

/*****************************************
 * Visit functions of the implementation *
 *****************************************/

static void abImpVisitItems(void *item)
{
	AB_Item *ab_item = (AB_Item *) item;
	userVisitItems(ab_item->getItem());
}

static void abImpVisitKeys(void *item)
{
	AB_Item *ab_item = (AB_Item *) item;
	userVisitKeys(ab_item->getKey());
}

static void abImpVisitBoth(void *item)
{
	AB_Item *ab_item = (AB_Item *) item;
	userVisitBoth(ab_item->getItem(), ab_item->getKey());
}

/*******************************************
 * Traverse function of the implementation *
 *******************************************/

static void abImpActions(void *item)
{
	AB_Item *ab_item = (AB_Item *) item;
	userActions(ab_item->getItem(), ab_item->getKey());
}

/******************************************
 * Destroy function of the implementation *
 ******************************************/

static void abImpDestroy(void *item)
{
	AB_Item *ab_item = (AB_Item *) item;
	delete ab_item;
}

/****************************
 * Constructor & Destructor *
 ****************************/

AB_Tree::AB_Tree(unsigned int A, unsigned int B)
{
	root = NULL;
	this->A = A;
	this->B = B;
	counter = 0;
}

AB_Tree::~AB_Tree()
{
	destroy();
}

/***********
 * Getters *
 ***********/

AB_Node *AB_Tree::getRoot() const
{
	return root;
}

unsigned int AB_Tree::getA() const
{
	return A;
}

unsigned int AB_Tree::getB() const
{
	return B;
}

unsigned int AB_Tree::getCounter() const
{
	return counter;
}

/*****************************************************************
 * Returns 'true' if the tree has no nodes, else returns 'false' *
 *****************************************************************/

bool AB_Tree::isEmpty() const
{
	return counter == 0;
}

/*******************************************
 * Returns the number of nodes in the tree *
 *******************************************/

unsigned int AB_Tree::size() const
{
	return sizeRec(root);
}

/***************************************************************************
 * Finds the number of nodes of the subtree with root being the given node *
 ***************************************************************************/

unsigned int AB_Tree::sizeRec(AB_Node *node) const
{
	/* If the node is 'NULL', there is no size */
	if(node == NULL)
		return 0;

	/* If the node is a leaf, its size is 1 */
	if(node->isLeaf())
		return 1;

	/* Else we initialize a size counter to 1,
	 * because we count instantly the given node
	 */
	unsigned int size = 1;

	/* We access the list of the children of the node */
	List *children = node->getChildren();

	/* Starting from the head of the list, we traverse
	 * the list and compute the size of each subtree
	 * that starts from a child of the current node.
	 */
	Listnode *current = children->getHead();

	/* As long as we have not reached the end of the list */
	while(current != NULL)
	{
		/* We retrieve the AB Node stored in the current node */
		AB_Node *current_child = (AB_Node *) current->getItem();

		/* We increase the total size by the size of the
		 * subtree that starts from the current child
		 */
		size += sizeRec(current_child);

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* Finally, we return the size of the subtree starting from 'node' */
	return size;
}

/**********************************
 * Returns the height of the tree *
 **********************************/

unsigned int AB_Tree::height() const
{
	/* We initialize the height to zero */
	unsigned int height = 0;

	/* If there is no root, we return the initial height (which is zero) */
	if(root == NULL)
		return height;

	/* Else by choosing any path from the root to any leaf we can find
	 * the height of the tree. For convenience, we will chose the path
	 * that is formed by traversing the leftmost child of each node.
	 *
	 * We will start taking our path from the root.
	 */
	AB_Node *nextNode = root;

	/* The level of the root is counted as one more level of height */
	height++;

	while(1)
	{
		/* We retrieve the children of the node */
		List *children = nextNode->getChildren();

		/* If the node is a leaf, we stop and return the final height */
		if(children->isEmpty())
			return height;

		/* Else we proceed to the leftmost child of the node */
		nextNode = (AB_Node *) children->getItemInPos(1);

		/* We count the level of the child as one more level of height */
		height++;
	}
}

/*************************************************************
 * Inserts an item in the tree which is accompanied by a key *
 *  A 'compare' operation must be given to compare the key   *
 *************************************************************/

void AB_Tree::insert(void *item, void *key, int (*compare)(void *, void *),
	bool *insertionWasSuccessful)
{
	/* First we consider that the insertion will be successful, so if
	 * a no-null 'insertionWasSuccessful' variable is given, we assign
	 * to that variable the value 'true'
	 */
	if(insertionWasSuccessful != NULL)
		(*insertionWasSuccessful) = true;

	/* We create a new AB Item with the given item and key.
	 * The new AB Item will be stored in the AB Tree.
	 */
	AB_Item *new_item = new AB_Item(item, key);

	/* Case the tree is empty */

	if(isEmpty())
	{
		/* We create a new root which is initialy empty */
		root = new AB_Node();

		/* We store the new AB Item in the root */
		root->insertBeforePos(new_item, 1);

		/* We increase the counter of items in the tree by 1 */
		counter++;

		/* There is nothing else to do in this case and so we return */
		return;
	}

	/* Case the tree is not empty
	 *
	 * We store the user's 'compare' function to the function pointer 'userCompare'
	 * because we want to use the operation 'abImpCompare' in the next instruction.
	 */
	userCompare = compare;

	/* We search for the correct place in the tree where we must
	 * put the new item (place = node + position in the node)
	 */
	KeyPosition *insertionPos = root->searchCorrectPosForKey(new_item, abImpCompare);

	/* If 'NULL' was returned, that means the new AB Item already exists in the tree.
	 * In that case we cannot re-insert the item in the tree. We delete the new item
	 * and return immediatelly.
	 */
	if(insertionPos == NULL)
	{
		/* In case a no-null 'insertionWasSuccessful' variable is given,
		 * we also inform the user that the insertion was not successful
		 * by assigning the value 'false' to that variable.
		 */
		if(insertionWasSuccessful != NULL)
			(*insertionWasSuccessful) = false;

		/* We delete the newly created AB Item and return immediatelly */

		delete new_item;
		return;
	}

	/* Else we retrieve the returned node and position
	 * in the node where we must insert the new AB Item
	 *
	 * This is the node where the item must be placed
	 */
	AB_Node *node = (AB_Node *) insertionPos->getItem();

	/* This is the position in the node where the item must be placed */
	int pos = insertionPos->getKey();

	/* Now we will insert the new AB Item in the tree, at the place that
	 * was requested by the operation 'AB_Node::searchCorrectPosForKey'
	 */
	node->insertBeforePos(new_item, pos);

	/* We need to rebalance the requested node because it may be overflown */
	node->rebalance(B, &root);

	/* We do not need the position of the new node anymore. We need to terminate
	 * it using the static operation 'AB_Node::terminatePosition' provided by the
	 * interface of the AB Node.
	 */
	AB_Node::terminatePosition(insertionPos);

	/* We increase the number of total items in the tree by 1 */
	counter++;
}

/**************************************************************
 * Removes a pair of item and key from the tree. The key that *
 * will be removed is the one which will be found to be equal *
 * to the given key after comparing the two with the provided *
 *                      compare function                      *
 **************************************************************/

void AB_Tree::remove(void *key, int (*compare)(void *, void *),
	bool *removalWasSuccessful)
{
	/* First, we examine if the desired key for deletion exists in the tree
	 *
	 * To do this, we store the user's 'compare' function to the function
	 * pointer 'userCompare', which in turn will be used by 'abImpCompare'.
	 */
	userCompare = compare;

	/* We create a "dummy" item that we will use to search the key */
	AB_Item temp_item = AB_Item(NULL, key);

	/* The compare operation 'abImpCompare' is the actual operation which
	 * we will use to compare the keys. We will search if the key exists
	 * in the subtree that starts from the root (which is the whole tree).
	 *
	 * If the key exists, we will receive via a 'KeyPosition' item the
	 * node and the position inside the node where the key is located.
	 * If the key does not exist, the searching result will be 'NULL'.
	 */
	KeyPosition *removalPos = root->searchKey(&temp_item, abImpCompare);

	/* If the key does not exist, the removal operation cannot be performed */
	if(removalPos == NULL)
	{
		/* In case a no-null 'removalWasSuccessful' variable is given,
		 * we also inform the user that the removal was not successful
		 * by assigning the value 'false' to that variable.
		 */
		if(removalWasSuccessful != NULL)
			(*removalWasSuccessful) = false;

		return;
	}

	/* If this part is reached, the key for deletion exists, so the
	 * removal operation can be performed normally and delete the key.
	 * If a no-null 'removalWasSuccessful' flag was given, we assign
	 * to that flag the value 'true', since a removal here is possible.
	 *
	 * We note that there are no other possible ways for a removal to
	 * fail other than the desired key for removal not existing in the
	 * tree. Consequently, if the key exists in the tree, we can safely
	 * claim that we will have a successful removal.
	 */
	if(removalWasSuccessful != NULL)
		(*removalWasSuccessful) = true;

	/* Case the tree has only one element */

	if(counter == 1)
	{
		/* After the deletion of the item, there will be nothing left
		 * in the tree. Consequently, we will get the same result if
		 * we just call 'destroy' to empty the tree
		 */
		destroy();

		/* We need to terminate the 'KeyPosition' item with the position
		 * of the key for deletion we received by the searching operation
		 * using the static operation 'AB_Node::terminatePosition' which
		 * is provided by the interface of the AB Node.
		 */
		AB_Node::terminatePosition(removalPos);

		/* In this special case there is nothing else to do */
		return;
	}

	/* We retrieve the node where the key is being stored */
	AB_Node *node = (AB_Node *) removalPos->getItem();

	/* We retrieve the position in the node where the key is stored */
	int pos = removalPos->getKey();

	/* Case the node with the key for deletion is not a leaf */

	if(!node->isLeaf())
	{
		/* We will use a temporary AB Node to help us in the removal process.
		 *
		 * We will assign to that temporary node the Inorder Predecessor. If
		 * the Inorder Predecessor has not more than 'A' items, we assign to
		 * 'temp' the Inorder Successor instead.
		 *
		 * In case the Inorder Predecessor is the final value of 'temp',
		 * that means we want to remove the key the user wants and place
		 * in its position the rightmost key of the Inorder Predecessor.
		 *
		 * In case the Inorder Successor is the final value of 'temp', we
		 * need to remove the key the user wants and put in its position
		 * the leftmost key of the Inorder Successor.
		 *
		 * The key that we need to use for the replacement will be stored
		 * in the 'item' variable below. We named this variable 'item' as
		 * it is an 'AB_Item' and in the grand scheme of things, the keys
		 * in the tree are 'AB_Items', which are compared by comparing the
		 * real keys of the user that are stored in those 'AB_Items'.
		 *
		 * The 'replacementKeyPos' variable indicates the position of the
		 * above item (that we will use for replacement) in the list of
		 * items of the predecessor/successor node that is stored in 'temp'.
		 */
		AB_Node *temp;
		AB_Item *item;
		unsigned int replacementKeyPos;

		/* We retrieve the Inorder Predecessor of the node */
		temp = node->inorderPredecessor(pos);

		/* The item we will use to replace the key for deletion is located
		 * at the end of the list of items of the Inorder Predecessor.
		 *
		 * Arithmetically, that position is the number of items in the node.
		 */
		replacementKeyPos = temp->getItemsNum();

		/* We get the amount of items in the Inorder Predecessor node */
		unsigned int predecessorsItemsNum = temp->getItemsNum();

		/* In case we cannot remove any keys from the Inorder Predecessor
		 * (because it has already the least amount of keys), we retrieve
		 * the Inorder Successor.
		 *
		 * If the successor does not have more than the minimum amount of
		 * items either, we will just remove the item we need from the
		 * successor and eventually it will be balanced later. Couldn't we
		 * have done this with the predecessor too? Yes, but we choose to
		 * test both the path of the predecessor and the successor because
		 * we will save time in case the predecessor needs rebalancing but
		 * the successor does not.
		 */
		if(predecessorsItemsNum == A-1)
		{
			/* We retrieve the Inorder Successor node */
			temp = node->inorderSuccessor(pos);

			/* The succeeding key of the key for deletion is the leftmost
			 * item of the Inorder Successor, which is located in position 1
			 * of the list of items
			 */
			replacementKeyPos = 1;
		}

		/* We retrieve the item stored in the position of the replacement key
		 * This is the item we will use to replace the key for deletion.
		 */
		item = (AB_Item *) temp->getItems()->getItemInPos(replacementKeyPos);

		/* We remove the item we will use for replacement from the Inorder
		 * Predecessor (or Successor) node where it is stored right now.
		 */
		temp->removeItemInPos(replacementKeyPos);

		/* We remove the key for deletion from the node that contains it */
		node->removeItemInPos(pos, abImpDestroy);

		/* We insert to that node the item that will replace that key */
		node->insertBeforePos(item, pos);

		/* We update the initial node with the auxiliary node we used
		 * (because now we need to balance the auxiliary node).
		 *
		 * The initial node has no more or less items right now and
		 * therefore does not need rebalancing. For that node, nothing
		 * special has changed.
		 */
		node = temp;
	}

	/* Case the node with the key for deletion is a leaf */

	else
	{
		/* We remove the key in the suggested node and position in the node
		 * and we also use the destroy function of the implementation to
		 * free the allocated memory for the 'AB_Item' which we need to drop.
		 */
		node->removeItemInPos(pos, abImpDestroy);
	}

	/* We have to refill the node with an item borrowed from its parent in case
	 * the node has now less items than the minimum amount.
	 *
	 * Then the parent will either be refilled too by an item of a sibling with
	 * more than the minimum amount of items or a fusion between the node and
	 * its left or its right sibling will take place, lowering both the amount
	 * of items and the amount of children of the parent by 1.
	 *
	 * Then, the parent is rebalanced too if it has less than minimum items.
	 *
	 * In the above process, we may need to delete some nodes. This is logical,
	 * because in the fusion process two nodes become one, so we have to delete
	 * the spare node which is now useless. Because multiple fusions may take
	 * place recursively, there may be multiple nodes that need to be deleted.
	 *
	 * All those nodes that need to be deleted are saved in the trash can that
	 * we will provide to the operation 'AB_Node::refill'. Then, we will visit
	 * and delete every AB Node in the trash can.
	 *
	 * Why the 'refill' operation cannot delete the nodes by itself?
	 *
	 * Because sometimes the node for deletion is the value "this", which cannot
	 * be deleted. If the node for deletion is not "this", the operation will
	 * delete it by itself. That means, the trash can in the end will have all
	 * nodes that were the value "this" at some point and could not be deleted.
	 *
	 * Here we initialize the trash can
	 */
	List trashNodes = List();

	/* We refill the node in case it has less than the minimum amount of items */
	node->refill(A, &root, &trashNodes);

	/* We traverse the trash can serially and delete each node we encounter */
	trashNodes.traverseFromHead(delete_AB_Node);

	/* We need to terminate the 'KeyPosition' item with the position of the key
	 * for deletion we received by the search operation with the static operation
	 * 'AB_Node::terminatePosition' provided by the interface of the AB Node.
	 */
	AB_Node::terminatePosition(removalPos);

	/* We decrease the number of total items in the tree by 1 */
	counter--;
}

/*********************************************************************
 * Casts the 'item' to 'AB_Node *' and then deletes it with 'delete' *
 *********************************************************************/

void AB_Tree::delete_AB_Node(void *item)
{
	AB_Node *node = (AB_Node *) item;
	delete node;
}

/****************************************************************
 * Searches the given key in the tree using the given 'compare' *
 * operation and returns 'true' if the key exists, else 'false' *
 ****************************************************************/

bool AB_Tree::search(void *key, int (*compare)(void *, void *))
{
	/* If the tree is empty, the search of any key is unsuccessful */
	if(isEmpty())
		return false;

	/* We store the user's compare function to the function pointer 'userCompare' */
	userCompare = compare;

	/* We create a "dummy" item that we will use to search in the tree */
	AB_Item temp_item = AB_Item(NULL, key);

	/* We search in the tree for the given key using the above "dummy" item
	 * and the compare function of the implementation of this tree, which
	 * eventually invokes the function 'userCompare' which stores 'compare'.
	 */
	KeyPosition *search_result = root->searchCorrectPosForKey(&temp_item, abImpCompare);

	/* If the returned result of the search was 'NULL', that means the key was
	 * found in the tree (Yes! If the result is 'NULL', the search is successful!)
	 */
	bool final_result = (search_result == NULL) ? true : false;

	/* We have to terminate the returned result by the searching method */
	AB_Node::terminatePosition(search_result);

	/* We return the final result of searching */
	return final_result;
}

/****************************************************************
 * Searches the given key in the tree using the given 'compare' *
 *   operation. If the key exists 'true' is returned and also:  *
 *                                                              *
 * - If a no-null 'retrievedItem' address has been given,       *
 *   it will be assigned the "item" of the node with the        *
 *   identical key that was found by the searching method       *
 *                                                              *
 * - If a no-null 'retrievedKey' address has been given,        *
 *   it will be assigned the "key" of the node with the         *
 *   identical key that was found by the searching method       *
 *                                                              *
 *   If the given key does not exist in the tree, 'false' is    *
 *    returned and if any of the two optional addresses is a    *
 *  no-null pointer, 'NULL' is assigned to the content pointed  *
 *                            by it                             *
 ****************************************************************/

bool AB_Tree::searchAndRetrieve(void *key, int (*compare)(void *, void *),
	void **retrievedItem, void **retrievedKey)
{
	/* If the tree is empty, the search of any key is unsuccessful */
	if(isEmpty())
	{
		if(retrievedItem != NULL)
			(*retrievedItem) = NULL;

		if(retrievedKey != NULL)
			(*retrievedKey) = NULL;

		return false;
	}

	/* We store the user's compare function to the function pointer 'userCompare' */
	userCompare = compare;

	/* We create a "dummy" item that we will use to search in the tree */
	AB_Item temp_item = AB_Item(NULL, key);

	/* We search in the tree for the given key using the above "dummy" item
	 * and the compare function of the implementation of this tree, which
	 * eventually invokes the function 'userCompare' which stores 'compare'.
	 */
	void *search_result = root->searchAndRetrieveKey(&temp_item, abImpCompare);

	/* If the returned key is not 'NULL', the search was successful*/
	bool final_result = (search_result != NULL) ? true : false;

	/* We update the pointers for retrieval
	 *
	 * Case the search was successful
	 */
	if(final_result == true)
	{
		AB_Item *searched_item = (AB_Item *) search_result;

		if(retrievedItem != NULL)
			(*retrievedItem) = searched_item->getItem();

		if(retrievedKey != NULL)
			(*retrievedKey) = searched_item->getKey();
	}

	/* Case the search was not successful */

	else
	{
		if(retrievedItem != NULL)
			(*retrievedItem) = NULL;

		if(retrievedKey != NULL)
			(*retrievedKey) = NULL;
	}

	/* We return the final result of searching */
	return final_result;
}

/*******************************************************************************
 * Prints the items of the tree with the given 'visit' operation in the given  *
 *  traversal order. If a no-null 'contextBetweenItems' function is given, it  *
 *  will be invoked to print the desired context between the items. Similarly, *
 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to *
 * print the desired context between the nodes of the tree. When the order of  *
 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will  *
 *  be used to print context between the nodes of different levels if it is a  *
 * no-null value. For 'Inorder' traversal the context between nodes cannot be  *
 *  used, because a part of a node is printed, then a part of another node is  *
 * printed etc, and not the whole node, which means there are not any nodes to *
 * seperate with context. The contents of the nodes are blended in the result  *
 *******************************************************************************/

void AB_Tree::printItems(OrderOfTraversal traversalOrder,
	void (*visit)(void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	printItemsRec(root, traversalOrder, visit,
		contextBetweenItems,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*****************************************************
 *  Prints recursively the items of the given node   *
 * and all its children in the given traversal order *
 *****************************************************/

void AB_Tree::printItemsRec(AB_Node *node, OrderOfTraversal traversalOrder,
	void (*visit)(void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	/* If the node is 'NULL', we return immediatelly */

	if(node == NULL)
		return;

	switch(traversalOrder)
	{
		/* Case the traversal order is 'Preorder' */

		case Preorder:
		{
			/* We print the contents of the current node */
			userVisitItems = visit;
			node->printItems(abImpVisitItems, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printItemsRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			break;
		}

		/* Case the traversal order is 'Inorder' */

		case Inorder:
		{
			/* Subcase 1: The node is a leaf */

			if(node->isLeaf())
			{
				/* We print the contents of the current node */
				userVisitItems = visit;
				node->printItems(abImpVisitItems, contextBetweenItems);
			}

			/* Subcase 2: The node is an internal node */

			else
			{
				/* We retrieve the list of items in the node */
				List *items = node->getItems();

				/* We retrieve the list of children of the node */
				List *children = node->getChildren();

				/* We will start traversing the items' list from the head */
				Listnode *currentItem = items->getHead();

				/* We will aslo traverse the children's list from the head */
				Listnode *currentChild = children->getHead();

				while(currentItem != NULL)
				{
					/* We retrieve the AB Node stored
					 * in the 'currentChild' Listnode
					 */
					AB_Node *child = (AB_Node *) currentChild->getItem();

					/* We visit that node */
					printItemsRec(child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We print the given context after
					 * the last item of the above child
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We retrieve the AB Item stored
					 * in the 'currentItem' Listnode
					 */
					AB_Item *item = (AB_Item *) currentItem->getItem();

					/* We store the user's visit function to the function
					 * pointer 'userVisitItems', used by 'abImpVisitItems'
					 */
					userVisitItems = visit;

					/* We print that item */
					abImpVisitItems(item);

					/* If any context between items must
					 * be printed, we print it now
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We proceed to the next child */
					currentChild = currentChild->getNext();

					/* We proceed to the next item */
					currentItem = currentItem->getNext();
				}

				/* We retrieve the last child */
				AB_Node *child = (AB_Node *) currentChild->getItem();

				/* We visit the last child */
				printItemsRec(child, traversalOrder, visit,
					contextBetweenItems, contextBetweenNodes);
			}

			break;
		}

		/* Case the traversal order is 'Postorder' */

		case Postorder:
		{
			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printItemsRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			/* We print the contents of the current node */
			userVisitItems = visit;
			node->printItems(abImpVisitItems, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			break;
		}

		/* Case the traversal order is 'Levelorder' */

		case Levelorder:
		{
			/* First we find the height of the tree */
			unsigned int treeHeight = height();
			unsigned int i;

			/* For each level of the tree, we print the
			 * nodes of the level (the root level is 1)
			 */
			for(i = 1; i <= treeHeight; i++)
			{
				/* We store the given visit function to the function
				 * pointer 'userVisitItems', used by 'abImpVisitItems'
				 */
				userVisitItems = visit;

				/* In this traversal, the context between nodes in not
				 * only printed between the nodes, but also before the
				 * first node of each level and after the last node of
				 * each level
				 */
				if(contextBetweenNodes != NULL)
					contextBetweenNodes();

				/* We print the content of each node in level 'i' */
				printLevel(node, i, abImpVisitItems,
					contextBetweenItems,
					contextBetweenNodes);

				/* If a no-null operation for context between levels
				 * has been given, it will be invoked to print some
				 * context between nodes of diffrent levels
				 */
				if(contextBetweenLevels != NULL)
					contextBetweenLevels();
			}

			break;
		}
	}
}

/*******************************************************************************
 *  Prints the keys of the tree with the given 'visit' operation in the given  *
 *  traversal order. If a no-null 'contextBetweenItems' function is given, it  *
 *  will be invoked to print the desired context between the items. Similarly, *
 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to *
 * print the desired context between the nodes of the tree. When the order of  *
 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will  *
 *  be used to print context between the nodes of different levels if it is a  *
 * no-null value. For 'Inorder' traversal the context between nodes cannot be  *
 *  used, because a part of a node is printed, then a part of another node is  *
 * printed etc, and not the whole node, which means there are not any nodes to *
 * seperate with context. The contents of the nodes are blended in the result  *
 *******************************************************************************/

void AB_Tree::printKeys(OrderOfTraversal traversalOrder,
	void (*visit)(void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	printKeysRec(root, traversalOrder, visit,
		contextBetweenItems,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*****************************************************
 *   Prints recursively the keys of the given node   *
 * and all its children in the given traversal order *
 *****************************************************/

void AB_Tree::printKeysRec(AB_Node *node, OrderOfTraversal traversalOrder,
	void (*visit)(void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	/* If the node is 'NULL', we return immediatelly */

	if(node == NULL)
		return;

	switch(traversalOrder)
	{
		/* Case the traversal order is 'Preorder' */

		case Preorder:
		{
			/* We print the contents of the current node */
			userVisitKeys = visit;
			node->printItems(abImpVisitKeys, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printKeysRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			break;
		}

		/* Case the traversal order is 'Inorder' */

		case Inorder:
		{
			/* Subcase 1: The node is a leaf */

			if(node->isLeaf())
			{
				/* We print the contents of the current node */
				userVisitKeys = visit;
				node->printItems(abImpVisitKeys, contextBetweenItems);
			}

			/* Subcase 2: The node is an internal node */

			else
			{
				/* We retrieve the list of items in the node */
				List *items = node->getItems();

				/* We retrieve the list of children of the node */
				List *children = node->getChildren();

				/* We will start traversing the items' list from the head */
				Listnode *currentItem = items->getHead();

				/* We will aslo traverse the children's list from the head */
				Listnode *currentChild = children->getHead();

				while(currentItem != NULL)
				{
					/* We retrieve the AB Node stored
					 * in the 'currentChild' Listnode
					 */
					AB_Node *child = (AB_Node *) currentChild->getItem();

					/* We visit that node */
					printKeysRec(child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We print the given context after
					 * the last item of the above child
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We retrieve the AB Item stored
					 * in the 'currentItem' Listnode
					 */
					AB_Item *item = (AB_Item *) currentItem->getItem();

					/* We store the user's visit function to the function
					 * pointer 'userVisitKeys', used by 'abImpVisitKeys'
					 */
					userVisitKeys = visit;

					/* We print that item */
					abImpVisitKeys(item);

					/* If any context between items must
					 * be printed, we print it now
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We proceed to the next child */
					currentChild = currentChild->getNext();

					/* We proceed to the next item */
					currentItem = currentItem->getNext();
				}

				/* We retrieve the last child */
				AB_Node *child = (AB_Node *) currentChild->getItem();

				/* We visit the last child */
				printKeysRec(child, traversalOrder, visit,
					contextBetweenItems, contextBetweenNodes);
			}

			break;
		}

		/* Case the traversal order is 'Postorder' */

		case Postorder:
		{
			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printKeysRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			/* We print the contents of the current node */
			userVisitKeys = visit;
			node->printItems(abImpVisitKeys, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			break;
		}

		/* Case the traversal order is 'Levelorder' */

		case Levelorder:
		{
			/* First we find the height of the tree */
			unsigned int treeHeight = height();
			unsigned int i;

			/* For each level of the tree, we print the
			 * nodes of the level (the root level is 1)
			 */
			for(i = 1; i <= treeHeight; i++)
			{
				/* We store the given visit function to the function
				 * pointer 'userVisitKeys', used by 'abImpVisitKeys'
				 */
				userVisitKeys = visit;

				/* In this traversal, the context between nodes in not
				 * only printed between the nodes, but also before the
				 * first node of each level and after the last node of
				 * each level
				 */
				if(contextBetweenNodes != NULL)
					contextBetweenNodes();

				/* We print the content of each node in level 'i' */
				printLevel(node, i, abImpVisitKeys,
					contextBetweenItems,
					contextBetweenNodes);

				/* If a no-null operation for context between levels
				 * has been given, it will be invoked to print some
				 * context between nodes of diffrent levels
				 */
				if(contextBetweenLevels != NULL)
					contextBetweenLevels();
			}

			break;
		}
	}
}

/*******************************************************************************
 *  Prints both items and keys with the given 'visit' operation in the given   *
 *  traversal order. If a no-null 'contextBetweenItems' function is given, it  *
 *  will be invoked to print the desired context between the items. Similarly, *
 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to *
 * print the desired context between the nodes of the tree. When the order of  *
 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will  *
 *  be used to print context between the nodes of different levels if it is a  *
 * no-null value. For 'Inorder' traversal the context between nodes cannot be  *
 *  used, because a part of a node is printed, then a part of another node is  *
 * printed etc, and not the whole node, which means there are not any nodes to *
 * seperate with context. The contents of the nodes are blended in the result  *
 *******************************************************************************/

void AB_Tree::printBoth(OrderOfTraversal traversalOrder,
	void (*visit)(void *, void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	printBothRec(root, traversalOrder, visit,
		contextBetweenItems,
		contextBetweenNodes,
		contextBetweenLevels);
}

/***************************************************************
 * Prints recursively both the items and the keys of the given *
 *   node and all its children in the given traversal order    *
 ***************************************************************/

void AB_Tree::printBothRec(AB_Node *node, OrderOfTraversal traversalOrder,
	void (*visit)(void *, void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	/* If the node is 'NULL', we return immediatelly */

	if(node == NULL)
		return;

	switch(traversalOrder)
	{
		/* Case the traversal order is 'Preorder' */

		case Preorder:
		{
			/* We print the contents of the current node */
			userVisitBoth = visit;
			node->printItems(abImpVisitBoth, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printBothRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			break;
		}

		/* Case the traversal order is 'Inorder' */

		case Inorder:
		{
			/* Subcase 1: The node is a leaf */

			if(node->isLeaf())
			{
				/* We print the contents of the current node */
				userVisitBoth = visit;
				node->printItems(abImpVisitBoth, contextBetweenItems);
			}

			/* Subcase 2: The node is an internal node */

			else
			{
				/* We retrieve the list of items in the node */
				List *items = node->getItems();

				/* We retrieve the list of children of the node */
				List *children = node->getChildren();

				/* We will start traversing the items' list from the head */
				Listnode *currentItem = items->getHead();

				/* We will aslo traverse the children's list from the head */
				Listnode *currentChild = children->getHead();

				while(currentItem != NULL)
				{
					/* We retrieve the AB Node stored
					 * in the 'currentChild' Listnode
					 */
					AB_Node *child = (AB_Node *) currentChild->getItem();

					/* We visit that node */
					printBothRec(child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We print the given context after
					 * the last item of the above child
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We retrieve the AB Item stored
					 * in the 'currentItem' Listnode
					 */
					AB_Item *item = (AB_Item *) currentItem->getItem();

					/* We store the user's visit function to the function
					 * pointer 'userVisitKeys', used by 'abImpVisitKeys'
					 */
					userVisitBoth = visit;

					/* We print that item */
					abImpVisitBoth(item);

					/* If any context between items must
					 * be printed, we print it now
					 */
					if(contextBetweenItems != NULL)
						contextBetweenItems();

					/* We proceed to the next child */
					currentChild = currentChild->getNext();

					/* We proceed to the next item */
					currentItem = currentItem->getNext();
				}

				/* We retrieve the last child */
				AB_Node *child = (AB_Node *) currentChild->getItem();

				/* We visit the last child */
				printBothRec(child, traversalOrder, visit,
					contextBetweenItems, contextBetweenNodes);
			}

			break;
		}

		/* Case the traversal order is 'Postorder' */

		case Postorder:
		{
			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					printBothRec(current_child, traversalOrder, visit,
						contextBetweenItems, contextBetweenNodes);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			/* We print the contents of the current node */
			userVisitBoth = visit;
			node->printItems(abImpVisitBoth, contextBetweenItems);

			/* After printing the contents of the current node, we
			 * print the context between nodes if any has been given
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			break;
		}

		/* Case the traversal order is 'Levelorder' */

		case Levelorder:
		{
			/* First we find the height of the tree */
			unsigned int treeHeight = height();
			unsigned int i;

			/* For each level of the tree, we print the
			 * nodes of the level (the root level is 1)
			 */
			for(i = 1; i <= treeHeight; i++)
			{
				/* We store the given visit function to the function
				 * pointer 'userVisitBoth', used by 'abImpVisitBoth'
				 */
				userVisitBoth = visit;

				/* In this traversal, the context between nodes in not
				 * only printed between the nodes, but also before the
				 * first node of each level and after the last node of
				 * each level
				 */
				if(contextBetweenNodes != NULL)
					contextBetweenNodes();

				/* We print the content of each node in level 'i' */
				printLevel(node, i, abImpVisitBoth,
					contextBetweenItems,
					contextBetweenNodes);

				/* If a no-null operation for context between levels
				 * has been given, it will be invoked to print some
				 * context between nodes of diffrent levels
				 */
				if(contextBetweenLevels != NULL)
					contextBetweenLevels();
			}

			break;
		}
	}
}

/*********************************************************************
 * Prints only the nodes of the given level (the root is on level 1) *
 *********************************************************************/

void AB_Tree::printLevel(AB_Node *node, unsigned int level,
	void (*visit)(void *),
	void (*contextBetweenItems)(),
	void (*contextBetweenNodes)())
{
	/* If the node is 'NULL' or the level is smaller than
	 * one, there is nothing to do in this operation
	 */
	if(node == NULL || level < 1)
		return;

	/* Base Case: The level is 1
	 * ^^^^^^^^^
	 *
	 * We print the node and then
	 * the context between nodes
	 */
	if(level == 1)
	{
		/* Here we visit the given node. We take care to store
		 * the user's functions to temporary variables.
		 *
		 * If the AB Tree consists of inner AB Trees, then at
		 * the time those inner AB Trees are visited the user's
		 * functions will be overwritten so as to visit properly
		 * each of the inner AB Trees. When that happens, we do
		 * not want to lose the initial values of the user's
		 * functions, which are used to visit the outer tree.
		 * For that reason, we save these values to temporary
		 * function pointers so as to retrieve them back later.
		 */
		void (*tempUserVisitItems)(void *) = userVisitItems;
		void (*tempUserVisitKeys)(void *) = userVisitKeys;
		void (*tempUserVisitBoth)(void *, void *) = userVisitBoth;

		/* We visit the items of the node */
		node->printItems(visit, contextBetweenItems);

		/* We retrieve the initial values of the user's functions */
		userVisitItems = tempUserVisitItems;
		userVisitKeys = tempUserVisitKeys;
		userVisitBoth = tempUserVisitBoth;

		/* If any no-null context between nodes
		 * has been given, we print it now
		 */
		if(contextBetweenNodes != NULL)
			contextBetweenNodes();
	}

	/* Recursion Case: The level is greater than 1
	 * ^^^^^^^^^^^^^^
	 *
	 * The aim is to print all nodes of the level that is given through
	 * the argument 'level'. This operation prints a node only if the
	 * given level is 1. If the level is greater than 1, the operation
	 * does not print directly any node, but instead it calls itself for
	 * all the children of the node with one less level. This will allow
	 * us to travel to the required depth in the tree in order to find
	 * and finally print the nodes of the suggested level.
	 */
	else
	{
		/* We retrieve the list of the node's children */
		List *children = node->getChildren();

		/* We will traverse the list starting from the head */
		Listnode *current = children->getHead();

		/* As long as we have not reached the end of the list */
		while(current != NULL)
		{
			/* We retrieve the child saved in the current node */
			AB_Node *currentChild = (AB_Node *) current->getItem();

			/* We call the operation recursively
			 * for the child and one less level
			 */
			printLevel(currentChild, level - 1, visit,
				contextBetweenItems, contextBetweenNodes);

			/* We proceed to the next node */
			current = current->getNext();
		}
	}
}

/***********************************************************************
 *  This operation should be used when the user wants to execute some  *
 *   specific actions for each pair of 'item' and 'key' in the tree    *
 *                                                                     *
 *  The user must first create a function that expects an item and a   *
 *   key and applies the actions the user wants on that item and key   *
 *                                                                     *
 * In the 'traverse' operation, the above user function will be called *
 * for all the pairs of 'item' and 'key' of each node in the suggested *
 *    traversal order (Preorder, Inorder, Postorder or Levelorder)     *
 ***********************************************************************/

void AB_Tree::traverse(OrderOfTraversal traversalOrder, void (*actions)(void *, void *))
{
	traverseRec(root, traversalOrder, actions);
}

/******************************************************************
 * Traverses recursively both the items and the keys of the given *
 *     node and all its children in the given traversal order     *
 ******************************************************************/

void AB_Tree::traverseRec(AB_Node *node, OrderOfTraversal traversalOrder,
	void (*actions)(void *, void *))
{
	/* If the node is 'NULL', we return immediatelly */

	if(node == NULL)
		return;

	switch(traversalOrder)
	{
		/* Case the traversal order is 'Preorder' */

		case Preorder:
		{
			/* We retrieve the list of AB Items of the node */
			List *items = node->getItems();

			/* We store the user's 'actions' operation to the
			 * function pointer 'userActions', which in turn
			 * will be used by the operation 'abImpActions'
			 */
			userActions = actions;

			/* We traverse the list of items of the node with
			 * the traverse operation of the implementation
			 */
			items->traverseFromHead(abImpActions);

			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We traverse that AB Node */
					traverseRec(current_child, traversalOrder, actions);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			break;
		}

		/* Case the traversal order is 'Inorder' */

		case Inorder:
		{
			/* Subcase 1: The node is a leaf */

			if(node->isLeaf())
			{
				/* We traverse the contents of the current node
				 *
				 * We retrieve the list of AB Items of the node
				 */
				List *items = node->getItems();

				/* We store the user's 'actions' operation to the
				 * function pointer 'userActions', which in turn
				 * will be used by the operation 'abImpActions'
				 */
				userActions = actions;

				/* We traverse the list of items of the node with
				 * the traverse operation of the implementation
				 */
				items->traverseFromHead(abImpActions);
			}

			/* Subcase 2: The node is an internal node */

			else
			{
				/* We retrieve the list of items in the node */
				List *items = node->getItems();

				/* We retrieve the list of children of the node */
				List *children = node->getChildren();

				/* We will start traversing the items' list from the head */
				Listnode *currentItem = items->getHead();

				/* We will aslo traverse the children's list from the head */
				Listnode *currentChild = children->getHead();

				while(currentItem != NULL)
				{
					/* We retrieve the AB Node stored
					 * in the 'currentChild' Listnode
					 */
					AB_Node *child = (AB_Node *) currentChild->getItem();

					/* We visit that node */
					traverseRec(child, traversalOrder, actions);

					/* We retrieve the AB Item stored
					 * in the 'currentItem' Listnode
					 */
					AB_Item *item = (AB_Item *) currentItem->getItem();

					/* We store the user's 'actions' function to the function
					 * pointer 'userActions', which is used by 'abImpActions'
					 */
					userActions = actions;

					/* We print that item */
					abImpActions(item);

					/* We proceed to the next child */
					currentChild = currentChild->getNext();

					/* We proceed to the next item */
					currentItem = currentItem->getNext();
				}

				/* We retrieve the last child */
				AB_Node *child = (AB_Node *) currentChild->getItem();

				/* We traverse the last child */
				traverseRec(child, traversalOrder, actions);
			}

			break;
		}

		/* Case the traversal order is 'Postorder' */

		case Postorder:
		{
			/* If the current node is not a leaf, we print
			 * the contents of all the children of the node
			 */
			if(!node->isLeaf())
			{
				/* Here we retrieve the list of children and
				 * we prepare a listnode to traverse the list
				 */
				List *children = node->getChildren();
				Listnode *current = children->getHead();

				/* As long as we have not reached
				 * the end of the list of children
				 */
				while(current != NULL)
				{
					/* We retrieve the AB Node stored in the current node */
					AB_Node *current_child = (AB_Node *) current->getItem();

					/* We visit that AB Node */
					traverseRec(current_child, traversalOrder, actions);

					/* We proceed to the next node */
					current = current->getNext();
				}
			}

			/* We retrieve the list of AB Items of the node */
			List *items = node->getItems();

			/* We store the user's 'actions' operation to the
			 * function pointer 'userActions', which in turn
			 * will be used by the operation 'abImpActions'
			 */
			userActions = actions;

			/* We traverse the list of items of the node with
			 * the traverse operation of the implementation
			 */
			items->traverseFromHead(abImpActions);

			break;
		}

		/* Case the traversal order is 'Levelorder' */

		case Levelorder:
		{
			/* First we find the height of the tree */
			unsigned int treeHeight = height();
			unsigned int i;

			/* For each level of the tree, we traverse the
			 * nodes of the level (the root level is 1)
			 */
			for(i = 1; i <= treeHeight; i++)
			{
				/* We print the content of each node in level 'i' */
				traverseLevel(node, i, actions);
			}

			break;
		}
	}
}

/************************************************************************
 * Traverses only the nodes of the given level (the root is on level 1) *
 ************************************************************************/

void AB_Tree::traverseLevel(AB_Node *node, unsigned int level,
	void (*actions)(void *, void *))
{
	/* If the node is 'NULL' or the level is smaller than
	 * one, there is nothing to do in this operation
	 */
	if(node == NULL || level < 1)
		return;

	/* Base Case: The level is 1
	 * ^^^^^^^^^
	 *
	 * We print the node and then
	 * the context between nodes
	 */
	if(level == 1)
	{
		/* We retrieve the list of AB Items of the node */
		List *items = node->getItems();

		/* We store the user's 'actions' operation to the
		 * function pointer 'userActions', which in turn
		 * will be used by the operation 'abImpActions'
		 */
		userActions = actions;

		/* We traverse the list of items of the node with
		 * the traverse operation of the implementation
		 */
		items->traverseFromHead(abImpActions);
	}

	/* Recursion Case: The level is greater than 1
	 * ^^^^^^^^^^^^^^
	 *
	 * The aim is to traverse all nodes of the level that is given through the argument
	 * 'level'. This operation traverses a node only if the given level is 1. If the
	 * level is greater than 1, the operation does not traverse directly any node, but
	 * instead it calls itself for all the children of the node with one less level.
	 * This will allow us to travel to the required depth in the tree in order to find
	 * and finally traverse the nodes of the suggested level.
	 */
	else
	{
		/* We retrieve the list of the node's children */
		List *children = node->getChildren();

		/* We will traverse the list starting from the head */
		Listnode *current = children->getHead();

		/* As long as we have not reached the end of the list */
		while(current != NULL)
		{
			/* We retrieve the child saved in the current node */
			AB_Node *currentChild = (AB_Node *) current->getItem();

			/* We call the operation recursively
			 * for the child and one less level
			 */
			traverseLevel(currentChild, level - 1, actions);

			/* We proceed to the next node */
			current = current->getNext();
		}
	}
}

/**************************************************************************
 * Destroys the children of the given AB Node and then the AB Node itself *
 **************************************************************************/

void AB_Tree::destroyRec(AB_Node *node)
{
	/* If the node is 'NULL', there is nothing to destroy */
	if(node == NULL)
		return;

	/* If the node is an internal node, we recursively
	 * destroy all the children of the node first
	 */
	if(!node->isLeaf())
	{
		/* We retrieve the list of children */
		List *children = node->getChildren();

		/* We traverse the list starting from the head */
		Listnode *current = children->getHead();

		/* As long as we have not reached the end of the list */
		while(current != NULL)
		{
			/* We retrieve the child stored in the current node */
			AB_Node *current_child = (AB_Node *) current->getItem();

			/* We destroy the subtree that starts from that child */
			destroyRec(current_child);

			/* We proceed to the next node */
			current = current->getNext();
		}
	}

	/* We destroy all the items stored in the node
	 *
	 * We retrieve the list of items
	 */
	List *items = node->getItems();

	/* We traverse the list starting from the head */
	Listnode *current = items->getHead();

	/* As long as we have not reached the end of the list */
	while(current != NULL)
	{
		/* We retrieve the item stored in the current node */
		AB_Item *stored_item = (AB_Item *) current->getItem();

		/* We delete that item */
		delete stored_item;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* Finally, we delete the node itself */
	delete node;
}

/*************************************
 * Destroys every node from the tree *
 *************************************/

void AB_Tree::destroy()
{
	/* We destroy the subtree that starts from
	 * the root (which is the tree itself)
	 */
	destroyRec(root);

	/* We reset the counter of items to zero */
	counter = 0;

	/* We also set the value of root to 'NULL' */
	root = NULL;
}
