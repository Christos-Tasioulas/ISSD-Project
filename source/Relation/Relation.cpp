#include <iostream>
#include "Relation.h"

/****************************
 * Constructor & Destructor *
 ****************************/

Relation::Relation(Tuple *tuples, unsigned int numOfTuples)
{
	this->tuples = tuples;
	this->numOfTuples = numOfTuples;
}

Relation::~Relation() {}

/****************************************
 * Getter - Returns the array of tuples *
 ****************************************/

Tuple *Relation::getTuples() const
{
	return tuples;
}

/******************************************************
 * Getter - Returns the amount of tuples in the array *
 ******************************************************/

unsigned int Relation::getNumOfTuples() const
{
	return numOfTuples;
}

/****************************************
 * Setter - Updates the array of tuples *
 ****************************************/

void Relation::setTuples(Tuple *newTuples)
{
    tuples = newTuples;
}

/******************************************************
 * Setter - Updates the amount of tuples in the array *
 ******************************************************/

void Relation::setNumOfTuples(unsigned int newNumOfTuples)
{
    numOfTuples = newNumOfTuples;
}

/*********************************************
 * Returns the size in bytes of the relation *
 *********************************************/

unsigned int Relation::getSize() const
{
    return numOfTuples * sizeof(Tuple);
}

/*****************************************
 * Prints all the tuples of the relation *
 *****************************************/

void Relation::print(void (*visitTuple)(void *, unsigned int),
    void (*contextBetweenTuples)()) const
{
    /* Auxiliary variable used for counting */
    unsigned int i;

    /* If a no-null 'contextBetweenTuples' operation has been
     * given to print context between the tuples, we call it now
     */
    if(contextBetweenTuples != NULL)
        contextBetweenTuples();

    /* Here we will print all the tuples of the relation */

    for(i = 0; i < numOfTuples; i++)
    {
        /* We print the next tuple */
        tuples[i].print(visitTuple);

        /* If a no-null 'contextBetweenTuples' operation has been
         * given to print context between the tuples, we call it now
         */
        if(contextBetweenTuples != NULL)
            contextBetweenTuples();
    }
}
