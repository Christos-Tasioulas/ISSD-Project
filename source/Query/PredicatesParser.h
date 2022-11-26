#ifndef _PREDICATES_PARSER_H_
#define _PREDICATES_PARSER_H_

class PredicatesParser {

private:

	unsigned int leftArray;
	unsigned int leftArrayColumn;
	unsigned int rightArray;
	unsigned int rightArrayColumn;
	unsigned int filterValue;
	char filterOperator;
	bool rvalueIsConstant;

public:

	PredicatesParser(char *predicate_string);
	~PredicatesParser();


	void print() const;

};

#endif
