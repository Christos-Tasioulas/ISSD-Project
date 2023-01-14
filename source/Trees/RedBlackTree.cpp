#include <iostream>
#include <cstdio>
#include "RedBlackTree.h"

/*******************************************
 * User & Implementation compare functions *
 *******************************************/

static int (*rbtUserCompare)(void *, void *);
static int rbtImpCompare(void *, void *);

/*****************************************
 * User & Implementation visit functions *
 *****************************************/

static void (*rbtUserVisit)(void *);
static void (*rbtUserVisitBoth)(void *, void *);
static void rbtImpVisitKeys(void *);
static void rbtImpVisitColoredKeys(void *);
static void rbtImpVisitBoth(void *, void *);
static void rbtImpVisitColoredBoth(void *, void *);

/*******************************
 * Traverse operations of the  *
 * user and the implementation *
 *******************************/

static void (*rbtUserTraverse)(void *, void *);
static void rbtImpTraverse(void *item);

/*********************************
 * User color notation functions *
 *********************************/

static void (*rbtUserRedNotation)();
static void (*rbtUserBlackNotation)();

/************************************
 * Default color notation functions *
 ************************************/

void RedBlackTree::defaultRed()
{
	printf(RED_COLOR);
}

void RedBlackTree::defaultBlack()
{
	printf(BLACK_COLOR);
}

/****************************
 * Constructor & Destructor *
 ****************************/

RedBlackTree::RedBlackTree()
{
	BinarySearchTree();
}

RedBlackTree::~RedBlackTree()
{
	deleteRedBlackKey(root);
}

/*****************************************************
 * Returns the color of a node in the red-black tree *
 *****************************************************/

bool RedBlackTree::getColor(Treenode *node) const
{
	BSTObject *node_item = (BSTObject *) node->getItem();
	RedBlackKey *node_rbk = (RedBlackKey *) node_item->getKey();

	return node_rbk->getKey();
}

/*****************************************************
 * Changes the color of a node in the red-black tree *
 *****************************************************/

void RedBlackTree::changeColor(Treenode *node)
{
	BSTObject *node_item = (BSTObject *) node->getItem();
	RedBlackKey *node_rbk = (RedBlackKey *) node_item->getKey();

	node_rbk->setKey(!node_rbk->getKey());
}

/*********************************
 * Insert a new node in the tree *
 *********************************/

void RedBlackTree::insert(void *item, void *key,
	int (*compare)(void *, void *))
{
	/* Initially, we insert a new node in the tree
	 * storing the given item as if the tree was a
	 * classic binary search tree, painting the node red
	 */

	rbtUserCompare = compare;
	RedBlackKey *new_key = new RedBlackKey(key, RED);
	Treenode *inserted;
	BinarySearchTree::insert(item, new_key, rbtImpCompare, &inserted);

	/* We fix possible violations caused by the classic
	 * BST insertion by recoloring and rotating the tree
	 */

	fixInsertion(inserted);
}

/*****************************************************
 * Implementation compare type function that breaks  *
 * the Pair Bool Item to its parts and uses the user *
 *      function to compare the desired data         *
 *****************************************************/

int rbtImpCompare(void *item1, void *item2)
{
	RedBlackKey *rbk1 = (RedBlackKey *) item1;
	RedBlackKey *rbk2 = (RedBlackKey *) item2;

	return rbtUserCompare(rbk1->getItem(), rbk2->getItem());
}

/***************************************
 * Recolor & Rebalance after insertion *
 ***************************************/

void RedBlackTree::fixInsertion(Treenode *node)
{
	/* If the node is 'NULL' we return immediatelly */

	if(node == NULL)
		return;

	/* If the new node is the root,
	 * we change its color to black
	 */

	if(node == root)
	{
		if(getColor(root) == RED)
			changeColor(root);

		return;
	}

	/* We get the parent and
	 * grandparent of the node
	 */

	Treenode *parent = node->getParent();
	Treenode *grandp = node->getGrandparent();

	/* If the parent or the node is black, we do not
	 * enter the following 'if' block. Therefore we
	 * return immediatelly (the end of the operation
	 * is below the following 'if' block)
	 */

	if(getColor(node) == RED && getColor(parent) != BLACK)
	{
		/* We get the uncle of the node and, according
		 * to its color, we proceed to the corresponding
		 * actions of rebalancing & recoloring
		 */

		Treenode *uncle = node->getUncle();

		/* Actions if the uncle is red */

		if(uncle != NULL)
		{
			if(getColor(uncle) == RED)
			{
				changeColor(uncle);
				changeColor(parent);
				changeColor(grandp);
				fixInsertion(grandp);
				return;
			}
		}

		/* Actions if the uncle is 'NULL' or black
		 *
		 * Left - Left case: The parent is the left
		 * child of the grandparent and the node is
		 * the left child of the parent
		 */

		if(grandp->getLeft() == parent
		&& parent->getLeft() == node)
		{
			insertionCaseLeftLeft(node, parent, grandp);
		}

		/* Left - Right case: The parent is the left
		 * child of the grandparent and the node is
		 * the right child of the parent
		 */

		else if(grandp->getLeft() == parent
			 && parent->getRight() == node)
		{
			parent->rotation(Left, &root);
			insertionCaseLeftLeft(parent, node, grandp);
		}

		/* Right - Right case: The parent is the right
		 * child of the grandparent and the node is
		 * the right child of the parent
		 */

		else if(grandp->getRight() == parent
			 && parent->getRight() == node)
		{
			insertionCaseRightRight(node, parent, grandp);
		}

		/* Right - Left case: The parent is the right
		 * child of the grandparent and the node is
		 * the left child of the parent
		 */

		else if(grandp->getRight() == parent
			 && parent->getLeft() == node)
		{
			parent->rotation(Right, &root);
			insertionCaseRightRight(parent, node, grandp);
		}

		/* In any of the four cases above, we further fix the
		 * tree with the grandparent being the reference node
		 */

		fixInsertion(grandp);
	}
}

