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

};

#endif
