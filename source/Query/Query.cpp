#include <iostream>
#include <cstdlib>
#include <cstring>
#include "Query.h"

/*****************************************
 * Converts a string to unsigned integer * 
 *****************************************/

static unsigned int atou(char *arithmetic_string, unsigned int *read_bytes = NULL)
{
	/* We convert the given argument to unsigned long integer
	 * by using the function 'strtoul'. Then we will cast that
	 * unsigned long integer to unsigned int and return it.
	 */
	char *parsingStopPoint;

	unsigned long unsignedLongResult = strtoul(arithmetic_string,
		&parsingStopPoint, 10);

    /* We store the amount of bytes that were successfully parsed
     * in case the user has given a no-null unsigned int address
     */
    if(read_bytes != NULL)
    {
        /* The amount of bytes parsed successfully is the address
         * where 'strtoul' stopped parsing minus the address where
         * 'strtou' started parsing.
         */
        (*read_bytes) = parsingStopPoint - arithmetic_string;
    }

	/* Finally, if no error has occured, we cast the unsigned
	 * long result to unsigned int and we return it.
	 */
	return (unsigned int) unsignedLongResult;
}

/********************************************
 * Operations needed to process the list of *
 *                relations                 *
 *                ^^^^^^^^^                 *
 ********************************************/

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

/********************************************
 * Operations needed to process the list of *
 *                predicates                *
 *                ^^^^^^^^^^                *
 ********************************************/

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

/********************************************
 * Operations needed to process the list of *
 *               projections                *
 *               ^^^^^^^^^^^                *
 ********************************************/

static void printProjectionsParser(void *item)
{
	ProjectionsParser *my_parser = (ProjectionsParser *) item;
	my_parser->print();
}

static void contextBetweenProjectionsParsers()
{
	std::cout << " ";
}

static void deleteProjectionsParser(void *item)
{
	ProjectionsParser *my_parser = (ProjectionsParser *) item;
	delete my_parser;
}

/***************
 * Constructor *
 ***************/

Query::Query(char *initialization_string)
{
	/* We initialize the three lists */
	relations = new List();
	predicates = new List();
	projections = new List();

	/* Auxiliary pointers that will help us in the parsing of the query */
	char *relationsAsString, *predicatesAsString, *projectionsAsString;

	/* Currently we have the query in string form (constructor argument)
	 *
	 * That string consists of three main parts, which are the three
	 * fields of our structure - Relations, Predicates, Projections.
	 *
	 * We split the string of the query to these three parts and save
	 * the start of each substring in the auxiliary pointers we created.
	 */
	relationsAsString = strtok(initialization_string, "|");
	predicatesAsString = strtok(NULL, "|");
	projectionsAsString = strtok(NULL, "|");

	/* We will start processing the string for the relations
	 *                                             ^^^^^^^^^
	 * that take part in the query. We split the string in spaces.
	 *
	 * Here we retrieve the first relation.
	 */
	char *relationToken = strtok(relationsAsString, " ");

	/* As long as we have not processed all the
	 * relations, we do the following 'while' loop
	 */
	while(relationToken != NULL)
	{
		/* We insert the current relation in the list of relations */
		relations->insertLast(new unsigned int(atou(relationToken)));

		/* We proceed to the next relation */
		relationToken = strtok(NULL, " ");
	}

	/* Now we will start processing the string for the predicates
	 *                                                 ^^^^^^^^^^
	 * In the string form the predicates are distincted by '&'.
	 *
	 * We split the string in the '&'. We retrieve the first token.
	 */
	char *predicateToken = strtok(predicatesAsString, "&");

	/* As long as we have not processed all the
	 * predicates, we do the following 'while' loop
	 */
	while(predicateToken != NULL)
	{
		/* We create an object that will handle the current predicate */
		PredicatesParser *parser_of_current_predicate =
			new PredicatesParser(predicateToken);

		/* We insert the new object in the 'predicates' list */
		predicates->insertLast(parser_of_current_predicate);

		/* We proceed to the next predicate */
		predicateToken = strtok(NULL, "&");
	}

	/* Now we will start processing the string for the projections
	 *                                                 ^^^^^^^^^^^
	 * In the string form the projections are distincted by spaces.
	 *
	 * We split the string in spaces. We retrieve the first token.
	 */
	char *projectionToken = strtok(projectionsAsString, " ");

	/* As long as we have not processed all the
	 * projections, we do the following 'while' loop
	 */
	while(projectionToken != NULL)
	{
		/* We create an object that will handle the current projection */
		ProjectionsParser *parser_of_current_projection =
			new ProjectionsParser(projectionToken);

		/* We insert the new object in the 'projections' list */
		projections->insertLast(parser_of_current_projection);

		/* We proceed to the next projection */
		projectionToken = strtok(NULL, " ");
	}
}

/**************
 * Destructor *
 **************/

Query::~Query()
{
	/* We free the allocated memory for each relation */
	relations->traverseFromHead(deleteUnsignedInteger);
	delete relations;

	/* We free the allocated memory for each predicate */
	predicates->traverseFromHead(deletePredicatesParser);
	delete predicates;

	/* We free the allocated memory for each projection */
	projections->traverseFromHead(deleteProjectionsParser);
	delete projections;
}

/***********************************************************
 * Getter - Returns the relations taking part in the query *
 ***********************************************************/

List *Query::getRelations() const
{
	return relations;
}

/********************************************************
 * Getter - Returns the list of predicates of the query *
 ********************************************************/

List *Query::getPredicates() const
{
	return predicates;
}

/*********************************************************
 * Getter - Returns the list of projections of the query *
 *********************************************************/

List *Query::getProjections() const
{
	return projections;
}

/**********************************************
 * Returns the relation in the given position *
 **********************************************/

unsigned int Query::getRelationInPos(unsigned int pos) const
{
	return *((unsigned int *) relations->getItemInPos(pos + 1));
}

/**************************************************************
 * Prints a query (its relations, predicates and projections) *
 **************************************************************/

void Query::print() const
{
	/* We print the relations taking part in the query in one line */
	std::cout << "Relations: ";
	relations->printFromHead(printUnsignedInt, contextBetweenUnsignedIntegers);

	/* In the next line we print the predicates of the query */
	std::cout << "\nPredicates: ";
	predicates->printFromHead(printPredicatesParser, contextBetweenPredicatesParsers);

	/* Finally, in the next line we print the projections of the query */
	std::cout << "\nProjections: ";
	projections->printFromHead(printProjectionsParser, contextBetweenProjectionsParsers);

	/* We print a new line to escape the line of projections */
	std::cout << std::endl;
}
