#include <iostream>
#include "PartitionedHashJoin.h"

int main(int argc, char const *argv[])
{
	/* We create a 'PartitionedHashJoin' object by giving
	 * the input and configuration files to initialize it
	 */
	PartitionedHashJoin phj = PartitionedHashJoin(NULL, "../config.txt");

	/* We use the initialized object to perform the join */
	RowIdRelation *result = phj.executeJoin();

	/* We display the result returned by the operation */
	phj.printJoinResult(result);

	/* We terminate the result returned by the operation */
	phj.freeJoinResult(result);
}
