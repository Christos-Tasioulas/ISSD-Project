#include <iostream>
#include "QueryHandler.h"

/*****************
 * Main Function *
 *****************/

int main(int argc, char const *argv[])
{
	QueryHandler qh = QueryHandler("../input/small.init",
		"../input/small.work", "../config.txt");

	//qh.print();

	qh.addressQueries("../results.txt");
}
