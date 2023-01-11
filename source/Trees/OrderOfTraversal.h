#ifndef _ORDER_OF_TRAVERSAL_H_
#define _ORDER_OF_TRAVERSAL_H_

/* An enumeration with all the ways to traverse a Tree */

enum OrderOfTraversal {

	/* For Binary Trees:
	 * ^^^^^^^^^^^^^^^^
	 * First the node is visited, then its
	 * left subtree, then its right subtree
	 *
	 * For Non-Binary Trees:
	 * ^^^^^^^^^^^^^^^^^^^^
	 * First the node is visited, then
	 * its children from left to right
	 */
	Preorder,

	/* For Binary Trees:
	 * ^^^^^^^^^^^^^^^^
	 * First the left subtree of the node is visited,
	 * then the node itself, then its right subtree
	 *
	 * For Non-Binary Trees:
	 * ^^^^^^^^^^^^^^^^^^^^
	 * The first child of the node is visited first, then the
	 * first item of the node is visited. Then the second
	 * child of the node is visited and then the second item
	 * of the node is visited and the same pattern is applied
	 * until all children and items have been visited
	 */
	Inorder,

	/* For Binary Trees:
	 * ^^^^^^^^^^^^^^^^
	 * First the left subtree of the node is visited,
	 * then its right subtree, then the node itself
	 *
	 * For Non-Binary Trees:
	 * ^^^^^^^^^^^^^^^^^^^^
	 * First the children are visited from left to
	 * right, then the node itself is visited
	 */
	Postorder,

	/* For Binary Trees & Non-Binary Trees:
	 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	 * The root, whose level of height is 1, is visited first, then
	 * all nodes with level of height equal to 2 are visited (those
	 * are the children of the root), then all nodes with level of
	 * height equal to 3 are visited (those are the children of the
	 * root's children) and the same pattern applies until the nodes
	 * of the last level of height are visited (those are the leaves)
	 */
	Levelorder

};

#endif
