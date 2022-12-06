#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include "Relation.h"
#include "Table.h"
#include "Query.h"

/* A namespace with operations that read data from the
 * input files and the configuration file of the program
 */
namespace FileReader {

/* Reads the input file and stores the user's input to the
 * given addresses passed as arguments to the function
 */
void readInputFile(
    const char *input_file,
    Relation **relR,
    Relation **relS
);

/* Reads the "init" file that contains all the relation names that
 * will take part in the follow-up queries. Then it reads the binary
 * input file of each relation. Returns a linked list that stores a
 * (Table *) in each node. Each 'Table' object stores all the rows
 * and columns of a relation.
 */
List *readInitFile(const char *init_file);

/* Reads the "work" file that contains all the queries that will
 * be given to the program. Every query is stored in a 'Query'
 * object. A linked list of lists will be returned. Each inner
 * linked list contains a full batch of input queries. The outer
 * list contains all the input batches.
 */
List *readWorkFile(const char *work_file);

/* Reads the given configuration file and returns the
 * value of each option in the file through the memory
 * addresses that are given as arguments to the function.
 */
void readConfigFile(
    const char *config_file,
    unsigned int *bitsNumForHashing,
    bool *showInitialRelations,
    bool *showAuxiliaryArrays,
    bool *showHashTable,
    bool *showSubrelations,
    bool *showResult,
    unsigned int *hopscotchBuckets,
    unsigned int *hopscotchRange,
    bool *resizableByLoadFactor,
    double *loadFactor,
    double *maxAllowedSizeModifier,
    unsigned int *maxPartitionDepth
);

};

#endif
