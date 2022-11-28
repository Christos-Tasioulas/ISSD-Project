#include <iostream>
#include "CompleteBinaryTree.h"

/******************************
 * Operation to compute power *
 ******************************/

static int power(int base, int exponent)
{
	if(exponent == 0)
		return 1;
	
	int start = base, i;

	for(i = 1; i < exponent; i++)
		base *= start;

	return base;
}

/****************************
 * Constructor & Destructor *
 ****************************/

CompleteBinaryTree::CompleteBinaryTree()
{
	BinaryTree();
	last = NULL;
}

CompleteBinaryTree::~CompleteBinaryTree() {}

/*********************************
 * Insert a new node in the tree *
 *********************************/

void CompleteBinaryTree::insert(void *item)
{
	/* In order to insert a new node in the complete
	 * tree, we examine a number of different cases
	 *
	 * 1) The tree is empty (we add a new root)
	 */

	if(isEmpty() == true)
	{
		insertRoot(item);
		last = root;
		return;
	}

	/* 2) The tree has one node - the root */

	if(counter == 1)
	{
		/* The new node should be the left child of the root */

		insertLeft(root, item);
		last = root->getLeft();
		return;
	}

	/* 3) The tree has two nodes - the root and a left child */

	if(counter == 2)
	{
		/* The new node should be the right child of the root */

		insertRight(root, item);
		last = root->getRight();
		return;
	}

	/* 4) The bottom level of the tree is complete */

	if(counter == (unsigned int) power(2, height()) - 1)
	{
		/* The height of the tree is increased by 1.
		 *
		 * We insert the new node at the leftmost position
		 * of the new bottom level
		 */

		Treenode *temp = root;

		/* To reach that position, we start from the root and
		 * recursively retrieve the left child. If the left
		 * child of the node is 'NULL', we stop at that node
		 */

		while(temp->getLeft() != NULL)
			temp = temp->getLeft();

		/* The new node should be the left child of the above node */

		insertLeft(temp, item);
		last = temp->getLeft();
		return;
	}

	/* 5) Case we can still add nodes to the current bottom level */

	Treenode *parentOfLast = last->getParent();

	/* If the last inserted node is a left child, we simply have
	 * to insert a right child. So our new node will be the
	 * right child of the last's parent (and sibling of 'last')
	 */

	if(parentOfLast->getRight() == NULL)
	{
		insertRight(parentOfLast, item);
		last = parentOfLast->getRight();
		return;
	}

	/* Else we compute the total number of nodes in the
	 * bottom level and then we consider two sub-cases
	 */

	unsigned int nodesOfLastLevel = counter - power(2, height() - 1) + 1;

	/* At this point we have examined the case that we should
	 * insert the new node as a right child (just above).
	 * So, in the following part of the code we will always
	 * have to insert a new left child. That means, a leaf node
	 * will become parent for its first time.
	 *
	 * Sub-case 1: The leaf that will become parent is the uncle of 'last'
	 */

	if(nodesOfLastLevel % 4 != 0)
	{
		/* We reach the uncle (in the code 'nextParent')
		 * and insert a new left child of the uncle
		 */

		Treenode *nextParent = parentOfLast->getParent()->getRight();
		insertLeft(nextParent, item);
		last = nextParent->getLeft();
		return;
	}

	/* Sub-case 2: The leaf that will become
	 * parent is the left cousin of last's parent
	 */

	Treenode *temp = last->getParent();

	/* In this case both last's parent and last's uncle have two children.
	 * So the leaf that must become parent this time is their left cousin.
	 *
	 * We reach the parent that connects the two sides
	 * (last's parent and uncle with cousins)
	 */

	while(temp->getParent()->getRight() == temp)
		temp = temp->getParent();

	/* Then we reach the other side of the 'family' (the right side) */

	temp = temp->getParent()->getRight();

	/* We retrieve the left cousin and insert a left child of it */

	while(temp->getLeft() != NULL)
		temp = temp->getLeft();

	insertLeft(temp, item);
	last = temp->getLeft();
}

/**************************************
 * Remove the last node from the tree *
 **************************************/

void CompleteBinaryTree::removeLast()
{
	/* Just like insertion, we consider a number of cases to apply removal
	 *
	 * 1) The tree is empty (no removal is possible)
	 */

	if(isEmpty() == true)
		return;

	/* 2) The tree has only the root (the root must be removed) */

	if(counter == 1)
	{
		remove(root);
		last = NULL;
		return;
	}

	/* 3) The tree has two nodes - the root and a left child of the root.
	 *    The left child of the root must be removed.
	 */

	if(counter == 2)
	{
		remove(root->getLeft());
		last = root;
		return;		
	}

	/* 4) The bottom level of the tree has 1 node */

	if(counter == (unsigned int) power(2, height() - 1))
	{
		/* We remember the 'last' node (we save it in another variable) */

		Treenode *temp = last;

		/* We mark the rightmost node of the level above the bottom as 'last' */

		last = root;

		while(last->getRight() != NULL)
			last = last->getRight();

		/* Then we remove that 1 node of the bottom */

		remove(temp);
		return;
	}

	/* 5) The bottom level has more that 1 nodes */

	Treenode *temp = last;

	/* Case the 'last' node is a right child (its parent
	 * has two children, so 'last' is the right)
	 */

	if(temp->getParent()->getLeft() != NULL &&
	   temp->getParent()->getRight() != NULL)
	{
		/* We simply mark the left child of last's parent as 'last' */

		last = temp->getParent()->getLeft();
		remove(temp);
		return;
	}

	/* Else we compute the total number of nodes in the
	 * bottom level and then we consider two sub-cases
	 */

	unsigned int nodesOfLastLevel = counter - power(2, height() - 1) + 1;

	/* At this point 'last' is always a left child. Let's consider
	 * the parent of 'last' and its sibling (the uncle of 'last')
	 *
	 * Sub-case 1: The uncle of 'last' has two children
	 */

	if((nodesOfLastLevel - 1) % 4 != 0)
	{
		/* The right child of the uncle is marked as 'last' */

		last = temp->getParent()->getParent()->getLeft()->getRight();
		remove(temp);
		return;
	}

	/* Sub-case 2: The uncle of 'last' has no children
	 *
	 * We reach the parent that connects the two sides
	 * (parent & uncle of 'last' with cousins of them)
	 */

	while(last->getParent()->getLeft() == last)
		last = last->getParent();

	/* We get to the other side (the left side) */

	last = last->getParent()->getLeft();

	/* We reach the right cousin and mark its right child as 'last' */

	while(last->getRight() != NULL)
		last = last->getRight();
	
	remove(temp);
}
