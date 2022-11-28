#ifndef _PAIR_INT_H_
#define _PAIR_INT_H_

class PairInt {

private:

/* A pair int stores an address
 * and an integer as key
 */
	void *item;
	int key;

public:

/* Constructor & Destructor */
	PairInt(void *item, int key = 0);
	~PairInt();

/* Getters */
	void *getItem() const;
	int getKey() const;

/* Setters */
	void setItem(void *item);
	void setKey(int key = 0);

};

#endif