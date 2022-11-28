#ifndef _COMPLEX_ITEM_H_
#define _COMPLEX_ITEM_H_

class ComplexItem {

private:

/* A complex item stores two
 * addresses - an item and a key
 */
	void *item;
	void *key;

public:

/* Constructor & Destructor */
	ComplexItem(void *item, void *key);
	~ComplexItem();

/* Getters */
	void *getItem() const;
	void *getKey() const;

/* Setters */
	void setItem(void *item);
	void setKey(void *key);

};

#endif
