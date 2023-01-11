#ifndef _AB_NODE_H_
#define _AB_NODE_H_

#include "List.h"
#include "PairInt.h"

typedef PairInt KeyPosition;

class AB_Node {

private:

	/* An AB Node stores a set of items given by the user */
	List *items;

	/* Also, the AB Node has an amount of children, which are AB Nodes */
	List *children;

	/* The AB Node has a parent, unless it is the root of the tree
	 * (in the latter case the 'parent' field is set to 'NULL')
	 */
	AB_Node *parent;

	/* Transfers an item from the parent to the node and another
	 * item from the given sibling of the node to the parent
	 */
	void transfer(unsigned int nodePos, AB_Node *sibling,
		unsigned int siblingPos);

	/* Merges the node with the given sibling, making the two nodes one. After this
	 * process is complete, an AB Node will need to be deleted because it will have
	 * been rendered useless. Sometimes the node that must be deleted is "this".
	 * Because we cannot delete the value "this", we insert it in the list that is
	 * given as fourth argument. That list plays the role of a trash can. Eventually,
	 * the trash can must be visited and each node inside it must be deleted.
	 */
	void fusion(unsigned int nodePos, AB_Node *sibling,
		unsigned int siblingPos, List *trashCan);

public:

	/* Constructor & Destructor */
	AB_Node();
	~AB_Node();

	/* Getters */
	List *getItems() const;
	List *getChildren() const;
	AB_Node *getParent() const;

	/* Setters */
	void setItems(List *new_items);
	void setChildren(List *new_children);
	void setParent(AB_Node *parent);

	/* Returns the current amount of items in the node */
	unsigned int getItemsNum();

	/* Returns the current amount of children in the node */
	unsigned int getChildrenNum();

	/* Returns 'true' if the node has no children */
	bool isLeaf() const;

	/* Inserts the given item in the node before the implied position
	 * If the implied position is greater than the items in the node,
	 * the given item is stored in the rightmost position in the node
	 */
	void insertBeforePos(void *item, unsigned int pos);

	/* Rebalances the node in case it is overflown with items */
	void rebalance(unsigned int maxItemsAllowed, AB_Node **root);

	/* Removes the item in the suggested position in the node.
	 *
	 * Before deleting the item, it terminates it with the
	 * destroy function that is optionally given as second
	 * argument. If no destroy function is given, the item
	 * will just be dropped from the list of items.
	 */
	void removeItemInPos(unsigned int pos, void (*destroy)(void *) = NULL);

	/* Replenishes the node with an item that is borrowed from the
	 * parent in case the node has less than minimum allowed items
	 *
	 * The parent is refilled either with an item of a sibling of
	 * the node (transfer) or it is not refilled and a fusion
	 * takes place between the node and its left or right sibling.
	 *
	 * Then the 'refill' operation is called recursively for the
	 * parent. The root may change in the process. Therefore, the
	 * address of the root must be given as argument. Finally, a
	 * list with the role of "trash can" must be provided, where
	 * the operation will store every node that needs to be deleted
	 * in this process. Then the caller has to traverse the trash
	 * can and delete all the AB Nodes stored in it.
	 */
	void refill(unsigned int minItemsAllowed, AB_Node **root, List *trashCan);

	/* Searches for the correct position to place the given key at the subtree
	 * of this node. If the given key exists already in the tree, there is no
	 * valid position in the tree to insert it again, because the tree does not
	 * support keys with the same value existing multiple times.
	 *
	 * In case the key does not exist in the tree, a no-null 'KeyPosition' item
	 * will be returned, which consists of two fields:
	 *
	 *   1) an 'AB_Node *' value, which stores the correct node where the
	 *      key should be inserted.
	 *
	 *   2) an 'int' value, which stores the correct position in the node
	 *      stated by value (1) where the key should be inserted.
	 *
	 * To retrieve the value (1), simply use the operation 'getItem' and cast
	 * its result to the type 'AB_Node *'. To retrieve the value (2), use the
	 * operation 'getKey' (in the latter case no casting is needed).
	 *
	 * If the given key exists in the tree already, it is impossible to be
	 * re-inserted and in this case this operation returns 'NULL'.
	 */
	KeyPosition *searchCorrectPosForKey(void *key, int (*compare)(void *, void *));

	/* Searches the given key using the given compare function and returns the
	 * place in the tree where the key is stored, if it exists.
	 *
	 * In case the key exists in the tree, a no-null 'KeyPosition' item will be
	 * returned, which consists of two fields:
	 *
	 *   1) an 'AB_Node *' value, which stores the correct node where the
	 *      key is being stored.
	 *
	 *   2) an 'int' value, which stores the correct position in the node
	 *      stated by value (1) where the key is being stored.
	 *
	 * To retrieve the value (1), simply use the operation 'getItem' and cast
	 * its result to the type 'AB_Node *'. To retrieve the value (2), use the
	 * operation 'getKey' (in the latter case no casting is needed).
	 *
	 * If the given key does not exist in the tree, 'NULL' is returned.
	 */
	KeyPosition *searchKey(void *key, int (*compare)(void *, void *));

	/* Searches the given key using the given compare function and returns the
	 * identical key stored in the tree. Returns 'NULL' if it does not exist
	 */
	void *searchAndRetrieveKey(void *key, int (*compare)(void *, void *));

	/* Frees the allocated memory for the result returned
 	 * by 'searchCorrectPosForKey' or 'searchKey'
 	 */
	static void terminatePosition(KeyPosition *returnedPosition);

	/* Prints every item of the node using the given 'visit' operation and
	 * if a no-null 'context' operation is given as second argument, it is
	 * invoked to print some context between the nodes
	 */
	void printItems(void (*visit)(void *), void (*context)() = NULL) const;

	/* Returns the node that contains the Inorder Predecessor.
	 *
	 * The Inorder Predecessor Node of an item is the node that is
	 *     ^^^^^^^^^^^^^^^^^^^^^^^^               ^^^^^^^^
	 * visited exactly before that item in Inorder Traversal. The
	 *         ^^^^^^^^^^^^^^
	 * largest item of that node is called "Inorder Predecessor".
	 *         ^^^^                         ^^^^^^^^^^^^^^^^^^^
	 */
	AB_Node *inorderPredecessor(unsigned int pos) const;

	/* Returns the node that contains the Inorder Successor.
	 *
	 * The Inorder Successor Node of an item is the node that is
	 *     ^^^^^^^^^^^^^^^^^^^^^^               ^^^^^^^^
	 * visited exactly after that item in Inorder Traversal. The
	 *         ^^^^^^^^^^^^^
	 * smallest item of that node is called "Inorder Successor".
	 *          ^^^^                         ^^^^^^^^^^^^^^^^^
	 */
	AB_Node *inorderSuccessor(unsigned int pos) const;

};

#endif
