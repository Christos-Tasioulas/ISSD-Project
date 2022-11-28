#include <iostream>
#include "BinaryTree.h"

/****************************
 * Constructor & Destructor *
 ****************************/

BinaryTree::BinaryTree()
{
	root = NULL;
	counter = 0;
}

BinaryTree::~BinaryTree()
{
	destroy();
}

/***********
 * Getters *
 ***********/

Treenode *BinaryTree::getRoot() const
{
	return root;
}

unsigned int BinaryTree::getCounter() const
{
	return counter;
}

/***********************************
 * Retrieve the item of a treenode *
 ***********************************/

void *BinaryTree::getItem(Treenode *node) const
{
	return node->getItem();
}

/*************************************
 * Returns true if the tree is empty *
 *************************************/

bool BinaryTree::isEmpty() const
{
	return counter == 0;
}

/*******************************************
 * Returns the number of nodes in the tree *
 * (it is an alternative to 'getCounter')  *
 *******************************************/

unsigned int BinaryTree::size() const
{
	return sizeRec(root);
}

/*********************************************
 * Finds recursively the size of the subtree *
 *   that begins from the 'node' argument    *
 *********************************************/

unsigned int BinaryTree::sizeRec(Treenode *node) const
{
	if(node == NULL)
		return 0;
	
	return 1 + sizeRec(node->getLeft()) + sizeRec(node->getRight());
}

/**********************************
 * Returns the height of the tree *
 **********************************/

unsigned int BinaryTree::height() const
{
	return heightRec(root);
}

/***********************************************
 * Finds recursively the height of the subtree *
 *    that begins from the 'node' argument     *
 ***********************************************/

unsigned int BinaryTree::heightRec(Treenode *node) const
{
	if(node == NULL)
		return 0;
	
	unsigned int h1 = heightRec(node->getLeft());
	unsigned int h2 = heightRec(node->getRight());
	
	return (h1 > h2) ? h1 + 1 : h2 + 1;
}

/*********************************
 * Insert a new node in the tree *
 *********************************/

void BinaryTree::insertRoot(void *item)
{
	if(isEmpty() == false)
		return;
	
	root = new Treenode(item);
	counter = 1;	
}

void BinaryTree::insertLeft(Treenode *node, void *item)
{
	if(isEmpty() == true)
		return;
		
	Treenode *new_node = new Treenode(item);
	node->setLeft(new_node);
	new_node->setParent(node);
	counter++;
}

void BinaryTree::insertRight(Treenode *node, void *item)
{
	if(isEmpty() == true)
		return;
		
	Treenode *new_node = new Treenode(item);
	node->setRight(new_node);
	new_node->setParent(node);
	counter++;
}

/******************************************
 * Change the value of the item of a node *
 ******************************************/

void BinaryTree::change(Treenode *node, void *item)
{
	node->setItem(item);
}

/****************************************************************
 *  Prints the items of the tree. The operation passes through  *
 *  every node in the sequence implied by the given traversal   *
 *  order. A 'visit' function is required to print an item of   *
 *  a node. If the 'contextBetweenNodes' function is a no-null  *
 *  value, it is called to print the context the user desires   *
 * between the items that are going to be printed. In case the  *
 *  chosen traversal order is 'Levelorder', the user may also   *
 * provide an operation that prints the desired context between *
 *  consecutive levels of the tree. Also, again, if the chosen  *
 *  traversal order is 'Levelorder', the context between nodes  *
 *  is also printed at the start and at the end of every level  *
 ****************************************************************/

void BinaryTree::print(OrderOfTraversal traversal_order,
	void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)()) const
{
	printRec(root, traversal_order, visit,
		contextBetweenNodes,
		contextBetweenLevels);
}

/**************************************************************
 * Prints recursively the subtree that begins from the 'node' *
 * argument with the given traversal order and rest functions *
 **************************************************************/

void BinaryTree::printRec(Treenode *node,
	OrderOfTraversal traversal_order,
	void (*visit)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)()) const
{
	if(node == NULL)
		return;

	switch(traversal_order)
	{
		case Preorder:
		{
			/* Printing order: node -> left subtree -> right subtree
			 *
			 * We visit the current node
			 */
			node->print(visit);

			/* If a no-null context between nodes has been given,
			 * we print it now, exactly after visiting this node
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			/* Then we visit the left subtree of the node */
			printRec(node->getLeft(), traversal_order, visit);

			/* Then we visit the right subtree of the node */
			printRec(node->getRight(), traversal_order, visit);

			break;
		}

		case Inorder:
		{
			/* Printing order: left subtree -> node -> right subtree
			 *
			 * First, we visit the left subtree of the node
			 */
			printRec(node->getLeft(), traversal_order, visit);

			/* Then we visit the current node */
			node->print(visit);

			/* If a no-null context between nodes has been given,
			 * we print it now, exactly after visiting this node
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			/* Then we visit the right subtree of the node */
			printRec(node->getRight(), traversal_order, visit);

			break;
		}

		case Postorder:
		{
			/* Printing order: left subtree -> right subtree -> node
			 *
			 * First, we visit the left subtree of the node
			 */
			printRec(node->getLeft(), traversal_order, visit);

			/* Then we visit the right subtree of the node */
			printRec(node->getRight(), traversal_order, visit);

			/* Then we visit the current node */
			node->print(visit);

			/* If a no-null context between nodes has been given,
			 * we print it now, exactly after visiting this node
			 */
			if(contextBetweenNodes != NULL)
				contextBetweenNodes();

			break;
		}

		case Levelorder:
		{
			/* In this case, 'node' will always be the root
			 *
		     * Printing order:
			 *
			 * First the root is printed
			 * Then the children of the root are printed
			 * Then the children of the children of the root are printed
			 * ...
			 * The same pattern is applied until the leaves are printed
			 *
			 * First, we find the height of the tree.
			 */
			unsigned int treeHeight = height(), i;

			/* Then for each level of height,
			 * we print all nodes of that level
			 */
			for(i = 1; i <= treeHeight; i++)
			{
				/* If a no-null context between nodes has been given,
				 * we print it now, exactly after visiting this node
				 */
				if(contextBetweenNodes != NULL)
					contextBetweenNodes();

				/* We print all nodes of the current level */
				printLevel(node, i, visit, contextBetweenNodes);

				/* We need to print the context between levels only
			     * between the levels (as the function name suggests)
				 *
				 * That means, if we have just printed the leaves,
				 * we should not print the context between levels
				 * after them.
				 */
				if((contextBetweenLevels != NULL) && (i != treeHeight))
					contextBetweenLevels();
			}

			break;
		}
	}
}

