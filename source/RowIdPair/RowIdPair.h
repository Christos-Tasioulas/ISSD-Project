#ifndef _ROW_ID_PAIR_H_
#define _ROW_ID_PAIR_H_

class RowIdPair {

private:

/* The integer that represents the ID of the left row */
	unsigned int leftRowId;

/* The integer that represents the ID of the right row */
	unsigned int rightRowId;

public:

/* Constructor */
	RowIdPair(unsigned int leftRowId = 0, unsigned int rightRowId = 0);

/* Destructor */
	~RowIdPair();

/* Getter - Returns the ID of the left row */
	unsigned int getLeftRowId() const;

/* Getter - Returns the ID of the right row */
	unsigned int getRightRowId() const;

/* Setter - Updates the ID of the left row */
	void setLeftRowId(unsigned int newLeftRowId);

/* Setter - Updates the ID of the right row */
	void setRightRowId(unsigned int newRightRowId);

/* Prints the pair of row IDs that have been stored */
	void print(void (*visit)(unsigned int, unsigned int)) const;

};

#endif
