#include <iostream>
#include <cstdlib>
#include <cstring>
#include <PredicatesParser.h>

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

/***************
 * Constructor *
 ***************/

PredicatesParser::PredicatesParser(char *predicate_string)
{
	/* An auxiliary pointer that will help parsing the predicate */
	char *predicate_string_pointer = predicate_string;

	/* We reach the first dot */
	while(*predicate_string_pointer != '.')
		predicate_string_pointer++;

	/* Now we have identified the left relation taking part.
	 *
	 * We store the relation as string.
	 */
	char leftArrayAsString[predicate_string_pointer - predicate_string + 1];
	memcpy(leftArrayAsString, predicate_string, predicate_string_pointer - predicate_string);
	leftArrayAsString[predicate_string_pointer - predicate_string] = 0;

	/* We convert the string of the relation to unsigned integer */
	leftArray = atou(leftArrayAsString);

	/* We proceed to the character after the dot */
	predicate_string_pointer++;

	/* We set the pointer of the predicate to point
	 * at the first character after the dot. We
	 * do not need the previous information anymore.
	 */
	predicate_string = predicate_string_pointer;

	/* We try to reach the operator of the predicate */
	while((*predicate_string_pointer != '<')
	   && (*predicate_string_pointer != '>')
	   && (*predicate_string_pointer != '='))
	{
		predicate_string_pointer++;
	}

	/* Now we have identified the column of the array of the left side.
	 *
	 * We store the column we just identified as string.
	 */
	char leftArrayColumnAsString[predicate_string_pointer - predicate_string + 1];
	memcpy(leftArrayColumnAsString, predicate_string, predicate_string_pointer - predicate_string);
	leftArrayColumnAsString[predicate_string_pointer - predicate_string] = 0;

	/* We convert the string of the column to unsigned integer */
	leftArrayColumn = atou(leftArrayColumnAsString);

	/* The auxiliary pointer is currently pointing to the operator.
	 *
	 * We store the operator in the proper field of the class.
	 */
	filterOperator = *predicate_string_pointer;

	/* We proceed to the next character of the operator */
	predicate_string_pointer++;

	/* We set the pointer of the predicate to point
	 * at the first character after the operator. We
	 * do not need the previous information anymore.
	 */
	predicate_string = predicate_string_pointer;

	/* Here the right part starts
	 *
	 * We need to find out whether the right part is constant or array.
	 * If it is an array, it will have a dot '.' character. We start
	 * looking for the dot. If we find it, the right part is an array.
	 * Else if we do not find it, the right part is an integer constant.
	 */
	bool dotWasFound = false;

	/* We start parsing the rest of the
	* predicate string to look for the dot
	*/
	while(*predicate_string_pointer != 0)
	{
		/* Case we found the dot
		 *
		 * We set the boolean flag to 'true' and break. In
		 * this case the auxiliary pointer will be pointing
		 * to the dot after this 'while' is escaped.
		 * ^^^^^^^^^^
		 */
		if(*predicate_string_pointer == '.')
		{
			dotWasFound = true;
			break;
		}

		/* Case we did not find the dot in this loop
		 *
		 * We proceed to the next character and repeat.
		 *
		 * If we reach the end of the string and still have not
		 * found the dot, that means the right part is an integer
		 * and not an array. In this case the auxiliary pointer
		 * will be pointing to the final zero of the string after
		 *                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		 * this 'while' is escaped
		 */
		predicate_string_pointer++;
	}

	/* Now we proceed to one of the two cases below,
	 * depending on whether or not we found the dot
	 *
	 * Case the dot was found (the right part is an array column)
	 */
	if(dotWasFound)
	{
		/* We set the value of the boolean flag of the class to 'false' */
		rvalueIsConstant = false;

		/* We have identified the relation of the right part.
		 *
		 * We store the relation as string.
		 */
		char rightArrayAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(rightArrayAsString, predicate_string, predicate_string_pointer - predicate_string);
		rightArrayAsString[predicate_string_pointer - predicate_string] = 0;

		/* We convert the string of the relation to unsigned integer */
		rightArray = atou(rightArrayAsString);

		/* We set the auxiliary variable point one character after the dot */
		predicate_string_pointer++;

		/* We set the pointer of the predicate to point
	 	 * at the first character after the dot. We
	 	 * do not need the previous information anymore.
		 */
		predicate_string = predicate_string_pointer;

		/* We parse the rest of the string */
		while(*predicate_string_pointer != 0)
			predicate_string_pointer++;

		/* Now we have identified the column of the right relation.
		 *
		 * We store the column of the right relation as string.
		 */
		char rightArrayColumnAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(rightArrayColumnAsString, predicate_string, predicate_string_pointer - predicate_string);
		rightArrayColumnAsString[predicate_string_pointer - predicate_string] = 0;

		/* We convert the string of the right array column to unsigned integer */
		rightArrayColumn = atou(rightArrayColumnAsString);
	}

	/* Case the dot was not found (the right part is an integer constant) */

	else
	{
		/* We set the value of the boolean flag of the class to 'true' */
		rvalueIsConstant = true;

		/* In this case the auxiliary variable is already pointing
		 * to the final zero of the string. That means we have
		 * identified the unsigned integer constant of the right part.
		 *
		 * We store the integer constant as string
		 */
		char filterValueAsString[predicate_string_pointer - predicate_string + 1];
		memcpy(filterValueAsString, predicate_string, predicate_string_pointer - predicate_string);
		filterValueAsString[predicate_string_pointer - predicate_string] = 0;

		/* We convert the string to unsigned integer */
		filterValue = atou(filterValueAsString);
	}
}

/**************
 * Destructor *
 **************/

PredicatesParser::~PredicatesParser() {}

/***************************************************************
 * Getter - Returns the relation where the left column belongs *
 ***************************************************************/

unsigned int PredicatesParser::getLeftArray() const
{
	return leftArray;
}

/*****************************************************
 * Getter - Returns the left column of the predicate *
 *****************************************************/

unsigned int PredicatesParser::getLeftArrayColumn() const
{
	return leftArrayColumn;
}

/****************************************************************
 * Getter - Returns the relation where the right column belongs *
 ****************************************************************/

unsigned int PredicatesParser::getRightArray() const
{
	return rightArray;
}

/******************************************************
 * Getter - Returns the right column of the predicate *
 ******************************************************/

unsigned int PredicatesParser::getRightArrayColumn() const
{
	return rightArrayColumn;
}

/********************************************************************
 * Getter - Returns the constant of the right part of the predicate *
 ********************************************************************/

unsigned int PredicatesParser::getFilterValue() const
{
	return filterValue;
}

/******************************************************************
 * Getter - Returns the operator of the predicate ('<', '>', '=') *
 ******************************************************************/

char PredicatesParser::getFilterOperator() const
{
	return filterOperator;
}

/*******************************************************************
 * Returns 'true' if the right part of the predicate is a constant *
 *******************************************************************/

bool PredicatesParser::hasConstant() const
{
	return rvalueIsConstant;
}

/************************
 * Prints the predicate *
 ************************/

void PredicatesParser::print() const
{
	/* We print the left part and the operator of the predicate */
	std::cout << "{" << leftArray << "." << leftArrayColumn << filterOperator;

	/* Then we print the right part. The printing of the right
	 * part consists of two cases, depending on whether or not
	 * the right part is a constant of the column of an arary
	 *
	 * Case the right part is a constant
	 */
	if(rvalueIsConstant)
		std::cout << filterValue << "}";

	/* Case the right part is an array column */
	else
		std::cout << rightArray << "." << rightArrayColumn << "}";
}
