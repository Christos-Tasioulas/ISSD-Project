#ifndef _COLUMN_IDENTITY_H_
#define _COLUMN_IDENTITY_H_

#include "List.h"
#include "Query.h"
#include "ColumnStatistics.h"

class ColumnIdentity {

private:

    /* A unique identifier of the column */
    unsigned int id;

    /* The name of the table the column belongs to with query notation */
    unsigned int tableName;

    /* The name of the column itself */
    unsigned int tableColumn;

    /* The real name of the table where the column belongs to */
    unsigned int realTableName;

    /* The initial statistics of the column (or after having been filtered) */
    ColumnStatistics *columnStats;

    /* A list of other columns that need to be joined with this one */
    List *neighbors;

    /* A list of predicates that are assossiacted with each neighbor */
    List *neighborPredicates;

    /* A variable that produces an ID for each column identity */
    static unsigned int idCounter;

    /* Prints a neighbor (which is a column identity) */
    static void printNeighbor(void *item);

    /* Prints a predicate */
    static void printPredicate(void *item);

    /* Prints a space (this is the context that
     * will be printed between column identities)
     */
    static void contextBetweenNeighbors();

    /* Prints a '&' (this is the context
     * that will be printed between predicates)
     */
    static void contextBetweenPredicates();

public:

    /* Constructor */
    ColumnIdentity(
        unsigned int tableName,
        unsigned int tableColumn,
        unsigned int realTableName,
        ColumnStatistics *columnStats
    );

    /* Constructor for creating "dummy" column identities
     * (dummy columns can be used for a quick search)
     */
    ColumnIdentity(
        unsigned int tableName,
        unsigned int tableColumn
    );

    /* Destructor */
    ~ColumnIdentity();

    /* Getter - Returns the ID of the column identity */
    unsigned int getId() const;

    /* Inserts a new neighbor along with the related
     * join predicate to the lists of the class
     */
    void insertNeighbor(ColumnIdentity *neighbor,
        PredicatesParser *relatedPredicate);

    /* Prints the contents of a column identity */
    void print() const;

    /* Compares the column identity to another column identities */
    int compare(ColumnIdentity *other) const;

};

#endif
