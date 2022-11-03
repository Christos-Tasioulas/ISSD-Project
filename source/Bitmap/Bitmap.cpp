#include <iostream>
#include "Bitmap.h"

/***************
 * Constructor *
 ***************/

Bitmap::Bitmap(unsigned int bitCapacity)
{
	/* This is the number of bits of an unsigned short integer */
	unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

	/* By dividing the requested capacity of bits by the number of bits
	 * of an unsigned short integer we find the amount of unsigned short
	 * integers we will need to represent the requested capacity of bits.
	 * The last integer may have some spare bits that will not be used.
	 *
	 * If the modulus of this division is non-zero, then there will be
	 * some spare bits of the last integer that will not be used. The
	 * value of the modulus implies the amount of spare bits. The 'size'
	 * field determines the amount of unsigned short integers we will
	 * need for the whole representation. We will need as many integers
	 * as the quotient of the division, plus 1 more integer if the mod
	 * of the division is not zero (the latter will have spare bits).
	 */
	if(bitCapacity % bits_per_u16 != 0)
		size = (bitCapacity / bits_per_u16) + 1;

	/* Case we will not have any spare bits. This will happen when the
	 * requested capacity is a multiple of 16 (= bits of short integer).
	 *
	 * For this reason, ideally the capacity should be a multiple of 16.
	 */
	else
		size = bitCapacity / bits_per_u16;

	/* We allocate memory in the heap for the amount
	 * of unsigned short integers that we will need
	 */
	bitArray = new unsigned short[size];

	/* We initialize all those unsigned shorts to 0 */
	unsigned int i;

	for(i = 0; i < size; i++)
		bitArray[i] = 0;

	/* We store the bit capacity in the 'bitCapacity' field of the class */
	this->bitCapacity = bitCapacity;
}

/**************
 * Destructor *
 **************/

Bitmap::~Bitmap()
{
	delete[] bitArray;
}

/**************************************
 * Getter - Returns the array of bits *
 **************************************/

unsigned short *Bitmap::getBitArray() const
{
	return bitArray;
}

/***************************************************************************
 * Getter - Returns the number of the unsigned short elements in the array *
 ***************************************************************************/

unsigned int Bitmap::getSize() const
{
	return size;
}

/****************************************************
 * Getter - Returns the number of bits in the array *
 ****************************************************/

unsigned int Bitmap::getBitCapacity() const
{
	return bitCapacity;
}

/************************************************************
 * Sets the value of the bit in the requested position to 1 *
 ************************************************************/

void Bitmap::setBit(unsigned int pos)
{
	/* This operation does not have any meaning
	 * if the requested position is out of bounds
	 */
	if(pos > bitCapacity || pos == 0)
		return;

	/* This is the number of bits of an unsigned short integer */
	unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

	/* This is the unsigned short integer the requested bit belongs to */
	unsigned int targeted_u16;

	/* We need to identify the targeted integer. By dividing the
	 * requested position by 16 we find the targeted integer. If
	 * the modulus of the division is non-zero, then the targeted
	 * integer is the next of the one implied by the quotient.
	 *
	 * Case the targeted integer is the next of the one implied
	 * by the result of the division
	 */
	if(pos % bits_per_u16 != 0)
		targeted_u16 = (pos / bits_per_u16) + 1;

	/* Case the requested position is a multiple of 16 */

	else
		targeted_u16 = pos / bits_per_u16;

	/* We identify which bit of the targeted integer we have
	 * to set. The position of that bit varies from 1 to 16.
	 */
	unsigned int bit_pos_in_target = bits_per_u16
		- (targeted_u16 * bits_per_u16 - pos);

	/* Since we want to change a bit from 0 to 1, we have to
	 * add a specific power of 2 to the targeted integer.
	 */
	unsigned short valueToBeAdded = 1;
	unsigned int i;

	/* According to the position of the requested bit, the
	 * power that must be used needs to change. The greater
	 * the position of the bit, the greater the power of 2
	 */
	for(i = 1; i < bit_pos_in_target; i++)
		valueToBeAdded *= 2;

	/* Now we have the correct power of 2 that we need to
	 * add. But if the requested bit has already been set
	 * to 1 in the past, we must not add that power again
	 * and instead return immediatelly. In order to check
	 * whether the bit has been set, we create a mask with
	 * the value of the power of 2 that we created above.
	 * If the bitwise AND between the targeted integer and
	 * the mask is zero, then the bit has value 0 and that
	 * means we must add the power of 2 we created.
	 */
	unsigned short mask = valueToBeAdded;

	/* Here we examine the condition under which we change
	 * the bit and if it is true, we set the bit value to 1
	 */
	if((bitArray[targeted_u16 - 1] & mask) == 0)
		bitArray[targeted_u16 - 1] += valueToBeAdded;
}

