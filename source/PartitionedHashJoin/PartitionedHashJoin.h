#ifndef _PARTITIONED_HASH_JOIN_H
#define _PARTITIONED_HASH_JOIN_H

#include "FileReader.h"
#include "HashTable.h"
#include "Relation.h"
#include "RowIdRelation.h"
#include "List.h"

class PartitionedHashJoin {

private:

/* The first relation that takes part in the join operation */
    Relation *relR;

/* The second relation that takes part in the join operation */
    Relation *relS;

/* The amount of included bits to hash the input integer elements */
    unsigned int bitsNumForHashing;

/* Indicates whether at least one of the relations 'relR'
 * and 'relS' is a subset of a larger relational array
 */
    bool hasSubrelations;

    bool showInitialRelations;
    bool showAuxiliaryArrays;
    bool showSubrelations;

/* Returns 'true' if both relational arrays are small enough
 * to fit in the level-2 cache and 'false' otherwise
 */
    bool noPartitionRequired(long lvl_2_cache_size) const;

/* Determines wheter a bucket of the relation 'rerR' or 'relS'
 * needs to be further partitioned to additional buckets
 */
    bool multiplePartitionRequired(
        unsigned int R_numOfItemsInBucket,
        unsigned int S_numOfItemsInBucket,
        unsigned int item_size,
        unsigned int lvl_2_cache_size) const;

/* Displays in the screen the contents of the initial relations */
    void displayInitialRelations(const char *message) const;

/* Displays in the screen the contents of the auxiliary arrays */
    void displayAuxiliaryArrays(unsigned int size,
        unsigned int *R_hist, unsigned int *S_hist,
        unsigned int *R_psum, unsigned int *S_psum) const;

/* Hashes a given integer into the value of
 * its rightmost 'bitsNumForHashing' bits
 */
    unsigned int bitReductionHash(int integer) const;

/* Given the address of a tuple, it hashes its
 * contents to a non-negative integer value
 */
    static unsigned int hashTuple(void *item);

/* Executes Building and Probing for a pair of buckets of the
 * relations 'S' and 'R'. The four indexes determine the start
 * and the end of the buckets. The end index is not included
 *
 * The method places the row ID pairs that exist in the buckets
 * and satisfy the join operation in the provided list 'result'
 */
    void probeRelations(
        unsigned int R_start_index,
        unsigned int R_end_index,
        unsigned int S_start_index,
        unsigned int S_end_index,
        List *result) const;

public:

/* Constructor */
    PartitionedHashJoin(const char *input_file, const char *config_file);

/* Secondary Constructor */
    PartitionedHashJoin(Relation *relR, Relation *relS,
        unsigned int bitsNumForHashing);

/* Destructor */
    ~PartitionedHashJoin();

/* Getter - Returns the relational array 'relR' */
    Relation *getRelR() const;

/* Getter - Returns the relational array 'relS' */
    Relation *getRelS() const;

/* Getter - Returns the number of included bits for hashing */
    unsigned int getBitsNumForHashing() const;

/* Executes the Partitioned Hash Join Algorithm */
    RowIdRelation *executeJoin();

/* Displays in the screen the result returned by 'executeJoin' */
    static void printJoinResult(RowIdRelation *resultOfExecuteJoin);

/* Frees the result that was returned by 'executeJoin' */
    static void freeJoinResult(RowIdRelation *resultOfExecuteJoin);

};

#endif
