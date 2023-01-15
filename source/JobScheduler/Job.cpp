#include <iostream>
#include "Job.h"

/***************
 * Constructor *
 ***************/

Job::Job(
	void (*histogramJob)(
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
	JoinJobInput *joinJobInput)
{
	this->histogramJob = histogramJob;
	this->histogramJobInput = histogramJobInput;
	this->partitionJob = partitionJob;
	this->partitionJobInput = partitionJobInput;
	this->joinJob = joinJob;
	this->joinJobInput = joinJobInput;

	if(histogramJob != NULL)
		jobType = new JobTypes(HistogramJob);

	else if(partitionJob != NULL)
		jobType = new JobTypes(PartitionJob);

	else
		jobType = new JobTypes(JoinJob);
}

/**************
 * Destructor *
 **************/

Job::~Job()
{
	delete jobType;
}

/********************
 * Executes the job *
 ********************/

void Job::executeJob() const
{
	/* We will take different actions according to the job type */
	switch(*jobType)
	{
		/* Case the type of job is Histogram Job */
		case HistogramJob:
		{
			/* We execute the Histogram Job */
			histogramJob(
				histogramJobInput->histogram,
				histogramJobInput->relation,
				histogramJobInput->leftLimit,
				histogramJobInput->rightLimit,
				histogramJobInput->bitsNumForHashing
			);

			/* There is nothing else to do in this case */
			break;
		}

		/* Case the type of job is Partition Job */
		case PartitionJob:
		{
			/* We execute the Partition Job */
			partitionJob(
				partitionJobInput->relation,
				partitionJobInput->reorderedRelation,
				partitionJobInput->prefixSum,
				partitionJobInput->elementsCounter,
				partitionJobInput->leftLimit,
				partitionJobInput->rightLimit,
				partitionJobInput->bitsNumForHashing,
				partitionJobInput->utilityMutex
			);

			/* There is nothing else to do in this case */
			break;
		}

		/* Case the type of job is Join Job */
		case JoinJob:
		{
			/* We execute the Partition Job */
			joinJob(
				joinJobInput->rank,
				joinJobInput->leftRel,
				joinJobInput->rightRel,
				joinJobInput->leftRelSize,
				joinJobInput->rightRelSize,
				joinJobInput->leftPrefixSum,
				joinJobInput->rightPrefixSum,
				joinJobInput->bucketsNum,
				joinJobInput-> hopscotchBuckets,
				joinJobInput->hopscotchRange,
				joinJobInput->resizableByLoadFactor,
				joinJobInput->loadFactor,
				joinJobInput->utilityMutex,
				joinJobInput->result,
				joinJobInput->resultHasAlreadyBeenDeposited
			);

			/* There is nothing else to do in this case */
			break;
		}

		/* Case the type of job is unknown */
		default:
		{
			/* We print a message in the screen about the error */
			std::cout << "Unknown job type" << std::endl;

			/* There is nothing else to do in this case */
			break;
		}
	}
}
