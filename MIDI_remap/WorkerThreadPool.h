#pragma once
#include "Job.h"
#include "WorkerThread.h"
#include <queue>
#include <vector>
#include <Windows.h>

/*
Contains all running threads and provides a job queuing and execution system
*/
class WorkerThreadPool {
public:
	/*
	Gets a singleton refrence to the thread pool
	*/
	static WorkerThreadPool * getThreadPool();

	/*
	Destroys the singleton object
	*/
	static void destroy();

	/*
	To be called by the worker thread when it starts processing
	*/
	void _jobStarted(WorkerThread*);

	/*
	to be called by the worker thread when it finishes processing
	*/
	void _jobCompleted(WorkerThread*);

	/*
	Sends a Job to be executed
	*/
	void submitJob(Job* ); 

private:
	int numThreads; //the number of threads in the pool

	int readyThreadCount; //the number of threads that are ready for a job

	std::queue<Job*> jobQueue; //queue of jobs to be executed

	std::vector<WorkerThread*> allThreads; //running threads that are busy

	std::queue<WorkerThread*> busyThreads; //running threads that are busy

	std::queue<WorkerThread*> readyThreads; //running threads waiting for a job

	WorkerThreadPool(); //creates worker threads

	~WorkerThreadPool(); //deletes worker threads

	static WorkerThreadPool * instance; //singleton thread pool
};

