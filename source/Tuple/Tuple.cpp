#include <iostream>
#include "Tuple.h"

/****************************
 * Constructor & Destructor *
 ****************************/

Tuple::Tuple(void *item, unsigned int rowId)
{
	/* We assign the given item to the 'item' field */
	this->item = item;

	/* We assign the given row ID to the 'rowId' field */
	this->rowId = rowId;
}

Tuple::~Tuple() {}

/********************
 * Copy Constructor *
 ********************/

Tuple::Tuple(const Tuple& other)
{
	/* We copy the item of the other tuple to this one */
	item = other.item;

	/* We copy the row ID of the other tuple to this one */
	rowId = other.rowId;
}

/*************************************************************
 * Getter - Returns the user data that is saved in the tuple *
 *************************************************************/

void *Tuple::getItem() const
{
	return item;
}

/************************************************************************
 * Getter - Returns the index of the row where the user data is located *
 ************************************************************************/

unsigned int Tuple::getRowId() const
{
	return rowId;
}

/*************************************************************
 * Setter - Updates the user data that is saved in the tuple *
 *************************************************************/

void Tuple::setItem(void *newItem)
{
	item = newItem;
}

/************************************************************************
 * Setter - Updates the index of the row where the user data is located *
 ************************************************************************/

void Tuple::setRowId(unsigned int newRowId)
{
	rowId = newRowId;
}

/***********************************************
 * Examines if two tuples have equal user data *
 ***********************************************/

bool Tuple::equals(Tuple *other, int (*compare)(void *, void *)) const
{
	return (compare(item, other->item) == 0);
}

/****************************************************
 * Prints the user data and the row ID of the tuple *
 ****************************************************/

void Tuple::print(void (*visit)(void *, unsigned int)) const
{
	visit(item, rowId);
}
