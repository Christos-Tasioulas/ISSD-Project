#include <iostream>
#include <unistd.h>
#include "PartitionedHashJoin.h"

static void printTable(void *item)
{
	Table *table = (Table *) item;
	table->print();
}

static void deleteTable(void *item)
{
	Table *table = (Table *) item;
	delete table;
}

static void printQuery(void *item)
{
	Query *query = (Query *) item;
	query->print();
}

static void deleteQuery(void *item)
{
	Query *query = (Query *) item;
	delete query;
}

static void printBatch(void *item)
{
	List *batch = (List *) item;
	std::cout << "================ Next Batch ================" << std::endl;
	batch->traverseFromHead(printQuery);
}

static void deleteBatch(void *item)
{
	List *batch = (List *) item;
	batch->traverseFromHead(deleteQuery);
	delete batch;
}

int main(int argc, char const *argv[])
{
	List *tables = FileReader::readInitFile("../input/small.init");
	std::cout << "Done" << std::endl;
	std::cout << "Sleeping for 1 second..." << std::endl;
	sleep(1);

	tables->traverseFromHead(printTable);
	tables->traverseFromHead(deleteTable);
	delete tables;

	List *batches = FileReader::readWorkFile("../input/small.work");
	batches->traverseFromHead(printBatch);
	batches->traverseFromHead(deleteBatch);
	delete batches;

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
