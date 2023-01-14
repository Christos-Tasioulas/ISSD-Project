#ifndef _INTERMEDIATE_ARRAY_H_
#define _INTERMEDIATE_ARRAY_H_

#include "List.h"
#include "Table.h"
#include "PartitionedHashJoin.h"
#include "IntermediateRelation.h"
#include "JobScheduler.h"

class IntermediateArray {

private:

	/* A pointer to the tables of all relations in the system */
	List *tables;

	/* A list with all the relations of the intermediate array */
	List *relations;

	/* A list of unsigned integer arrays, representing the row IDs of each relation */
	List *rowIdArrays;

	/* The amount of row IDs each of the arrays has */
	unsigned int rowsNum;

	/* The additional parameters for a 'JOIN' operation */
	PartitionedHashJoinInput *joinParameters;

    /* A job scheduler that we will use for parallel
     * execution of the tasks of a join predicate
     */
	JobScheduler *jobScheduler;

	/* Finds and returns the position in the list of the given relation */
	unsigned int posOfRelationInList(unsigned int relationName,
		unsigned int relationPriority) const;

	/* Prints a relation - used to traverse the list of relations */
	static void printIntermediateRelation(void *item);

	/* Deletes a relation - used to traverse the list of relations */
	static void deleteIntermediateRelation(void *item);

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
	IntermediateArray(
		unsigned int leftRel,
		unsigned int leftRelColumn,
		unsigned int leftRelPriority,
		unsigned int rightRel,
		unsigned int rightRelColumn,
		unsigned int rightRelPriority,
		List *tables,
		PartitionedHashJoinInput *joinParameters,
		JobScheduler *jobScheduler = NULL);

	/* A constructor that initializes the array
	 * with one relation that must be filtered
	 */
	IntermediateArray(
		unsigned int relName,
		unsigned int relColumn,
		unsigned int relPriority,
		unsigned int filterValue,
		char filterOperator,
		List *tables,
		PartitionedHashJoinInput *joinParameters,
		JobScheduler *jobScheduler = NULL);

	/* Destructor */
	~IntermediateArray();

	/* Getter - Returns the list of relations of the intermediate array */
	List *getRelations() const;

	/* Getter - Returns the list of row ID arrays */
	List *getRowIdArrays() const;

	/* Getter - Returns the number of rows of the intermediate array */
	unsigned int getRowsNum() const;

	/* Searches whether the given relation exists in the intermediate array */
	bool search(unsigned int foreignRelationName,
		unsigned int foreignRelationPriority) const;

	/* Executes 'JOIN' between a relation of the intermediate
	 * array and a foreign relation given as argument
	 */
	void executeJoinWithForeignRelation(
		unsigned int localRelationName,
		unsigned int localRelationColumn,
		unsigned int localRelationPriority,
		unsigned int foreignRelationName,
		unsigned int foreignRelationColumn,
		unsigned int foreignRelationPriority);

	/* Executes 'JOIN' between two relations of this intermediate array */
	void executeJoinWithTwoRelationsInTheArray(
		unsigned int leftLocalRelationName,
		unsigned int leftLocalRelationColumn,
		unsigned int leftLocalRelationPriority,
		unsigned int rightLocalRelationName,
		unsigned int rightLocalRelationColumn,
		unsigned int rightLocalRelationPriority);

	/* Executes 'JOIN' between a relation of the intermediate
	 * array and a relation of another intermediate array
	 */
	void executeJoinWithRelationOfOtherArray(
		IntermediateArray *other,
		unsigned int localRelationName,
		unsigned int localRelationColumn,
		unsigned int localRelationPriority,
		unsigned int foreignRelationName,
		unsigned int foreignRelationColumn,
		unsigned int foreignRelationPriority);

	/* Applies the given filter to the implied local relation */
	void executeFilter(
		unsigned int relationName,
		unsigned int relationColumn,
		unsigned int relationPriority,
		unsigned int filterValue,
		char filterOperator);

	/* Prints the sum of the items in the reserved row IDs
	 * of the given relation in the requested column
	 */
	void produceSum(
		unsigned int relName,
		unsigned int relColumn,
		unsigned int relPriority);

	/* Prints the intermediate array */
	void print() const;

};

#endif
