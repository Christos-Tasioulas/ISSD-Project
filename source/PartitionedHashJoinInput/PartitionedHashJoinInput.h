#ifndef _PARTITIONED_HASH_JOIN_INPUT_H_
#define _PARTITIONED_HASH_JOIN_INPUT_H_

#include "FileReader.h"

/* A structure that "wraps" all the input parameters
 * for the Partitioned Hash join algorithm
 */
struct PartitionedHashJoinInput {

/* Parameters of the Partitioned Hash Join Algorithm
 *
 * See the configuration file for a documentation
 * for each of the parameters listed below
 */
    unsigned int bitsNumForHashing;
    bool showInitialRelations;
    bool showAuxiliaryArrays;
    bool showHashTable;
    bool showSubrelations;
    bool showResult;
    unsigned int hopscotchBuckets;
    unsigned int hopscotchRange;
    bool resizableByLoadFactor;
    double loadFactor;
    double maxAllowedSizeModifier;
    unsigned int maxPartitionDepth;

/* Constructor & Destructor */
    PartitionedHashJoinInput(const char *config_file);
    ~PartitionedHashJoinInput();

/* Prints the value of each field of the class */
    void print() const;

};

#endif
