#include <iostream>
#include "IntermediateArray.h"

/*************************************************************
 * Auxiliary static variable used for the printing operation *
 *************************************************************/

static unsigned int staticRowsNum;

/*******************************************
 * Operations used to traverse the list of *
 *             relation names              *
 *             ^^^^^^^^^^^^^^              *
 *******************************************/

void IntermediateArray::printUnsignedInteger(void *item)
{
	unsigned int my_uint = *((unsigned int *) item);
	std::cout << my_uint << " ";
}

void IntermediateArray::deleteUnsignedInteger(void *item)
{
	delete (unsigned int *) item;
}

/*******************************************
 * Operations used to traverse the list of *
 *              row ID arrays              *
 *              ^^^^^^^^^^^^^              *
 *******************************************/

void IntermediateArray::printUnsignedIntegerArray(void *item)
{
	unsigned int *my_uint_array = (unsigned int *) item;
	unsigned int i;

	std::cout << "[ ";

	for(i = 0; i < staticRowsNum; i++)
		std::cout << my_uint_array[i] << " ";

	std::cout << "]" << std::endl;
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
	/* We initialize all the fields of the class (apart from 'rowsNum' for now) */
	this->tables = tables;
	this->relations = new List();
	this->rowIdArrays = new List();
	this->joinParameters = joinParameters;

	/* We retrieve pointers to the original left and right tables with all the data */
	Table *leftTable = (Table *) tables->getItemInPos(leftRel + 1);
	Table *rightTable = (Table *) tables->getItemInPos(rightRel + 1);

	/* We retrieve the amount of rows of the left and right table */
	unsigned long long leftTableRows = leftTable->getNumOfTuples();
	unsigned long long rightTableRows = rightTable->getNumOfTuples();

	/* Auxiliary variable (used for counting) */
	unsigned long long i;

	/* We create the array of tuples for the left array
	 *
	 * Each tuple will have the form <RowIdLeft,ValueLeft>
	 */
	Tuple *leftTuples = new Tuple[leftTableRows];

	for(i = 0; i < leftTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		leftTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		leftTuples[i].setItem(new unsigned long long(leftTable->getTable()[leftRelColumn][i]));
	}

	/* We create the array of tuples for the right array
	 *
	 * Each tuple will have the form <RowIdRight,ValueRight>
	 */
	Tuple *rightTuples = new Tuple[rightTableRows];

	for(i = 0; i < rightTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		rightTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		rightTuples[i].setItem(new unsigned long long(rightTable->getTable()[rightRelColumn][i]));
	}

	/* We use the tuples we made above to create the input
	 * relations for the Partitioned Hash Join Algorithm
	 */
	Relation *left = new Relation(leftTuples, leftTableRows);
	Relation *right = new Relation(rightTuples, rightTableRows);

	/* We execute the Partitioned Hash Join Algorithm */
	PartitionedHashJoin *join = new PartitionedHashJoin(left, right, this->joinParameters);
	RowIdRelation *joinResult = join->executeJoin();

	/* We retrieve the row ID pairs of the join result as well as the amount of them */
	RowIdPair *resultPairs = joinResult->getRowIdPairs();
	unsigned int resultRowsNum = joinResult->getNumOfRowIdPairs();

	/* We update the 'rowsNum' field of the class with the amount of row ID pairs */
	this->rowsNum = resultRowsNum;

	/* We create two arrays of row IDs - one for the left and one for the right relation */
	unsigned int *leftRowIds = new unsigned int[resultRowsNum];
	unsigned int *rightRowIds = new unsigned int[resultRowsNum];

	/* We place each row ID of the join result in these two arrays */

	for(i = 0; i < resultRowsNum; i++)
	{
		/* We place each row ID of the left relation to the left array */
		leftRowIds[i] = resultPairs[i].getLeftRowId();

		/* We place each row ID of the right relation to the right array */
		rightRowIds[i] = resultPairs[i].getRightRowId();
	}

	/* We free the result of join */
	join->freeJoinResult(joinResult);
	delete join;

	/* We free the relations for the join */
	delete right;
	delete left;

	/* We free the allocated memory for the copies
	 * of each data value of the right array
	 */
	for(i = 0; i < rightTableRows; i++)
		delete (unsigned long long *) rightTuples[i].getItem();

	/* We free the allocated memory for the copies
	 * of each data value of the left array
	 */
	for(i = 0; i < leftTableRows; i++)
		delete (unsigned long long *) leftTuples[i].getItem();

	/* We free the arrays of tuples for the left and right relation */
	delete[] rightTuples;
	delete[] leftTuples;

	/* We insert the left relation name in the list of relations */
	relations->insertLast(new unsigned int(leftRel));

	/* We insert the right relation name in the list of relations */
	relations->insertLast(new unsigned int(rightRel));

	/* We insert the array of left row IDs in the list of arrays */
	rowIdArrays->insertLast(leftRowIds);

	/* We insert the array of right row IDs in the list of arrays */
	rowIdArrays->insertLast(rightRowIds);
}

