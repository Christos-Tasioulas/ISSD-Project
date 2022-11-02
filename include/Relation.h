#ifndef _RELATION_H_
#define _RELATION_H_

#include "Tuple.h"

/* The Relation Structure
 *
 * It consists of an array of tuples which is stored
 * in the heap and the amount of tuples of the array
 */
class Relation {

private:

/* The array of tuples (always in the heap) */
	Tuple *tuples;

/* The amount of tuples in the array */
	unsigned int numOfTuples;

public:

/* Constructor & Destructor */
	Relation(Tuple *tuples = NULL, unsigned int numOfTuples = 0);
	~Relation();

/* Getters */
	Tuple *getTuples() const;
	unsigned int getNumOfTuples() const;

/* Setters */
	void setTuples(Tuple *newTuples);
	void setNumOfTuples(unsigned int newNumOfTuples);

/* Returns the size in bytes of the relation */
	unsigned int getSize() const;

/* Prints all the tuples of the relation */
	void print(void (*visitTuple)(void *, unsigned int),
		void (*contextBetweenTuples)() = NULL) const;

};

#endif
