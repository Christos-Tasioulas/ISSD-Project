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

/********************************************
 * A constructor that initializes the array *
 *  with two relations that must be joined  *
 ********************************************/

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

/**********************************************
 *  A constructor that initializes the array  *
 *  with one relation that must be filtered   *
 **********************************************/

IntermediateArray::IntermediateArray(unsigned int relName, unsigned int relColumn,
	unsigned int filterValue, char filterOperator, List *tables,
	PartitionedHashJoinInput *joinParameters)
{
	/* We initialize all the fields of the class (apart from 'rowsNum' for now) */
	this->tables = tables;
	this->relations = new List();
	this->rowIdArrays = new List();
	this->joinParameters = joinParameters;

	/* We retrieve a pointer to the original table with all the data */
	Table *table = (Table *) tables->getItemInPos(relName + 1);

	/* We find the number of rows of the given relation */
	unsigned int numOfRows = table->getNumOfTuples();

	/* A list with the row IDs of the intermediate array that contain
	 * row IDs of the given relation that satisfy the filter
	 */
	List *resultRowIds = new List();

	/* Auxiliary variable (used for counting) */
	unsigned int i;

	for(i = 0; i < numOfRows; i++)
	{
		unsigned long long currentElement = table->getTable()[relColumn][i];

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

	i = 0;

	unsigned int *resultRowIdArray = new unsigned int[rowsNum];

	while(!resultRowIds->isEmpty())
	{
		unsigned int *currentRowIdAddress = (unsigned int *) resultRowIds->getItemInPos(1);
		unsigned int currentRowId = (*currentRowIdAddress);
		resultRowIds->removeFront();
		delete currentRowIdAddress;

		resultRowIdArray[i] = currentRowId;

		i++;
	}

	relations->insertLast(new unsigned int(relName));
	rowIdArrays->insertLast(resultRowIdArray);

	delete resultRowIds;
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

/********************************************************************
 * Executes 'JOIN' between two relations of this intermediate array *
 ********************************************************************/

void IntermediateArray::executeJoinWithTwoRelationsInTheArray(
	unsigned int leftLocalRelationName,
	unsigned int leftLocalRelationColumn,
	unsigned int rightLocalRelationName,
	unsigned int rightLocalRelationColumn)
{
	/* We retrieve pointers to the original tables with all the data */
	Table *leftLocalTable = (Table *) tables->getItemInPos(leftLocalRelationName + 1);
	Table *rightLocalTable = (Table *) tables->getItemInPos(rightLocalRelationName + 1);

	/* We retrieve the amount of rows of the both tables.
	 * The amount of rows of both tables is the amount
	 * of rows of the intermediate array. This amount is
	 * stored in the 'rowsNum' field of the class.
	 */
	unsigned long long leftLocalTableRows = rowsNum;
	unsigned long long rightLocalTableRows = rowsNum;

	/* Auxiliary variables (used for counting) */
	unsigned long long i, j;

	/* We create the array of tuples for the left array
	 *
	 * Each tuple will have the form <RowIdIntermediateArray,LeftLocalValue>
	 */
	Tuple *leftLocalTuples = new Tuple[leftLocalTableRows];

	unsigned int posOfLeftLocalRelInList = posOfRelationInList(leftLocalRelationName);
	unsigned int *leftLocalRowIds = (unsigned int *) rowIdArrays->getItemInPos(posOfLeftLocalRelInList);

	for(i = 0; i < leftLocalTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		leftLocalTuples[i].setRowId(i);

		/* We place the corresponding value of the intermediate table to the current tuple */
		leftLocalTuples[i].setItem(new unsigned long long(leftLocalTable->getTable()[leftLocalRelationColumn][leftLocalRowIds[i]]));
	}

	/* We create the array of tuples for the right array
	 *
	 * Each tuple will have the form <RowIdIntermediateArray,RightLocalValue>
	 */
	Tuple *rightLocalTuples = new Tuple[rightLocalTableRows];

	unsigned int posOfRightLocalRelInList = posOfRelationInList(rightLocalRelationName);
	unsigned int *rightLocalRowIds = (unsigned int *) rowIdArrays->getItemInPos(posOfRightLocalRelInList);

	for(i = 0; i < rightLocalTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		rightLocalTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		rightLocalTuples[i].setItem(new unsigned long long(rightLocalTable->getTable()[rightLocalRelationColumn][rightLocalRowIds[i]]));
	}

	/* We use the tuples we made above to create the input
	 * relations for the Partitioned Hash Join Algorithm
	 */
	Relation *left = new Relation(leftLocalTuples, leftLocalTableRows);
	Relation *right = new Relation(rightLocalTuples, rightLocalTableRows);

	/* We execute the Partitioned Hash Join Algorithm */
	PartitionedHashJoin *join = new PartitionedHashJoin(left, right, this->joinParameters);
	RowIdRelation *joinResult = join->executeJoin();

	/* We retrieve the row ID pairs of the join result as well as the amount of them */
	RowIdPair *resultPairs = joinResult->getRowIdPairs();
	unsigned int resultRowsNum = joinResult->getNumOfRowIdPairs();

	/* We retrieve the amount of relations in the intermediate array */
	unsigned int localRelsCount = relations->getCounter();
	unsigned int *localRowIdArrays[localRelsCount];
	unsigned int *renewedLocalRowIdArrays[localRelsCount];

	for(i = 0; i < localRelsCount; i++)
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);

	unsigned int finalRowsOfResult = 0;
	List *results = new List();

	for(i = 0; i < resultRowsNum; i++)
	{
		unsigned int currentLeftRowId = resultPairs[i].getLeftRowId();
		unsigned int currentRightRowId = resultPairs[i].getRightRowId();

		if(currentLeftRowId == currentRightRowId)
		{
			finalRowsOfResult++;
			results->insertLast(new unsigned int(currentLeftRowId));
		}
	}

	for(i = 0; i < localRelsCount; i++)
		renewedLocalRowIdArrays[i] = new unsigned int[finalRowsOfResult];

	for(i = 0; i < finalRowsOfResult; i++)
	{
		unsigned int *currentRowIdAddress = (unsigned int *) results->getItemInPos(1);
		unsigned int currentRowId = (*currentRowIdAddress);
		delete currentRowIdAddress;
		results->removeFront();

		for(j = 0; j < localRelsCount; j++)
	 		renewedLocalRowIdArrays[j][i] = localRowIdArrays[j][currentRowId];
	}

	this->rowsNum = finalRowsOfResult;

	delete results;

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
	for(i = 0; i < leftLocalTableRows; i++)
		delete (unsigned long long *) leftLocalTuples[i].getItem();

	/* We free the allocated memory for the copies
	 * of each data value of the local array
	 */
	for(i = 0; i < rightLocalTableRows; i++)
		delete (unsigned long long *) rightLocalTuples[i].getItem();

	/* We free the arrays of tuples for the local and foreign relation */
	delete[] rightLocalTuples;
	delete[] leftLocalTuples;

	for(i = 0; i < localRelsCount; i++)
		rowIdArrays->insertLast(renewedLocalRowIdArrays[i]);
}

/**********************************************************
 * Executes 'JOIN' between a relation of the intermediate *
 *   array and a relation of another intermediate array   *
 **********************************************************/

void IntermediateArray::executeJoinWithRelationOfOtherArray(
	IntermediateArray *other,
	unsigned int localRelationName,
	unsigned int localRelationColumn,
	unsigned int foreignRelationName,
	unsigned int foreignRelationColumn)
{
	/* We retrieve pointers to the original tables with all the data */
	Table *localTable = (Table *) tables->getItemInPos(localRelationName + 1);
	Table *foreignTable = (Table *) tables->getItemInPos(foreignRelationName + 1);

	/* We retrieve the amount of rows of the both tables */
	unsigned long long localTableRows = rowsNum;
	unsigned long long foreignTableRows = other->getRowsNum();

	/* Auxiliary variables (used for counting) */
	unsigned long long i, j;

	/* We create the array of tuples for the left array
	 *
	 * Each tuple will have the form <RowIdLocalIntermediateArray (Ih), LocalValue>
	 * (h is an index to an intermediate result inside the current Intermediate Array)
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
	 * Each tuple will have the form <RowIdForeignIntermediateArray (Ig), ForeignValue>
	 * (g is an index to an intermediate result different from Ih outside of the current Intermediate Array)
	 */
	Tuple *foreignTuples = new Tuple[foreignTableRows];

	unsigned int posOfForeignRelInList = other->posOfRelationInList(foreignRelationName);
	unsigned int *foreignRowIds = (unsigned int *) other->rowIdArrays->getItemInPos(posOfForeignRelInList);

	for(i = 0; i < foreignTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		foreignTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		foreignTuples[i].setItem(new unsigned long long(foreignTable->getTable()[foreignRelationColumn][foreignRowIds[i]]));
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

	/* We retrieve the amount of relations in both intermediate arrays */
	unsigned int localRelsCount = relations->getCounter();
	unsigned int foreignRelsCount = other->relations->getCounter();

	/* We retrieve pointers to the tables of both intermediate arrays */
	unsigned int *localRowIdArrays[localRelsCount];
	unsigned int *foreignRowIdArrays[foreignRelsCount];

	for(i = 0; i < localRelsCount; i++)
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);

	for(i = 0; i < foreignRelsCount; i++)
		foreignRowIdArrays[i] = (unsigned int *) other->rowIdArrays->getItemInPos(i+1);

	unsigned int totalRelsCount = localRelsCount + foreignRelsCount;

	unsigned int *renewedRowIdArrays[totalRelsCount];

	for(i = 0; i < totalRelsCount; i++)
		renewedRowIdArrays[i] = new unsigned int[resultRowsNum];	

	for(i = 0; i < resultRowsNum; i++)
	{
		unsigned int currentLeftRowId = resultPairs[i].getLeftRowId();
		unsigned int currentRightRowId = resultPairs[i].getRightRowId();

		for(j = 0; j < localRelsCount; j++)
			renewedRowIdArrays[j][i] = localRowIdArrays[j][currentLeftRowId];

		for(j = localRelsCount; j < totalRelsCount; j++)
			renewedRowIdArrays[j][i] = foreignRowIdArrays[j][currentRightRowId];
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
	for(i = 0; i < localTableRows; i++)
		delete (unsigned long long *) localTuples[i].getItem();

	/* We free the allocated memory for the copies
	 * of each data value of the local array
	 */
	for(i = 0; i < foreignTableRows; i++)
		delete (unsigned long long *) foreignTuples[i].getItem();

	/* We free the arrays of tuples for the local and foreign relation */
	delete[] localTuples;
	delete[] foreignTuples;

	this->relations->append(other->relations);
	this->rowsNum = resultRowsNum;

	for(i = 0; i < totalRelsCount; i++)
		this->rowIdArrays->insertLast(renewedRowIdArrays[i]);
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