/********************************************************
 * Black or 'NULL' Uncle - Special cases of rebalancing *
 ********************************************************/

void RedBlackTree::insertionCaseLeftLeft(Treenode *node,
	Treenode *parent, Treenode *grandparent)
{
	grandparent->rotation(Right, &root);
	changeColor(grandparent);
	changeColor(parent);
}

void RedBlackTree::insertionCaseRightRight(Treenode *node,
	Treenode *parent, Treenode *grandparent)
{
	grandparent->rotation(Left, &root);
	changeColor(grandparent);
	changeColor(parent);
}

/*******************************
 * Remove a node from the tree *
 ********************************/

void RedBlackTree::remove(void *key, int (*compare)(void *, void *),
	bool *success)
{
	/* Initially, we search for the
	 * appropriate key in the tree
	 */

	Treenode *toberemoved;
	rbtUserCompare = compare;

	/* We create a temporary 'RedBlackKey' item
	 * that will be used to search if the desired
	 * key exists in the tree. The color does not
	 * matter in the search process
	 */

	RedBlackKey *temp_key = new RedBlackKey(key);

	/* If the desired key is not in the tree,
	 * we return immediatelly
	 */

	if(BinarySearchTree::search(temp_key,
		rbtImpCompare, &toberemoved) == false)
	{
		if(success != NULL)
			(*success) = false;

		delete temp_key;
		return;
	}

	/* We free the allocated key */

	delete temp_key;

	/* If this part is reached, that means the
	 * above search has been successful and the
	 * 'toberemoved' node contains the key that
	 * will be removed from the tree
	 */

	if(success != NULL)
		(*success) = true;

	/* We set 'temp' as a treenode that points to
	 * the node with the desired key for deletion.
	 * 'temp' cannot be removed from the tree right
	 * now, because it is an internal node. We can
	 *                       ^^^^^^^^^^^^^
	 * only remove leaf nodes from a binary tree.
	 * With the following process we will transfer
	 * the key for deletion to the bottom level of
	 * the tree, so as it can be removed afterwards.
	 */

	Treenode *temp = toberemoved;

	/* As long as 'temp' is not a leaf node, we will
	 * repeat the following process to move the key.
	 */

	while(temp->getLeft() != NULL || temp->getRight() != NULL)
	{
		/* We find the inorder predecessor of the node */

		Treenode *helper = inorderPredecessor(temp);

		/* If the inorder predecessor does not exist,
		 * we find the inorder successor of the node
		 */

		if(helper == NULL)
			helper = inorderSuccessor(temp);

		/* We swap the data of the node with the key for deletion
		 * with the data of the inorder predeccessor/successor
		 */

		helper->swap(temp);

		/* When the data of two nodes is swapped, their colors
		 * are also swapped, so if the colors are different, we
		 * swap them again. We want to swap the user data only,
		 * not the colors
		 */

		if(getColor(helper) != getColor(temp))
		{
			changeColor(helper);
			changeColor(temp);
		}

		/* Now the key for deletion is contained in the inorder
		 * predecessor/successor. We continue the same loop until
		 * 'temp' is a leaf node. Then it will be easily removed
		 * with the classic removal process of the Binary Tree
		 * and at the same time the Binary Search Tree property
		 * will be restored again when we finally delete the key.
		 */

		temp = helper;
	}

	/* 'temp' is now a leaf node and contains the desired
	 * key for deletion. But before we cut it from the tree,
	 * we need to restore the Red Black Property as it can
	 * be violated in the cut process
	 */

	fixRemoval(temp);

	/* Finally, we remove the node 'temp' */

	nodeCut(temp);
}

/**************************************
 * Recolor & Rebalance during removal *
 **************************************/

