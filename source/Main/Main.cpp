#include <iostream>
#include <unistd.h>
#include "PartitionedHashJoin.h"

static void deleteTable(void *item)
{
	Table *table = (Table *) item;
	delete table;
}

static void printContents(void *item)
{
	Table *table = (Table *) item;
	table->print();
}

int main(int argc, char const *argv[])
{
	List *list = FileReader::initialize("../input/small.init");
	std::cout << "Done" << std::endl;
	std::cout << "Sleeping for 1 second..." << std::endl;
	sleep(1);

	list->traverseFromHead(printContents);
	list->traverseFromHead(deleteTable);
	delete list;

	/* We create a 'PartitionedHashJoin' object by giving
	 * the input and configuration files to initialize it
	 */
	//PartitionedHashJoin phj = PartitionedHashJoin("../input.txt", "../config.txt");

	/* We use the initialized object to perform the join */
	//RowIdRelation *result = phj.executeJoin();

	/* We display the result returned by the operation */
	//phj.printJoinResult(result);

	/* We terminate the result returned by the operation */
	//phj.freeJoinResult(result);
}
