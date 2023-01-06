#include <iostream>
#include "Job.h"

/***************
 * Constructor *
 ***************/

Job::Job(void (*job)())
{
	this->job = job;
}

/**************
 * Destructor *
 **************/

Job::~Job() {}

/********************
 * Executes the job *
 ********************/

void Job::executeJob() const
{
	job();
}