/**************
 * Destructor *
 **************/

IntermediateArray::~IntermediateArray()
{
	/* We free the allocated memory for each relation
	 * name and the list of relation names itself
	 */
	relations->traverseFromHead(deleteUnsignedInteger);
	delete relations;

	/* We free the allocated memory for each row ID
	 * array and the list of row ID arrays itself
	 */
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

/********************************************************************
 * Finds and returns the position in the list of the given relation *
 ********************************************************************/

unsigned int IntermediateArray::posOfRelationInList(unsigned int relationName) const
{
	unsigned int pos = 1;

	Listnode *current = relations->getHead();

	while(current != NULL)
	{
		unsigned int currentRelationName = *((unsigned int *) current->getItem());

		if(currentRelationName == relationName)
			return pos;

		pos++;
		current = current->getNext();
	}

	return 0;
}

/********************************************************
 * Executes 'JOIN' between the intermediate array and a *
 * new relation given as argument along with its tuples *
 ********************************************************/

void IntermediateArray::executeJoinWithForeignRelation(
	unsigned int localRelationName,
	unsigned int localRelationColumn,
	unsigned int foreignRelationName,
	unsigned int foreignRelationColumn)
{
	/* We retrieve pointers to the original tables with all the data */
	Table *localTable = (Table *) tables->getItemInPos(localRelationName + 1);
	Table *foreignTable = (Table *) tables->getItemInPos(foreignRelationName + 1);

	/* We retrieve the amount of rows of the both tables.
	 *
	 * For the local tables the amount of rows is just
	 * the amount of rows of this intermediate array.
	 */
	unsigned long long localTableRows = rowsNum;
	unsigned long long foreignTableRows = foreignTable->getNumOfTuples();

	/* Auxiliary variables (used for counting) */
	unsigned long long i, j;

	/* We create the array of tuples for the local array
	 *
	 * Each tuple will have the form <RowIdIntermediateArray,LocalValue>
	 */
	Tuple *localTuples = new Tuple[localTableRows];

	unsigned int posOfLocalRelInList = posOfRelationInList(localRelationName);

	unsigned int *localRowIds = (unsigned int *) rowIdArrays->getItemInPos(posOfLocalRelInList);

	for(i = 0; i < localTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		localTuples[i].setRowId(i);

		/* We place the corresponding value of the intermediate table to the current tuple */
		localTuples[i].setItem(new unsigned long long(localTable->getTable()[localRelationColumn][localRowIds[i]]));
	}

	/* We create the array of tuples for the right array
	 *
	 * Each tuple will have the form <RowIdRight,ValueRight>
	 */
	Tuple *foreignTuples = new Tuple[foreignTableRows];

	for(i = 0; i < foreignTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		foreignTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		foreignTuples[i].setItem(new unsigned long long(foreignTable->getTable()[foreignRelationColumn][i]));
	}

	/* We use the tuples we made above to create the input
	 * relations for the Partitioned Hash Join Algorithm
	 */
	Relation *left = new Relation(localTuples, localTableRows);
	Relation *right = new Relation(foreignTuples, foreignTableRows);

	/* We execute the Partitioned Hash Join Algorithm */
	PartitionedHashJoin *join = new PartitionedHashJoin(left, right, this->joinParameters);
	RowIdRelation *joinResult = join->executeJoin();

	/* We retrieve the row ID pairs of the join result as well as the amount of them */
	RowIdPair *resultPairs = joinResult->getRowIdPairs();
	unsigned int resultRowsNum = joinResult->getNumOfRowIdPairs();

	/* We update the 'rowsNum' field of the class with the amount of row ID pairs */
	this->rowsNum = resultRowsNum;

	/* We retrieve the amount of relations in the intermediate array */
	unsigned int localRelsCount = relations->getCounter();
	unsigned int *localRowIdArrays[localRelsCount];
	unsigned int *renewedLocalRowIdArrays[localRelsCount];

	for(i = 0; i < localRelsCount; i++)
	{
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);
		renewedLocalRowIdArrays[i] = new unsigned int[resultRowsNum];
	}

	/* We create the array of row IDs of the foreign relation */
	unsigned int *foreignRowIds = new unsigned int[resultRowsNum];

	/* We place each row ID of the join result in these two arrays */

	for(i = 0; i < resultRowsNum; i++)
	{
		for(j = 0; j < localRelsCount; j++)
			renewedLocalRowIdArrays[j][i] = localRowIdArrays[j][resultPairs[i].getLeftRowId()];

		/* We place each row ID of the right relation to the right array */
		foreignRowIds[i] = resultPairs[i].getRightRowId();
	}

	rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);

	while(!rowIdArrays->isEmpty())
		rowIdArrays->removeFront();

	/* We free the result of join */
	join->freeJoinResult(joinResult);
	delete join;

	/* We free the relations for the join */
	delete right;
	delete left;

	/* We free the allocated memory for the copies
	 * of each data value of the foreign array
	 */
	for(i = 0; i < foreignTableRows; i++)
		delete (unsigned long long *) foreignTuples[i].getItem();

	/* We free the allocated memory for the copies
	 * of each data value of the local array
	 */
	for(i = 0; i < localTableRows; i++)
		delete (unsigned long long *) localTuples[i].getItem();

	/* We free the arrays of tuples for the local and foreign relation */
	delete[] foreignTuples;
	delete[] localTuples;

	for(i = 0; i < localRelsCount; i++)
		rowIdArrays->insertLast(renewedLocalRowIdArrays[i]);

	rowIdArrays->insertLast(foreignRowIds);

	relations->insertLast(new unsigned int(foreignRelationName));
}

