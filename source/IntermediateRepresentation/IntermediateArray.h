#ifndef _INTERMEDIATE_ARRAY_H_
#define _INTERMEDIATE_ARRAY_H_

#include "List.h"
#include "Table.h"
#include "PartitionedHashJoin.h"

class IntermediateArray {

private:

	/* A pointer to the tables of all relations in the system */
	List *tables;

	/* A list with all the relation names of the intermediate array */
	List *relations;

	/* A list of unsigned integer arrays, representing the row IDs of each relation */
	List *rowIdArrays;

	/* The amount of row IDs each of the arrays has */
	unsigned int rowsNum;

	/* The additional parameters for a 'JOIN' operation */
	PartitionedHashJoinInput *joinParameters;

	static void printUnsignedInteger(void *item);

	static void printUnsignedIntegerArray(void *item);

	static void deleteUnsignedInteger(void *item);

	static void deleteUnsignedIntegerArray(void *item);

public:

	/* Constructor & Destructor */
	IntermediateArray(unsigned int leftRel, unsigned int leftRelColumn,
		unsigned int rightRel, unsigned int rightRelColumn, List *tables,
		PartitionedHashJoinInput *joinParameters);
	~IntermediateArray();

	/* Getters */
	List *getRelations() const;
	List *getRowIdArrays() const;
	unsigned int getRowsNum() const;

	/* Searches whether the given relation
	 * name exists in the intermediate array
	 */
	bool search(unsigned int foreignRelationName) const;

	/* Executes 'JOIN' between the intermediate array and a
	 * new relation given as argument along with its tuples
	 */
	void executeJoinWithRelation(unsigned int foreignRelationName,
		Relation *foreignRelationTuples);

	/* Executes 'JOIN' between this intermediate
	 * array and another intermediate array
	 */
	void executeJoinWithIntermediateArray(IntermediateArray *other);

	/* Prints the intermediate array */
	void print() const;

};

#endif
