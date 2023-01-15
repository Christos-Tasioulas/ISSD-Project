#ifndef _JOB_INPUTS_H_
#define _JOB_INPUTS_H_

#include <pthread.h>
#include "Tuple.h"
#include "List.h"

/* The Input for a Histogram Job
 *                 ^^^^^^^^^
 */
struct HistogramJobInput {

    /* The partial histogram that will be given to the job */
    unsigned int *histogram;

    /* The relational array that will be given to the job */
    Tuple *relation;

    /* The job will only work within the given left
     * and right limits, not within the whole array
     */
    unsigned int leftLimit;
    unsigned int rightLimit;

    /* Determines the amount of bits selected for hashing */
    unsigned int bitsNumForHashing;

    /* A simple constructor for the structure */

    HistogramJobInput(
        unsigned int *histogram,
        Tuple *relation,
        unsigned int leftLimit,
        unsigned int rightLimit,
        unsigned int bitsNumForHashing
    ) : histogram(histogram),
        relation(relation),
        leftLimit(leftLimit),
        rightLimit(rightLimit),
        bitsNumForHashing(bitsNumForHashing)
    {}

};

/* The Input for a Partition Job
 *                 ^^^^^^^^^
 */
struct PartitionJobInput {

    /* The relational array that needs to be reordered */
    Tuple *relation;

    /* The relation with the correct order of elements */
    Tuple *reorderedRelation;

    /* The prefix sum that will be used for reordering */
    unsigned int *prefixSum;

    /* The counter of elements that will be used for reordering */
    unsigned int *elementsCounter;

    /* The job will only work within the given left
     * and right limits, not within the whole array
     */
    unsigned int leftLimit;
    unsigned int rightLimit;

    /* Determines the amount of bits selected for hashing */
    unsigned int bitsNumForHashing;

    /* The utility mutex that will be used in critical sections inside the job
     *                                                          ^^^^^^
     */
    pthread_mutex_t *utilityMutex;

    /* A simple constructor for the structure */

    PartitionJobInput(
        Tuple *relation,
        Tuple *reorderedRelation,
        unsigned int *prefixSum,
        unsigned int *elementsCounter,
        unsigned int leftLimit,
        unsigned int rightLimit,
        unsigned int bitsNumForHashing,
        pthread_mutex_t *utilityMutex
    ) : relation(relation),
        reorderedRelation(reorderedRelation),
        prefixSum(prefixSum),
        elementsCounter(elementsCounter),
        leftLimit(leftLimit),
        rightLimit(rightLimit),
        bitsNumForHashing(bitsNumForHashing),
        utilityMutex(utilityMutex)
    {}

};

/* The Input for a Join Job
 *                 ^^^^
 */
struct JoinJobInput {

    /* The rank of the job */
    unsigned int rank;

    /* The left and right relations whose buckets must be joined */
    Tuple *leftRel;
    Tuple *rightRel;

    /* The size of the left relation and the right relation */
    unsigned int leftRelSize;
    unsigned int rightRelSize;

    /* The prefix sums of both relations */
    unsigned int *leftPrefixSum;
    unsigned int *rightPrefixSum;

    /* The amount of buckets (both relations have the same number of buckets) */
    unsigned int bucketsNum;

    /* The parameters to configure a Hopscotch Hash Table */
    unsigned int hopscotchBuckets;
    unsigned int hopscotchRange;
    bool resizableByLoadFactor;
    double loadFactor;

    /* The utility mutex that will be used in critical sections inside the job
     *                                                          ^^^^^^
     */
    pthread_mutex_t *utilityMutex;

    /* The list where all the results will be deposited */
    List *result;

    /* In some cases we do not need to actually execute
     * the job because the buckets have been joined already
     * in the past. It happens in cases of multipartitioning
     */
    bool resultHasAlreadyBeenDeposited;

    /* A simple constructor for the structure */

    JoinJobInput(
        unsigned int rank,
        Tuple *leftRel,
        Tuple *rightRel,
        unsigned int leftRelSize,
        unsigned int rightRelSize,
        unsigned int *leftPrefixSum,
        unsigned int *rightPrefixSum,
        unsigned int bucketsNum,
        unsigned int hopscotchBuckets,
        unsigned int hopscotchRange,
        bool resizableByLoadFactor,
        double loadFactor,
        pthread_mutex_t *utilityMutex,
        List *result,
        bool resultHasAlreadyBeenDeposited
    ) : rank(rank),
        leftRel(leftRel),
        rightRel(rightRel),
        leftRelSize(leftRelSize),
        rightRelSize(rightRelSize),
        leftPrefixSum(leftPrefixSum),
        rightPrefixSum(rightPrefixSum),
        bucketsNum(bucketsNum),
        hopscotchBuckets(hopscotchBuckets),
        hopscotchRange(hopscotchRange),
        resizableByLoadFactor(resizableByLoadFactor),
        loadFactor(loadFactor),
        utilityMutex(utilityMutex),
        result(result),
        resultHasAlreadyBeenDeposited(resultHasAlreadyBeenDeposited)
    {}

};

#endif
