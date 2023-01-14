#include <iostream>
#include "InvertedIndex.h"

/*****************************************
 * User & Implementation visit functions *
 *****************************************/

void (*userVisitIndexKeys)(void *);
void (*userVisitItemAndItemKeys)(void *, void *);
void impVisitItemAndItemKeys(void *, void *);

/****************************
 * Constructor & Destructor *
 ****************************/

InvertedIndex::InvertedIndex()
{
    RedBlackTree();
    items_count = 0;
}

InvertedIndex::~InvertedIndex()
{
    deleteRedBlackTreeItem(root);
}

/************************************************************
 * Deletes the allocated 'RedBlackTree' item from all nodes *
 *                   (used by destructor)                   *
 ************************************************************/

void InvertedIndex::deleteRedBlackTreeItem(Treenode *node)
{
    /* In post-order traversal we delete recursively the
     * 'RedBlackTree' item of the children of the node.
     * Then, we delete the 'RedBlackTree' item of the node
     */

    if(node != NULL)
    {
        deleteRedBlackTreeItem(node->getLeft());
        deleteRedBlackTreeItem(node->getRight());

        BSTObject *bst_obj = (BSTObject *) node->getItem();
        delete (RedBlackTree *) bst_obj->getItem();
    }
}

/***********
 * Getters *
 ***********/

unsigned int InvertedIndex::getItemsCount() const
{
    return items_count;
}

/**********************************************
 * Insert a new element in the inverted index *
 **********************************************/

void InvertedIndex::insert(void *item, void *index_key, void *item_key,
    int (*compareIndexKeys)(void *, void *), int (*compareItemKeys)(void *, void *))
{
    /* First, we search for the tree with the desired index key */

    RedBlackTree *searchedTree = (RedBlackTree *) searchItem(index_key, compareIndexKeys);

    /* If the desired index key does not exist, we insert a new node
     * in the inverted index and we add the new item with its item key
     * in a new Red Black Tree stored in the new node
     */

    if(searchedTree == NULL)
    {
        RedBlackTree *new_rbt = new RedBlackTree();
        RedBlackTree::insert(new_rbt, index_key, compareIndexKeys);
        new_rbt->insert(item, item_key, compareItemKeys);
        items_count++;

        return;
    }

    /* Else, if the index key exists, we simply add the new item
     * with its item key in the Red Black Tree that is stored in
     * the node of that index key
     */

    searchedTree->insert(item, item_key, compareItemKeys);
    items_count++;
}

/*************************************************************
 * Remove an item with a specific 'index key' and 'item key' *
 *************************************************************/

void InvertedIndex::remove(void *index_key, void *item_key,
    int (*compareIndexKeys)(void *, void *), int (*compareItemKeys)(void *, void *))
{
    /* First, we search for the tree with the desired index key */

    RedBlackTree *searchedTree = (RedBlackTree *) searchItem(index_key, compareIndexKeys);

    /* If the desired index key does not
     * exist, we return immediatelly
     */

    if(searchedTree == NULL)
        return;

    /* If the index key exists, we remove the desired node
     * (the one with the desired 'item key') from the Red
     * Black Tree of the node with the index key
     */

    searchedTree->remove(item_key, compareItemKeys);

    /* If that was the only node of the inner tree, then
     * that tree has now zero elements, so we delete it
     * and we also delete the inverted index's node of
     * that index key, because there are no more items
     * with that index key
     */

    if(searchedTree->getCounter() == 0)
    {
        RedBlackTree::remove(index_key, compareIndexKeys);
        delete searchedTree;
    }

    items_count--;
}
/***************************************************
 * Retrieve an 'index key' from the inverted index *
 ***************************************************/

void *InvertedIndex::getIndexKey(void *index_key,
    int (*compareIndexKeys)(void *, void *))
{
    return searchKey(index_key, compareIndexKeys);
}

/***********************************************
 * Search an 'index key' in the inverted index *
 ***********************************************/

RedBlackTree *InvertedIndex::searchIndexKey(void *index_key,
    int (*compareIndexKeys)(void *, void *))
{
    return (RedBlackTree *) searchItem(index_key, compareIndexKeys);
}

/**********************************************
 * Search an 'item_key' in the inverted index *
 **********************************************/

void *InvertedIndex::searchItemKey(void *index_key, void *item_key,
    int (*compareIndexKeys)(void *, void *), int (*compareItemKeys)(void *, void *))
{
    /* We search for the tree with the desired index key */

    RedBlackTree *searchedTree = (RedBlackTree *) searchItem(index_key, compareIndexKeys);

    /* If the index does not exists, we return 'NULL' */

    if(searchedTree == NULL)
        return NULL;

    /* Else we search the node with the desired item key
     * in the Red Black Tree of the node with the given index key
     */

    return searchedTree->searchItem(item_key, compareItemKeys);
}

/****************************
 * Print the inverted index *
 ****************************/

void InvertedIndex::print(void (*visitIndexKeys)(void *),
    void (*visitItemAndItemKeys)(void *, void *), OrderOfTraversal traversal_order)
{
    /* We assign the user printing functions to the function pointers
     * of this implementation and we use the visit operation of the
     * implementation to print the inverted index
     * ^^^^^^^^^^^^^^
     */

    userVisitIndexKeys = visitIndexKeys;
    userVisitItemAndItemKeys = visitItemAndItemKeys;

    RedBlackTree::printBoth(traversal_order, impVisitItemAndItemKeys);
}

/***************************************************************
 * Implementation of the visit type function used to print the *
 *         Red Black Tree and the Index Key of a node          *
 ***************************************************************/

void impVisitItemAndItemKeys(void *rbt_item, void *index_key)
{
    /* The visit operation of the implementation uses the user
     * functions to print the unknown data (index key, item key
     * and item) and the 'printBoth' operation to print the
     * inner Red Black Tree of each node in inorder traversal
     */

    userVisitIndexKeys(index_key);

    RedBlackTree *rbt = (RedBlackTree *) rbt_item;
    rbt->printBoth(Inorder, userVisitItemAndItemKeys);
}
