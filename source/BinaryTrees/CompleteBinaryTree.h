#ifndef _COMPLETE_BINARY_TREE_H_
#define _COMPLETE_BINARY_TREE_H_

#include "BinaryTree.h"

class CompleteBinaryTree : public BinaryTree {

public:

/* Constructor & Destructor */
	CompleteBinaryTree();
	~CompleteBinaryTree();

/* Insert a new node in the tree */
	void insert(void *item);

/* Remove a node from the tree */
	void removeLast();

protected:

/* The Complete Binary Tree, in addition to
 * the inherited members of the Binary Tree,
 * contains a pointer to the treenode that
 * was last inserted in the tree
 */
	Treenode *last;

};

#endif
