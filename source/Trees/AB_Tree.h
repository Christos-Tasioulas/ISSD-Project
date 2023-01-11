#ifndef _AB_TREE_H_
#define _AB_TREE_H_

#include "AB_Node.h"
#include "ComplexItem.h"
#include "OrderOfTraversal.h"

typedef ComplexItem AB_Item;

/* The AB Tree Class */

class AB_Tree {

public:

	/* Constructor & Destructor */
	AB_Tree(unsigned int A, unsigned int B);
	virtual ~AB_Tree();

	/* Getters */
	AB_Node *getRoot() const;
	unsigned int getA() const;
	unsigned int getB() const;
	unsigned int getCounter() const;

	/* Returns 'true' if the tree has no nodes, else returns 'false' */
	bool isEmpty() const;

	/* Returns the number of nodes in the tree */
	unsigned int size() const;

	/* Returns the height of the tree */
	unsigned int height() const;

	/* Inserts an item in the tree which is accompanied by a key.
	 * A 'compare' operation must be given to compare the key.
	 *
	 * If the key exists in the tree already, the insertion will
	 * fail, because there must be no duplicate keys in the tree.
	 *
	 * The user may ensure that the insertion was successful by
	 * giving the address of a boolean variable as last argument.
	 * If the insertion is successful, the value 'true' will be
	 * assigned to the boolean variable, else if the insertion
	 * fails, the value 'false' will be assiged to the variable.
	 *
	 * Providing such a variable is optional.
	 */
	void insert(void *item, void *key, int (*compare)(void *, void *),
		bool *insertionWasSuccessful = NULL);

	/* Removes a pair of item and key from the tree. The key that
	 * will be removed is the one which will be found to be equal
	 * to the given key after comparing the two with the provided
	 * compare function.
	 *
	 * The user may ensure that the removal was successful by
	 * giving the address of a boolean variable as last argument.
	 * If the removal is successful, the value 'true' will be
	 * assigned to the boolean variable, else if the removal
	 * fails, the value 'false' will be assiged to the variable.
	 *
	 * Providing such a variable is optional.
	 */
	void remove(void *key, int (*compare)(void *, void *),
		bool *removalWasSuccessful = NULL);

	/* Searches the given key in the tree using the given 'compare'
	 * operation and returns 'true' if the key exists, else 'false'.
	 */
	bool search(void *key, int (*compare)(void *, void *));

	/* Searches the given key in the tree using the given 'compare'
	 * operation. If the key exists 'true' is returned and also:
	 *
	 * - If a no-null 'retrievedItem' address has been given,
	 *   it will be assigned the "item" of the node with the
	 *   identical key that was found by the searching method.
	 *
	 * - If a no-null 'retrievedKey' address has been given,
	 *   it will be assigned the "key" of the node with the
	 *   identical key that was found by the searching method.
	 *
	 * If the given key does not exist in the tree, 'false' is
	 * returned and if any of the two optional addresses is a
	 * no-null pointer, 'NULL' is assigned to the content pointed
	 * by it.
	 */
	bool searchAndRetrieve(void *key, int (*compare)(void *, void *),
		void **retrievedItem = NULL, void **retrievedKey = NULL);