/************************************************************
 * Sets the value of the bit in the requested position to 0 *
 ************************************************************/

void Bitmap::unsetBit(unsigned int pos)
{
	/* This operation does not have any meaning
	 * if the requested position is out of bounds
	 */
	if(pos > bitCapacity || pos == 0)
		return;

	/* This is the number of bits of an unsigned short integer */
	unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

	/* This is the unsigned short integer the requested bit belongs to */
	unsigned int targeted_u16;

	/* We need to identify the targeted integer. By dividing the
	 * requested position by 16 we find the targeted integer. If
	 * the modulus of the division is non-zero, then the targeted
	 * integer is the next of the one implied by the quotient.
	 *
	 * Case the targeted integer is the next of the one implied
	 * by the result of the division
	 */
	if(pos % bits_per_u16 != 0)
		targeted_u16 = (pos / bits_per_u16) + 1;

	/* Case the requested position is a multiple of 16 */

	else
		targeted_u16 = pos / bits_per_u16;

	/* We identify which bit of the targeted integer we have
	 * to unset. The position of that bit varies from 1 to 16.
	 */
	unsigned int bit_pos_in_target = bits_per_u16
		- (targeted_u16 * bits_per_u16 - pos);

	/* Since we want to change a bit from 1 to 0, we have to
	 * subtract a specific power of 2 from the targeted integer.
	 */
	unsigned short valueToBeSubtracted = 1;
	unsigned int i;

	/* According to the position of the requested bit, the
	 * power that must be used needs to change. The greater
	 * the position of the bit, the greater the power of 2
	 */
	for(i = 1; i < bit_pos_in_target; i++)
		valueToBeSubtracted *= 2;

	/* Now we have the correct power of 2 that we need to
	 * subtract. But if the requested bit has already the
	 * value 0, we must not subtract that power again and
	 * instead return immediatelly. In order to check if
	 * the bit has been already 0, we create a mask with
	 * the value of the power of 2 that we created above.
	 * If the bitwise AND between the targeted integer and
	 * the mask is equal to the mask itself, then the bit
	 * has value 1 and that means we must subtract the power
	 * of 2 we created.
	 */
	unsigned short mask = valueToBeSubtracted;

	/* Here we examine the condition under which we change
	 * the bit and if it is true, we set the bit value to 0
	 */
	if((bitArray[targeted_u16 - 1] & mask) == mask)
		bitArray[targeted_u16 - 1] -= valueToBeSubtracted;
}

/******************************************************************
 * Examines whether the bit in the requested position has value 1 *
 ******************************************************************/