void RedBlackTree::fixRemoval(Treenode *node)
{
	bool node_color = getColor(node);

	/* If the node is red, we return immediatelly.
	 * The Red Black property will not be violated
	 * when this node is removed.
	 */

	if(node_color == RED)
		return;

	/* If the node is black we cannot simply remove
	 * it from the tree, because the Red Black
	 * property will be violated. We have to fix
	 * the tree before that node is removed.
	 *
	 * If the node is the root, we can just remove
	 * it and have an empty tree (we remind that
	 * 'node' is always a leaf in this funcion).
	 *
	 * If the node is not the root, we declare it
	 * as "double black" and then we examine the
	 * sibling of that node. If the sibling does
	 * not exist, we return immediatelly. In else
	 * case we take the appropriate actions to
	 * rebalance the tree according to the color
	 * of the sibling.
	 */

	Treenode *doubleBlack = node;

	/* Only if the node is not the root we proceed
	 * inside the block, else we return immediatelly
	 */

	while(doubleBlack != root)
	{
		/* We retrieve the sibling of the node */

		Treenode *sibling = doubleBlack->getSibling();

		/* If the sibling does not exist, we return */

		if(sibling == NULL)
			return;

		/* 1) Actions if sibling is black
		 *                          ^^^^^
		 */

		if(getColor(sibling) == BLACK)
		{
			/* 1.1) Actions if the black sibling
			 *      has two black children
			 *
			 * ('NULL' nodes are considered black)
			 */

			if(hasTwoBlackChildren(sibling))
			{
				/* We retrieve the parent of the node */

				Treenode *parent = doubleBlack->getParent();

				/* We change the color of the sibling
				 * (from black to red)
				 */

				changeColor(sibling);

				/* If the parent is red, it becomes black.
				 * If the parent is black, it becomes
				 * "double black". In general, it always
				 * takes a "lay of black" from the sibling.
				 *
				 * Red -> Black
				 * Black -> Double Black
				 *
				 * The sibling "gives" its black color to
				 * its parent, so the sibling first became
				 * red (because it lost its black color)
				 * and the parent took a lay of black.
				 *
				 * In case the parent becomes just black,
				 * we return immediatelly and in case the
				 * parent becomes double black we repeat
				 * the 'while' loop of this function.
				 */

				if(getColor(parent) == RED)
				{
					changeColor(parent);
					break;
				}

				else
					doubleBlack = parent;
			}

			/* 1.2) Actions if the black sibling
			 *      has at least one red child
			 */

			else
			{
				/* We retrieve the close & distant nephews of the node */

				Treenode *closeNephew = doubleBlack->getCloseNephew();
				Treenode *distantNephew = doubleBlack->getDistantNephew();

				/* 1.2.1) Close nephew is red, distant nephew is black */

				if((closeNephew != NULL && getColor(closeNephew) == RED)
				&& (distantNephew == NULL || getColor(distantNephew) == BLACK))
				{
					/* The sibling and the close nephew swap colors */

					changeColor(sibling);
					changeColor(closeNephew);

					/* We retrieve the parent of the sibling */

					Treenode *parent = sibling->getParent();

					/* If the sibling is the left child of its parent,
					 * we apply a left rotation to the sibling
					 */

					if(parent->getLeft() == sibling)
						sibling->rotation(Left, &root);

					/* Else if the sibling is the right child of its
					 * parent, we apply a right rotation to the sibling
					 */

					else
						sibling->rotation(Right, &root);
				}

				/* 1.2.2) Distant nephew is red */

				if(distantNephew != NULL && getColor(distantNephew) == RED)
				{
					/* We retrieve the parent of the sibling */

					Treenode *parent = sibling->getParent();

					/* The sibling and its parent swap their colors
					 * (if they have the same color, nothing is changed)
					 */

					if(getColor(parent) != getColor(sibling))
					{
						changeColor(parent);
						changeColor(sibling);
					}

					/* If the node marked as "double black" is
					 * the left child of the sibling's parent,
					 * a left rotation of the parent is performed
					 */

					if(parent->getLeft() == doubleBlack)
						parent->rotation(Left, &root);

					/* If the node marked as "double black" is
					 * the right child of the sibling's parent,
					 * a right rotation of the parent is performed
					 */

					else
						parent->rotation(Right, &root);

					/* Lastly, the color of the distant
					 * nephew is changed and we return
					 */

					changeColor(distantNephew);
					break;
				}
			}
		}

		/* 2) Actions if sibling is red
		 *                          ^^^
		 */

		else
		{
			/* We retrieve the parent of the sibling */

			Treenode *parent = sibling->getParent();

			/* The parent gives its black color to the sibling
			 * of the node. In other words, the sibling gets a
			 * "lay of black" and the parent loses a "lay of
			 * black" and becomes red.
			 */

			if(getColor(parent) != getColor(sibling))
			{
				changeColor(parent);
				changeColor(sibling);
			}

			/* If the double black node is the left child
			 * of the sibling's parent, we apply a left
			 * rotation to the sibling's parent.
			 */

			if(doubleBlack == parent->getLeft())
				parent->rotation(Left, &root);

			/* If the double black node is the right child
			 * of the sibling's parent, we apply a right
			 * rotation to the sibling's parent.
			 */

			else
				parent->rotation(Right, &root);
		}
	}
}

/*********************************
 * Cut a leaf node from the tree *
 *********************************/

