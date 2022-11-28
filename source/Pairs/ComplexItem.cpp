#include <iostream>
#include "ComplexItem.h"

/****************************
 * Constructor & Destructor *
 ****************************/

ComplexItem::ComplexItem(void *item, void *key)
{
	this->item = item;
	this->key = key;
}

ComplexItem::~ComplexItem() {}

/***********
 * Getters *
 ***********/

void *ComplexItem::getItem() const
{
	return item;
}

void *ComplexItem::getKey() const
{
	return key;
}

/***********
 * Setters *
 ***********/

void ComplexItem::setItem(void *item)
{
	this->item = item;
}

void ComplexItem::setKey(void *key)
{
	this->key = key;
}