#include <iostream>
#include "IntermediateArray.h"

/*************************************************************
 * Auxiliary static variable used for the printing operation *
 *************************************************************/

static unsigned int staticRowsNum;

/*******************************************
 * Operations used to traverse the list of *
 *         intermediate relations          *
 *         ^^^^^^^^^^^^^^^^^^^^^^          *
 *******************************************/

void IntermediateArray::printIntermediateRelation(void *item)
{
	IntermediateRelation *my_rel = (IntermediateRelation *) item;
	my_rel->print();
}

void IntermediateArray::deleteIntermediateRelation(void *item)
{
	delete (IntermediateRelation *) item;
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

IntermediateArray::IntermediateArray(
	unsigned int leftRel,
	unsigned int leftRelColumn,
	unsigned int leftRelPriority,
	unsigned int rightRel,
	unsigned int rightRelColumn,
	unsigned int rightRelPriority,
	List *tables,
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
		leftTuples[i].setItem(new unsigned long long(leftTable->
			getTable()[leftRelColumn][i]));
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
		rightTuples[i].setItem(new unsigned long long(rightTable->
			getTable()[rightRelColumn][i]));
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

	/* We insert the left relation in the list of relations */
	relations->insertLast(new IntermediateRelation(leftRel, leftRelPriority));

	/* We insert the right relation in the list of relations */
	relations->insertLast(new IntermediateRelation(rightRel, rightRelPriority));

	/* We insert the array of left row IDs in the list of arrays */
	rowIdArrays->insertLast(leftRowIds);

	/* We insert the array of right row IDs in the list of arrays */
	rowIdArrays->insertLast(rightRowIds);

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
}

/**********************************************
 *  A constructor that initializes the array  *
 *  with one relation that must be filtered   *
 **********************************************/

IntermediateArray::IntermediateArray(
	unsigned int relName,
	unsigned int relColumn,
	unsigned int relPriority,
	unsigned int filterValue,
	char filterOperator,
	List *tables,
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
		/* We retrieve the value of the current element */
		unsigned long long currentElement = table->getTable()[relColumn][i];

		/* We initialize an auxiliary boolean flag to 'false' */
		bool elementSatisfiesFilter = false;

		/* Depending on the filter operator we take one of the actions below */

		switch(filterOperator)
		{
			/* Case the filter operator is '<' */

			case '<':
			{
				if(currentElement < filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			/* Case the filter operator is '>' */

			case '>':
			{
				if(currentElement > filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			/* Case the filter operator is '=' */

			case '=':
			{
				if(currentElement == filterValue)
					elementSatisfiesFilter = true;

				break;
			}
		}

		/* If the current element satisfies the filter, we
		 * insert the row ID of that element in the list
		 */
		if(elementSatisfiesFilter)
			resultRowIds->insertLast(new unsigned int(i));
	}

	/* We update the amount of row IDs of the intermediate array */
	this->rowsNum = resultRowIds->getCounter();

	/* We will move each row ID from the list to the intermediate array */
	i = 0;

	/* Here we create the array that will be stored in the structure */
	unsigned int *resultRowIdArray = new unsigned int[rowsNum];

	/* We move every element from the list to the above array */

	while(!resultRowIds->isEmpty())
	{
		unsigned int *currentRowIdAddress = (unsigned int *) resultRowIds->getItemInPos(1);
		unsigned int currentRowId = (*currentRowIdAddress);
		resultRowIds->removeFront();
		delete currentRowIdAddress;
		resultRowIdArray[i] = currentRowId;
		i++;
	}

	/* We insert the relation in the list of relations */
	relations->insertLast(new IntermediateRelation(relName, relPriority));

	/* We insert the array of row IDs in the list of row ID arrays */
	rowIdArrays->insertLast(resultRowIdArray);

	/* We free the allocated memory for the temporary list */
	delete resultRowIds;
}

/**************
 * Destructor *
 **************/

IntermediateArray::~IntermediateArray()
{
	/* We free the allocated memory for each
	 * relation and the list of relations itself
	 */
	relations->traverseFromHead(deleteIntermediateRelation);
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

bool IntermediateArray::search(unsigned int foreignRelationName,
	unsigned int foreignRelationPriority) const
{
	/* We will start looking for the relation name from the head of the list */
	Listnode *currentNode = relations->getHead();

	/* As long as we have not reached the end of the list, we do the following */

	while(currentNode != NULL)
	{
		/* We retrieve the relation in the current node */
		IntermediateRelation *relationOfCurrentNode = (IntermediateRelation *)
			currentNode->getItem();

		/* If the relation in the node matches the
		 * given one, we just found the given relation
		 * in the list. Consequently, we return 'true'
		 */
		if((relationOfCurrentNode->getName() == foreignRelationName)
		&& (relationOfCurrentNode->getPriority() == foreignRelationPriority))
		{
			return true;
		}

		/* We proceed to the next node */
		currentNode = currentNode->getNext();
	}

	/* If this part is reached, that means we didn't find the given relation */
	return false;
}

/********************************************************************
 * Finds and returns the position in the list of the given relation *
 ********************************************************************/

unsigned int IntermediateArray::posOfRelationInList(unsigned int relationName,
	unsigned int relationPriority) const
{
	/* A counter of positions. The position '1'
	 * is the position of the head of the list
	 */
	unsigned int pos = 1;

	/* We will traverse the list from the head */
	Listnode *current = relations->getHead();

	/* As long as we have not finished traversing the list */

	while(current != NULL)
	{
		/* We retrieve the relation stored in the current node */
		IntermediateRelation *ir = (IntermediateRelation *) current->getItem();

		/* If it matches the given arguments, we return
		 * the position in the list where we found it
		 */
		if((ir->getName() == relationName) && (ir->getPriority() == relationPriority))
			return pos;

		/* Else we increase the positions counter by 1,
		 * since we did not find the name in this position
		 */
		pos++;

		/* We proceed to the next node */
		current = current->getNext();
	}

	/* If this part is reached, that means the
	 * given relation does not exist in
	 * the list. In this case we return zero.
	 */
	return 0;
}

/********************************************************
 * Executes 'JOIN' between the intermediate array and a *
 * new relation given as argument along with its tuples *
 ********************************************************/

void IntermediateArray::executeJoinWithForeignRelation(
	unsigned int localRelationName,
	unsigned int localRelationColumn,
	unsigned int localRelationPriority,
	unsigned int foreignRelationName,
	unsigned int foreignRelationColumn,
	unsigned int foreignRelationPriority)
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

	/* We find the position of the local relation in the relations list */
	unsigned int posOfLocalRelInList = posOfRelationInList(
		localRelationName, localRelationPriority);

	/* We use the position we found just above to retrieve
	 * the array of reserved row IDs of the local relation
	 */
	unsigned int *localRowIds = (unsigned int *) rowIdArrays->
		getItemInPos(posOfLocalRelInList);

	/* We fill the tuples of the local relation with contents */

	for(i = 0; i < localTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		localTuples[i].setRowId(i);

		/* We place the corresponding value of the intermediate table to the current tuple */
		localTuples[i].setItem(new unsigned long long(localTable->getTable()
			[localRelationColumn][localRowIds[i]]));
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
		foreignTuples[i].setItem(new unsigned long long(foreignTable->getTable()
			[foreignRelationColumn][i]));
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

	/* We delete the old row ID arrays and remove them from the list */

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

	/* We insert the renewed arrays of reserved row IDs in the structure */

	for(i = 0; i < localRelsCount; i++)
		rowIdArrays->insertLast(renewedLocalRowIdArrays[i]);

	/* We insert the array of valid row IDs of the
	 * foreign relation in the intermediate array
	 */
	rowIdArrays->insertLast(foreignRowIds);

	/* The foreign relation is not foreign anymore.
	 *
	 * We insert its name and priority in the list of relations.
	 */
	relations->insertLast(new IntermediateRelation(foreignRelationName,
		foreignRelationPriority));
}

/********************************************************************
 * Executes 'JOIN' between two relations of this intermediate array *
 ********************************************************************/

void IntermediateArray::executeJoinWithTwoRelationsInTheArray(
	unsigned int leftLocalRelationName,
	unsigned int leftLocalRelationColumn,
	unsigned int leftLocalRelationPriority,
	unsigned int rightLocalRelationName,
	unsigned int rightLocalRelationColumn,
	unsigned int rightLocalRelationPriority)
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

	/* We find the position of the left local relation in the relations list */
	unsigned int posOfLeftLocalRelInList = posOfRelationInList(
		leftLocalRelationName, leftLocalRelationPriority);

	/* We use the position we found just above to retrieve
	 * the array of reserved row IDs of the left local relation
	 */
	unsigned int *leftLocalRowIds = (unsigned int *) rowIdArrays->
		getItemInPos(posOfLeftLocalRelInList);

	/* We fill the left tuples with the data of the intermediate array */

	for(i = 0; i < leftLocalTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		leftLocalTuples[i].setRowId(i);

		/* We place the corresponding value of the intermediate table to the current tuple */
		leftLocalTuples[i].setItem(new unsigned long long(leftLocalTable->getTable()
			[leftLocalRelationColumn][leftLocalRowIds[i]]));
	}

	/* We create the array of tuples for the right array
	 *
	 * Each tuple will have the form <RowIdIntermediateArray,RightLocalValue>
	 */
	Tuple *rightLocalTuples = new Tuple[rightLocalTableRows];

	/* We find the position of the right local relation in the relations list */
	unsigned int posOfRightLocalRelInList = posOfRelationInList(
		rightLocalRelationName, rightLocalRelationPriority);

	/* We use the position we found just above to retrieve
	 * the array of reserved row IDs of the right local relation
	 */
	unsigned int *rightLocalRowIds = (unsigned int *) rowIdArrays->
		getItemInPos(posOfRightLocalRelInList);

	/* We fill the right tuples with the data of the intermediate array */

	for(i = 0; i < rightLocalTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		rightLocalTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		rightLocalTuples[i].setItem(new unsigned long long(rightLocalTable->getTable()
			[rightLocalRelationColumn][rightLocalRowIds[i]]));
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

	/* We retrieve the local arrays of reserved row IDs */

	for(i = 0; i < localRelsCount; i++)
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);

	/* We create a list with all the row IDs of
	 * the Intermediate array that we must save
	 */
	unsigned int finalRowsOfResult = 0;
	List *results = new List();

	for(i = 0; i < resultRowsNum; i++)
	{
		/* We retrieve the current row ID of the intermediate array
		 * that is pointed by the results of the left relation
		 */
		unsigned int currentLeftRowId = resultPairs[i].getLeftRowId();

		/* We retrieve the current row ID of the intermediate array
		 * that is pointed by the results of the right relation
		 */
		unsigned int currentRightRowId = resultPairs[i].getRightRowId();

		/* We store the current row ID of the intermediate array in the
		 * list only if the row IDs pointed by the results are the same
		 */
		if(currentLeftRowId == currentRightRowId)
		{
			/* We increase the amount of row IDs of the result */
			finalRowsOfResult++;

			/* We insert the current row ID in the temporary list */
			results->insertLast(new unsigned int(currentLeftRowId));
		}
	}

	/* We will renew each local array of reserved row IDs
	  with* the contents pointed by the above temporary list
	 */
	for(i = 0; i < localRelsCount; i++)
		renewedLocalRowIdArrays[i] = new unsigned int[finalRowsOfResult];

	/* As long as we have not finished extracting the
	 * results of the list, we do the following actions
	 */
	for(i = 0; i < finalRowsOfResult; i++)
	{
		/* We retrieve the row ID address of the current head of the list */
		unsigned int *currentRowIdAddress = (unsigned int *) results->getItemInPos(1);

		/* We store the content of the address in a seperate variable */
		unsigned int currentRowId = (*currentRowIdAddress);

		/* We free the allocated memory for the above address */
		delete currentRowIdAddress;

		/* We remove the current head of the list */
		results->removeFront();

		/* We use the row ID we retrieved from the list in this repetition
		 * to update the contents of each local array of reserved row IDs
		 */
		for(j = 0; j < localRelsCount; j++)
	 		renewedLocalRowIdArrays[j][i] = localRowIdArrays[j][currentRowId];
	}

	/* We update the amount of row IDs of the intermediate array */
	this->rowsNum = finalRowsOfResult;

	/* We delete the temporary list of results */
	delete results;

	/* We delete and remove every local array from the intermediate array */

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

	/* We insert the renewed arrays in the intermediate array */

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
	unsigned int localRelationPriority,
	unsigned int foreignRelationName,
	unsigned int foreignRelationColumn,
	unsigned int foreignRelationPriority)
{
	/* We retrieve pointers to the original tables with all the data */
	Table *localTable = (Table *) tables->getItemInPos(localRelationName + 1);
	Table *foreignTable = (Table *) tables->getItemInPos(foreignRelationName + 1);

	/* We retrieve the amount of rows of the both tables */
	unsigned long long localTableRows = rowsNum;
	unsigned long long foreignTableRows = other->rowsNum;

	/* Auxiliary variables (used for counting) */
	unsigned long long i, j;

	/* We create the array of tuples for the left array
	 *
	 * Each tuple will have the form <RowIdLocalIntermediateArray (Ih), LocalValue>
	 * (h is an index to an intermediate result inside the current Intermediate Array)
	 */
	Tuple *localTuples = new Tuple[localTableRows];

	/* We find the position of the left local relation in the relations list */
	unsigned int posOfLocalRelInList = posOfRelationInList(
		localRelationName, localRelationPriority);

	/* We use the position we found just above to retrieve
	 * the array of reserved row IDs of the left local relation
	 */
	unsigned int *localRowIds = (unsigned int *) rowIdArrays->
		getItemInPos(posOfLocalRelInList);

	for(i = 0; i < localTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		localTuples[i].setRowId(i);

		/* We place the corresponding value of the intermediate table to the current tuple */
		localTuples[i].setItem(new unsigned long long(localTable->getTable()
			[localRelationColumn][localRowIds[i]]));
	}

	/* We create the array of tuples for the right array
	 *
	 * Each tuple will have the form <RowIdForeignIntermediateArray (Ig), ForeignValue>
	 *
	 * (g is an index to an intermediate result different
	 * from Ih outside of the current Intermediate Array)
	 */
	Tuple *foreignTuples = new Tuple[foreignTableRows];

	/* We find the position of the right relation in the
	 * relations list of the foreign intermediate array
	 */
	unsigned int posOfForeignRelInList = other->posOfRelationInList(
		foreignRelationName, foreignRelationPriority);

	/* We use the position we found just above to retrieve
	 * the array of reserved row IDs of the foreign relation
	 */
	unsigned int *foreignRowIds = (unsigned int *) other->
		rowIdArrays->getItemInPos(posOfForeignRelInList);

	for(i = 0; i < foreignTableRows; i++)
	{
		/* We will take the row IDs in natural order (0, 1, 2, 3, ...) */
		foreignTuples[i].setRowId(i);

		/* We place the corresponding value of the table to the current tuple */
		foreignTuples[i].setItem(new unsigned long long(foreignTable->getTable()
			[foreignRelationColumn][foreignRowIds[i]]));
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

	/* We retrieve the local arrays of this intermediate array */

	for(i = 0; i < localRelsCount; i++)
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);

	/* We retrieve the local arrays of the foreign intermediate array */

	for(i = 0; i < foreignRelsCount; i++)
		foreignRowIdArrays[i] = (unsigned int *) other->rowIdArrays->getItemInPos(i+1);

	/* The total amount of row ID arrays is the sum
	 * row ID arrays of both intermediate arrays
	 */
	unsigned int totalRelsCount = localRelsCount + foreignRelsCount;

	/* We will renew each array (of both intermediate arrays) */
	unsigned int *renewedRowIdArrays[totalRelsCount];

	for(i = 0; i < totalRelsCount; i++)
		renewedRowIdArrays[i] = new unsigned int[resultRowsNum];	

	/* As long as we have not finished traversing the results, we do the following */

	for(i = 0; i < resultRowsNum; i++)
	{
		/* We retrieve the next row ID of this intermediate array */
		unsigned int currentLeftRowId = resultPairs[i].getLeftRowId();

		/* We retrieve the next row ID of the foreign intermediate array */
		unsigned int currentRightRowId = resultPairs[i].getRightRowId();

		/* We update the local arrays of this intermediate array */

		for(j = 0; j < localRelsCount; j++)
			renewedRowIdArrays[j][i] = localRowIdArrays[j][currentLeftRowId];

		/* We update the local arrays of the foreign intermediate array */

		for(j = localRelsCount; j < totalRelsCount; j++)
			renewedRowIdArrays[j][i] = foreignRowIdArrays[j-localRelsCount][currentRightRowId];
	}

	/* We remove the local arrays from the structure */

	rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);

	while(!rowIdArrays->isEmpty())
		rowIdArrays->removeFront();

	/* We remove the local arrays from the foreign structure */

	other->rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);

	while(!other->rowIdArrays->isEmpty())
		other->rowIdArrays->removeFront();

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

	/* The two intermediate arrays will become one.
	 *
	 * We append the relations of the foreign array to this array.
	 */
	Listnode *currentNode = other->relations->getHead();

	/* As long as we have not finished traversing the list
	 * of relation of the foreign array, we do the following
	 */
	while(currentNode != NULL)
	{
		/* We retrieve the relation stored in the current node */
		IntermediateRelation *currentRelation = (IntermediateRelation *) currentNode->getItem();

		/* We append a copy of that relation in the list
		 * of relations of the current intermediate array
		 */
		this->relations->insertLast(new IntermediateRelation(
			currentRelation->getName(), currentRelation->getPriority()));

		/* We proceed to the next node */
		currentNode = currentNode->getNext();
	}

	/* We update the amount of reserved row IDs of each local array */
	this->rowsNum = resultRowsNum;

	/* We insert the renewed row ID arrays of this intermediate array
	 * and the foreign intermediate array in this intermediate array
	 */
	for(i = 0; i < totalRelsCount; i++)
		this->rowIdArrays->insertLast(renewedRowIdArrays[i]);
}

/**********************************************************
 * Applies the given filter to the implied local relation *
 **********************************************************/

void IntermediateArray::executeFilter(
	unsigned int relationName,
	unsigned int relationColumn,
	unsigned int relationPriority,
	unsigned int filterValue,
	char filterOperator)
{
	/* We retrieve a pointer to the original table with all the data */
	Table *table = (Table *) tables->getItemInPos(relationName + 1);

	/* We find the position of the relation in the list of the intermediate array */
	unsigned int relationPosInList = posOfRelationInList(relationName, relationPriority);

	/* We retrieve a pointer to the table with the reserved row IDs for this relation */
	unsigned int *reservedRowIdsOfRel = (unsigned int *) rowIdArrays->
		getItemInPos(relationPosInList);

	/* A list with the row IDs of the intermediate array that contain
	 * row IDs of the given relation that satisfy the filter
	 */
	List *resultRowIds = new List();

	/* Auxiliary variables (used for counting) */
	unsigned int i, j;

	for(i = 0; i < rowsNum; i++)
	{
		/* We retrieve the value of the current element */
		unsigned long long currentElement = table->getTable()
			[relationColumn][reservedRowIdsOfRel[i]];

		/* We initialize an auxiliary boolean flag to 'false' */
		bool elementSatisfiesFilter = false;

		/* Depending on the filter operator we take one of the actions below */

		switch(filterOperator)
		{
			/* Case the filter operator is '<' */

			case '<':
			{
				if(currentElement < filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			/* Case the filter operator is '>' */

			case '>':
			{
				if(currentElement > filterValue)
					elementSatisfiesFilter = true;

				break;
			}

			/* Case the filter operator is '=' */

			case '=':
			{
				if(currentElement == filterValue)
					elementSatisfiesFilter = true;

				break;
			}
		}

		/* If the current element satisfies the filter, we
		 * insert the row ID of that element in the list
		 */
		if(elementSatisfiesFilter)
			resultRowIds->insertLast(new unsigned int(i));
	}

	/* We update the amount of row IDs of the intermediate array */
	this->rowsNum = resultRowIds->getCounter();

	/* We retrieve the amount of relations in the intermediate array */
	unsigned int localRelsCount = relations->getCounter();
	unsigned int *localRowIdArrays[localRelsCount];
	unsigned int *renewedLocalRowIdArrays[localRelsCount];

	/* We retrieve the current row ID arrays
	 * and allocate memory for the new ones
	 */
	for(i = 0; i < localRelsCount; i++)
	{
		localRowIdArrays[i] = (unsigned int *) rowIdArrays->getItemInPos(i+1);
		renewedLocalRowIdArrays[i] = new unsigned int[rowsNum];
	}

	/* We will update the arrays of reserved row IDs */
	i = 0;

	/* As long as the list is not empty, we do the following */

	while(!resultRowIds->isEmpty())
	{
		/* We retrieve the row ID of the current head of the list */
		unsigned int *currentRowIdAddress = (unsigned int *) resultRowIds->getItemInPos(1);

		/* We retrieve the row ID value stored in the address */
		unsigned int currentRowId = (*currentRowIdAddress);

		/* We remove the current head of the list */
		resultRowIds->removeFront();

		/* We free the allocated memory for the row ID */
		delete currentRowIdAddress;

		/* We update the row ID arrays  with the row IDs indicated by the list */

		for(j = 0; j < localRelsCount; j++)
			renewedLocalRowIdArrays[j][i] = localRowIdArrays[j][currentRowId];

		/* We increase the offset for the arrays */
		i++;
	}

	/* We delete the old row ID arrays and remove them from the list */

	rowIdArrays->traverseFromHead(deleteUnsignedIntegerArray);

	while(!rowIdArrays->isEmpty())
		rowIdArrays->removeFront();

	/* We insert the renewed row ID arrays in the structure */

	for(i = 0; i < localRelsCount; i++)
		rowIdArrays->insertLast(renewedLocalRowIdArrays[i]);

	/* We free the allocated memory for the temporary list */
	delete resultRowIds;
}

/*******************************************************
 * Prints the sum of the items in the reserved row IDs *
 *    of the given relation in the requested column    *
 *******************************************************/

void IntermediateArray::produceSum(
	unsigned int relName,
	unsigned int relColumn,
	unsigned int relPriority)
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
	unsigned int posOfRel = posOfRelationInList(relName, relPriority);

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
	relations->traverseFromHead(printIntermediateRelation);

	std::cout << "\nRow ID arrays of the above relations:\n";
	rowIdArrays->traverseFromHead(printUnsignedIntegerArray);
}