void RedBlackTree::nodeCut(Treenode *node)
{
	/* We retrieve the item saved in the node
	 * and free the allocated memory for the
	 * BST Object and the Red Black Key. Then
	 * we remove the leaf node from the tree.
	 */

	BSTObject *bst_obj = (BSTObject *) node->getItem();
	RedBlackKey *rbk = (RedBlackKey *) bst_obj->getKey();
	delete rbk;
	delete bst_obj;
	BinaryTree::remove(node);
}

/*****************************************************
 * Returns 'true' if the node has two black children *
 *****************************************************/

bool RedBlackTree::hasTwoBlackChildren(Treenode *node) const
{
	Treenode *left = node->getLeft();
	Treenode *right = node->getRight();

	/* 'NULL' nodes are considered black, so the value
	 * 'false' will be returned only if at least one
	 * existing child node is red
	 */

	if(left != NULL || right != NULL)
	{
		if(left != NULL)
		{
			if(getColor(left) == RED)
				return false;
		}

		if(right != NULL)
		{
			if(getColor(right) == RED)
				return false;
		}
	}

	return true;
}

/***************************************************************
 * Search a key in the tree - Returns 'true' if the key exists *
 ***************************************************************/

bool RedBlackTree::search(void *key, int (*compare)(void *, void *))
{
	/* We follow the simple steps below to search the key in the tree:
	 *
	 * 1) We assign the user's compare function to
	 *    the 'rbtUserCompare' function pointer
	 */

	rbtUserCompare = compare;

	/* Generally, the keys of the tree are of type 'RedBlackKey'.
	 * So, in order to search the given key of the user, we need
	 * to "encapsulate" it in a temporary 'RedBlackKey' object.
	 * Therefore, we will create a temporary 'RedBlackKey' object
	 * that will be containing the key of the user. Then we will
	 * be able to use that object to search the key in the tree.
	 * The color of the temporary 'RedBlackKey' does not matter,
	 * because it does not play any role in the search process.
	 *
	 * 2) We create a 'RedBlackKey' that will store the user's key.
	 *    (The color of that 'RedBlackKey' does not play any role)
	 */

	RedBlackKey *temp_key = new RedBlackKey(key);

	/* Now we will use the created 'RedBlackKey' to search the desired
	 * key in the tree. In order to compare the red-black keys, we are
	 * going to use a local function of this implementation which is
	 * called 'rbtImpCompare'. This function will "break" the red-black
	 * keys to their two parts (item and color) and retrieve the "real"
	 * key (the item) of each 'RedBlackKey' object. Then it will call
	 * the 'rbtUserCompare' function of step (1) to compare the two "real"
	 * keys and finally it will return the result of that function.
	 *
	 * 3) We call the 'search' operation of the Binary Search Tree
	 *    using the local 'rbtImpCompare' operation. The result of
	 *    the search (true/false) is stored in the 'result' variable
	 */

	bool result = BinarySearchTree::search(temp_key, rbtImpCompare);

	/* We completed the search and we do not need the 'RedBlackKey'
	 * object that used to encapsulate the user's key anymore. Thus,
	 * we delete the key (this is why we called it "temporary" key).
	 *
	 * 4) We delete the 'RedBlackKey' object that we created in step (2).
	 */

	delete temp_key;

	/* 5) Finally, we return the result of the search to the user */

	return result;
}

/***************************************************************
 * Search a key in the tree - Item of desired node is returned *
 ***************************************************************/

void *RedBlackTree::searchItem(void *key, int (*compare)(void *, void *))
{
	/* The procedure in this function is exactly the same as in the
	 * operation 'RedBlackTree::search' with the only difference
	 * that in step (3) we are using a different variation of search
	 * function of these that the Binary Search Tree provides. The
	 * result of that function will be returned to the user eventually.
	 */

	rbtUserCompare = compare;
	RedBlackKey *temp_key = new RedBlackKey(key);
	void *searched_item = BinarySearchTree::searchItem(temp_key, rbtImpCompare);
	delete temp_key;
	return searched_item;
}

/**************************************************************
 * Search a key in the tree - Key of desired node is returned *
 **************************************************************/

