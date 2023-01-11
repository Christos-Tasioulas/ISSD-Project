#include <iostream>
#include "AB_Node.h"

/****************************
 * Constructor & Destructor *
 ****************************/

AB_Node::AB_Node()
{
	items = new List();
	children = new List();
	parent = NULL;
}

AB_Node::~AB_Node()
{
	delete items;
	delete children;
}

/***********
 * Getters *
 ***********/

List *AB_Node::getItems() const
{
	return items;
}

List *AB_Node::getChildren() const
{
	return children;
}

AB_Node *AB_Node::getParent() const
{
	return parent;
}

/***********
 * Setters *
 ***********/

void AB_Node::setItems(List *new_items)
{
	items = new_items;
}

void AB_Node::setChildren(List *new_children)
{
	children = new_children;
}

void AB_Node::setParent(AB_Node *parent)
{
	this->parent = parent;
}

/***************************************************
 * Returns the current amount of items in the node *
 ***************************************************/

unsigned int AB_Node::getItemsNum()
{
	return items->getCounter();
}

/******************************************************
 * Returns the current amount of children in the node *
 ******************************************************/

unsigned int AB_Node::getChildrenNum()
{
	return children->getCounter();
}

/**********************************************
 * Returns 'true' if the node has no children *
 **********************************************/

bool AB_Node::isLeaf() const
{
	return children->isEmpty();
}

/******************************************************************
 * Inserts the given item in the node before the implied position *
 * If the implied position is greater than the items in the node, *
 * the given item is stored in the rightmost position in the node *
 ******************************************************************/

void AB_Node::insertBeforePos(void *item, unsigned int pos)
{
	/* We retrieve the number of items in the node */
	unsigned int itemsNum = getItemsNum();

	/* If the implied position is greater than the amount of items
	 * in the node, we place the new item in the end of the list
	 */
	if(pos > itemsNum)
		this->items->insertLast(item);

	/* Else we normally place the item before the requested position */
	else
		this->items->insertBeforePos(item, pos);
}

/**********************************************************
 * Rebalances the node in case it is overflown with items *
 **********************************************************/

