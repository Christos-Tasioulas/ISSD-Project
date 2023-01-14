#ifndef _RED_BLACK_TREE_
#define _RED_BLACK_TREE_

#include "BinarySearchTree.h"
#include "PairBool.h"
#include "Colors.h"

#define RED   false
#define BLACK  true

/* A Red Black Key is a user's key combined with a
 * color implied by a boolean variable (Red or Black)
 */
typedef PairBool RedBlackKey;

/* The Red Black Tree Class */

class RedBlackTree : public BinarySearchTree {

public:

/* Constructor & Destructor */
	RedBlackTree();
	~RedBlackTree();

/* Insert a new node in the tree */
	void insert(void *item, void *key, int (*compare)(void *, void *));

/* Remove a node from the tree */
	void remove(void *key, int (*compare)(void *, void *), bool *success = NULL);

/* Search a key in the tree (returns 'true' if the key exists) */
	bool search(void *key, int (*compare)(void *, void *));

/* Search a key in the tree (item of desired node is returned) */
	void *searchItem(void *key, int (*compare)(void *, void *));

/* Search a key in the tree (key of desired node is returned) */
	void *searchKey(void *key, int (*compare)(void *, void *));

/* Retrieve the key of a treenode */
	void *getKey(Treenode *node) const;

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
		void (*visitKeys)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

/* Prints the keys of the tree in sorted order. This method does the same
 * as 'printKeys', but with predecided traversal order (which is 'Inorder')
 * and the context between levels being always 'NULL', as that operation has
 * a meaning only for 'Levelorder' traversals (while in this case the order
 * of traversal is always 'Inorder'). With those predecided parameters the
 * keys will be printed in sorted order. That means, the user only has to
 * give a 'visit' function to print each key and optionally nodes' context.
 */
	void printSortedKeys(void (*visitKeys)(void *),
		void (*contextBetweenNodes)() = NULL);

/* Prints the keys of the tree in the given traversal order with colored
 * output. The user may enter their own methods to introduce red and black
 * context, but the default methods of the class will be used in case the
 * user does not give their own. That means, each key of the tree will be
 * printed with red or black color (if the default functions are used) or
 * the red and black notation functions of the user will be called if they
 * are no-null values. The rest args are the same as in the above versions.
 */
	void printColoredKeys(OrderOfTraversal traversal_order,
		void (*visitKeys)(void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL,
		void (*redNotation)() = defaultRed,
		void (*blackNotation)() = defaultBlack);

/* Prints the items & the keys of the tree in the given order. A 'visit'
 * is required to print both. If the given context between nodes is a
 * a no-null value, it is called to print the context the user desires
 * between the items that are going to be printed. If the given context
 * between levels is a no-null value, it is called to print the desired
 * context between consecutive levels of the tree (this operation has
 * a meaning only for 'Levelorder'). Also, if the order of traversal is
 * 'Levelorder', the context between nodes is also printed at the start
 * and at the end of each level of the tree.
 */
	void printBoth(OrderOfTraversal traversal_order,
		void (*visitBoth)(void *, void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

/* Prints the items & the keys of the tree in sorted order. This method does
 * the same as 'printBoth', but with predecided traversal order (which is
 * 'Inorder') and the context between levels being always 'NULL', as that
 * operation has a meaning only for 'Levelorder' traversals (while in this
 * case the order of traversal is always 'Inorder'). With those predecided
 * parameters the items & keys will be printed in sorted order. That means,
 * the user only has to give a 'visit' function to print each pair of item
 * and key and optionally a function that prints context between those pairs.
 */
	void printSortedBoth(void (*visitBoth)(void *, void *),
		void (*contextBetweenNodes)() = NULL);

/* Prints the items & keys of the tree in the given order with colored
 * output. The user may enter their own methods to introduce red and black
 * context, but the default methods of the class will be used in case the
 * user does not give their own. That means, each pair of item & key of the
 * tree will be printed with red or black color (if the default functions
 * are used) or the red and black notation functions of the user will be
 * called if they are no-null values. The rest args are the same as in the
 * above versions.
 */
	void printColoredBoth(OrderOfTraversal traversal_order,
		void (*visitBoth)(void *, void *),
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL,
		void (*redNotation)() = defaultRed,
		void (*blackNotation)() = defaultBlack);

/* Invokes the 'actions' operation for every pair of item &
 * key in the nodes of the tree in the given traversal order
 */
	void traverse(OrderOfTraversal traversal_order,
		void (*actions)(void *, void *));

/* Removes every node from the tree */
	void destroy();

/* The default function to print red-painted nodes */
	static void defaultRed();

/* The default function to print black-painted nodes */
	static void defaultBlack();

private:

/* Returns the color of a node in the red-black tree */
	bool getColor(Treenode *node) const;

/* Changes the color of a node in the red-black tree */
	void changeColor(Treenode *node);

/* Recolors and rebalances the tree after an insertion.
 * The inserted node must be passed as argument
 */
	void fixInsertion(Treenode *node);

/* Special cases of 'fixInsertion' when the uncle
 * of the examined node is 'NULL' or black
 */
	void insertionCaseLeftLeft(Treenode *node, Treenode *parent, Treenode *grandparent);
	void insertionCaseRightRight(Treenode *node, Treenode *parent, Treenode *grandparent);

/* Recolors and rebalances the tree in the node removal process.
 * The node which is going to be removed must by passed as argument
 */
	void fixRemoval(Treenode *node);

/* Used by 'fixRemoval' to cut a leaf node from the Red Black Tree
 * by deleting the allocated 'RedBlackKey' & 'BSTObject' items of it
 * and then removing the node in the classic way of the Binary Tree
 */
	void nodeCut(Treenode *node);

/* Returns 'true' if the node has two black children ('NULL' children
 * are considered black). The operation is mostly used by 'fixRemoval'
 */
	bool hasTwoBlackChildren(Treenode *node) const;

/* Deletes all Red Black Keys in the subtree that starts from 'node' */
	void deleteRedBlackKey(Treenode *node);

};

#endif