bool Bitmap::isSet(unsigned int pos) const
{
	/* There is no bit in a position greater than the capacity
	 * of bits itself. In case such a position is given, we
	 * do nothing and just return the arbitrary value 'false'
	 *
	 * Same if the requested position is zero
	 */
	if(pos > bitCapacity || pos == 0)
		return false;

	/* This is the number of bits of an unsigned short integer */
	unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

	/* This is the unsigned short integer the requested bit belongs to */
	unsigned int targeted_u16;

	/* We need to identify the targeted integer. By dividing the
	 * requested position by 16 we find the targeted integer. If
	 * the modulus of the division is non-zero, then the targeted
	 * integer is the next of the one implied by the quotient.
	 *
	 * Case the targeted integer is the next of the one implied
	 * by the result of the division
	 */
	if(pos % bits_per_u16 != 0)
		targeted_u16 = (pos / bits_per_u16) + 1;

	/* Case the requested position is a multiple of 16 */

	else
		targeted_u16 = pos / bits_per_u16;

	/* We identify which bit of the targeted integer we have
	 * to check. The position of that bit varies from 1 to 16.
	 */
	unsigned int bit_pos_in_target = bits_per_u16
		- (targeted_u16 * bits_per_u16 - pos);

	/* We will use a mask to examine if that bit is set to 1.
	 *
	 * First we have to create the proper mask. The mask we
	 * have to use varies according to the position of the bit.
	 *
	 * The mask will be definetely a power of 2.
	 */
	unsigned short mask = 1;
	unsigned int i;

	/* According to the position of the requested bit, the
	 * power that must be used needs to change. The greater
	 * the position of the bit, the greater the power of 2
	 */
	for(i = 1; i < bit_pos_in_target; i++)
		mask *= 2;

	/* Now we have the correct mask. We will perform a bitwise
	 * AND operation between the targeted integer and the mask.
	 * If the result is zero, then the requested bit is zero.
	 *
	 * Here we examine whether the result of the AND operation
	 * is zero and if it is, we return 'false' (the bit is 0).
	 */
	if((bitArray[targeted_u16 - 1] & mask) == 0)
		return false;

	/* Else if the result is not zero, it is equal to the mask
	 * itself and that implies the requested bit has value 1.
	 *
	 * In this case we return 'true', because the bit has value 1.
	 */
	return true;
}

/**************************************************************
 * Examines whether all the bits of the sequence have value 1 *
 **************************************************************/

bool Bitmap::isFull() const
{
	/* Helper variable for counting */
	unsigned int i;

	/* We examine for each bit of the array whether it has the value
	 * 1 or 0. We do this by using the 'isSet' operation we have built
	 * above. This method returns 'true' only if all the bits of the
	 * sequence have value 1. Even if one bit in the array has value 0,
	 * this operation will return 'false'.
	 */
	for(i = 1; i <= bitCapacity; i++)
	{
		/* Here we examine if the i-th bit has value 0.
		 * If it does, we immeditelly return 'false'.
		 */
		if(!isSet(i))
			return false;
	}

	/* If this part is reached, that means all the bits have value 1 */

	return true;
}

/************************************************************
 *   Returns the position of the first 1 in the sequence    *
 *  If no bit with value 1 exists, the operation returns 0  *
 ************************************************************/

unsigned int Bitmap::posOfFirstAce() const
{
	/* Helper variable for counting */
	unsigned int i;

	/* We examine for each bit of the array whether it has
	 * the value 1 or 0. We do this by using the 'isSet'
	 * operation we have built above.
	 */
	for(i = 1; i <= bitCapacity; i++)
	{
		/* If the current bit has value 1,
		 * we return its position
		 */
		if(isSet(i))
			return i;
	}

	/* If this part is reached, that means all the
	 * bits have value 0. Consequently, we return 0
	 */
	return 0;
}

/**********************************************************
 *  Returns the position of the first 1 in the sequence,  *
 *  but this time we start examining from position 'pos'  *
 * If no bit with value 1 exists, the operation returns 0 *
 *                                                        *
 * If the given position is zero or greater than the bit  *
 * capacity, the user is informed that an error has taken *
 *   place. The user may pass the address of a boolean    *
 *  variable and if they do, 'true' will be assigned in   *
 *  case an invalid position has been given as argument,  *
 *  else the value 'false' will assigned to the variable  *
 **********************************************************/

unsigned int Bitmap::posOfFirstAceFromPos(unsigned int pos,
	bool *errorOccured) const
{
	/* Zero is an invalid position. In case zero has been
	 * given as argument, we set the given position to 1
	 *
	 * Essentially, we calculate the position of the 1st ace
	 */
	if(pos == 0 || pos > bitCapacity)
	{
		/* If a no-null boolean address has been given,
		 * we assign the value 'true' to its content.
		 */
		if(errorOccured != NULL)
			(*errorOccured) = true;

		/* We return zero arbitrarily in this case */
		return 0;
	}

	/* If a no-null boolean address has been given,
	 * we assign the value 'false' to its content,
	 * since we now know that no error will take place.
	 */
	if(errorOccured != NULL)
		(*errorOccured) = false;

	/* Helper variable for counting */
	unsigned int i;

	/* We examine for each bit of the array, starting from
	 * the position 'pos', whether it has the value 1 or 0.
	 * We do this by using the 'isSet' method of the class.
	 */
	for(i = pos; i <= bitCapacity; i++)
	{
		/* If the current bit has value 1,
		 * we return its position
		 */
		if(isSet(i))
			return i;
	}

	/* If this part is reached, that means all the
	 * bits have value 0. Consequently, we return 0
	 */
	return 0;
}