void AB_Node::rebalance(unsigned int maxItemsAllowed, AB_Node **root)
{
	/* We retrieve the number of items in the node */
	unsigned int itemsNum = getItemsNum();

	/* If the maximum amount of items in the node
	 * has not been reached, we return immediatelly
	 */
	if(itemsNum < maxItemsAllowed)
		return;

	/* We find the index of the middle item of the node */
	unsigned int middle_pos = (itemsNum + 1) / 2;

	/* We retrieve the item in the middle position of the node */
	void *middle_item = this->items->getItemInPos(middle_pos);

	/* We remove the middle item from the node */
	this->items->removePos(middle_pos);

	/* We are going to split the list of items of this node in two
	 * equal parts. We will refer to them as left and right part.
	 * We will use the operation 'List::split' to do this action.
	 */
	List *leftPart;
	List *rightPart;

	/* Here we split the list in two equal parts */
	this->items->split(&leftPart, &rightPart);

	/* We delete the current list of items */
	delete this->items;

	/* This node will only have the items of the left part instead */
	this->items = leftPart;

	/* We create the node that will have the items of the right part */
	AB_Node *rightNode = new AB_Node();

	/* We free the initial list of the right node and
	 * we set the list with the right items instead.
	 */
	delete rightNode->items;
	rightNode->items = rightPart;

	/* We need to distribute the children of the node correctly
	 * in the two nodes
	 *
	 * We will split the children's list in two equal parts
	 */
	List *leftChildren;
	List *rightChildren;

	/* Here the splitting of the children is being performed */
	this->children->split(&leftChildren, &rightChildren);

	/* We delete the list of children of the node */
	delete this->children;

	/* This node will have only the left children from now on */
	this->children = leftChildren;

	/* Similarly, the right node will only have the right children */
	delete rightNode->children;
	rightNode->children = rightChildren;

	/* Now we need to set the parent of each child of the right node
	 * to be the right node (because currently it is the node "this")
	 */
	Listnode *current = rightChildren->getHead();

	/* As long as we have not reached the end of the list */
	while(current != NULL)
	{
		/* We retrieve the AB Node stored in the current Listnode */
		AB_Node *currentChild = (AB_Node *) current->getItem();

		/* We set the parent of the node to be the right node */
		currentChild->setParent(rightNode);

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* Case the node does not have a parent */
	if(this->parent == NULL)
	{
		/* We will create a new root with the middle item */
		AB_Node *newRoot = new AB_Node();
		newRoot->items->insertLast(middle_item);

		/* The two nodes with the left and right items
		 * will be the children of the new root
		 */
		newRoot->children->insertLast(this);
		newRoot->children->insertLast(rightNode);

		/* We set the new root as parent of both nodes */
		this->setParent(newRoot);
		rightNode->setParent(newRoot);

		/* The root address now stores the address of the new root */
		(*root) = newRoot;
	}

	/* Case the node has a no-null parent */
	else
	{
		/* We retrieve the list of the parent's children */
		List *childrenOfParent = parent->children;

		/* We will start traversing the list to find the
		 * position where the node "this" is stored
		 */
		Listnode *current = childrenOfParent->getHead();

		/* Initialy we consider the node is stored in
		 * position 1 (which is the head of the list)
		 */
		unsigned int currentIndex = 1;

		/* We retrieve the item of the head */
		AB_Node *currentItem = (AB_Node *) current->getItem();

		/* As long as the item is not found to be our node */
		while(currentItem != this)
		{
			/* We increase the position by 1 */
			currentIndex++;

			/* We proceed to the next node */
			current = current->getNext();

			/* If we reached the end of the list, we exit */
			if(current == NULL)
				break;

			/* We update the current item with
			 * the item of the new node
			 */
			currentItem = (AB_Node *) current->getItem();
		}

		/* We have found the position of the node "this" in the list of
		 * parent's children. That position is equal to 'currentIndex'
		 */
		unsigned int positionOfThisInParent = currentIndex;

		/* Exactly after the node "this", we need to insert the
		 * right node in the list of the parent's children
		 */
		childrenOfParent->insertAfterPos(rightNode,
			positionOfThisInParent);

		/* The parent of the right node becomes the parent of "this" */
		rightNode->setParent(parent);

		/* We insert the middle item in the place of the parent
		 * where the left node (which is "this") is linked
		 */
		parent->insertBeforePos(middle_item, positionOfThisInParent);

		/* We rebalance the parent because it might be overflown */
		parent->rebalance(maxItemsAllowed, root);
	}
}

/**********************************************************
 * Removes the item in the suggested position in the node *
 *                                                        *
 *  Before deleting the item, it terminates it with the   *
 *  destroy function that is optionally given as second   *
 *  argument. If no destroy function is given, the item   *
 *      will just be dropped from the list of items       *
 **********************************************************/

void AB_Node::removeItemInPos(unsigned int pos, void (*destroy)(void *))
{
	/* If a no-null 'destroy' function is given, we use it
	 * to terminate the item before dropping it from the list
	 */
	if(destroy != NULL)
	{
		/* We retrieve the item in the suggested position */
		void *item = this->items->getItemInPos(pos);

		/* We call the 'destroy' function to terminate the item
		 * before dropping it from the list of items of the node
		 */
		destroy(item);
	}

	/* We remove the item in the suggested position in the list of items */
	this->items->removePos(pos);
}

/****************************************************************
 * Replenishes the node with an item that is borrowed from the  *
 * parent in case the node has less than minimum allowed items  *
 *                                                              *
 *  The parent is refilled either with an item of a sibling of  *
 *    the node (transfer) or it is not refilled and a fusion    *
 *  takes place between the node and its left or right sibling  *
 *                                                              *
 *  Then the 'refill' operation is called recursively for the   *
 *  parent. The root may change in the process. Therefore, the  *
 *  address of the root must be given as argument. Finally, a   *
 *  list with the role of "trash can" must be provided, where   *
 * the operation will store every node that needs to be deleted *
 *  in this process. Then the caller has to traverse the trash  *
 *         can and delete all the AB Nodes stored in it         *
 ****************************************************************/

void AB_Node::refill(unsigned int minItemsAllowed, AB_Node **root, List *trashCan)
{
	/* We retrieve the number of items in the node */
	unsigned int itemsNum = getItemsNum();

	/* If there we more items than the minimum allowed value
	 * minus one, the node does not need any rebalancing
	 */
	if(itemsNum >= minItemsAllowed - 1)
		return;

	/* If the node is the root, then it does not need any rebalancing, because
	 * the root is the only node that may have less than the minimum amount
	 * of items (and children). The only case that we must take actions for
	 * the root is when the root is empty (when it has no items). If that is
	 * true, then the root has only one child. The current root is deleted and
	 * its only-child becomes the new root.
	 */
	if((*root) == this)
	{
		/* We store in a temporary variable the root node */
		AB_Node *currentRoot = (*root);

		/* We retrieve the list of items of the root */
		List *rootItems = currentRoot->items;

		/* If there are no items in the root, we delete
		 * it and set as new root its one and only child
		 */
		if(rootItems->isEmpty())
		{
			/* We retrieve the only-child of the root */
			AB_Node *rootChild = (AB_Node *) currentRoot->children->getItemInPos(1);

			/* We delete the current root */
			delete currentRoot;

			/* The only-child of the previous root becomes the new root */
			(*root) = rootChild;

			/* Since the root child is now the new root, it has no parent */
			rootChild->setParent(NULL);
		}

		/* In this case there is nothing more to do, so we return immediatelly */
		return;
	}

	/* If this part is reached, the current node is not the
	 * root, but a leaf node or another internal node.
	 *
	 * We will find the left and the right siblings of the node
	 */
	AB_Node *leftSibling;
	AB_Node *rightSibling;

	/* We need to find the relative position of this node (e.g. is it the
	 * first child of its parent? is it the second child of its parent?)
	 *
	 * In other words, if the node is the n-th child of its parent, we
	 * need to find the value of 'n'.
	 *
	 * To do that, we will traverse the list of children of this node's
	 * parent. For each child we visit we will examine if it is equal to
	 * this node. If it is, we stop the traversal. Meanwhile, we keep a
	 * counter of how many children we have already visited. In this way
	 * we will remember the correct position of the node in the end.
	 */
	Listnode *current = this->parent->children->getHead();

	/* We initialize the amount of children we have visited to zero */
	unsigned int visitedChildren = 0;

	/* As long as the list of children of the parent has not ended */
	while(current != NULL)
	{
		/* We retrieve the child saved in the current Listnode */
		AB_Node *currentChild = (AB_Node *) current->getItem();

		/* We increase the amount of visited children by 1 */
		visitedChildren++;

		/* If the current child is this node, we found its position.
		 * It is arithmetically equal to the 'visitedChildren'variable.
		 */
		if(currentChild == this)
			break;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* As we said above, the position of the node is arithmetically equal
	 * to the counter we used in the above loop, named 'visitedChildren'.
	 */
	unsigned int nodePosition = visitedChildren;

	/* The position of the left sibling is by 1 smaller than the node's
	 * position, because the left sibling directly precedes this node
	 */
	unsigned int leftPosition = nodePosition - 1;

	/* The position of the right sibling is by 1 greater than the node's
	 * position, because the right sibling directly succeeds this node
	 */
	unsigned int rightPosition = nodePosition + 1;

	/* However, if the left position is equal to zero, that means the
	 * the position of this node is 1 and, consequently, this node is
	 * the leftmost child of its parent. Therefore, it has no left
	 * sibling. In this case, we set the value 'NULL' to 'leftSibling'
	 */
	if(leftPosition == 0)
		leftSibling = NULL;

	/* Else we find and retrieve the left sibling from the list of
	 * children of this node's parent and set it to 'leftSibling'
	 */
	else
	{
		leftSibling = (AB_Node *) this->parent->children->
			getItemInPos(leftPosition);
	}

	/* Similarly, if the right position is equal to the amount of children
	 * of the parent plus 1, this node is the rightmost child. Therefore, it has
	 * no right sibling. In this case, we set the value 'NULL' to 'rightSibling'
	 */
	if(rightPosition == 1 + (this->parent->children->getCounter()))
		rightSibling = NULL;

	/* Else we find and retrieve the right sibling from the list of
	 * children of this node's parent and set it to 'rightSibling'
	 */
	else
	{
		rightSibling = (AB_Node *) this->parent->children->
			getItemInPos(rightPosition);
	}

	/* We retrieve the amount of items of the left sibling
	 * (or zero if the left sibling is non-existent)
	 */
	unsigned int leftItemsNum = (leftSibling != NULL) ?
		leftSibling->getItemsNum() : 0;

	/* We retrieve the amount of items of the right sibling
	 * (or zero if the right sibling is non-existent)
	 */
	unsigned int rightItemsNum = (rightSibling != NULL) ?
		rightSibling->getItemsNum() : 0;

	/* If the left sibling has more than the minimum amount
	 * of keys, it can give one key without any problem
	 */
	if(leftItemsNum >= minItemsAllowed)
		transfer(nodePosition, leftSibling, leftPosition);

	/* Else if the right sibling has more than the minimum
	 * amount of keys, it can give one without any problem
	 */
	else if(rightItemsNum >= minItemsAllowed)
		transfer(nodePosition, rightSibling, rightPosition);

	/* Else if both the left and the right sibling have exactly the
	 * minimum amount of keys (or don't exist), a fusion must take place
	 */
	else
	{
		/* We fuse the node with its left sibling if it exists */

		if(leftSibling != NULL)
		{
			/* The item of the parent in the position that is arithmetically
			 * equal to the position of the node minus 1 migrates from the
			 * parent to the node. First we retrieve the item in that position.
			 */
			void *itemFromParent = parent->items->getItemInPos(nodePosition - 1);

			/* We insert the item of the parent in the front of the list of
			 * items of the node, because the item that migrates from the
			 * parent to the node is smaller than all the items of the node
			 */
			this->items->insertFront(itemFromParent);

			/* We remove the item that migrated from the parent */
			parent->items->removePos(nodePosition - 1);

			/* Now we are ready to fuse the node with its left sibling */
			fusion(nodePosition, leftSibling, leftPosition, trashCan);
		}

		/* Else we fuse the node with its right sibling
		 *
		 * The right sibling exists if the left does not
		 */

		else if(rightSibling != NULL)
		{
			/* The item of the parent in the position that is arithmetically
			 * equal to the position of the node migrates from the parent to
			 * the node. First we retrieve the item in that position.
			 */
			void *itemFromParent = parent->items->getItemInPos(nodePosition);

			/* We insert the item of the parent in the end of the list of
			 * items of the node, because the item that migrates from the
			 * parent to the node is greater than all the items of the node
			 */
			this->items->insertLast(itemFromParent);

			/* We remove the item that migrated from the parent */
			parent->items->removePos(nodePosition);

			/* Now we are ready to fuse the node with its right sibling */
			fusion(nodePosition, rightSibling, rightPosition, trashCan);
		}

		/* The parent may have one less item by this time. We need to refill it. */
		parent->refill(minItemsAllowed, root, trashCan);
	}
}

/*************************************************************
 * Transfers an item from the parent to the node and another *
 *   item from the given sibling of the node to the parent   *
 *************************************************************/

void AB_Node::transfer(unsigned int nodePos, AB_Node *sibling,
	unsigned int siblingPos)
{
	/* If the position of the node is smaller than the
	 * position of its sibling, the node is on the
	 * "left" side and the sibling on the "right" side
	 */
	if(nodePos < siblingPos)
	{
		/* The item of the parent in the position that is arithmetically
		 * equal to the position of the node migrates from the parent to
		 * the node. First we retrieve the item in that position.
		 */
		void *itemFromParent = parent->items->getItemInPos(nodePos);

		/* We insert the item of the parent in the end of the list of
		 * items of the node, because the item that migrates from the
		 * parent to the node is greater than all the items of the node
		 */
		this->items->insertLast(itemFromParent);

		/* We remove the item that migrated from the parent */
		parent->items->removePos(nodePos);

		/* Similarly, the leftmost item of the sibling will
		 * migrate to the parent. It will be inserted at
		 * the position where the previously migrated item was
		 */
		void *itemFromSibling = sibling->items->getItemInPos(1);

		/* Here we insert the item that migrated from
		 * the sibling to the list of items of the parent
		 */
		parent->insertBeforePos(itemFromSibling, nodePos);

		/* We remove the item that migrated from the sibling */
		sibling->items->removePos(1);

		/* If the sibling is not a leaf, it must give one of
		 * its children to the node that is being rebalanced
		 *
		 * This is because the sibling has one less key by now,
		 * so it can have one less child. It must give a child
		 * to the node that is being rebalanced, because that
		 * node will take one more key, so it must take one more
		 * child as well.
		 */
		if(!sibling->isLeaf())
		{
			/* We retrieve the list of children of the sibling */
			List *siblingChildren = sibling->children;

			/* We retrieve the leftmost child of the sibling
			 * (because the sibling is on the "right" side, we
			 * need to transfer its leftmost child to the node)
			 */
			AB_Node *leftmostChildOfSibling = (AB_Node *) siblingChildren->
				getItemInPos(1);

			/* We insert the leftmost child of the sibling as rightmost
			 * child of the node (because the node is on the "left" side)
			 */
			this->children->insertLast(leftmostChildOfSibling);

			/* We remove the child from the list of the sibling's children */
			siblingChildren->removeFront();

			/* The new parent of the child is the node that is rebalanced */
			leftmostChildOfSibling->setParent(this);
		}
	}

	/* If the position of the node is greater than the
	 * position of its sibling, the node is on the
	 * "right" side and the sibling on the "left" side
	 */
	else
	{
		/* The item of the parent in the position that is arithmetically
		 * equal to the position of the node minus 1 migrates from the
		 * parent to the node. First we retrieve the item in that position.
		 */
		void *itemFromParent = parent->items->getItemInPos(nodePos - 1);

		/* We insert the item of the parent in the front of the list of
		 * items of the node, because the item that migrates from the
		 * parent to the node is smaller than all the items of the node
		 */
		this->items->insertFront(itemFromParent);

		/* We remove the item that migrated from the parent */
		parent->items->removePos(nodePos - 1);

		/* Similarly, the rightmost item of the sibling will
		 * migrate to the parent. It will be inserted at
		 * the position where the previously migrated item was
		 */
		void *itemFromSibling = sibling->items->getItemInPos(
			sibling->items->getCounter());

		/* Here we insert the item that migrated from
		 * the sibling to the list of items of the parent
		 */
		parent->insertBeforePos(itemFromSibling, nodePos - 1);

		/* We remove the item that migrated from the sibling */
		sibling->items->removePos(sibling->items->getCounter());

		/* If the sibling is not a leaf, it must give one of
		 * its children to the node that is being rebalanced
		 *
		 * This is because the sibling has one less key by now,
		 * so it can have one less child. It must give a child
		 * to the node that is being rebalanced, because that
		 * node will take one more key, so it must take one more
		 * child as well.
		 */
		if(!sibling->isLeaf())
		{
			/* We retrieve the list of children of the sibling */
			List *siblingChildren = sibling->children;

			/* We retrieve the rightmost child of the sibling
			 * (because the sibling is on the "left" side, we
			 * need to transfer its rightmost child to the node)
			 */
			AB_Node *rightmostChildOfSibling = (AB_Node *) siblingChildren->
				getItemInPos(siblingChildren->getCounter());

			/* We insert the rightmost child of the sibling as leftmost
			 * child of the node (because the node is on the "right" side)
			 */
			this->children->insertFront(rightmostChildOfSibling);

			/* We remove the child from the list of the sibling's children */
			siblingChildren->removeLast();

			/* The new parent of the child is the node that is rebalanced */
			rightmostChildOfSibling->setParent(this);
		}
	}
}

/**********************************************************************
 *  Merges the node with the given sibling, making the two nodes one  *
 *                                                                    *
 * After this process is complete, an AB Node will need to be deleted *
 *   because it will have been rendered useless. Sometimes the node   *
 * that must be deleted is "this". Because we cannot delete the value *
 * "this", we insert it in the list that is given as fourth argument  *
 *                                                                    * 
 * That list plays the role of a trash can. Eventually, the trash can *
 *      must be visited and each node inside it must be deleted       *
 **********************************************************************/

void AB_Node::fusion(unsigned int nodePos, AB_Node *sibling,
	unsigned int siblingPos, List *trashCan)
{
	/* If the position of the node is smaller than the
	 * position of its sibling, the node is on the
	 * "left" side and the sibling on the "right" side
	 */
	if(nodePos < siblingPos)
	{
		/* We append all items and children of the node in
		 * the "right" side to the node in the "left" side
		 */
		this->items->append(sibling->items);
		this->children->append(sibling->children);

		/* We need to set the parent of each appended child
		 * to be the left node instead of the right node
		 */
		Listnode *current = sibling->children->getHead();

		/* We start traversing the list of the children
		 * that were just appended to the left node
		 *
		 * As long as the we have not reached the end of the list
		 */
		while(current != NULL)
		{
			/* We retrieve the child stored in the current node */
			AB_Node *currentChild = (AB_Node *) current->getItem();

			/* We set its parent to be the node of the "left"
			 * side (in this case, that node is "this")
			 */
			currentChild->setParent(this);

			/* We proceed to the next node */
			current = current->getNext();
		}

		/* We are going to delete the right node (here the sibling
		 * of "this"). Consequently, we need to remove it from the
		 * list of children of its parent
		 */
		parent->children->removePos(siblingPos);

		/* Finaly we delete the sibling of "this" */
		delete sibling;
	}

	/* If the position of the node is greater than the
	 * position of its sibling, the node is on the
	 * "right" side and the sibling on the "left" side
	 */
	else
	{
		/* We append all items and children of the node in
		 * the "right" side to the node in the "left" side
		 */
		sibling->items->append(this->items);
		sibling->children->append(this->children);

		/* We need to set the parent of each appended child
		 * to be the left node instead of the right node
		 */
		Listnode *current = this->children->getHead();

		/* We start traversing the list of the children
		 * that were just appended to the left node
		 *
		 * As long as the we have not reached the end of the list
		 */
		while(current != NULL)
		{
			/* We retrieve the child stored in the current node */
			AB_Node *currentChild = (AB_Node *) current->getItem();

			/* We set its parent to be the node of the "left" side
			 * (in this case, that node is the sibling of "this")
			 */
			currentChild->setParent(sibling);

			/* We proceed to the next node */
			current = current->getNext();
		}

		/* We are going to delete the right node (here the right is
		 * the node "this"). Consequently, we need to remove it from
		 * the list of children of its parent
		 */
		this->parent->children->removePos(nodePos);

		/* Because we cannot apply the 'delete' operator on the r-value
		 * "this", we add it to the trash can that is given as argument.
		 *
		 * Later the trash can will be visited and each node inside it
		 * will be deleted. For now, we just insert it in the trash can.
		 */
		trashCan->insertLast(this);
	}
}

/****************************************************************************
 * Searches for the correct position to place the given key at the subtree  *
 *  of this node. If the given key exists already in the tree, there is no  *
 * valid position in the tree to insert it again, because the tree does not *
 *         support keys with the same value existing multiple times         *
 *                                                                          *
 *   In case the key does not exist in the tree, a no-null 'KeyPosition'    *
 *           item will be returned, which consists of two fields:           *
 *                                                                          *
 *   1) an 'AB_Node *' value, which stores the correct node where the       *
 *      key should be inserted                                              *
 *                                                                          *
 *   2) an 'int' value, which stores the correct position in the node       *
 *      stated by value (1) where the key should be inserted                *
 *                                                                          *
 *  To retrieve the value (1), simply use the operation 'getItem' and cast  *
 *  its result to the type 'AB_Node *'. To retrieve the value (2), use the  *
 *       operation 'getKey' (in the latter case no casting is needed)       *
 *                                                                          *
 *   If the given key exists in the tree already, it is impossible to be    *
 *        re-inserted and in this case this operation returns 'NULL'        *
 ****************************************************************************/

KeyPosition *AB_Node::searchCorrectPosForKey(void *key, int (*compare)(void *, void *))
{
	/* We retrieve the head of the list with the stored items */
	Listnode *current = items->getHead();

	/* "Pointer" to the index of the current child. We begin from the first
	 * child of the node. We need this variable to determine to which child
	 * we will continue the search in case we need to continue the search to
	 * another node.
	 */
	unsigned int current_child = 1;

	/* As long as the list is not empty */
	while(current != NULL)
	{
		/* We retrieve the item stored in the current node */
		void *current_item = current->getItem();

		/* We compare the retrieved item with the given key */
		int compare_result = compare(key, current_item);

		/* If they are equal, the key was found in the tree.
		 * In this case, it cannot be re-inserted.
		 */
		if(compare_result == 0)
			return NULL;

		/* Else if the given key is smaller than the current item */
		if(compare_result < 0)
		{
			/* If the current node is a leaf, the search is not successful */
			if(isLeaf())
				return new KeyPosition(this, current_child);

			/* Else we retrieve the child to which we need to continue the search */
			AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

			/* We return the result of searching in the child */
			return next_child->searchCorrectPosForKey(key, compare);
		}

		/* We point to the next child, which may be
		 * used to continue the search in the next loop
		 */
		current_child++;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* If the current node is a leaf, the search is not successful */
	if(isLeaf())
		return new KeyPosition(this, current_child);

	/* Else we retrieve the child to which we need to continue
	 * the search, which in this case is the rightmost child
	 */
	AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

	/* We return the result of searching in the child */
	return next_child->searchCorrectPosForKey(key, compare);
}

/***************************************************************************
 * Searches the given key using the given compare function and returns the *
 *         place in the tree where the key is stored, if it exists         *
 *                                                                         *
 *    In case the key exists in the tree, a no-null 'KeyPosition' item     *
 *             will be returned, which consists of two fields:             *
 *                                                                         *
 *   1) an 'AB_Node *' value, which stores the correct node where the      *
 *      key is being stored                                                *
 *                                                                         *
 *   2) an 'int' value, which stores the correct position in the node      *
 *      stated by value (1) where the key is being stored                  *
 *                                                                         *
 * To retrieve the value (1), simply use the operation 'getItem' and cast  *
 * its result to the type 'AB_Node *'. To retrieve the value (2), use the  *
 *      operation 'getKey' (in the latter case no casting is needed)       *
 *                                                                         *
 *     If the given key does not exist in the tree, 'NULL' is returned     *
 ***************************************************************************/

KeyPosition *AB_Node::searchKey(void *key, int (*compare)(void *, void *))
{
	/* We retrieve the head of the list with the stored items */
	Listnode *current = items->getHead();

	/* "Pointer" to the index of the current child. We begin from the first
	 * child of the node. We need this variable to determine to which child
	 * we will continue the search in case we need to continue the search to
	 * another node.
	 */
	unsigned int current_child = 1;

	/* As long as the list is not empty */
	while(current != NULL)
	{
		/* We retrieve the item stored in the current node */
		void *current_item = current->getItem();

		/* We compare the retrieved item with the given key */
		int compare_result = compare(key, current_item);

		/* If they are equal, the key was found in the tree.
		 * In this case, we return the node and the position
		 * inside the node where the key was exactly found.
		 */
		if(compare_result == 0)
			return new KeyPosition(this, current_child);

		/* Else if the given key is smaller than the current item */
		if(compare_result < 0)
		{
			/* If the current node is a leaf, the search is not successful */
			if(isLeaf())
				return NULL;

			/* Else we retrieve the child to which we need to continue the search */
			AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

			/* We return the result of searching in the child */
			return next_child->searchKey(key, compare);
		}

		/* We point to the next child, which may be
		 * used to continue the search in the next loop
		 */
		current_child++;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* If the current node is a leaf, the search is not successful */
	if(isLeaf())
		return NULL;

	/* Else we retrieve the child to which we need to continue
	 * the search, which in this case is the rightmost child
	 */
	AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

	/* We return the result of searching in the child */
	return next_child->searchKey(key, compare);
}

/***************************************************************************
 * Searches the given key using the given compare function and returns the *
 *  identical key stored in the tree. Returns 'NULL' if it does not exist  *
 ***************************************************************************/

void *AB_Node::searchAndRetrieveKey(void *key, int (*compare)(void *, void *))
{
	/* We retrieve the head of the list with the stored items */
	Listnode *current = items->getHead();

	/* "Pointer" to the index of the current child. We begin from the first
	 * child of the node. We need this variable to determine to which child
	 * we will continue the search in case we need to continue the search to
	 * another node.
	 */
	unsigned int current_child = 1;

	/* As long as the list is not empty */
	while(current != NULL)
	{
		/* We retrieve the item stored in the current node */
		void *current_item = current->getItem();

		/* We compare the retrieved item with the given key */
		int compare_result = compare(key, current_item);

		/* If they are equal, the key was found in the tree.
		 * In this case, we return the item of the current node.
		 */
		if(compare_result == 0)
			return current_item;

		/* Else if the given key is smaller than the current item */
		if(compare_result < 0)
		{
			/* If the current node is a leaf, the search is not successful */
			if(isLeaf())
				return NULL;

			/* Else we retrieve the child to which we need to continue the search */
			AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

			/* We return the result of searching in the child */
			return next_child->searchAndRetrieveKey(key, compare);
		}

		/* We point to the next child, which may be
		 * used to continue the search in the next loop
		 */
		current_child++;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* If the current node is a leaf, the search is not successful */
	if(isLeaf())
		return NULL;

	/* Else we retrieve the child to which we need to continue
	 * the search, which in this case is the rightmost child
	 */
	AB_Node *next_child = (AB_Node *) children->getItemInPos(current_child);

	/* We return the result of searching in the child */
	return next_child->searchAndRetrieveKey(key, compare);
}

/******************************************************
 * Frees the allocated memory for the result returned *
 *     by 'searchCorrectPosForKey' or 'searchKey'     *
 ******************************************************/

void AB_Node::terminatePosition(KeyPosition *returnedPosition)
{
	if(returnedPosition != NULL)
		delete returnedPosition;
}

/***********************************************************************
 * Prints every item of the node using the given 'visit' operation and *
 * if a no-null 'context' operation is given as second argument, it is *
 *           invoked to print some context between the nodes           *
 ***********************************************************************/

void AB_Node::printItems(void (*visit)(void *), void (*context)()) const
{
	items->printFromHead(visit, context);
}

/**************************************************************
 * Returns the node that contains the Inorder Predecessor key *
 **************************************************************/

AB_Node *AB_Node::inorderPredecessor(unsigned int pos) const
{
	/* If the node is a leaf, there is no Inorder Predecessor */
	if(isLeaf())
		return NULL;

	/* Else we retrieve the child that is visited exactly before
	 * the item in the suggested position in Inorder Traversal
	 */
	AB_Node *precedingChild = (AB_Node *) children->getItemInPos(pos);

	/* We initialize the final result that
	 * we will return with the preceding child
	 */
	AB_Node *inorderPredecessorNode = precedingChild;

	/* As long as the current node saved in the 'inorderPredecessorNode'
	 * variable is not a leaf, we update it with its rightmost child
	 */
	while(!inorderPredecessorNode->isLeaf())
	{
		/* We retrieve the amount of children of the current node */
		unsigned int childrenNum = inorderPredecessorNode->getChildrenNum();

		/* Given the amount of children, we retrieve the rightmost child  */
		AB_Node *rightmostChild = (AB_Node *) inorderPredecessorNode->
			children->getItemInPos(childrenNum);

		/* We update the Inorder Predecessor node with the rightmost child */
		inorderPredecessorNode = rightmostChild;
	}

	/* We return the leaf node that was found in the previous loop */
	return inorderPredecessorNode;
}

/************************************************************
 * Returns the node that contains the Inorder Successor key *
 ************************************************************/

AB_Node *AB_Node::inorderSuccessor(unsigned int pos) const
{
	/* If the node is a leaf, there is no Inorder Successor */
	if(isLeaf())
		return NULL;

	/* Else we retrieve the child that is visited exactly after
	 * the item in the suggested position in Inorder Traversal
	 */
	AB_Node *succeedingChild = (AB_Node *) children->getItemInPos(pos + 1);

	/* We initialize the final result that
	 * we will return with the succeeding child
	 */
	AB_Node *inorderSuccessorNode = succeedingChild;

	/* As long as the current node saved in the 'inorderSuccessorNode'
	 * variable is not a leaf, we update it with its leftmost child
	 */
	while(!inorderSuccessorNode->isLeaf())
	{
		/* We retrieve the leftmost child of the current node  */
		AB_Node *leftmostChild = (AB_Node *) inorderSuccessorNode->
			children->getItemInPos(1);

		/* We update the Inorder Successor node with the leftmost child */
		inorderSuccessorNode = leftmostChild;
	}

	/* We return the leaf node that was found in the previous loop */
	return inorderSuccessorNode;
}
