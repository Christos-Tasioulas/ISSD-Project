#include <iostream>
#include "PairBool.h"

/****************************
 * Constructor & Destructor *
 ****************************/

PairBool::PairBool(void *item, bool key)
{
	this->item = item;
	this->key = key;
}

PairBool::~PairBool() {}

/***********
 * Getters *
 ***********/

void *PairBool::getItem() const
{
	return item;
}

bool PairBool::getKey() const
{
	return key;
}

/***********
 * Setters *
 ***********/

void PairBool::setItem(void *item)
{
	this->item = item;
}

void PairBool::setKey(bool key)
{
	this->key = key;
}