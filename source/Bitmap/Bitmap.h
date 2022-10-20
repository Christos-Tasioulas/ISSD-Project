#ifndef _BITMAP_H_
#define _BITMAP_H_

class Bitmap {

private:

/* An array of bits represented by unsigned short elements */
	unsigned short *bitArray;

/* The number of the unsigned short elements in the array */
	unsigned int size;

/* The number of bits in the array */
	unsigned int bitCapacity;

public:

/* Constructor */
	Bitmap(unsigned int bitCapacity);

/* Destructor */
	~Bitmap();

/* Getter - Returns the array of bits */
	unsigned short *getBitArray() const;

/* Getter - Returns the number of the unsigned short elements in the array */
	unsigned int getSize() const;

/* Getter - Returns the number of bits in the array */
	unsigned int getBitCapacity() const;

/* Sets the value of the bit in the requested position to 1 */
	void setBit(unsigned int pos);

/* Sets the value of the bit in the requested position to 0 */
	void unsetBit(unsigned int pos);

/* Examines whether the bit in the requested position has value 1 */
	bool isSet(unsigned int pos) const;

/* Examines whether all the bits of the sequence have value 1 */
	bool isFull() const;

/* Returns the position of the first 1 in the sequence
 * If no bit with value 1 exists, the operation returns 0
 */
	unsigned int posOfFirstAce() const;

/* Returns the position of the first 1 in the sequence,
 * but this time we start examining from position 'pos'
 * If no bit with value 1 exists, the operation returns 0
 *
 * If the given position is zero or greater than the bit
 * capacity, the user is informed that an error has taken
 * place. The user may pass the address of a boolean
 * variable and if they do, 'true' will be assigned in
 * case an invalid position has been given as argument,
 * else the value 'false' will assigned to the variable
 */
	unsigned int posOfFirstAceFromPos(unsigned int pos,
		bool *errorOccured = NULL) const;

/* Prints the bitmap */
	void print() const;

/* Destroy the bitmap and create it again with new size */
	void resetAndChangeSize(unsigned int newBitCapacity);

};

#endif
