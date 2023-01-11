#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "JobScheduler.h"

/************************************************************
 * Determines whether or not the threads must be terminated *
 ************************************************************/

bool JobScheduler::timeToExit = false;

/********************************************************************************
 * A boolean array that shows which threads are currently working and which not *
 ********************************************************************************/

bool *JobScheduler::workingStatus = NULL;

/***********************
 * Initializes a mutex *
 ***********************/

static void mutex_init(pthread_mutex_t *mutex)
{
	/* We initialize the mutex */
	int init_result = pthread_mutex_init(mutex, NULL);

	/* Case the mutex was not initialized successfully.
	 * We print an informative message in the screen.
	 */
	if(init_result != 0)
	{
		printf("A mutex could not be initialized\n");
		printf("Reason: %s\n", strerror(init_result));
	}
}

/**********************
 * Terminates a mutex *
 **********************/

static void mutex_destroy(pthread_mutex_t *mutex)
{
	/* We terminate the mutex */
	int destroy_result = pthread_mutex_destroy(mutex);

	/* Case the mutex was not destroyed successfully.
	 * We print an informative message in the screen.
	 */
	if(destroy_result != 0)
	{
		printf("A mutex could not be terminated\n");
		printf("Reason: %s\n", strerror(destroy_result));
	}
}

/*************************
 * Locks the given mutex *
 *************************/

static void lock(pthread_mutex_t *mutex)
{
	int lock_result = pthread_mutex_lock(mutex);

	if(lock_result != 0)
	{
		printf("A mutex could not be locked\n");
		printf("Reason: %s\n", strerror(lock_result));
	}
}

/***************************
 * Unlocks the given mutex *
 ***************************/

static void unlock(pthread_mutex_t *mutex)
{
	int unlock_result = pthread_mutex_unlock(mutex);

	if(unlock_result != 0)
	{
		printf("A mutex could not be unlocked\n");
		printf("Reason: %s\n", strerror(unlock_result));
	}
}

/************************************
 * Initializes a condition variable *
 ************************************/

static void cond_init(pthread_cond_t *condition_variable)
{
	/* We initialize the condition variable */
	int init_result = pthread_cond_init(condition_variable, NULL);

	/* Case the condition variable was not initialized successfully.
	 * We print an informative message in the screen about the error.
	 */
	if(init_result != 0)
	{
		printf("A condition variable could not be initialized\n");
		printf("Reason: %s\n", strerror(init_result));
	}
}

/***********************************
 * Terminates a condition variable *
 ***********************************/

static void cond_destroy(pthread_cond_t *condition_variable)
{
	/* We destroy the condition variable */
	int destroy_result = pthread_cond_destroy(condition_variable);

	/* Case the condition variable was not destroyed successfully.
	 * We print an informative message in the screen about the error.
	 */
	if(destroy_result != 0)
	{
		printf("A condition variable could not be terminated\n");
		printf("Reason: %s\n", strerror(destroy_result));
	}
}

/***************************************************
 * Blocks a thread on the given condition variable *
 ***************************************************/

static void cond_wait(pthread_cond_t *condition_variable, pthread_mutex_t *mutex)
{
	int wait_result = pthread_cond_wait(condition_variable, mutex);

	if(wait_result != 0)
	{
		printf("Could not block on a condition variable\n");
		printf("Reason: %s\n", strerror(wait_result));
	}
}

/**********************************************************************
 * Unblocks a thread that was blocked on the given condition variable *
 **********************************************************************/

static void cond_signal(pthread_cond_t *condition_variable)
{
	int signal_result = pthread_cond_signal(condition_variable);

	if(signal_result != 0)
	{
		printf("Could not singal a blocked thread\n");
		printf("Reason: %s\n", strerror(signal_result));
	}
}

/**************************************************************************
 * Unblocks all threads that were blocked on the given condition variable *
 **************************************************************************/
/*
static void cond_broadcast(pthread_cond_t *condition_variable)
{
	int broadcast_result = pthread_cond_broadcast(condition_variable);

	if(broadcast_result != 0)
	{
		printf("Could not broadcast on a condition variable\n");
		printf("Reason: %s\n", strerror(broadcast_result));
	}
}
*/
/*****************************************************************
 *  Returns 'true' if there are no working threads. If at least  *
 * one thread is working on a job, the operation returns 'false' *
 *****************************************************************/

bool JobScheduler::silence()
{
	/* We lock the status mutex before we
	 * take any actions on the shared array
	 */
	lock(&statusMutex);

	/* Initially we consider there is silence */
	bool result = true;

	/* Auxiliary variable (used for counting) */
	unsigned int i;

	/* We will examine the status of each thread.
	 * If we find at least one working thread,
	 * there is no silence. In that case, we set
	 * the 'result' variable to 'false'.
	 */
	for(i = 0; i < maxThreads; i++)
	{
		if(workingStatus[i] == true)
			result = false;
	}

	/* We unlock the mutex guarding the array */
	unlock(&statusMutex);

	/* We return the final result */
	return result;
}

