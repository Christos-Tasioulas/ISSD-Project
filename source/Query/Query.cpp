#include <iostream>
#include <cstdlib>
#include <cstring>
#include "Query.h"

/*****************************************
 * Converts a string to unsigned integer * 
 *****************************************/

static unsigned int atou(char *arithmeticString)
{
	/* We convert the given argument to unsigned long integer
	 * by using the function 'strtoul'. Then we will cast that
	 * unsigned long integer to unsigned int and return it.
	 */
	char *conversionErrorMessage;

	unsigned long unsignedLongResult = strtoul(arithmeticString,
		&conversionErrorMessage, 10);

	/* Finally, if no error has occured, we cast the unsigned
	 * long result to unsigned int and we return it.
	 */
	return (unsigned int) unsignedLongResult;
}





static void printUnsignedInt(void *item)
{
	unsigned int my_uint = *((unsigned int *) item);
	std::cout << my_uint;
}

static void contextBetweenUnsignedIntegers()
{
	std::cout << " ";
}

static void deleteUnsignedInteger(void *item)
{
	unsigned int *my_uint = (unsigned int *) item;
	delete my_uint;
}




static void printPredicatesParser(void *item)
{
	PredicatesParser *my_parser = (PredicatesParser *) item;
	my_parser->print();
}

static void contextBetweenPredicatesParsers()
{
	std::cout << ", ";
}

static void deletePredicatesParser(void *item)
{
	PredicatesParser *my_parser = (PredicatesParser *) item;
	delete my_parser;
}





/***************
 * Constructor *
 ***************/

Query::Query(char *initialization_string)
{
	relations = new List();
	predicates = new List();
	projections = new List();

	char *relationsAsString, *predicatesAsString, *projectionsAsString;

	relationsAsString = strtok(initialization_string, "|");
	predicatesAsString = strtok(NULL, "|");
	projectionsAsString = strtok(NULL, "|");

	char *relationToken = strtok(relationsAsString, " ");

	while(relationToken != NULL)
	{
		relations->insertLast(new unsigned int(atou(relationToken)));
		relationToken = strtok(NULL, " ");
	}

	
	char *predicateToken = strtok(predicatesAsString, "&");

	while(predicateToken != NULL)
	{
		PredicatesParser *parser_of_current_predicate = new PredicatesParser(predicateToken);
		predicates->insertLast(parser_of_current_predicate);
		predicateToken = strtok(NULL, "&");
	}

	//char *projectionToken = strtok(projectionsAsString, " ");
}

/**************
 * Destructor *
 **************/

Query::~Query()
{
	relations->traverseFromHead(deleteUnsignedInteger);
	delete relations;

	predicates->traverseFromHead(deletePredicatesParser);
	delete predicates;


	delete projections;
}

void Query::print() const
{
	std::cout << "Relations: ";
	relations->printFromHead(printUnsignedInt, contextBetweenUnsignedIntegers);

	std::cout << "\nPredicates: ";
	predicates->printFromHead(printPredicatesParser, contextBetweenPredicatesParsers);

	std::cout << "\nProjections: ";
	std::cout << std::endl;
}