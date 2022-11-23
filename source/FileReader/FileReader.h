#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include "Relation.h"
#include "Table.h"

/* A namespace with operations that read data from the
 * input file and the configuration file of the user
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

/* Reads the given configuration file and returns the value
 * of each option in the file through the memory addresses
 * that are given as arguments to the function
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

/*
 * Reads the input file that contains all the relation names
 * stores the final relations in a list
 */
List *initialize(const char *init_file);

};

#endif
