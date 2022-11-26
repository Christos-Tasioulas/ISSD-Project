#ifndef _QUERY_H_
#define _QUERY_H_

#include "List.h"
#include "PredicatesParser.h"
#include "ProjectionsParser.h"

/* A structure that stores the information of a query
 *
 * Every query has a number of relations that take part to
 * it, a number of predicates and a number of projections
 */
class Query {

private:

	/* The list of relations taking part in the query */
	List *relations;

	/* The list of predicates of the query */
	List *predicates;

	/* The list of projections of the query */
	List *projections;

public:

	/* Constructor & Destructor */
	Query(char *initialization_string);
	~Query();

	/* Getter - Returns the relations taking part in the query */
	List *getRelations() const;

	/* Getter - Returns the list of predicates of the query */
	List *getPredicates() const;

	/* Getter - Returns the list of projections of the query */
	List *getProjections() const;

	/* Prints a query (its relations, predicates and projections) */
	void print() const;

};

#endif