void *RedBlackTree::searchKey(void *key, int (*compare)(void *, void *))
{
	/* The procedure in this function is exactly the same as in the
	 * operation 'RedBlackTree::search' with a couple differences
	 * that we will explain below.
	 *
	 * 1) Same as step (1) in 'RedBlackTree::search'
	 */

	rbtUserCompare = compare;

	/* 2) Same as step (2) in 'RedBlackTree::search' */

	RedBlackKey *temp_key = new RedBlackKey(key);

	/* 3) It is practically the same step as step (3) in 'RedBlackTree::search',
	 *    but this time we are using a different variation of search operation
	 *    of these that the Binary Search Tree class provides us.
	 *
	 * Specifically, the operation 'BinarySearchTree::searchKey' searches for
	 * a given key with a given compare function and if the key is found, a
	 * pointer to that key is returned to the user. If the key is not found,
	 * 'NULL' is returned.
	 */

	void *searched_key = BinarySearchTree::searchKey(temp_key, rbtImpCompare);

	/* 4) Same as step (4) in 'RedBlackTree::search' */

	delete temp_key;

	/* Before we proceed to step (5) of 'RedBlackTree::search' we
	 * must do an intermediate step that we are explaining below.
	 *
	 * Step 4.5 : If the key does not exist in the tree, the variable
	 * 'searched_key' will be 'NULL'. In that case, we immediatelly
	 * return 'NULL' to the user, otherwise we will have fatal memory
	 * problems in the next step of this operation.
	 */

	if(searched_key == NULL)
		return NULL;

	/* This part is reached only if 'searched_key' is not 'NULL'.
	 * That means the given key was found in the tree and it has
	 * been returned by 'BinarySearchTree::searchKey' in the
	 * variable 'searched_key'. However, 'searched_key' is of type
	 * 'RedBlackKey'. This is not what the user wants! The user
	 * wants the "real" key that is encapsulated in this object.
	 * So, first we retrieve the real key (the "item" field of the
	 * 'RedBlackKey' class) and we return that "item" to the user.
	 *
	 * 5) After casting 'searched_key' to its real type,
	 *    'RedBlackKey *', we return the "item" field of
	 *    that object - the key that the user seeks.
	 */

	RedBlackKey *rbk = (RedBlackKey *) searched_key;
	return rbk->getItem();
}

/**********************************
 * Retrieve the key of a treenode *
 **********************************/

void *RedBlackTree::getKey(Treenode *node) const
{
	RedBlackKey *rbk = (RedBlackKey *) BinarySearchTree::getKey(node);
	return rbk->getItem();
}

/************************************************************************
 *  Prints the keys of the tree in the given order. A 'visit' function  *
 *  is required to print a key. If the given context between nodes is   *
 * a no-null value, it is called to print the context the user desires  *
 * between the items that are going to be printed. If the given context *
 * between levels is a no-null value, it is called to print the desired *
 *  context between consecutive levels of the tree (this operation has  *
 * a meaning only for 'Levelorder'). Also, if the order of traversal is *
 * 'Levelorder', the context between nodes is also printed at the start *
 *               and at the end of each level of the tree               *
 ************************************************************************/

