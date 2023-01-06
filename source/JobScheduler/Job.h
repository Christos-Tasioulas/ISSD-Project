#ifndef _JOB_H_
#define _JOB_H_

class Job {

private:

	/* The code routine of the job */
	void (*job)();

public:

	/* Constructor */
	Job(void (*job)());

	/* Destructor */
	~Job();

	/* Executes the job */
	void executeJob() const;

};

#endif