/*******************************************
 * Prints the subtree that begins from the *
 * 'node' argument in Levelorder traversal *
 *******************************************/

void BinaryTree::printLevel(Treenode *node, unsigned int level,
	void (*visit)(void *),
	void (*contextBetweenNodes)()) const
{
	/* If the node is 'NULL', we return immediately */

	if(node == NULL)
		return;

	/* If 'level' == 1, we print the node and return */

	if(level == 1)
	{
		/* Here we print the node */
		node->print(visit);

		/* If a no-null context between nodes has been given,
		 * we print it now, exactly after visiting this node
		 */
		if(contextBetweenNodes != NULL)
			contextBetweenNodes();
	}

	/* If 'level' > 1, we print recursively the children
     * of the node, which are one level below the node
	 */

	else
	{
		printLevel(node->getLeft(), level - 1,
			visit, contextBetweenNodes);

		printLevel(node->getRight(), level - 1,
			visit, contextBetweenNodes);
	}
}

/*******************************************************
 * Call the given 'actions' operation for each item of *
 *  the tree as argument in the given traversal order  *
 *******************************************************/

void BinaryTree::traverse(OrderOfTraversal traversal_order,
	void (*actions)(void *)) const
{
	traverseRec(root, traversal_order, actions);
}

/*******************************************************************
 *  Traverses recursively the subtree that begins from the 'node'  *
 * argument with the given traversal order and calls the 'actions' *
 *              operation for every item in the tree               *
 *******************************************************************/

void BinaryTree::traverseRec(Treenode *node, OrderOfTraversal traversal_order,
	void (*actions)(void *)) const
{
	if(node == NULL)
		return;

	switch(traversal_order)
	{
		case Preorder:
		{
			/* Traversal order: node -> left subtree -> right subtree */

			actions(node->getItem());
			traverseRec(node->getLeft(), traversal_order, actions);
			traverseRec(node->getRight(), traversal_order, actions);
			break;
		}

		case Inorder:
		{
			/* Traversal order: left subtree -> node -> right subtree */

			traverseRec(node->getLeft(), traversal_order, actions);
			actions(node->getItem());
			traverseRec(node->getRight(), traversal_order, actions);
			break;
		}

		case Postorder:
		{
			/* Traversal order: left subtree -> right subtree -> node */

			traverseRec(node->getLeft(), traversal_order, actions);
			traverseRec(node->getRight(), traversal_order, actions);
			actions(node->getItem());
			break;
		}

		case Levelorder:
		{
			/* In this case, 'node' will always be the root
			 *
		     * Traversal order:
			 * 1st Level - root
			 * 2nd Level - left child of root, right child of root
			 * ...
			 */

			unsigned int treeHeight = height(), i;
			for(i = 1; i <= treeHeight; i++)
				traverseLevel(node, i, actions);
			break;
		}
	}
}

/**********************************************
 * Traverses the subtree that begins from the *
 *  'node' argument in Levelorder traversal   *
 **********************************************/

void BinaryTree::traverseLevel(Treenode *node, unsigned int level,
		void (*actions)(void *)) const
{
	/* If the node is 'NULL', we return immediately */

	if(node == NULL)
		return;

	/* If 'level' == 1, we call the 'actions' operation
	 * with the node as argument and then we return
	 */

	if(level == 1)
		actions(node->getItem());

	/* If 'level' > 1, we apply the actions recursively for the
     * children of the node, which are one level below the node
	 */

	else
	{
		traverseLevel(node->getLeft(), level - 1, actions);
		traverseLevel(node->getRight(), level - 1, actions);
	}
}

/**********************
 * Remove a leaf node *
 **********************/

void BinaryTree::remove(Treenode *node)
{
	/* If the node has children, it cannot be removed, so we return. */

	if(node->getLeft() != NULL || node->getRight() != NULL)
		return;

	/* If the node is the root, we delete the root and set counter = 0
	 * (since root is a leaf node, so the tree has just the root)
	 */

	if(node == root)
	{
		root = NULL;
		counter = 0;
		delete node;
		return;
	}

	/* Else if the node is a random leaf node, we decide whether it is
	 * the left or the right child of its parent. Then, we set the
	 * parent's corresponding pointer to 'NULL' and delete the child
	 */

	Treenode *parent = node->getParent();

	if(node == parent->getLeft())
		parent->setLeft(NULL);

	else
		parent->setRight(NULL);

	counter--;
	delete node;
}

/***********************************
 * Remove every node from the tree *
 ***********************************/

void BinaryTree::destroy()
{
	destroyRec(root);
}

void BinaryTree::destroyRec(Treenode *node)
{
	if(node != NULL)
	{
		destroyRec(node->getLeft());
		destroyRec(node->getRight());
		remove(node);
	}

	return;
}
