#include <iostream>
#include "QueryHandler.h"

static void test();

/*****************
 * Main Function *
 *****************/

int main(int argc, char const *argv[])
{
	/* We create a query handler giving
	 * the input and configuration files
	 */
	QueryHandler qh = QueryHandler("../input/small.init",
		"../input/small.work", "../config.txt");

	/* We print the data of the query handler */
	//qh.print();

	/* We have the handler address the input queries.
	 * This will print the results in the standard output.
	 */
	qh.addressQueries();

	//test();
}

/*****************
 * Test Function *
 *****************/

void test()
{
	unsigned int i;

	Tuple *tuple_array_1 = new Tuple[1000000];
	unsigned int num_tuples_1 = 1000000;

	for(i = 0; i < num_tuples_1; i++)
	{
		tuple_array_1[i].setItem(new unsigned long long(i));
		tuple_array_1[i].setRowId(i);
	}

	Tuple *tuple_array_2 = new Tuple[1000000];
	unsigned int num_tuples_2 = 1000000;

	for(i = 0; i < num_tuples_2; i++)
	{
		tuple_array_2[i].setItem(new unsigned long long(i + 999970));
		tuple_array_2[i].setRowId(i);
	}

	Relation r1 = Relation(tuple_array_1, num_tuples_1);
	Relation r2 = Relation(tuple_array_2, num_tuples_2);
	PartitionedHashJoinInput phj_params = PartitionedHashJoinInput("../config.txt");

	PartitionedHashJoin phj = PartitionedHashJoin(&r1, &r2, &phj_params);
	RowIdRelation *join = phj.executeJoin();
	phj.printJoinResult(join);

	std::cout << "There are " << join->getNumOfRowIdPairs() << " pairs of Row IDs in the result" << std::endl;

	phj.freeJoinResult(join);

	for(i = 0; i < num_tuples_1; i++)
		delete (unsigned long long *) tuple_array_1[i].getItem();

	for(i = 0; i < num_tuples_2; i++)
		delete (unsigned long long *) tuple_array_2[i].getItem();

	delete[] tuple_array_1;
	delete[] tuple_array_2;
}
