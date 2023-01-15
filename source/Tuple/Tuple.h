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

/* Copy Constructor */
	Tuple(const Tuple& other);

/* Getters */
	void *getItem() const;
	unsigned int getRowId() const;

/* Setters */
	void setItem(void *newItem);
	void setRowId(unsigned int newRowId);

/* Examines if two tuples have equal user data */
	bool hasEqualItem(Tuple *other, int (*compare)(void *, void *)) const;

/* Examines if two tuples have equal row ID */
	bool hasEqualRowId(Tuple *other) const;

/* Prints the user data and the row ID of the tuple */
	void print(void (*visit)(void *, unsigned int)) const;

/* Determines what happens when we assign a tuple to a variable */
	void operator=(const Tuple& other);

};

#endif