/*********************************************
 * Executes 'JOIN' between this intermediate *
 *   array and another intermediate array    *
 *********************************************/

void IntermediateArray::executeJoinWithIntermediateArray(
	IntermediateArray *other)
{
	
}

/**********************************************************
 * Applies the given filter to the implied local relation *
 **********************************************************/

void IntermediateArray::executeFilter(unsigned int relationName,
	unsigned int relationColumn, unsigned int filterValue, char filterOperator)
{
	/* We retrieve a pointer to the original table with all the data */
	Table *table = (Table *) tables->getItemInPos(relationName + 1);

	/* We find the position of the relation in the list of the intermediate array */
	unsigned int relationPosInList = posOfRelationInList(relationName);

	/* We retrieve a pointer to the table with the reserved row IDs for this relation */
	unsigned int *reservedRowIdsOfRel = (unsigned int *) rowIdArrays->getItemInPos(relationPosInList);

	/* A list with the row IDs of the intermediate array that contain
	 * row IDs of the given relation that satisfy the filter
	 */
	List *resultRowIds = new List();

	/* Auxiliary variables (used for counting) */
	unsigned int i, j;

	for(i = 0; i < rowsNum; i++)
	{
		unsigned long long currentElement = table->getTable()[relationColumn][reservedRowIdsOfRel[i]];

		bool elementSatisfiesFilter = false;

		switch(filterOperator)
		{
			case '<':
			{
				if(currentElement < filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			case '>':
			{
				if(currentElement > filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			case '=':
			{
				if(currentElement == filterValue)
					elementSatisfiesFilter = true;

				break;
			}
		}

		if(elementSatisfiesFilter)
			resultRowIds->insertLast(new unsigned int(i));
	}

	this->rowsNum = resultRowIds->getCounter();

	std::cout << "I have " << rowsNum << " elements" << std::endl;

	/* We retrieve the amount of relations in the intermediate array */
	unsigned int localRelsCount = relations->getCounter();
	unsigned int *localRowIdArrays[localRelsCount];
	unsigned int *renewedLocalRowIdArrays[localRelsCount];

	for(i = 0; i < localRelsCount; i++)
	{
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);
		renewedLocalRowIdArrays[i] = new unsigned int[rowsNum];
	}

	i = 0;

	while(!resultRowIds->isEmpty())
	{
		unsigned int *currentRowIdAddress = (unsigned int *) resultRowIds->getItemInPos(1);
		unsigned int currentRowId = (*currentRowIdAddress);
		resultRowIds->removeFront();
		delete currentRowIdAddress;

		for(j = 0; j < localRelsCount; j++)
			renewedLocalRowIdArrays[j][i] = localRowIdArrays[j][currentRowId];

		i++;
	}

	rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);

	while(!rowIdArrays->isEmpty())
		rowIdArrays->removeFront();

	for(i = 0; i < localRelsCount; i++)
		rowIdArrays->insertLast(renewedLocalRowIdArrays[i]);

	delete resultRowIds;
}

/*******************************************************
 * Prints the sum of the items in the reserved row IDs *
 *    of the given relation in the requested column    *
 *******************************************************/

void IntermediateArray::produceSum(unsigned int relName, unsigned int relColumn)
{
	/* If there are no rows in the intermediate array, there is no sum
	 * (the sum can be considered zero). In this case, we print 'NULL'.
	 */
	if(rowsNum == 0)
	{
		std::cout << "NULL ";
		return;
	}

	/* We initialize the 64-bit sum to zero */
	unsigned long long sum = 0;

	/* We retrieve a pointer to the table with all the data of the relation */
	Table *table = (Table *) tables->getItemInPos(relName + 1);

	/* We find the position of the given relation in the intermediate array */
	unsigned int posOfRel = posOfRelationInList(relName);

	/* We retrieve the table of the reserved row IDs of this relation */
	unsigned int *reservedRowIds = (unsigned int *) rowIdArrays->getItemInPos(posOfRel);

	/* Auxiliary variable (used for counting) */
	unsigned int i;

	/* We compute the suggested sum of the reserved row IDs */

	for(i = 0; i < rowsNum; i++)
		sum += table->getTable()[relColumn][reservedRowIds[i]];

	/* We print the sum in the standard output */
	std::cout << sum << " ";
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
