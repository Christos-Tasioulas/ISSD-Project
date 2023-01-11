#include <iostream>
#include "B_Tree.h"

/****************************
 * Constructor & Destructor *
 ****************************/

B_Tree::B_Tree(unsigned int B) : AB_Tree((B+1)/2, B) {}

B_Tree::~B_Tree() {}
