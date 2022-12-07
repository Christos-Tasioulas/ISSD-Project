#include <iostream>
#include "IntermediateRelation.h"

/***************
 * Constructor *
 ***************/

IntermediateRelation::IntermediateRelation(unsigned int name, unsigned int priority)
{
    this->name = name;
    this->priority = priority;
}

/**************
 * Destructor *
 **************/

IntermediateRelation::~IntermediateRelation() {}

/*********************************************
 * Getter - Returns the name of the relation *
 *********************************************/

unsigned int IntermediateRelation::getName() const
{
    return name;
}

/*************************************************
 * Getter - Returns the priority of the relation *
 *************************************************/

unsigned int IntermediateRelation::getPriority() const
{
    return priority;
}

/***********************************************
 * Prints the data of an intermediate relation *
 ***********************************************/

void IntermediateRelation::print() const
{
    std::cout << "{" << name << "," << priority << "}";
}
