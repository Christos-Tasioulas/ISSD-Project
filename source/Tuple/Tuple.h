#ifndef _TUPLE_H_
#define _TUPLE_H_

class Tuple {

private:

/* The user data that is saved in the tuple */
	void *item;

/* The index of the row where the user data is located */
	unsigned int rowId;

public:

/* Constructor & Destructor */
	Tuple(void *item = NULL, unsigned int rowId = 0);
	~Tuple();

/* Getters */
	void *getItem() const;
	unsigned int getRowId() const;

/* Examines if two tuples have equal user data */
	bool equals(Tuple *other, int (*compare)(void *, void *)) const;

/* Prints the user data and the row ID of the tuple */
	void print(void (*visit)(void *, unsigned int)) const;

};

#endif
