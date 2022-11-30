#ifndef _BINARY_SEARCH_TREE_H_
#define _BINARY_SEARCH_TREE_H_

#include "BinaryTree.h"
#include "ComplexItem.h"

/* Each node in a Binary Search Tree consists of an 'item' and a 'key'.
 * The 'item' and the 'key' are both of type 'void *'. Every insertion,
 * removal or search on the tree is performed by comparing the 'key'
 * data of the nodes. The 'item' does not participate at all in the
 * above operations. It is just some data of the user that accompanies
 * the 'key'.
 *
 * In order to save appropriately both the 'item' and the 'key' in a
 * tree node, we are going to use the data type 'ComplexItem' in our
 * implementation. But first, here we give it a new name - 'BSTOBject'.
 */

typedef ComplexItem BSTObject;

/* Binary Search Tree class */

class BinarySearchTree : public BinaryTree {

public:

/* Constructor & Destructor
 *
 * No arguments are needed for the construction of a tree.
 * The destructor is 'virtual' because subsidiary classes
 * exist (aka derived classes from the Binary Search Tree).
 */
	BinarySearchTree();
	virtual ~BinarySearchTree();

/* Inserts a new node in the tree
 *
 * The new node will be storing the 'item' and the 'key' that the
 * user gives in the first two arguments. A 'compare' function is
 * needed to compare the 'key' data of the new node with the 'key'
 * data of other nodes so as the new node is placed in the correct
 * place in the Binary Search Tree.
 *
 * Optionally, the user can retrieve the address of the newly created
 * tree node if they have passed the memory address of a 'Treenode *'
 * as the last argument. However, this feature is not meant to be used
 * by a typical user as it provides no useful information. This is
 * mostly used by subsidiary classes, such as the Red Black Tree.
 */
	void insert(void *item, void *key, int (*compare)(void *, void *),
		Treenode **inserted = NULL);

/* Removes a node with 'key' data equal to the key the user gives
 *
 * If there are many nodes with this key, only the first node that
 * will be found to have that key will be deleted. If there are no
 * nodes with the given key in the tree, nothing will be changed.
 *
 * A 'compare' function is needed to compare the 'key' data of the
 * nodes. The last argument is optional and it is a boolean address.
 * If it is not 'NULL', then after the end of the 'remove' operation
 * it will be saving 'true' if the removal was successful or 'false'
 * if nothing changed in the tree as the given key was not found.
 */
	void remove(void *key, int (*compare)(void *, void *), bool *removal_true = NULL);

/* Retrieve the item or the key of a treenode */
	void *getItem(Treenode *node) const;
	void *getKey(Treenode *node) const;
 
/* Prints the items of the tree in the given order. A 'visit' function
 * is required to print an item. If the given 'contextBetweenNodes' is
 * a no-null value, it is called to print the context the user desires
 * between the items that are going to be printed. If the given context
 * between levels is a no-null value, it is called to print the desired
 * context between consecutive levels of the tree (this operation has
 * a meaning only for 'Levelorder'). Also, if the order of traversal is
 * 'Levelorder', the context between nodes is also printed at the start
 * and at the end of each level of the tree.
 */
	void printItems(OrderOfTraversal traversal_order,
		void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

/* Prints the keys of the tree in the given order. A 'visit' function
 * is required to print a key. If the given context between nodes is
 * a no-null value, it is called to print the context the user desires
 * between the items that are going to be printed. If the given context
 * between levels is a no-null value, it is called to print the desired
 * context between consecutive levels of the tree (this operation has
 * a meaning only for 'Levelorder'). Also, if the order of traversal is
 * 'Levelorder', the context between nodes is also printed at the start
 * and at the end of each level of the tree.
 */
	void printKeys(OrderOfTraversal traversal_order,
		void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

/* Prints both the items and the keys of the tree in the given order. A
 * 'visit' function is required to print both. If the given context
 * between nodes is a no-null value, it is called to print the context
 * the user desires between the items that are going to be printed. If
 * the given context between levels is a no-null value, it is called to
 * print the desired context between consecutive levels of the tree
 * (this operation has a meaning only for 'Levelorder'). Also, if the
 * order of traversal is 'Levelorder', the context between nodes is also
 * printed at the start and at the end of each level of the tree.
 */
	void printBoth(OrderOfTraversal traversal_order,
		void (*visit)(void *, void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

/* Operations to print the items of a tree, the keys of a tree or both
 * in sorted order according to the values of the keys of the nodes
 *                                                ^^^^
 * That means, only the values of the keys play role in the sorting.
 *             ^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * This is a rule for all of the three operations for sorted printing
 * below. A 'visit' function with one argument is needed if the user wants
 * to print only the items or only the keys of the tree. If the user wants
 * to print both the items and the keys of the tree, a 'visit' function
 * with two arguments is required, the first representing the 'item' and
 * the second the 'key'. If the user wants to print something between the
 * items that are going to be printed, they can imply it with a no-null
 * 'contextBetweenNodes' operation as last argument of these three methods.
 */
	void printSortedItems(void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL);

	void printSortedKeys(void (*visit)(void *),
		void (*contextBetweenNodes)() = NULL);

	void printSortedBoth(void (*visit)(void *, void *),
		void (*contextBetweenNodes)() = NULL);

/* Searches the given key in the tree (returns 'true' or 'false')
 *
 * Returns 'true'  if the key exists
 * Returns 'false' if the key does not exist
 *
 * A 'compare' function is needed to compare the keys of the tree.
 *
 * Optionally, the user can retrieve the address of the node storing the
 * given key if they have passed the memory address of a 'Treenode *' as
 * the last argument. However, this feature is not meant to be used by a
 * typical user as it provides no useful information. This is mostly used
 * by subsidiary classes, such as the Red Black Tree.
 */
	bool search(void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* Searches the given key in the tree (returns an item)
 *
 * If the given key is found in the tree, the 'item' data
 * of the node storing that key is returned to the user.
 *
 * A 'compare' function is needed to compare the keys of the tree.
 *
 * Optionally, the user can retrieve the address of the node storing the
 * given key if they have passed the memory address of a 'Treenode *' as
 * the last argument. However, this feature is not meant to be used by a
 * typical user as it provides no useful information. This is mostly used
 * by subsidiary classes, such as the Red Black Tree.
 */
	void *searchItem(void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* Searches the given key in the tree (returns a key)
 *
 * If the given key is found in the tree, the 'key' data
 * of the node storing that key is returned to the user.
 *
 * A 'compare' function is needed to compare the keys of the tree.
 *
 * Optionally, the user can retrieve the address of the node storing the
 * given key if they have passed the memory address of a 'Treenode *' as
 * the last argument. However, this feature is not meant to be used by a
 * typical user as it provides no useful information. This is mostly used
 * by subsidiary classes, such as the Red Black Tree.
 */
	void *searchKey(void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* This operation should be used when the user wants to execute some
 * specific actions for each pair of 'item' and 'key' in the tree
 *
 * The user must first create a function that expects an item and a
 * key and applies the actions the user wants on that item and key.
 *
 * In the 'traverse' operation, the above user function will be called
 * for the pair of 'item' and 'key' of each node in the suggested
 * traversal order (Preorder, Inorder, Postorder or Levelorder).
 */
	void traverse(OrderOfTraversal traversal_order, void (*actions)(void *, void *));

/* Removes every node from the tree
 *
 * The tree will be empty after the process but will still exist.
 */
	virtual void destroy();

protected:

/* Returns the inorder predecessor of a node (the node
 * with the highest key value of the left subtree)
 */
	Treenode *inorderPredecessor(Treenode *node) const;

/* Returns the inorder successor of a node (the node
 * with the lowest key value of the right subtree)
 */
	Treenode *inorderSuccessor(Treenode *node) const;

private:

/* Uses recursion to insert a new node in the tree and at the
 * same time preserve the property of the Binary Search Tree
 */
	void insertRec(Treenode *node, void *item, void *key,
		int (*compare)(void *, void *), Treenode **inserted = NULL);

/* Uses recursion to search the desired key in the Binary Seach Tree
 * and returns 'true' if the key is found in the tree
 */
	bool searchRec(Treenode *node, void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* Uses recursion to search the desired key in the Binary Seach Tree
 * and returns the item stored in the corresponding tree node
 */
	void *searchItemRec(Treenode *node, void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* Uses recursion to search the desired key in the Binary Seach Tree
 * and returns the key stored in the corresponding tree node
 */
	void *searchKeyRec(Treenode *node, void *key, int (*compare)(void *, void *),
		Treenode **searched = NULL) const;

/* Deletes the allocated memory for the complex items saved in the tree */
	void deleteBSTObject(Treenode *node);

};

#endif
