#ifndef _QUERY_H_
#define _QUERY_H_

#include "List.h"
#include "PredicatesParser.h"

class Query {

private:

	/*  */
	List *relations;

	List *predicates;

	List *projections;

public:

	/* Constructor & Destructor */
	Query(char *initialization_string);
	~Query();

	List *getRelations() const;
	List *getPredicates() const;
	List *getProjections() const;

	void print() const;

};

#endif