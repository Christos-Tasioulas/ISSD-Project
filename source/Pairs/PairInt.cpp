#include <iostream>
#include "PairInt.h"

/****************************
 * Constructor & Destructor *
 ****************************/

PairInt::PairInt(void *item, int key)
{
	this->item = item;
	this->key = key;
}

PairInt::~PairInt() {}

/***********
 * Getters *
 ***********/

void *PairInt::getItem() const
{
	return item;
}

int PairInt::getKey() const
{
	return key;
}

/***********
 * Setters *
 ***********/

void PairInt::setItem(void *item)
{
	this->item = item;
}

void PairInt::setKey(int key)
{
	this->key = key;
}