/***********************************************
 * Examines whether the queue of jobs is empty *
 ***********************************************/

bool JobScheduler::queueIsEmpty(Queue *q, pthread_mutex_t *qMutex)
{
	/* We lock the mutex guarding the queue */
	lock(qMutex);

	/* Now we can examine the queue. We retrieve 'true'/'false'
	 * according to whether or not the queue is empty
	 */
	bool result = q->isEmpty();

	/* We unlock the mutex guarding the queue */
	unlock(qMutex);

	/* We return the result */
	return result;
}

/*************************************************
 * Start Routine of each thread of the Scheduler *
 *************************************************/

void *JobScheduler::threadsRoutine(void *input)
{
	/* We cast the input for the thread to its actual type */
	ThreadInput *threadInput = (ThreadInput *) input;

	/* We retrieve the attributes of the input */
	unsigned int myRank = threadInput->rank;
	bool *workStatus = threadInput->work_table;
	pthread_cond_t *sleepCondVar = threadInput->sleep_var;
	pthread_cond_t *exitCondVar = threadInput->exit_var;
	pthread_mutex_t *mutexForQueue = threadInput->queue_mutex;
	pthread_mutex_t *mutexForJobs = threadInput->job_mutex;
	pthread_mutex_t *mutexForStatus = threadInput->status_mutex;
	Queue *jobsInQueue = threadInput->queue;

	while(1)
	{
		/* This thread is ready to execute jobs */
		lock(mutexForJobs);

		/* The thread waits for the next job to come */

		while(queueIsEmpty(jobsInQueue, mutexForQueue))
		{
			/* If there are no jobs at the moment, the thread
			 * updates its status as "non-working" and waits
			 * on the condition variable, unlocking the job mutex
			 *
			 * Before writing to the table, we lock the mutex
			 * guarding it
			 */
			lock(mutexForStatus);

			/* We update the status with 'false' (= non-working) */
			workStatus[myRank] = false;

			/* We unlock the mutex guarding the table */
			unlock(mutexForStatus);

			/* We signal the main thread in case it is sleeping */
			cond_signal(sleepCondVar);

			/* We wait on the exit condition variable until
			 * a job is available
			 */
			cond_wait(exitCondVar, mutexForJobs);
		}

		/* We exit the 'while' loop if the boolean variable
		 * of the class indicates the end of execution.
		 */
		if(timeToExit == true)
		{
			/* We uock the mutex guarding the queue of jobs */
			lock(mutexForQueue);

			/* The thread consumes one item from the queue */
			jobsInQueue->remove();

			/* We unlock the mutex guarding the queue of jobs */
			unlock(mutexForQueue);

			/* Then it unlocks the mutex for jobs and terminates */
			unlock(mutexForJobs);

			/* The thead exits the 'while' loop */
			break;
		}

		/* Now the thread is ready to take a job.
		 *
		 * The thread updates its status as "working".
		 */
		lock(mutexForStatus);

		/* We update the status with 'true' (= working) */
		workStatus[myRank] = true;

		/* We unlock the mutex guarding the table */
		unlock(mutexForStatus);

		/* Here the thread will execute the next available job.
		 *
		 * We lock the mutex guarding the queue of jobs.
		 */
		lock(mutexForQueue);

		/* We retrieve the oldest item in the queue */
		Job *nextJob = (Job *) jobsInQueue->getOldestItem();

		/* We remove that item from the queue */
		jobsInQueue->remove();

		/* We unlock the mutex guarding the queue of jobs */
		unlock(mutexForQueue);

		/* When there is a number of jobs in the queue, the main
		 * thread only wakes up one worker thread. In case the
		 *                      ^^^
		 * queue has multiple jobs, the current worker thread wakes
		 * up another worker thread before it starts its job.
		 */
		if(!queueIsEmpty(jobsInQueue, mutexForQueue))
			cond_signal(exitCondVar);

		/* The thread now unlocks the mutex for jobs so as another
		 * worker thread may take another job and execute it
		 */
		unlock(mutexForJobs);

		/* The thread executes the next job */
		nextJob->executeJob();
	}

	/* Here each thread exits */
	return NULL;
}

/***************
 * Constructor *
 ***************/

