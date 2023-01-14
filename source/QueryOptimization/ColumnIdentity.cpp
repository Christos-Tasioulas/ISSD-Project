#include <iostream>
#include "ColumnIdentity.h"

/***********************************************************
 * A variable that produces an ID for each column identity *
 ***********************************************************/

unsigned int ColumnIdentity::idCounter = 0;

/**************************************************
 * Prints a neighbor (which is a column identity) *
 **************************************************/

void ColumnIdentity::printNeighbor(void *item)
{
    ColumnIdentity *colId = (ColumnIdentity *) item;
    std::cout << colId->getId();
}

/**********************
 * Prints a predicate *
 **********************/

void ColumnIdentity::printPredicate(void *item)
{
    PredicatesParser *pred = (PredicatesParser *) item;
    pred->print();
}

/**********************************************
 *  Prints a space (this is the context that  *
 * will be printed between column identities) *
 **********************************************/

void ColumnIdentity::contextBetweenNeighbors()
{
    std::cout << " ";
}

/******************************************
 * Prints a '&' (this is the context that *
 *   will be printed between predicates)  *
 ******************************************/

void ColumnIdentity::contextBetweenPredicates()
{
    std::cout << "&";
}

/***************
 * Constructor *
 ***************/

ColumnIdentity::ColumnIdentity(
    unsigned int tableName,
    unsigned int tableColumn,
    unsigned int realTableName,
    ColumnStatistics *columnStats)
{
    /* First we assign the given attributes to the fields of the class */
    this->tableName = tableName;
    this->tableColumn = tableColumn;
    this->realTableName = realTableName;
    this->columnStats = columnStats;

    /* We produce a new ID and assign it to the column identity */
    idCounter++;
    id = idCounter;

    /* We initialize the list of neighbors and neighbor predicates */
    neighbors = new List();
    neighborPredicates = new List();
}

/* Constructor for creating "dummy" column identities
 * (dummy columns can be used for a quick search)
 */
ColumnIdentity::ColumnIdentity(
    unsigned int tableName,
    unsigned int tableColumn)
{
    /* The dummy columns do not consume ID labels.
     *
     * They are only used temporarily and then the are deleted.
     */
    this->tableName = tableName;
    this->tableColumn = tableColumn;

    /* We need to initialize the list of neighbors
     * and neighbor predicates because the destructor
     * will always attempt to delete those no matter
     * whether the column identity is normal or dummy
     */
    neighbors = new List();
    neighborPredicates = new List();
}

/**************
 * Destructor *
 **************/

ColumnIdentity::~ColumnIdentity()
{
    delete neighbors;
    delete neighborPredicates;
}

/**************************************************
 * Getter - Returns the ID of the column identity *
 **************************************************/

unsigned int ColumnIdentity::getId() const
{
    return id;
}

/***********************************************************
 * Getter - Returns the table alias of the column identity *
 ***********************************************************/

unsigned int ColumnIdentity::getTableName() const
{
    return tableName;
}

/************************************************************
 * Getter - Returns the table column of the column identity *
 ************************************************************/

unsigned int ColumnIdentity::getTableColumn() const
{
    return tableColumn;
}

/***************************************************************
 * Getter - Returns the real table name of the column identity *
 ***************************************************************/

unsigned int ColumnIdentity::getRealTableName() const
{
    return realTableName;
}

/*********************************************************
 * Getter - Returns the neighbors of the column identity *
 *********************************************************/

List *ColumnIdentity::getNeighbors() const
{
    return neighbors;
}

/********************************************
 * Getter - Returns the column's statistics *
 ********************************************/

ColumnStatistics *ColumnIdentity::getColumnStats() const
{
    return columnStats;
}

/*****************************************************
 * Setter - Updates the stats of the column identity *
 *****************************************************/

void ColumnIdentity::setColumnStats(ColumnStatistics *newStats)
{
    columnStats = newStats;
}

/*************************************************
 * Inserts a new neighbor along with the related *
 *   join predicate to the lists of the class    *
 *************************************************/

void ColumnIdentity::insertNeighbor(ColumnIdentity *neighbor,
    PredicatesParser *relatedPredicate)
{
    neighbors->insertLast(neighbor);
    neighborPredicates->insertLast(relatedPredicate);
}

/********************************************
 * Prints the contents of a column identity *
 ********************************************/

void ColumnIdentity::print() const
{
    std::cout << "ColumnIdentity{"
        << "id=" << id << ","
        << "tableName=" << tableName << ","
        << "tableColumn=" << tableColumn << ","
        << "realTableName=" << realTableName << ","
        << "columnStats=";

    columnStats->print();

    std::cout << ",neighbors={";
    neighbors->printFromHead(printNeighbor, contextBetweenNeighbors);

    std::cout << "},neighborPredicates={";
    neighborPredicates->printFromHead(printPredicate, contextBetweenPredicates);
    std::cout << "}}";
}

/*************************************************************
 * Compares the column identity to another column identities *
 *************************************************************/

int ColumnIdentity::compare(ColumnIdentity *other) const
{
    /* Case the columns belong to the same table */
    if(tableName == other->tableName)
    {
        /* If the have the same column number too, they are equal */
        if(tableColumn == other->tableColumn)
            return 0;

        /* Else if this column is smaller than the other, we return -1 */
        if(tableColumn < other->tableColumn)
            return -1;

        /* Else if this column is greater than the other, we return 1 */
        return 1;
    }

    /* Case this table is smaller than the other */
    if(tableName < other->tableName)
        return -1;

    /* Case this table is greater than the other */
    return 1;
}