/*********************
 * Prints the bitmap *
 *********************/

void Bitmap::print() const
{
	/* We want to print a series of 0s and 1s that will
	 * represent the value of each bit in the bit array
	 */
	unsigned int i, bitsPrinted = 0;

	/* To do this, we will print each unsigned short
	 * integer one next to the other in their binary
	 * form (= the way they are written in the binary
	 * system). For aesthetic beauty, we will print
	 * four integers one next to the other, then we
	 * will change line, then we will print the next
	 * four integers and so on (all in binary form).
	 */
	for(i = 0; i < size; i++)
	{
		/* We always remember the amount of bits of a short integer */
		unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

		/* We will use a mask to examine whether the value of each
		 * bit is 1 or 0. The mask will always be a power of 2.
		 */
		unsigned short mask = 1;

		/* Helper variable for loop counting */
		unsigned int j;

		/* In the inner loop we print all the bit values of one
		 * unsigned short integer. That integer is the one whose
		 * position in the array is equal to the variable 'i'.
		 */
		for(j = 0; j < bits_per_u16; j++)
		{
			/* If we have printed as many bits as the capacity
			 * of bits in the array, we exit immediatelly
			 */
			if(bitsPrinted == bitCapacity)
				break;

			/* If the bitwise AND operation between the integer
			 * and the mask is equal to the mask itself, the
			 * current bit has value 1, so we print the string "1"
			 */
			if((bitArray[i] & mask) == mask)
				std::cout << "1";

			/* Else the bit has value 0, so we print "0" */

			else
				std::cout << "0";

			/* We just printed one more bit. We need to
			 * increase the amount of printed bits by 1
			 */
			bitsPrinted++;

			/* The mask grows to the next power of 2 so
			 * we can examine the value of the next bit
			 */
			mask *= 2;
		}

		/* We print a white space to distinguish the
		 * integers that are printed in the same row
		 */
		std::cout << " ";

		/* If we have printed as many bits as the capacity
		 * of the bit array, we exit immediatelly. We need
		 * to increase 'i' by 1 for the instruction below
		 */
		if(bitsPrinted == bitCapacity)
		{
			i++;
			break;
		}
	}
}

/********************************************************
 * Destroy the bitmap and create it again with new size *
 ********************************************************/

void Bitmap::resetAndChangeSize(unsigned int newBitCapacity)
{
	/* First we destroy the current bitmap */
	delete[] bitArray;

	/* This is the number of bits of an unsigned short integer */
	unsigned int bits_per_u16 = sizeof(unsigned short) * 8;

	/* By dividing the requested capacity of bits by the number of bits
	 * of an unsigned short integer we find the amount of unsigned short
	 * integers we will need to represent the requested capacity of bits.
	 * The last integer may have some spare bits that will not be used.
	 *
	 * If the modulus of this division is non-zero, then there will be
	 * some spare bits of the last integer that will not be used. The
	 * value of the modulus implies the amount of spare bits. The 'size'
	 * field determines the amount of unsigned short integers we will
	 * need for the whole representation. We will need as many integers
	 * as the quotient of the division, plus 1 more integer if the mod
	 * of the division is not zero (the latter will have spare bits).
	 */
	if(newBitCapacity % bits_per_u16 != 0)
		size = (newBitCapacity / bits_per_u16) + 1;

	/* Case we will not have any spare bits. This will happen when the
	 * requested capacity is a multiple of 16 (= bits of short integer).
	 *
	 * For this reason, ideally the capacity should be a multiple of 16.
	 */
	else
		size = newBitCapacity / bits_per_u16;

	/* We allocate memory in the heap for the amount
	 * of unsigned short integers that we will need
	 */
	bitArray = new unsigned short[size];

	/* We initialize all those unsigned shorts to 0 */
	unsigned int i;

	for(i = 0; i < size; i++)
		bitArray[i] = 0;

	/* We store the bit capacity in the 'bitCapacity' field of the class */
	this->bitCapacity = newBitCapacity;
}
