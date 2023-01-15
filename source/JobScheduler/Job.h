#ifndef _JOB_H_
#define _JOB_H_

#include "JobInputs.h"

/* An enumeration for all the available job types */

enum JobTypes {

	/* Hashes the given elements of a tuple into a histogram */
	HistogramJob,

	/* Reorders the tuples of a relation */
	PartitionJob,

	/* Joins two buckets of a pair of a relations */
	JoinJob

};

/* The Job Class */

class Job {

private:

	/* The code routine of the histogram job */
	void (*histogramJob)(
		unsigned int *histogram,
		Tuple *relation,
		unsigned int relationStartIndex,
		unsigned int relationEndIndex,
		unsigned int selectedBitsNumForHashing
	);

	/* The input for the histogram job routine */
	HistogramJobInput *histogramJobInput;

	/* The code routine of the partition job */
	void (*partitionJob)(
        Tuple *relation,
        Tuple *reorderedRelation,
        unsigned int *prefixSumOfRel,
        unsigned int *elementsCounterOfRel,
        unsigned int relationStartIndex,
        unsigned int relationEndIndex,
        unsigned int selectedBitsNumForHashing,
        pthread_mutex_t *util
    );

	/* The input for the partition job routine */
	PartitionJobInput *partitionJobInput;

	void (*joinJob)(
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
        pthread_mutex_t *util,
        List *result,
        bool resultHasAlreadyBeenDeposited
    );

	/* The input for the join job routine */
	JoinJobInput *joinJobInput;

	/* The type of the job */
	JobTypes *jobType;

public:

	/* Constructor */
	Job(void (*histogramJob)(
			unsigned int *histogram,
			Tuple *relation,
			unsigned int relationStartIndex,
			unsigned int relationEndIndex,
			unsigned int selectedBitsNumForHashing
		),
		HistogramJobInput *histogramJobInput,

		void (*partitionJob)(
			Tuple *relation,
			Tuple *reorderedRelation,
			unsigned int *prefixSumOfRel,
			unsigned int *elementsCounterOfRel,
			unsigned int relationStartIndex,
			unsigned int relationEndIndex,
			unsigned int selectedBitsNumForHashing,
			pthread_mutex_t *util
    	),
		PartitionJobInput *partitionJobInput,

		void (*joinJob)(
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
			pthread_mutex_t *util,
			List *result,
			bool resultHasAlreadyBeenDeposited
		),
		JoinJobInput *joinJobInput
	);

	/* Destructor */
	~Job();

	/* Executes the job */
	void executeJob() const;

};

#endif
