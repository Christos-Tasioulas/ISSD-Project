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

	/* Finds and returns the position in the list of the given relation */
	unsigned int posOfRelationInList(unsigned int relationName) const;

	/* Prints the name of a relation - used to traverse the list of relation names */
	static void printUnsignedInteger(void *item);

	/* Deletes the name of a relation - used to traverse the list of relation names */
	static void deleteUnsignedInteger(void *item);

	/* Prints a row ID array - used to traverse the list of row ID arrays */
	static void printUnsignedIntegerArray(void *item);

	/* Deletes a row ID array - used to traverse the list of row ID arrays */
	static void deleteUnsignedIntegerArray(void *item);

public:

	/* Constructors & Destructor
	 *
	 * A constructor that initializes the array
	 * with two relations that must be joined
	 */
	IntermediateArray(unsigned int leftRel, unsigned int leftRelColumn,
		unsigned int rightRel, unsigned int rightRelColumn, List *tables,
		PartitionedHashJoinInput *joinParameters);

	/* A constructor that initializes the array
	 * with one relation that must be filtered
	 */
	IntermediateArray(unsigned int relName, unsigned int relColumn,
		unsigned int filterValue, char filterOperator, List *tables,
		PartitionedHashJoinInput *joinParameters);

	/* Destructor */
	~IntermediateArray();

	/* Getter - Returns the list of relations of the intermediate array */
	List *getRelations() const;

	/* Getter - Returns the list of row ID arrays */
	List *getRowIdArrays() const;

	/* Getter - Returns the number of rows of the intermediate array */
	unsigned int getRowsNum() const;

	/* Searches whether the given relation
	 * name exists in the intermediate array
	 */
	bool search(unsigned int foreignRelationName) const;

	/* Executes 'JOIN' between a relation of the intermediate
	 * array and a foreign relation given as argument
	 */
	void executeJoinWithForeignRelation(
		unsigned int localRelationName,
		unsigned int localRelationColumn,
		unsigned int foreignRelationName,
		unsigned int foreignRelationColumn);

	/* Executes 'JOIN' between two relations of this intermediate array */
	void executeJoinWithTwoRelationsInTheArray(
		unsigned int leftLocalRelationName,
		unsigned int leftLocalRelationColumn,
		unsigned int rightLocalRelationName,
		unsigned int rightLocalRelationColumn);

	/* Executes 'JOIN' between a relation of the intermediate
	 * array and a relation of another intermediate array
	 */
	void executeJoinWithRelationOfOtherArray(
		IntermediateArray *other,
		unsigned int localRelationName,
		unsigned int localRelationColumn,
		unsigned int foreignRelationName,
		unsigned int foreignRelationColumn);

	/* Applies the given filter to the implied local relation */
	void executeFilter(unsigned int relationName, unsigned int relationColumn,
		unsigned int filterValue, char filterOperator);

	/* Prints the sum of the items in the reserved row IDs
	 * of the given relation in the requested column
	 */
	void produceSum(unsigned int relName, unsigned int relColumn);

	/* Prints the intermediate array */
	void print() const;

};

#endif