	/* Prints the items of the tree with the given 'visit' operation in the given
	 * traversal order. If a no-null 'contextBetweenItems' function is given, it
	 * will be invoked to print the desired context between the items. Similarly,
	 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to
	 * print the desired context between the nodes of the tree. When the order of
	 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will
	 * be used to print context between the nodes of different levels if it is a
	 * no-null value. For 'Inorder' traversal the context between nodes cannot be
	 * used, because a part of a node is printed, then a part of another node is
	 * printed etc, and not the whole node, which means there are not any nodes to
	 * seperate with context. The contents of the nodes are blended in the result.
	 */
	void printItems(OrderOfTraversal traversalOrder, void (*visit)(void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* Prints the keys of the tree with the given 'visit' operation in the given
	 * traversal order. If a no-null 'contextBetweenItems' function is given, it
	 * will be invoked to print the desired context between the items. Similarly,
	 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to
	 * print the desired context between the nodes of the tree. When the order of
	 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will
	 * be used to print context between the nodes of different levels if it is a
	 * no-null value. For 'Inorder' traversal the context between nodes cannot be
	 * used, because a part of a node is printed, then a part of another node is
	 * printed etc, and not the whole node, which means there are not any nodes to
	 * seperate with context. The contents of the nodes are blended in the result.
	 */
	void printKeys(OrderOfTraversal traversalOrder, void (*visit)(void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* Prints both items and keys with the given 'visit' operation in the given
	 * traversal order. If a no-null 'contextBetweenItems' function is given, it
	 * will be invoked to print the desired context between the items. Similarly,
	 * if a no-null 'contextBetweenNodes' function is given, it will be invoked to
	 * print the desired context between the nodes of the tree. When the order of
	 * traversal is 'Levelorder', the given operation 'contextBetweenLevels' will
	 * be used to print context between the nodes of different levels if it is a
	 * no-null value. For 'Inorder' traversal the context between nodes cannot be
	 * used, because a part of a node is printed, then a part of another node is
	 * printed etc, and not the whole node, which means there are not any nodes to
	 * seperate with context. The contents of the nodes are blended in the result.
	 */
	void printBoth(OrderOfTraversal traversalOrder, void (*visit)(void *, void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* This operation should be used when the user wants to execute some
	 * specific actions for each pair of 'item' and 'key' in the tree.
	 *
	 * The user must first create a function that expects an item and a
	 * key and applies the actions the user wants on that item and key.
	 *
	 * In the 'traverse' operation, the above user function will be called
	 * for all the pairs of 'item' and 'key' of each node in the suggested
	 * traversal order (Preorder, Inorder, Postorder or Levelorder).
	 */
	void traverse(OrderOfTraversal traversalOrder, void (*actions)(void *, void *));

	/* Destroys every node from the tree */
	void destroy();

protected:

	/* The AB Tree has a root node, which is an AB Node */
	AB_Node *root;

	/* Parameter 'A' of the AB Tree */
	unsigned int A;

	/* Parameter 'B' of the AB Tree */
	unsigned int B;

	/* The number of items in the tree */
	unsigned int counter;

private:

	/* Finds the number of nodes of the subtree with root being the given node */
	unsigned int sizeRec(AB_Node *node) const;

	/* Prints recursively the items of the given node
	 * and all its children in the given traversal order
	 */
	void printItemsRec(AB_Node *node, OrderOfTraversal traversalOrder,
		void (*visit)(void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* Prints recursively the keys of the given node
	 * and all its children in the given traversal order
	 */
	void printKeysRec(AB_Node *node, OrderOfTraversal traversalOrder,
		void (*visit)(void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* Prints recursively both the items and the keys of the given
	 * node and all its children in the given traversal order
	 */
	void printBothRec(AB_Node *node, OrderOfTraversal traversalOrder,
		void (*visit)(void *, void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL,
		void (*contextBetweenLevels)() = NULL);

	/* Prints only the nodes of the given level (the root is on level 1) */
	void printLevel(AB_Node *node, unsigned int level,
		void (*visit)(void *),
		void (*contextBetweenItems)() = NULL,
		void (*contextBetweenNodes)() = NULL);

	/* Traverses recursively both the items and the keys of the given
	 * node and all its children in the given traversal order
	 */
	void traverseRec(AB_Node *node, OrderOfTraversal traversalOrder,
		void (*actions)(void *, void *));

	/* Traverses only the nodes of the given level (the root is on level 1) */
	void traverseLevel(AB_Node *node, unsigned int level,
		void (*actions)(void *, void *));

	/* Destroys the children of the given AB Node and then the AB Node itself */
	void destroyRec(AB_Node *node);

	/* Casts the 'item' to 'AB_Node *' and then deletes it with 'delete' */
	static void delete_AB_Node(void *item);
};

#endif
