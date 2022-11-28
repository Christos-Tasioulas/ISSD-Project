#include <iostream>
#include "QueryHandler.h"

/*****************
 * Main Function *
 *****************/

int main(int argc, char const *argv[])
{
	QueryHandler qh = QueryHandler("../input/small.init", "../input/small.work");
	qh.print();
}
