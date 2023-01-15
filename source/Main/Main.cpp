#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "QueryHandler.h"

/*****************
 * Main Function *
 *****************/

int main(int argc, char const *argv[])
{
	/* We read the dataset type from the configuration file */
	char *dataset;
	FileReader::readDataset("../config.txt", &dataset);

	/* We find the length of the string with the dataset type */
	unsigned int datasetLen = strlen(dataset);

	/* We find the length of the "init" file we need to use */
	char initFile[strlen("../input/") + datasetLen
		+ 1 + datasetLen + strlen(".init") + 1];

	/* We find the length of the "work" file we need to use */
	char workFile[strlen("../input/") + datasetLen
		+ 1 + datasetLen + strlen(".work") + 1];

	/* We find the length of the "config" file we need to use */
	char configFile[strlen("../config.txt") + 1];

	/* We fill the "initFile" string with the proper path */
	sprintf(initFile, "../input/%s/%s.init", dataset, dataset);

	/* We fill the "initFile" string with the proper path */
	sprintf(workFile, "../input/%s/%s.work", dataset, dataset);

	/* We fill the "configFile" string with the proper path */
	strcpy(configFile, "../config.txt");

	/* We create a query handler giving the input and configuration files */
	QueryHandler qh = QueryHandler(initFile, workFile, configFile);

	/* We print the data of the query handler */
	//qh.print();

	/* We have the handler address the input queries.
	 * This will print the results in the standard output.
	 */
	qh.addressQueries();

	/* We free the allocated memory for the dataset type */
	free(dataset);
}
