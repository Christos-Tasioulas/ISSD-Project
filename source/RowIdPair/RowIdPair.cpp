#include <iostream>
#include "RowIdPair.h"

/***************
 * Constructor *
 ***************/

RowIdPair::RowIdPair(unsigned int leftRowId, unsigned int rightRowId)
{
	/* We store the given left row ID to the 'leftRowId' field */
	this->leftRowId = leftRowId;

	/* We store the given right row ID to the 'rightRowId' field */
	this->rightRowId = rightRowId;
}

/**************
 * Destructor *
 **************/

RowIdPair::~RowIdPair() {}

/*******************************************
 * Getter - Returns the ID of the left row *
 *******************************************/

unsigned int RowIdPair::getLeftRowId() const
{
	return leftRowId;
}

/********************************************
 * Getter - Returns the ID of the right row *
 ********************************************/

unsigned int RowIdPair::getRightRowId() const
{
	return rightRowId;
}

/*******************************************
 * Setter - Updates the ID of the left row *
 *******************************************/

void RowIdPair::setLeftRowId(unsigned int newLeftRowId)
{
	leftRowId = newLeftRowId;
}

/********************************************
 * Setter - Updates the ID of the right row *
 ********************************************/

void RowIdPair::setRightRowId(unsigned int newRightRowId)
{
	rightRowId = newRightRowId;
}

/****************************************************
 * Prints the pair of row IDs that have been stored *
 ****************************************************/

void RowIdPair::print(void (*visit)(unsigned int, unsigned int)) const
{
	visit(leftRowId, rightRowId);
}
