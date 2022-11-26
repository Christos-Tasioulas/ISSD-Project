#ifndef _PREDICATES_PARSER_H_
#define _PREDICATES_PARSER_H_

/* A structure used to parse the predicates of a query
 * and store the implied filters in seperate variables
 *
 * Each predicate has a left part, an operator (<,>,=)
 * and a right part and implies the mathematical
 * relation that the data of these parts should have
 * to satisfy the query and be chosen for the output.
 */
class PredicatesParser {

private:

	/* The array of the left part */
	unsigned int leftArray;

	/* The column of the array of the left part */
	unsigned int leftArrayColumn;

	/* The array of the right part */
	unsigned int rightArray;

	/* The column of the array of the right part */
	unsigned int rightArrayColumn;

	/* The right part may not have an array, but
	 * a constant unsigned integer value instead
	 *
	 * This variable describes that value
	 */
	unsigned int filterValue;

	/* The operator of the predicate ('<', '>' or '=') */
	char filterOperator;

	/* A boolean variable that determines whether the right part
	 * is an array column or a constant integer value. If the
	 * right part is just an integer, this variable will have
	 * the value 'true'. On the other hand, if the right part is
	 * an array column, this variable will have the value 'false'
	 */
	bool rvalueIsConstant;

public:

	/* Constructor & Destructor */
	PredicatesParser(char *predicate_string);
	~PredicatesParser();

	/* Prints the predicate */
	void print() const;

};

#endif
