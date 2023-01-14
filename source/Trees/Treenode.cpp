#include <iostream>
#include "Treenode.h"

/****************************
 * Constructor & Destructor *
 ****************************/

Treenode::Treenode(void *item)
{
	this->item = item;
	left = NULL;
	right = NULL;
	parent = NULL;
}

Treenode::~Treenode() {}

/***********
 * Getters *
 ***********/

void *Treenode::getItem() const
{
	return item;
}

Treenode *Treenode::getLeft() const
{
	return left;
}

Treenode *Treenode::getRight() const
{
	return right;
}

Treenode *Treenode::getParent() const
{
	return parent;
}

/***********
 * Setters *
 ***********/

void Treenode::setItem(void *item)
{
	this->item = item;
}

void Treenode::setLeft(Treenode *left)
{
	this->left = left;
}

void Treenode::setRight(Treenode *right)
{
	this->right = right;
}

void Treenode::setParent(Treenode *parent)
{
	this->parent = parent;
}

/************
 * Printing *
 ************/

void Treenode::print(void (*visit)(void *)) const
{
	visit(item);
}

/*******************************
 * Returns the uncle of a node *
 *******************************/

Treenode *Treenode::getUncle() const
{
	/* If the node's parent is 'NULL' (that means
	 * the node is the root), it has no uncle, so
	 * we return 'NULL'
	 */

	if(parent == NULL)
		return NULL;

	Treenode *gp = getGrandparent();

	/* If there is no grandparent (that means
	 * the parent is the root), then there is
     * no uncle as well (because the root has
	 * no siblings)
	 */

	if(gp == NULL)
		return NULL;

	/* Else, we examine if the node's parent
	 * is the left or the right child of the
	 * grandparent and we return the other
	 * child (right or left respectively)
	 */

	if(gp->left == parent)
		return gp->right;

	return gp->left;
}

/*************************************
 * Returns the grandparent of a node *
 *************************************/

Treenode *Treenode::getGrandparent() const
{
	Treenode *parent = this->parent;

	/* If the parent of the node is 'NULL',
	 * there is no grandparent
	 */

	if(parent == NULL)
		return NULL;

	return parent->parent;
}

/*********************************
 * Returns the sibling of a node *
 *********************************/

Treenode *Treenode::getSibling() const
{
	/* If the node has no parent, then it
	 * is the root node and has no sibling
	 */

	if(parent == NULL)
		return NULL;

	/* If the node is its parent's left
	 * child, then its sibling is its
	 * parent's right child. If the node
	 * is the parent's right child, then
	 * its sibling is its parent's left
	 * child
	 */

	if(parent->left == this)
		return parent->right;

	return parent->left;
}

/************************************
 * Returns the close nephew of node *
 ************************************/

Treenode *Treenode::getCloseNephew() const
{
	/* If the node is the root node, it has only children
	 * (and not nephews), because the root has no sibling.
	 * So there is no close nephew and 'NULL' is returned
	 */

	if(parent == NULL)
		return NULL;

	/* Else we retrieve the sibling */

	Treenode *sibling = getSibling();

	/* If the sibling does not exist, that means the parent
	 * of the node has only one child (for now): the node
	 * In this case there are no nephews ('NULL' is returned)
	 */

	if(sibling == NULL)
		return NULL;

	/* The close nephew will depend on whether the node is its
	 * parent's left or right child. If the node it its parent's
	 * left child, its close nephew is its sibling's left child.
	 * If the node is its parent's right child, its close nephew
	 * is its sibling's right child.
	 */

	return (this == parent->left) ? sibling->left : sibling->right;
}

/**************************************
 * Returns the distant nephew of node *
 **************************************/

Treenode *Treenode::getDistantNephew() const
{
	/* If the node is the root node, it has only children
	 * (and not nephews), because the root has no sibling.
	 * So there is no distant nephew and 'NULL' is returned
	 */

	if(parent == NULL)
		return NULL;

	/* Else we retrieve the sibling */

	Treenode *sibling = getSibling();

	/* If the sibling does not exist, that means the parent
	 * of the node has only one child (for now): the node
	 * In this case there are no nephews ('NULL' is returned)
	 */

	if(sibling == NULL)
		return NULL;

	/* The distant nephew will depend on whether the node is its
	 * parent's left or right child. If the node it its parent's
	 * left child, its distant nephew is its sibling's right child.
	 * If the node is its parent's right child, its distant nephew
	 * is its sibling's left child.
	 */

	return (this == parent->left) ? sibling->right : sibling->left;
}

/************************************
 * Rotate a tree node left or right *
 ************************************/

void Treenode::rotation(Rotation rotation_type, Treenode **root)
{
	switch(rotation_type)
	{
		/* Left rotation of the given treenode */

		case Left:
		{
			/* The link changes (the rotation is happening) */

			Treenode *r = right;
			right = r->left;
			r->left = this;

			/* Further link changes (we fix the parent of 'right') */

			if(right != NULL)
				right->parent = this;

			/* If the right node had parent before but now
			 * does not after the rotation, this node must
			 * become the root of the tree where all these
			 * nodes belong
			 */

			if(parent == NULL)
			{
				r->parent = NULL;
				(*root) = r;
			}

			/* Else if there is parent, we set the appropriate
			 * link to it so as it has 'r' as its new child
			 */

			else
			{
				if(parent->left == this)
					parent->left = r;

				else
					parent->right = r;

				r->parent = parent;
			}

			parent = r;
			break;
		}

		/* Right rotation of the given treenode */

		case Right:
		{
			/* The link changes (the rotation is happening) */

			Treenode *l = left;
			left = l->right;
			l->right = this;

			/* Further link changes (we fix the parent of 'left') */

			if(left != NULL)
				left->parent = this;

			/* If the left node had parent before but now
			 * does not after the rotation, this node must
			 * become the root of the tree where all these
			 * nodes belong
			 */

			if(parent == NULL)
			{
				l->parent = NULL;
				(*root) = l;
			}

			/* Else if there is parent, we set the appropriate
			 * link to it so as it has 'l' as its new child
			 */

			else
			{
				if(parent->left == this)
					parent->left = l;

				else
					parent->right = l;

				l->parent = parent;
			}

			parent = l;
			break;
		}
	}
}

/*************************************
 * Swaps the items of two tree nodes *
 *************************************/

void Treenode::swap(Treenode *node)
{
	void *temp = item;
	item = node->item;
	node->item = temp;
}
