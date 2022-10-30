#ifndef _PARTITIONED_HASH_JOIN_H_
#define _PARTITIONED_HASH_JOIN_H_

#include "FileReader.h"
#include "HashTable.h"
#include "RowIdRelation.h"
#include "List.h"

class PartitionedHashJoin {

public:

/* Constructor */
    PartitionedHashJoin(const char *input_file, const char *config_file);

/* Secondary Constructor */
    PartitionedHashJoin(
        Relation *relR,
        Relation *relS,
        unsigned int bitsNumForHashing,
        bool showInitialRelations,
        bool showAuxiliaryArrays,
        bool showHashTable,
        bool showSubrelations,
        bool showResult,
        unsigned int hopscotchBuckets,
        unsigned int hopscotchRange,
        bool resizableByLoadFactor,
        double loadFactor,
        double maxAllowedSizeModifier);

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
    void printJoinResult(RowIdRelation *resultOfExecuteJoin);

/* Frees the result that was returned by 'executeJoin' */
    static void freeJoinResult(RowIdRelation *resultOfExecuteJoin);

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

/* Determines whether or not the contents of the relations 'relR'
 * and 'relS' will be displayed in the screen. The option prints
 * the initial contents of the relations and the contents after
 * reordering the relational arrays
 */
    bool showInitialRelations;

/* Determines whether or not the contents of the histograms and
 * the prefix sums will be displayed in the user's screen
 */
    bool showAuxiliaryArrays;

/* Determines whether or not the contents of the hash table used
 * to probe the relations will be printed in the user's screen
 */
    bool showHashTable;

/* If one of the above two flags is 'true', this one determines
 * whether or not the corresponding information for all the
 * subrelations will be printed in the user's screen
 */
    bool showSubrelations;

/* Determines whether or not the result of the 'join' operation
 * will be printed in the user's screen
 */
    bool showResult;

/* The amount of initial buckets the hash table will have */
    unsigned int hopscotchBuckets;

/* The initial size of each neighborhood in the hash table */
    unsigned int hopscotchRange;

/* Determines if the hash table can be resized when a load
 * factor is surpassed, even if the Hopscotch algorithm does
 * not require the table to grow at that specific moment
 */
    bool resizableByLoadFactor;

/* The limit that if surpassed by (elements/buckets) forces
 * the hash table to grow if 'resizableByLoadFactor' is 'true'
 */
    double loadFactor;

/* This is a value between 0.0 and 1.0 that determines the
 * maximum size an array may have so as it does not need to
 * be partitioned divided by the size of the lvl-2 cache.
 */
    double maxAllowedSizeModifier;

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

};

#endif