JobScheduler::JobScheduler(unsigned int maxThreads)
{
	/* We save the given maximum amount of threads in the object */
	this->maxThreads = maxThreads;

	/* We initialize the condition variables & mutexes of the structure */
	cond_init(&exitConditionVariable);
	cond_init(&sleepConditionVariable);
	mutex_init(&jobMutex);
	mutex_init(&queueMutex);
	mutex_init(&sleepMutex);
	mutex_init(&statusMutex);

	/* We initialize the queue of the Scheduler
	 * where the submitted jobs will be waiting
	 */
	submittedJobs = new Queue();

	/* We initialize the array of inputs for each thread */
	threadInput = new ThreadInput *[maxThreads];

	/* We set each working status to 'false' by using 'calloc' */
	workingStatus = (bool *) calloc(maxThreads, sizeof(bool));

	/* We initialize the array of threads */
	threads = new pthread_t[maxThreads];

	/* Auxiliary variable (used for counting) */
	unsigned int i;

	/* Here we create the threads and store them in the array */

	for(i = 0; i < maxThreads; i++)
	{
		/* We create the input we will give to this thread */

		threadInput[i] = new ThreadInput(
			i,
			workingStatus,
			&exitConditionVariable,
			&sleepConditionVariable,
			&jobMutex,
			&queueMutex,
			&statusMutex,
			submittedJobs);

		/* We create a new thread */

		int creation_result = pthread_create(&threads[i],
			NULL, threadsRoutine, threadInput[i]);

		/* Case the creation was not successful. We print an
		 * informative message in the screen about the error
		 */
		if(creation_result != 0)
		{
			printf("Could not create thread #%u\nReason: %s\n",
				i, strerror(creation_result));
		}
	}
}

/**************
 * Destructor *
 **************/

JobScheduler::~JobScheduler()
{
	/* It is time for the threads to terminate */
	timeToExit = true;

	/* Auxiliary variable (used for counting) */
	unsigned int i;

	/* We wake all threads and suggest them to finish */

	for(i = 0; i < maxThreads; i++)
	{
		/* We insert a dummy item in the queue to
		 * make the thread consume it and terminate.
		 *
		 * We make sure to lock the queue mutex
		 * before we take any action in the queue.
		 */
		lock(&queueMutex);

		/* We insert a 'NULL' (dummy) item in the queue */
		submittedJobs->insert(NULL);

		/* We unlock the mutex guarding the queue */
		unlock(&queueMutex);

		/* We signal one of the threads that
		 * there are new items in the queue
		 */
		cond_signal(&exitConditionVariable);
	}

	/* We terminate the threads */

	for(i = 0; i < maxThreads; i++)
	{
		/* We join the current thread (the current thread is terminated) */
		int join_result = pthread_join(threads[i], NULL);

		/* Case the join result was not successful. We print an
		 * informative message in the screen about the error
		 */
		if(join_result != 0)
		{
			printf("Could not join thread #%u\nReason: %s\n",
				i, strerror(join_result));
		}
	}

	/* We free the allocated memory for the array of the threads */
	delete[] threads;

	/* We free the allocated memory for the input for the threads */

	for(i = 0; i < maxThreads; i++)
		delete threadInput[i];

	delete[] threadInput;

	/* We free the allocated memory for the working status of each thread */
	free(workingStatus);

	/* We free the allocated memory for the queue */
	delete submittedJobs;

	/* We terminate the condition variables & mutexes of the structure */
	cond_destroy(&exitConditionVariable);
	cond_destroy(&sleepConditionVariable);
	mutex_destroy(&jobMutex);
	mutex_destroy(&queueMutex);
	mutex_destroy(&sleepMutex);
	mutex_destroy(&statusMutex);

	/* We prepare the static variable for use from another
	 * job scheduler that may be initialized in the future
	 */
	timeToExit = false;
}

/********************************************
 * Places a new job at the end of the queue *
 ********************************************/

void JobScheduler::submitJob(Job *newJob)
{
	/* We lock the mutex guarding the queue */
	lock(&queueMutex);

	/* We place the new job in the end of the queue */
	submittedJobs->insert(newJob);

	/* We unlock the mutex guarding the queue */
	unlock(&queueMutex);
}

/*******************************************
 * Starts executing all the available jobs *
 *******************************************/

void JobScheduler::executeAllJobs()
{
	cond_signal(&exitConditionVariable);
}

/******************************************************
 * The main thread sleeps until all jobs are finished *
 ******************************************************/

void JobScheduler::waitAllTasksFinish()
{
	/* This mutex is only used by the main thread, not
	 * by mutliple threads. So why do we use it? -> We
	 * use it just because we want to be able to wait
	 * on the sleep condition variable. The main thread
	 * should sleep on a condition variable while the
	 * worker threads are executing their jobs. When all
	 * the threads finish their jobs and no more jobs
	 * are in the queue, the main thread is signaled.
	 *
	 * We lock the sleep mutex to prepare a wait on the
	 * sleep condition variable.
	 */
	lock(&sleepMutex);

	/* As long as there are still working threads or the
	 * queue of jobs is not empty, the main thread sleeps
	 * because the task of this function is to wait until
	 * all jobs are finished
	 */
	while((!silence()) || (!queueIsEmpty(submittedJobs, &queueMutex)))
		cond_wait(&sleepConditionVariable, &sleepMutex);

	/* We unlock the sleep mutex */
	unlock(&sleepMutex);
}
