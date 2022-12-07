#include <iostream>
#include "QueryHandler.h"

/*****************
 * Main Function *
 *****************/

int main(int argc, char const *argv[])
{
	/* We create a query handler giving
	 * the input and configuration files
	 */
	QueryHandler qh = QueryHandler(
		"../input/small.init",
		"../input/small.work",
		"../config.txt");

	/* We print the data of the query handler */
	//qh.print();

	/* We have the handler address the input queries.
	 * This will print the results in the standard output.
	 */
	qh.addressQueries();
}
