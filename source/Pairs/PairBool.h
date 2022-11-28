#ifndef _PAIR_BOOL_H_
#define _PAIR_BOOL_H_

class PairBool {

private:

/* A pair bool stores an address
 * and a boolean as key
 */
	void *item;
	bool key;

public:

/* Constructor & Destructor */
	PairBool(void *item, bool key = false);
	~PairBool();

/* Getters */
	void *getItem() const;
	bool getKey() const;

/* Setters */
	void setItem(void *item);
	void setKey(bool key = false);

};

#endif