void RedBlackTree::printKeys(OrderOfTraversal traversal_order,
	void (*visitKeys)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	/* We store the user's 'visitKeys' function to 'rbtUserVisit'.
	 * Then, 'rbtUserVisit' will be used by the 'rbtImpVisitKeys'
	 * operation of this file, which will visit the user's keys in
	 * the way the user suggested through their 'visitKeys' method.
	 *
	 * We need to use 'rbtImpVisitKeys' because we cannot instantly
	 * visit the keys of the tree with the user's visit function,
	 * because the keys of the tree are of type 'RedBlackKey' - not
	 * the user's key type. The 'RedBlackKey' type encapsulates the
	 * type of the user's key and the reason 'rbtImpVisitKeys' is
	 * needed is because it can access the user's original key type
	 * from a given 'RedBlackKey' type. Then it can use the 'visit'
	 * function of the user to print the original key. But how will
	 * 'rbtImpVisitKeys' know the user's 'visit' function? We place
	 * its memory address to the function pointer 'rbtUserVisit' of
	 * this file and 'rbtImpVisitKeys' invokes the function pointer.
	 */
	rbtUserVisit = visitKeys;

	/* We use the printing operation of the Binary Search Tree
	 * to print the user's keys with the given traversal order,
	 * the built-in visit operation of this implementation and
	 * the parameters for context printing.
	 */
	BinarySearchTree::printKeys(traversal_order,
		rbtImpVisitKeys,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*****************************************************************************
 *  Prints the keys of the tree in sorted order. This method does the same   *
 *  as 'printKeys', but with predecided traversal order (which is 'Inorder') *
 * and the context between levels being always 'NULL', as that operation has *
 * a meaning only for 'Levelorder' traversals (while in this case the order  *
 *  of traversal is always 'Inorder'). With those predecided parameters the  *
 *  keys will be printed in sorted order. That means, the user only has to   *
 *  give a 'visit' function to print each key and optionally nodes' context  *
 *****************************************************************************/

void RedBlackTree::printSortedKeys(void (*visitKeys)(void *),
	void (*contextBetweenNodes)())
{
	/* For the same reason that was described in the implementation
	 * of 'printKeys' we assign the user's 'visitKeys' operation to
	 * the function pointer 'rbtUserVisit'
	 */
	rbtUserVisit = visitKeys;

	/* We use the printing operation of the Binary Search Tree
	 * to print the user's keys in sorted order (with 'Inorder'
	 * traversal), the visit operation of this implementation
	 * and the parameters for context printing.
	 */
	BinarySearchTree::printSortedKeys(
		rbtImpVisitKeys,
		contextBetweenNodes);
}

/***************************************************************************
 *  Prints the keys of the tree in the given traversal order with colored  *
 * output. The user may enter their own methods to introduce red and black *
 * context, but the default methods of the class will be used in case the  *
 * user does not give their own. That means, each key of the tree will be  *
 * printed with red or black color (if the default functions are used) or  *
 * the red and black notation functions of the user will be called if they *
 * are no-null values. The rest args are the same as in the above versions *
 ***************************************************************************/

void RedBlackTree::printColoredKeys(OrderOfTraversal traversal_order,
	void (*visitKeys)(void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)(),
	void (*redNotation)(),
	void (*blackNotation)())
{
	/* For the same reason that was described in the implementation
	 * of 'printKeys' we assign the user's 'visitKeys' operation to
	 * the function pointer 'rbtUserVisit' and for similar reasons
	 * we need to do the same with the two color notation operations.
	 */
	rbtUserRedNotation = redNotation;
	rbtUserBlackNotation = blackNotation;
	rbtUserVisit = visitKeys;

	/* We use the printing operation of the Binary Search Tree
	 * to print the user's keys with the given traversal order,
	 * the built-in visit operation of this implementation and
	 * the parameters for context printing.
	 */
	BinarySearchTree::printKeys(traversal_order,
		rbtImpVisitColoredKeys,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*************************************************************************
 * Prints the items & the keys of the tree in the given order. A 'visit' *
 *  is required to print both. If the given context between nodes is a   *
 *  a no-null value, it is called to print the context the user desires  * 
 * between the items that are going to be printed. If the given context  *
 * between levels is a no-null value, it is called to print the desired  *
 *  context between consecutive levels of the tree (this operation has   *
 * a meaning only for 'Levelorder'). Also, if the order of traversal is  *
 * 'Levelorder', the context between nodes is also printed at the start  *
 *               and at the end of each level of the tree                *
 *************************************************************************/

void RedBlackTree::printBoth(OrderOfTraversal traversal_order,
	void (*visitBoth)(void *, void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)())
{
	/* For a similar reason that was described in the implementation
	 * of 'printKeys' we assign the user's 'visitBoth' operation to
	 * the function pointer 'rbtUserVisitBoth'.
	 */
	rbtUserVisitBoth = visitBoth;

	/* We use the printing operation of the Binary Search Tree to
	 * print the user's items & keys with the given traversal order,
	 * the built-in visit operation of this implementation and the
	 * parameters for context printing.
	 */
	BinarySearchTree::printBoth(traversal_order,
		rbtImpVisitBoth,
		contextBetweenNodes,
		contextBetweenLevels);
}

/*****************************************************************************
 * Prints the items & the keys of the tree in sorted order. This method does *
 *  the same as 'printBoth', but with predecided traversal order (which is   *
 *  'Inorder') and the context between levels being always 'NULL', as that   *
 *  operation has a meaning only for 'Levelorder' traversals (while in this  *
 *  case the order of traversal is always 'Inorder'). With those predecided  *
 *  parameters the items & keys will be printed in sorted order. That means, *
 *  the user only has to give a 'visit' function to print each pair of item  *
 * and key and optionally a function that prints context between those pairs *
 *****************************************************************************/

void RedBlackTree::printSortedBoth(void (*visitBoth)(void *, void *),
	void (*contextBetweenNodes)())
{
	/* For a similar reason that was described in the implementation
	 * of 'printKeys' we assign the user's 'visitBoth' operation to
	 * the function pointer 'rbtUserVisitBoth'.
	 */
	rbtUserVisitBoth = visitBoth;

	/* We use the printing operation of the Binary Search Tree to
	 * print the user's items & keys in sorted order ('Inorder'),
	 * the built-in visit operation of this implementation and the
	 * parameters for context printing.
	 */
	BinarySearchTree::printSortedBoth(rbtImpVisitBoth,
		contextBetweenNodes);
}

/****************************************************************************
 *   Prints the items & keys of the tree in the given order with colored    *
 * output. The user may enter their own methods to introduce red and black  *
 *  context, but the default methods of the class will be used in case the  *
 * user does not give their own. That means, each pair of item & key of the *
 *  tree will be printed with red or black color (if the default functions  *
 *  are used) or the red and black notation functions of the user will be   *
 * called if they are no-null values. The rest args are the same as in the  *
 *                              above versions                              *
 ****************************************************************************/

void RedBlackTree::printColoredBoth(OrderOfTraversal traversal_order,
	void (*visitBoth)(void *, void *),
	void (*contextBetweenNodes)(),
	void (*contextBetweenLevels)(),
	void (*redNotation)(),
	void (*blackNotation)())
{
	/* For a similar reason that was described in the implementation
	 * of 'printKeys' we assign the user's 'visitBoth' operation to
	 * the function pointer 'rbtUserVisitBoth'. For similar reasons we
	 * assign the color notation functions to specific function pointers.
	 */
	rbtUserRedNotation = redNotation;
	rbtUserBlackNotation = blackNotation;
	rbtUserVisitBoth = visitBoth;

	/* We use the printing operation of the Binary Search Tree to
	 * print the user's items & keys with the given traversal order,
	 * the built-in visit operation of this implementation and the
	 * parameters for context printing.
	 */
	BinarySearchTree::printBoth(traversal_order,
		rbtImpVisitColoredBoth,
		contextBetweenNodes,
		contextBetweenLevels);
}

/******************************************************************
 * Visits an item of type 'RedBlackKey' by accessing the original *
 * key of the user (which is stored inside the Red Black Key) and *
 * visiting the original key with the user's visit function - the *
 *     user's visit function is stored in the function pointer    *
 *                         'rbtUserVisit'                         *
 ******************************************************************/

void rbtImpVisitKeys(void *key)
{
	/* We know the key is of type 'RedBlackKey' */
	RedBlackKey *rbk = (RedBlackKey *) key;

	/* We store the user's visit method to a temporary function pointer, because
	 * if the tree stores inner Red Black Trees, then if those are visited, the
	 * function pointer 'rbtUserVisit' will be overwritten so as the inner trees
	 * are visited properly. But when that happens, we do not want to lose the
	 * initial visit function to print the outer tree. For that reason, we save
	 * its value in a temporary function pointer.
	 */
	void (*tempUserVisit)(void *) = rbtUserVisit;

	/* We visit the user's original key with their visit function */
	rbtUserVisit(rbk->getItem());

	/* We retrieve the visit function of the user */
	rbtUserVisit = tempUserVisit;
}

/******************************************************************
 * Visits an item of type 'RedBlackKey' by accessing the original *
 * key of the user (which is stored inside the Red Black Key) and *
 * visiting the original key with the user's visit function - the *
 *     user's visit function is stored in the function pointer    *
 *  'rbtUserVisit'. Also, we print the color of the specific key  *
 ******************************************************************/

void rbtImpVisitColoredKeys(void *key)
{
	/* We know the key is of type 'RedBlackKey' */
	RedBlackKey *rbk = (RedBlackKey *) key;

	/* From this Red Black Key we retrieve the original key */
	void *item = rbk->getItem();

	/* We also retrieve the color of the node storing that key */
	bool color = rbk->getKey();

	/* We store the user's visit method and the color notation
	 * methods to temporary function pointers for exactly the
	 * same reason that was mentioned inside the implementation
	 * of 'rbtImpVisitKeys'. Their values might be overwritten,
	 * so in that case we have to retrieve them back.
	 */
	void (*tempUserVisit)(void *) = rbtUserVisit;
	void (*color1)() = rbtUserRedNotation;
	void (*color2)() = rbtUserBlackNotation;

	/* If the node is red, we call the red notation function */

	if(color == RED)
		rbtUserRedNotation();

	/* Else we call the black notation function */

	else
		rbtUserBlackNotation();

	/* We visit the user's original key with their visit function */
	rbtUserVisit(item);

	/* We retrieve the user's visit function and the color
	 * notation functions from the temporary pointers where
	 * we saved them earlier
	 */
	rbtUserVisit = tempUserVisit;
	rbtUserRedNotation = color1;
	rbtUserBlackNotation = color2;

	/* If the node is red and the default red notation
	 * function was used, we have to reset the terminal
	 * color output because earlier it was set to red.
	 */
	if((color == RED) && (rbtUserRedNotation == RedBlackTree::defaultRed))
		printf(RESET_COLOR);

	/* If the node is black and the default black notation
	 * function was used, we have to reset the terminal
	 * color output because earlier it was set to black.
	 */
	if((color == BLACK) && (rbtUserBlackNotation == RedBlackTree::defaultBlack))
		printf(RESET_COLOR);
}

/**********************************************************************
 * Visits both the item and the key of a node. The item is the user's *
 * original item, but the key is not the user's original key. Instead *
 * it is a class type named 'RedBlackKey'. Within that Red Black Key  *
 * the original key of the user is stored. This operation visits the  *
 * original item and the original key of the user by casting the key  *
 *  to 'RedBlackKey' and then accessing the original key through it   *
 **********************************************************************/

void rbtImpVisitBoth(void *item, void *key)
{
	/* We know the key is of type 'RedBlackKey' */
	RedBlackKey *rbk = (RedBlackKey *) key;

	/* We store the user's visit method to a temporary function pointer, because
	 * if the tree stores inner Red Black Trees, then if those are visited, the
	 * function pointer 'rbtUserVisitBoth' will be overwritten so as the inner
	 * trees are visited properly. But when that happens, we do not want to lose
	 * the initial visit function to print the outer tree. For that reason, we
	 * save its value in a temporary function pointer.
	 */
	void (*tempUserVisit)(void *, void *) = rbtUserVisitBoth;

	/* We visit the original item and the original
	 * key with the user's visit function
	 */
	rbtUserVisitBoth(item, rbk->getItem());
	rbtUserVisitBoth = tempUserVisit;
}

/**********************************************************************
 * Visits both the item and the key of a node. The item is the user's *
 * original item, but the key is not the user's original key. Instead *
 * it is a class type named 'RedBlackKey'. Within that Red Black Key  *
 * the original key of the user is stored. This operation visits the  *
 * original item and the original key of the user by casting the key  *
 *  to 'RedBlackKey' and then accessing the original key through it.  *
 *  Also, this function calls the proper color notation method given  *
 *   the color of the node where the item and the key are stored in   *
 **********************************************************************/

void rbtImpVisitColoredBoth(void *item, void *key)
{
	/* We know the key is of type 'RedBlackKey' */
	RedBlackKey *rbk = (RedBlackKey *) key;
	
	/* From this Red Black Key we retrieve the original key */
	void *rbk_item = rbk->getItem();

	/* We also retrieve the color of the node storing that key */
	bool color = rbk->getKey();

	/* We store the user's visit method and the color notation
	 * methods to temporary function pointers for exactly the
	 * same reason that was mentioned inside the implementation
	 * of 'rbtImpVisitBoth'. Their values might be overwritten,
	 * so in that case we have to retrieve them back.
	 */
	void (*tempUserVisit)(void *, void *) = rbtUserVisitBoth;
	void (*color1)() = rbtUserRedNotation;
	void (*color2)() = rbtUserBlackNotation;

	/* If the node is red, we call the red notation function */

	if(color == RED)
		rbtUserRedNotation();

	/* Else we call the black notation function */

	else
		rbtUserBlackNotation();

	/* We visit the user's original item and
	 * original key with their visit function
	 */
	rbtUserVisitBoth(item, rbk_item);

	/* We retrieve the user's visit function and the color
	 * notation functions from the temporary pointers where
	 * we saved them earlier
	 */
	rbtUserVisitBoth = tempUserVisit;
	rbtUserRedNotation = color1;
	rbtUserBlackNotation = color2;

	/* If the node is red and the default red notation
	 * function was used, we have to reset the terminal
	 * color output because earlier it was set to red.
	 */
	if((color == RED) && (rbtUserRedNotation == RedBlackTree::defaultRed))
		printf(RESET_COLOR);

	/* If the node is black and the default black notation
	 * function was used, we have to reset the terminal
	 * color output because earlier it was set to black.
	 */
	if((color == BLACK) && (rbtUserBlackNotation == RedBlackTree::defaultBlack))
		printf(RESET_COLOR);
}

/*************************************************************
 * Call the 'actions' operation for every pair of item & key *
 *   in the nodes of the tree in the given traversal order   *
 *************************************************************/

void RedBlackTree::traverse(OrderOfTraversal traversal_order,
	void (*actions)(void *, void *))
{
	rbtUserTraverse = actions;
	BinaryTree::traverse(traversal_order, rbtImpTraverse);
}

/************************************************************
 * Operation that accesses the user's original item and key *
 * and calls the user's 'actions' operation by passing the  *
 *            original item and key as arguments            *
 ************************************************************/

void rbtImpTraverse(void *item)
{
	/* Generally a node in the tree stores an item of type 'BSTObject' */
	BSTObject *bst_obj = (BSTObject *) item;

	/* That 'BSTObject' consists of two fields: the user's original item
	 * and a 'RedBlackKey', which stores the user's original key and the
	 * color of the node.
	 *
	 * We retrieve the original item of the user
	 */
	void *user_item = bst_obj->getItem();

	/* We retrieve the 'RedBlackKey' of the 'BSTObject' */
	RedBlackKey *rbk = (RedBlackKey *) bst_obj->getKey();

	/* We retrieve the original key of the user from the 'RedBlackKey' */
	void *user_key = rbk->getItem();

	/* We store the user's traverse function to a temporary pointer */
	void (*tempUserTraverse)(void *, void *) = rbtUserTraverse;

	/* We call the user's traverse function with the
	 * original item and the original key as parameters
	 */
	rbtUserTraverse(user_item, user_key);

	/* We retrieve the user's traverse function from the
	 * temporary function pointer we saved it earlier.
	 */
	rbtUserTraverse = tempUserTraverse;
}

/*********************************************************************
 * Deletes all Red Black Keys in the subtree that starts from 'node' *
 *********************************************************************/

void RedBlackTree::deleteRedBlackKey(Treenode *node)
{
	/* If the node is 'NULL', there is nothing to delete */
	if(node == NULL)
		return;

	/* We retrieve the 'BSTObject' stored inside the node */
	BSTObject *node_item = (BSTObject *) node->getItem();

	/* From the 'BSTObject' we retrieve the 'RedBlackKey' */
	RedBlackKey *key = (RedBlackKey *) node_item->getKey();

	/* We delete the Red Black Key */
	delete key;

	/* We delete the Red Black Key of the left child of the node */
	deleteRedBlackKey(node->getLeft());

	/* We delete the Red Black Key of the right child of the node */
	deleteRedBlackKey(node->getRight());
}

/***********************************
 * Remove every node from the tree *
 ***********************************/

void RedBlackTree::destroy()
{
	/* We delete all the Red Black Keys in the tree */
	deleteRedBlackKey(root);

	/* Now we delete every node in the tree with the 'destroy'
	 * operation that is provided by the Binary Search Tree.
	 */
	BinarySearchTree::destroy();
}
