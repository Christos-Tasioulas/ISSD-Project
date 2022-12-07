#ifndef _INTERMEDIATE_REPRESENTATION_H_
#define _INTERMEDIATE_REPRESENTATION_H_

#include "IntermediateArray.h"

class IntermediateRepresentation {

private:

	/* A list of all intermediate arrays created for the query */
	List *intermediateArrays;

	/* A pointer to the data of all the input tables */
	List *tables;

	/* The additional parameters of the Partitioned Hash Join Algorithm */
	PartitionedHashJoinInput *joinParameters;

	/* Searches if the given relation exists in any of the
	 * intermediate arrays of the intermediate representation.
	 * If it exists, that intermediate array is returned
	 */
	IntermediateArray *relationExists(unsigned int relationName,
		unsigned int relationPriority) const;

	/* Returns the position in the list of the given intermediate array */
	unsigned int posOfIntermediateArray(IntermediateArray *array) const;

	/* Delets an intermediate array - used to traverse the list of intermediate arrays */
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
	void executeJoin(
		unsigned int leftRel,
		unsigned int leftRelColumn,
		unsigned int leftRelPriority,
		unsigned int rightRel,
		unsigned int rightRelColumn,
		unsigned int rightRelPriority);

	/* Applies the given filter to the given relation */
	void executeFilter(
		unsigned int relName,
		unsigned int relColumn,
		unsigned int relPriority,
		unsigned int filterValue,
		char filterOperator);

	/* Prints the sum of the items in the reserved row IDs
	 * of the given relation in the requested column
	 */
	void produceSum(
		unsigned int relName,
		unsigned int relColumn,
		unsigned int relPriority);

};

#endif
