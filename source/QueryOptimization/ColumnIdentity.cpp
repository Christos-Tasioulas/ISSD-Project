#include <iostream>
#include "ColumnIdentity.h"

/***************
 * Constructor *
 ***************/

ColumnIdentity::ColumnIdentity(unsigned int tableName, unsigned int tableColumn,
    unsigned int priorityInQuery, ColumnStatistics *columnStats, Query *query)
{
    /* First we assign the four given attributes to the fields of the class */
    this->tableName = tableName;
    this->tableColumn = tableColumn;
    this->priorityInQuery = priorityInQuery;
    this->columnStats = columnStats;

    // List *predicatesOfQuery = query->getPredicates();
    // Listnode *currentNode = predicatesOfQuery->getHead();

    // while(currentNode != NULL)
    // {
    //     PredicatesParser *currentPredicate = (PredicatesParser *) currentNode->getItem();

    //     unsigned int leftArray = currentPredicate->getLeftArray();
    //     unsigned int leftArrayColumn = currentPredicate->getLeftArrayColumn();
    //     unsigned int rightArray = currentPredicate->getRightArray();
    //     unsigned int rightArrayColumn = currentPredicate->getRightArrayColumn();

    //     if((leftArray == tableName) && (leftArrayColumn == tableColumn))
    //     {

    //     }

    //     currentNode = currentNode->getNext();
    // }
}

/**************
 * Destructor *
 **************/

ColumnIdentity::~ColumnIdentity() {}

/********************************************
 * Prints the contents of a column identity *
 ********************************************/

void ColumnIdentity::print() const
{

}
