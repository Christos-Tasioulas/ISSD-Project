#ifndef _JOB_SCHEDULER_H_
#define _JOB_SCHEDULER_H_

#include <pthread.h>
#include "Queue.h"
#include "Job.h"

struct ThreadInput {

	/* A unique identifier for each thread */
	unsigned int rank;

	/* Indicates which threads are working and which not */
	bool *work_table;

	/* An exit condition variable is part of the threads' input */
	pthread_cond_t *exit_var;

	/* A sleep condition variable is part of the threads' input */
	pthread_cond_t *sleep_var;

	/* A job mutex is part of the threads' input  */
	pthread_mutex_t *job_mutex;

	/* A queue mutex is part of the threads' input */
	pthread_mutex_t *queue_mutex;

	/* A status mutex is part of the threads' input */
	pthread_mutex_t *status_mutex;

	/* The job queue is part of the threads' input */
	Queue *queue;

	/* A simple constructor for the structure */
	ThreadInput(
		unsigned int rank,
		bool *work_table,
		pthread_cond_t *exit_var,
		pthread_cond_t *sleep_var,
		pthread_mutex_t *job_mutex,
		pthread_mutex_t *queue_mutex,
		pthread_mutex_t *status_mutex,
		Queue *queue) : rank(rank), work_table(work_table), exit_var(exit_var),
		sleep_var(sleep_var), job_mutex(job_mutex), queue_mutex(queue_mutex),
		status_mutex(status_mutex), queue(queue) {}

};

class JobScheduler {

private:

	/* An array holding all the threads of the Scheduler */
	pthread_t *threads;

	/* The amount of available threads of the Scheduler */
	unsigned int maxThreads;

	/* The input we will give to each thread for its execution */
	ThreadInput **threadInput;

	/* The queue of submitted jobs */
	Queue *submittedJobs;

	/* All threads will be terminated with the help of this condition variable */
	pthread_cond_t exitConditionVariable;

	/* The main thread will sleep with this condition
	 * variable when we are waiting for all jobs to finish
	 */
	pthread_cond_t sleepConditionVariable;

	/* A mutex that will be allowing/disallowing threads from doing a job */
	pthread_mutex_t jobMutex;

	/* A mutex that will be guarding the queue of jobs */
	pthread_mutex_t queueMutex;

	/* A mutex that will be guarding the queue of jobs */
	pthread_mutex_t sleepMutex;

	/* A mutex that will be guarding the working status table */
	pthread_mutex_t statusMutex;

	/* Returns 'true' if there are no working threads. If at least
	 * one thread is working on a job, the operation returns 'false'
	 */
	bool silence();

	/* A boolean array that shows which threads are currently working and which not */
	static bool *workingStatus;

	/* Determines whether or not the threads must be terminated */
	static bool timeToExit;

	/* Examines whether the queue of jobs is empty */
	static bool queueIsEmpty(Queue *q, pthread_mutex_t *qMutex);

	/* Start Routine of each thread of the Scheduler */
	static void *threadsRoutine(void *input);

public:

	/* Constructor */
	JobScheduler(unsigned int maxThreads);

	/* Destructor */
	~JobScheduler();

	/* Places a new job at the end of the queue */
	void submitJob(Job *newJob);

	/* Starts executing all the available jobs */
	void executeAllJobs();

	/* The main thread sleeps until all jobs are finished */
	void waitAllTasksFinish();

};

#endif
