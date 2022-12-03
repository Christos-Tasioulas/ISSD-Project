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
	PartitionedHashJoinInput *joinParameters)
{
	this->intermediateArrays = new List();
	this->tables = tables;
	this->joinParameters = joinParameters;
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

IntermediateArray *IntermediateRepresentation::relationExists(unsigned int relation) const
{
	/* We will traverse the list of intermediate arrays from the head */
	Listnode *currentNode = intermediateArrays->getHead();

	/* As long as we have not finished traversing the list */

	while(currentNode != NULL)
	{
		/* We retrieve the intermediate array of the current node */
		IntermediateArray *arrayInCurrentNode = (IntermediateArray *) currentNode->getItem();

		/* If the given relation exists in the array, we return the array */
		if(arrayInCurrentNode->search(relation))
			return arrayInCurrentNode;

		/* Else we proceed to the next node */
		currentNode = currentNode->getNext();
	}

	/* If this part is reached, the given relation does not exist in any
	 * of the intermediate arrays of the list. In this case, we return 'NULL'
	 */
	return NULL;
}

/*****************************************************************
 * Executes the 'JOIN' operation between the two given relations *
 *****************************************************************/

void IntermediateRepresentation::executeJoin(unsigned int leftRel,
	unsigned int leftRelColumn, unsigned int rightRel, unsigned rightRelColumn)
{
	/* We search both relations in the intermediate representation */
	IntermediateArray *leftIntermediateArray = relationExists(leftRel);
	IntermediateArray *rightIntermediateArray = relationExists(rightRel);

	if(leftIntermediateArray == NULL)
	{
		if(rightIntermediateArray == NULL)
		{
			IntermediateArray *newArray = new IntermediateArray(leftRel,
				leftRelColumn, rightRel, rightRelColumn, tables, joinParameters);

			intermediateArrays->insertLast(newArray);
		}

		else
		{
			rightIntermediateArray->executeJoinWithForeignRelation(
				rightRel, rightRelColumn, leftRel, leftRelColumn);
		}
	}

	else
	{
		if(rightIntermediateArray == NULL)
		{
			std::cout << "I'm there!" << std::endl;
			leftIntermediateArray->executeJoinWithForeignRelation(
				leftRel, leftRelColumn, rightRel, rightRelColumn);
		}

		else
		{
			std::cout << " " << std::endl;
		}
	}
}

/**************************************************
 * Applies the given filter to the given relation *
 **************************************************/

void IntermediateRepresentation::executeFilter(unsigned int relName,
	unsigned int relColumn, unsigned int filterValue, char filterOperator)
{
	/* We search the relation in the intermediate representation */
	IntermediateArray *intermediateArray = relationExists(relName);

	if(intermediateArray == NULL)
	{
		std::cout << std::endl;
	}

	else
	{
		std::cout << "I'm going to do the filter" << std::endl;
		intermediateArray->executeFilter(relName, relColumn,
			filterValue, filterOperator);
	}
}

/*******************************************************
 * Prints the sum of the items in the reserved row IDs *
 *    of the given relation in the requested column    *
 *******************************************************/

void IntermediateRepresentation::produceSum(unsigned int relName, unsigned int relColumn)
{
	/* We search the relation in the intermediate representation */
	IntermediateArray *intermediateArray = relationExists(relName);

	if(intermediateArray == NULL)
	{
		unsigned long long sum = 0, i;

		Table *table = (Table *) tables->getItemInPos(relName + 1);
		unsigned long long numRows = table->getNumOfTuples();

		if(numRows == 0)
			std::cout << "NULL ";

		else
		{
			for(i = 0; i < numRows; i++)
				sum += table->getTable()[relColumn][i];

			std::cout << sum << " ";
		}
	}

	else
	{
		intermediateArray->produceSum(relName, relColumn);
	}
}
