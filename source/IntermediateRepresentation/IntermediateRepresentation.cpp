#include <iostream>
#include "IntermediateRepresentation.h"

void IntermediateRepresentation::deleteIntermediateArray(void *item)
{
	IntermediateArray *array = (IntermediateArray *) item;
	delete array;
}

/***************
 * Constructor *
 ***************/

IntermediateRepresentation::IntermediateRepresentation(List *tables,
	PartitionedHashJoinInput *joinParameters, JobScheduler *jobScheduler)
{
	this->intermediateArrays = new List();
	this->tables = tables;
	this->joinParameters = joinParameters;
	this->jobScheduler = jobScheduler;
}

/**************
 * Destructor *
 **************/

IntermediateRepresentation::~IntermediateRepresentation()
{
	intermediateArrays->traverseFromHead(deleteIntermediateArray);
	delete intermediateArrays;
}

/*************************************************************************
 * Getter - Returns the given tables for the intermediate representation *
 *************************************************************************/

List *IntermediateRepresentation::getTables() const
{
	return tables;
}

/***********************************************************************
 * Getter - Returns the additional parameters for the 'JOIN' operation *
 ***********************************************************************/

PartitionedHashJoinInput *IntermediateRepresentation::getJoinParameters() const
{
	return joinParameters;
}

/**********************************************************
 *  Searches if the given relation exists in any of the   *
 * intermediate arrays of the intermediate representation *
 *   If it exists, that intermediate array is returned    *
 **********************************************************/

IntermediateArray *IntermediateRepresentation::relationExists(
	unsigned int relationName, unsigned int relationPriority) const
{
	/* We will traverse the list of intermediate arrays from the head */
	Listnode *currentNode = intermediateArrays->getHead();

	/* As long as we have not finished traversing the list */

	while(currentNode != NULL)
	{
		/* We retrieve the intermediate array of the current node */
		IntermediateArray *arrayInCurrentNode = (IntermediateArray *) currentNode->getItem();

		/* If the given relation exists in the array, we return the array */
		if(arrayInCurrentNode->search(relationName, relationPriority))
			return arrayInCurrentNode;

		/* Else we proceed to the next node */
		currentNode = currentNode->getNext();
	}

	/* If this part is reached, the given relation does not exist in any
	 * of the intermediate arrays of the list. In this case, we return 'NULL'
	 */
	return NULL;
}

/********************************************************************
 * Returns the position in the list of the given intermediate array *
 ********************************************************************/

unsigned int IntermediateRepresentation::posOfIntermediateArray(IntermediateArray *array) const
{
	/* We will traverse the list of intermediate arrays from the head */
	Listnode *currentNode = intermediateArrays->getHead();
	unsigned int pos = 1;

	/* As long as we have not finished traversing the list */

	while(currentNode != NULL)
	{
		/* We retrieve the intermediate array of the current node */
		IntermediateArray *arrayInCurrentNode = (IntermediateArray *) currentNode->getItem();

		/* If the given relation exists in the array, we return the array */
		if(arrayInCurrentNode == array)
			return pos;

		/* We increase the position counter by 1 because
		 * we didn't find the given array in this node
		 */ 
		pos++;

		/* Else we proceed to the next node */
		currentNode = currentNode->getNext();
	}

	/* If this part is reached, the intermediate array does
	 * not exist in the list. In this case, we return 0.
	 */
	return 0;
}

/*****************************************************************
 * Executes the 'JOIN' operation between the two given relations *
 *****************************************************************/

