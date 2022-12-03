#ifndef _INTERMEDIATE_REPRESENTATION_H_
#define _INTERMEDIATE_REPRESENTATION_H_

#include "IntermediateArray.h"

class IntermediateRepresentation {

private:

	List *intermediateArrays;
	List *tables;
	PartitionedHashJoinInput *joinParameters;

	/* Searches if the given relation exists in any of the
	 * intermediate arrays of the intermediate representation.
	 * If it exists, that intermediate array is returned
	 */
	IntermediateArray *relationExists(unsigned int relation) const;

	unsigned int posOfIntermediateArray(IntermediateArray *array) const;

	static void deleteIntermediateArray(void *item);

public:

	/* Constructor & Destructor */
	IntermediateRepresentation(List *tables, PartitionedHashJoinInput *joinParameters);
	~IntermediateRepresentation();

	/* Getter - Returns the arrays of the intermediate representation */
	List *getIntermediateArrays() const;

	/* Getter - Returns the given tables for the intermediate representation */
	List *getTables() const;

	/* Getter - Returns the additional parameters for the 'JOIN' operation */
	PartitionedHashJoinInput *getJoinParameters() const;

	/* Executes the 'JOIN' operation between the two given relations */
	void executeJoin(unsigned int leftRel, unsigned int leftRelColumn,
		unsigned int rightRel, unsigned rightRelColumn);

	/* Applies the given filter to the given relation */
	void executeFilter(unsigned int relName, unsigned int relColumn,
		unsigned int filterValue, char filterOperator);

	/* Prints the sum of the items in the reserved row IDs
	 * of the given relation in the requested column
	 */
	void produceSum(unsigned int relName, unsigned int relColumn);

};

#endif
