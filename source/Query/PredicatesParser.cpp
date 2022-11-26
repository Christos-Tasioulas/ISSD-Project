#include <iostream>
#include <cstdlib>
#include <cstring>
#include <PredicatesParser.h>

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

/***************
 * Constructor *
 ***************/

PredicatesParser::PredicatesParser(char *predicate_string)
{
	char *predicate_string_pointer = predicate_string;

	while(*predicate_string_pointer != '.')
		predicate_string_pointer++;

	char leftArrayAsString[predicate_string_pointer - predicate_string + 1];
	memcpy(leftArrayAsString, predicate_string, predicate_string_pointer - predicate_string);
	leftArrayAsString[predicate_string_pointer - predicate_string] = 0;
	leftArray = atou(leftArrayAsString);

	predicate_string_pointer++;

	predicate_string = predicate_string_pointer;

	while((*predicate_string_pointer != '<') && (*predicate_string_pointer != '>') && (*predicate_string_pointer != '='))
		predicate_string_pointer++;

	char leftArrayColumnAsString[predicate_string_pointer - predicate_string + 1];
	memcpy(leftArrayColumnAsString, predicate_string, predicate_string_pointer - predicate_string);
	leftArrayColumnAsString[predicate_string_pointer - predicate_string] = 0;
	leftArrayColumn = atou(leftArrayColumnAsString);

	filterOperator = *predicate_string_pointer;
	predicate_string_pointer++;

	bool dotWasFound = false;
	predicate_string = predicate_string_pointer;

	while(*predicate_string_pointer != 0)
	{
		if(*predicate_string_pointer == '.')
		{
			dotWasFound = true;
			break;
		}

		predicate_string_pointer++;
	}

	if(dotWasFound)
	{
		rvalueIsConstant = false;

		char rightArrayAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(rightArrayAsString, predicate_string, predicate_string_pointer - predicate_string);
		rightArrayAsString[predicate_string_pointer - predicate_string] = 0;
		rightArray = atou(rightArrayAsString);

		predicate_string_pointer++;

		predicate_string = predicate_string_pointer;

		while(*predicate_string_pointer != 0)
			predicate_string_pointer++;

		char rightArrayColumnAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(rightArrayColumnAsString, predicate_string, predicate_string_pointer - predicate_string);
		rightArrayColumnAsString[predicate_string_pointer - predicate_string] = 0;
		rightArrayColumn = atou(rightArrayColumnAsString);
	}

	else
	{
		rvalueIsConstant = true;

		char filterValueAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(filterValueAsString, predicate_string, predicate_string_pointer - predicate_string);
		filterValueAsString[predicate_string_pointer - predicate_string] = 0;
		filterValue = atou(filterValueAsString);
	}
}

/**************
 * Destructor *
 **************/

PredicatesParser::~PredicatesParser() {}

/************
 * Printing *
 ************/

void PredicatesParser::print() const
{
	std::cout << "{" << leftArray << "." << leftArrayColumn << filterOperator;

	if(rvalueIsConstant)
		std::cout << filterValue << "}";

	else
		std::cout << rightArray << "." << rightArrayColumn << "}";
}