#include <iostream>
#include "IntermediateArray.h"

static unsigned int staticRowsNum;

void IntermediateArray::printUnsignedInteger(void *item)
{
	unsigned int my_uint = *((unsigned int *) item);
	std::cout << my_uint << " ";
}

void IntermediateArray::printUnsignedIntegerArray(void *item)
{
	unsigned int *my_uint_array = (unsigned int *) item;
	unsigned int i;

	std::cout << "[ ";

	for(i = 0; i < staticRowsNum; i++)
		std::cout << my_uint_array[i] << " ";

	std::cout << "]" << std::endl;
}

void IntermediateArray::deleteUnsignedInteger(void *item)
{
	delete (unsigned int *) item;
}

void IntermediateArray::deleteUnsignedIntegerArray(void *item)
{
	delete[] (unsigned int *) item;
}

/***************
 * Constructor *
 ***************/

IntermediateArray::IntermediateArray(unsigned int leftRel, unsigned int leftRelColumn,
	unsigned int rightRel, unsigned int rightRelColumn, List *tables,
	PartitionedHashJoinInput *joinParameters)
{
	/*  */
	this->tables = tables;
	this->relations = new List();
	this->rowIdArrays = new List();
	this->joinParameters = joinParameters;

	Table *leftTable = (Table *) tables->getItemInPos(leftRel + 1);
	Table *rightTable = (Table *) tables->getItemInPos(rightRel + 1);

	unsigned long long leftTableRows = leftTable->getNumOfTuples();
	unsigned long long rightTableRows = rightTable->getNumOfTuples();
	unsigned long long i;

	Tuple *leftTuples = new Tuple[leftTableRows];

	for(i = 0; i < leftTableRows; i++)
	{
		leftTuples[i].setRowId(i);
		leftTuples[i].setItem(new unsigned long long(leftTable->getTable()[leftRelColumn][i]));
	}

	Tuple *rightTuples = new Tuple[rightTableRows];

	for(i = 0; i < rightTableRows; i++)
	{
		rightTuples[i].setRowId(i);
		rightTuples[i].setItem(new unsigned long long(rightTable->getTable()[rightRelColumn][i]));
	}

	Relation *left = new Relation(leftTuples, leftTableRows);
	Relation *right = new Relation(rightTuples, rightTableRows);

	PartitionedHashJoin *join = new PartitionedHashJoin(left, right, this->joinParameters);
	RowIdRelation *joinResult = join->executeJoin();
	join->printJoinResult(joinResult);

	RowIdPair *resultPairs = joinResult->getRowIdPairs();
	unsigned int resultRowsNum = joinResult->getNumOfRowIdPairs();
	this->rowsNum = resultRowsNum;

	unsigned int *leftRowIds = new unsigned int[resultRowsNum];
	unsigned int *rightRowIds = new unsigned int[resultRowsNum];

	for(i = 0; i < resultRowsNum; i++)
	{
		leftRowIds[i] = resultPairs[i].getLeftRowId();
		rightRowIds[i] = resultPairs[i].getRightRowId();
	}

	join->freeJoinResult(joinResult);
	delete join;

	delete right;
	delete left;

	for(i = 0; i < rightTableRows; i++)
		delete (unsigned long long *) rightTuples[i].getItem();

	for(i = 0; i < leftTableRows; i++)
		delete (unsigned long long *) leftTuples[i].getItem();

	delete[] rightTuples;
	delete[] leftTuples;

	relations->insertLast(new unsigned int(leftRel));
	relations->insertLast(new unsigned int(rightRel));
	rowIdArrays->insertLast(leftRowIds);
	rowIdArrays->insertLast(rightRowIds);

	print();
}

/**************
 * Destructor *
 **************/

IntermediateArray::~IntermediateArray()
{
	relations->traverseFromHead(deleteUnsignedInteger);
	delete relations;

	rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);
	delete rowIdArrays;
}

/********************************************************************
 * Getter - Returns the list of relations of the intermediate array *
 ********************************************************************/

List *IntermediateArray::getRelations() const
{
	return relations;
}

/**********************************************
 * Getter - Returns the list of row ID arrays *
 **********************************************/
 
List *IntermediateArray::getRowIdArrays() const
{
	return rowIdArrays;
}

/*****************************************************************
 * Getter - Returns the number of rows of the intermediate array *
 *****************************************************************/

unsigned int IntermediateArray::getRowsNum() const
{
	return rowsNum;
}

/********************************************
 * Searches whether the given relation name *
 *     exists in the intermediate array     *
 ********************************************/

bool IntermediateArray::search(unsigned int foreignRelationName) const
{
	Listnode *currentNode = relations->getHead();

	while(currentNode != NULL)
	{
		unsigned int relationOfCurrentNode = *((unsigned int *) currentNode->getItem());

		if(relationOfCurrentNode == foreignRelationName)
			return true;

		currentNode = currentNode->getNext();
	}

	return false;
}

/********************************************************
 * Executes 'JOIN' between the intermediate array and a *
 * new relation given as argument along with its tuples *
 ********************************************************/

void IntermediateArray::executeJoinWithRelation(
	unsigned int foreignRelationName, Relation *foreignRelationTuples)
{
	
}

/*********************************************
 * Executes 'JOIN' between this intermediate *
 *   array and another intermediate array    *
 *********************************************/

void IntermediateArray::executeJoinWithIntermediateArray(
	IntermediateArray *other)
{
	
}

/*********************************
 * Prints the intermediate array *
 *********************************/

void IntermediateArray::print() const
{
	staticRowsNum = rowsNum;

	std::cout << "Relations of the Intermediate array:\n";
	relations->traverseFromHead(printUnsignedInteger);

	std::cout << "\nRow ID arrays of the above relations:\n";
	rowIdArrays->traverseFromHead(printUnsignedIntegerArray);
}
