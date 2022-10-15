#include <iostream>
#include "PartitionedHashJoin.h"

int main(int argc, char const *argv[])
{
	/* We create a 'PartitionedHashJoin' object by giving
	 * the input and configuration files to initialize it
	 */
	PartitionedHashJoin phj = PartitionedHashJoin(NULL, NULL);

	/* We use the initialized object to perform the join */
	phj.executeJoin();
}