void IntermediateRepresentation::executeJoin(
	unsigned int leftRel,
	unsigned int leftRelColumn,
	unsigned int leftRelPriority,
	unsigned int rightRel,
	unsigned int rightRelColumn,
	unsigned int rightRelPriority)
{
	/* We search both relations in the intermediate representation */
	IntermediateArray *leftIntermediateArray = relationExists(leftRel, leftRelPriority);
	IntermediateArray *rightIntermediateArray = relationExists(rightRel, rightRelPriority);

	if(leftIntermediateArray == NULL)
	{
		/** 
		 * Case both relations are NULL.
		 * This is the first case that happens by default in each query.
		 * The constructor of the intermediate array executes by default 
		 * the first join either it is a join or a filter operation.
		 */
		if(rightIntermediateArray == NULL)
		{
			IntermediateArray *newArray = new IntermediateArray(
				leftRel,
				leftRelColumn,
				leftRelPriority,
				rightRel,
				rightRelColumn,
				rightRelPriority,
				tables,
				joinParameters,
				jobScheduler);

			intermediateArrays->insertLast(newArray);
		}

		/**
		 * Case the left relation is NULL
		 */
		else
		{
			rightIntermediateArray->executeJoinWithForeignRelation(
				rightRel,
				rightRelColumn,
				rightRelPriority,
				leftRel,
				leftRelColumn,
				leftRelPriority);
		}
	}

	else
	{
		/**
		 * Case the right relation is NULL
		 */
		if(rightIntermediateArray == NULL)
		{
			leftIntermediateArray->executeJoinWithForeignRelation(
				leftRel,
				leftRelColumn,
				leftRelPriority,
				rightRel,
				rightRelColumn,
				rightRelPriority);
		}

		else
		{
			/**
			 * Case both relations come from the same intermediate array
			 */
			if(leftIntermediateArray == rightIntermediateArray)
			{
				leftIntermediateArray->executeJoinWithTwoRelationsInTheArray(
					leftRel,
					leftRelColumn,
					leftRelPriority,
					rightRel,
					rightRelColumn,
					rightRelPriority);
			}

			/**
			 * Case both relations come from different intermediate arrays
			 */
			else
			{
				leftIntermediateArray->executeJoinWithRelationOfOtherArray(
					rightIntermediateArray,
					leftRel,
					leftRelColumn,
					leftRelPriority,
					rightRel,
					rightRelColumn,
					rightRelPriority);

				/* Since the two intermediate arrays became one, we remove
				 * the now useless intermediate array object from the list
				 *
				 * First we find the position of the intermediate array we
				 * want to remove.
				 */
				unsigned int posOfRightIntermediateArray = posOfIntermediateArray(
					rightIntermediateArray);

				/* We remove the item of the list in that position */
				intermediateArrays->removePos(posOfRightIntermediateArray);

				/* We free the allocated memory for the intermediate array object */
				delete rightIntermediateArray;
			}
		}
	}
}

/**************************************************
 * Applies the given filter to the given relation *
 **************************************************/

void IntermediateRepresentation::executeFilter(
	unsigned int relName,
	unsigned int relColumn,
	unsigned int relPriority,
	unsigned int filterValue,
	char filterOperator)
{
	/* We search the relation in the intermediate representation */
	IntermediateArray *intermediateArray = relationExists(relName, relPriority);

	/* If the relation does not exist in any array, we create one */

	if(intermediateArray == NULL)
	{
		IntermediateArray *newArray = new IntermediateArray(
			relName,
			relColumn,
			relPriority,
			filterValue,
			filterOperator,
			tables,
			joinParameters,
			jobScheduler);

		intermediateArrays->insertLast(newArray);
	}

	/* Else we have the array apply the filter on the relation */

	else
	{
		intermediateArray->executeFilter(
			relName,
			relColumn,
			relPriority,
			filterValue,
			filterOperator);
	}
}

/*******************************************************
 * Prints the sum of the items in the reserved row IDs *
 *    of the given relation in the requested column    *
 *******************************************************/

void IntermediateRepresentation::produceSum(
	unsigned int relName,
	unsigned int relColumn,
	unsigned int relPriority)
{
	/* We search the relation in the intermediate representation */
	IntermediateArray *intermediateArray = relationExists(relName, relPriority);

	/* If the relation does not exist in any intermediate array,
	 * we just sum all the elements of the suggested column
	 */
	if(intermediateArray == NULL)
	{
		/* The sum (initialized with zero) and an auxiliary counter */
		unsigned long long sum = 0, i;

		/* We retrieve the table pointed by the relation name */
		Table *table = (Table *) tables->getItemInPos(relName + 1);

		/* We retrieve the number of rows of the above table */
		unsigned long long numRows = table->getNumOfTuples();

		/* If the table has no rows, we print 'NULL' */

		if(numRows == 0)
			std::cout << "NULL";

		/* Else we sum the elements of each row in the
		 * designated column and print the final sum
		 */
		else
		{
			for(i = 0; i < numRows; i++)
				sum += table->getTable()[relColumn][i];

			std::cout << sum;
		}
	}

	/* If the relations exists in an intermediate array,
	 * we invoke the array's operation to produce the sum
	 */
	else
	{
		intermediateArray->produceSum(relName, relColumn, relPriority);
	}
}
