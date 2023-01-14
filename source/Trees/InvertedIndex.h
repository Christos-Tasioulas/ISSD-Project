#ifndef _INVERTED_INDEX_H_
#define _INVERTED_INDEX_H_

#include "RedBlackTree.h"

class InvertedIndex : public RedBlackTree {

public:

/* Constructor & Destructor */
    InvertedIndex();
    ~InvertedIndex();

/* Getters */
    unsigned int getItemsCount() const;

/* Insert a new element in the inverted index */
    void insert(void *item, void *index_key, void *item_key,
        int (*compareIndexKeys)(void *, void *), int (*compareItemKeys)(void *, void *));

/* Remove an item with a specific 'index key' and 'item key' */
    void remove(void *index_key, void *item_key, int (*compareIndexKeys)(void *, void *),
        int (*compareItemKeys)(void *, void *));

/* Retrieve an 'index key' from the inverted index */
    void *getIndexKey(void *index_key, int (*compareIndexKeys)(void *, void *));

/* Search an 'index key' in the inverted index */
    RedBlackTree *searchIndexKey(void *index_key, int (*compareIndexKeys)(void *, void *));

/* Search an 'item key' in the inverted index */
    void *searchItemKey(void *index_key, void *item_key, int (*compareIndexKeys)(void *, void *),
        int (*compareItemKeys)(void *, void *));

/* Print the inverted index */
    void print(void (*visitIndexKeys)(void *), void (*visitItemAndItemKeys)(void *, void *),
        OrderOfTraversal traversal_order = Inorder);

private:

/* Holds the total number of entries stored in the inverted index
 * (the sum of items stored in each inner Red Black Tree)
 */
    unsigned int items_count;

/* Frees the allocated memory for every 'RedBlackTree' item in the tree */
    void deleteRedBlackTreeItem(Treenode *node);

};

#endif
