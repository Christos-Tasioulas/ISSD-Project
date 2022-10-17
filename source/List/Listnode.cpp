#include <iostream>
#include "Listnode.h"

/****************************
 * Constructor & Destructor *
 ****************************/

Listnode::Listnode(void *item)
{
	this->item = item;
	next = NULL;
	previous = NULL;
}

Listnode::~Listnode() {}

/***********
 * Getters *
 ***********/

void *Listnode::getItem() const
{
	return item;
}

Listnode *Listnode::getNext() const
{
	return next;
}

Listnode *Listnode::getPrevious() const
{
	return previous;
}

/***********
 * Setters *
 ***********/

void Listnode::setItem(void *new_item)
{
	item = new_item;
}

void Listnode::setNext(Listnode *new_next)
{
	next = new_next;
}

void Listnode::setPrevious(Listnode *new_previous)
{
	previous = new_previous;
}

/************
 * Printing *
 ************/

void Listnode::print(void (*visit)(void *)) const
{
	visit(item);
}