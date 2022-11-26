#include <iostream>
#include <cstdlib>
#include <cstring>
#include "ProjectionsParser.h"

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

ProjectionsParser::ProjectionsParser(char *projection_string)
{
    /* An auxiliary pointer that will help parsing the projection */
	char *projection_string_pointer = projection_string;

	/* We reach the first dot */
	while(*projection_string_pointer != '.')
		projection_string_pointer++;

	/* Now we have identified the relation of the projection.
	 *
	 * We store the relation as string.
	 */
	char arrayAsString[projection_string_pointer - projection_string + 1];
	memcpy(arrayAsString, projection_string, projection_string_pointer - projection_string);
	arrayAsString[projection_string_pointer - projection_string] = 0;

	/* We convert the string of the relation to unsigned integer */
	array = atou(arrayAsString);

    /* We proceed to the next character of the dot */
    projection_string_pointer++;

    /* We set the pointer of the projection to point
	 * at the first character after the dot. We
	 * do not need the previous information anymore.
	 */
    projection_string = projection_string_pointer;

    /* We parse the rest of the string */

    while(*projection_string_pointer != 0)
        projection_string_pointer++;

    /* Now we have identified the column of the projection.
     *
     * We store the column as string.
     */
    char columnAsString[projection_string_pointer - projection_string + 1];
	memcpy(columnAsString, projection_string, projection_string_pointer - projection_string);
	columnAsString[projection_string_pointer - projection_string] = 0;

    /* We convert the string of the column to unsigned integer */
	column = atou(columnAsString);
}

/**************
 * Destructor *
 **************/

ProjectionsParser::~ProjectionsParser() {}

/************************************************
 * Getter - Returns the array of the projection *
 ************************************************/

unsigned int ProjectionsParser::getArray() const
{
    return array;
}

/*************************************************
 * Getter - Returns the column of the projection *
 *************************************************/

unsigned int ProjectionsParser::getColumn() const
{
    return column;
}

/**************************************************
 * Prints a projection (its array and its column) *
 **************************************************/

void ProjectionsParser::print() const
{
    std::cout << "{" << array << "." << column << "}";
}
