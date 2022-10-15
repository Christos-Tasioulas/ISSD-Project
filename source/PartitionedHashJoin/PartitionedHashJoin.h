#ifndef _PARTITIONED_HASH_JOIN_H
#define _PARTITIONED_HASH_JOIN_H

#include "HashTable.h"
#include "Relation.h"
#include "RowIdPair.h"

class PartitionedHashJoin {

private:

/* The first relation that takes part in the join operation */
    Relation *relR;

/* The second relation that takes part in the join operation */
    Relation *relS;

/* The amount of included bits to hash the input integer elements */
    unsigned int bitsNumForHashing;

/* Hashes a given integer into the value of
 * its rightmost 'bitsNumForHashing' bits
 */
    unsigned int bitReductionHash(int integer) const;

public:

/* Constructor */
    PartitionedHashJoin(char *input_file, char *config_file);

/* Destructor */
    ~PartitionedHashJoin();

/* Getter - Returns the relational array 'relR' */
    Relation *getRelR() const;

/* Getter - Returns the relational array 'relS' */
    Relation *getRelS() const;

/* Getter - Returns the number of included bits for hashing */
    unsigned int getBitsNumForHashing() const;

/* Executes the Partitioned Hash Join Algorithm */
    RowIdPair *executeJoin();

};

#endif
