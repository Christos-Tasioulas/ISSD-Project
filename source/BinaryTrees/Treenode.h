#ifndef _TREENODE_H_
#define _TREENODE_H_

/* There are two types of rotations: The left and
 * the right rotation. We include these two types
 * in the 'Rotation' enumeration
 */

enum Rotation {Left, Right};

class Treenode {

private:

/* A treenode stores an address
 * and has pointers to a left,
 * a right and a parent treenode
 */
	void *item;
	Treenode *left;
	Treenode *right;
	Treenode *parent;

public:

/* Constructor & Destructor */
	Treenode(void *item);
	~Treenode();

/* Getters */
	void *getItem() const;
	Treenode *getLeft() const;
	Treenode *getRight() const;
	Treenode *getParent() const;

/* Setters */
	void setItem(void *item);
	void setLeft(Treenode *left);
	void setRight(Treenode *right);
	void setParent(Treenode *parent);

/* Printing */
	void print(void (*visit)(void *)) const;

/* Returns the uncle of a node (the node which
 * has the same parent with the node's parent)
 */
	Treenode *getUncle() const;

/* Returns the grandparent of a node
 * (the parent of the node's parent)
 */
	Treenode *getGrandparent() const;

/* Returns the sibling of a node
 * (the node that has the same parent)
 */
	Treenode *getSibling() const;

/* Returns the close nephew of node */
	Treenode *getCloseNephew() const;

/* Returns the distant nephew of node */
	Treenode *getDistantNephew() const;

/* Rotates a given node by a given rotation type (left or right).
 * If the node has no parent, we refer to that node as a 'root'
 * node. In that case the given node won't be a root node after
 * the rotation, but one of its children will become a root node.
 * If the user has a pointer that always points the root node of
 * a set of treenodes, the address of that pointer must be given
 * in the second argument and in case the root changes it will be
 * updated appropriatelly.
 */
	void rotation(Rotation rotation_type, Treenode **root);

/* Swaps the items of two tree nodes */
	void swap(Treenode *node);

};

#endif
