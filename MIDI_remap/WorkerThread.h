#pragma once
#include "Job.h"

struct WorkerThreadPrivateData; //predefinition

class WorkerThread
{
public:
	/*
	Creates a new thread
	*/
	WorkerThread();
	
	/*
	Destroys a thread
	*/
	~WorkerThread();

	/*
	Gets if the thread is ready for a job
	*/
	volatile bool isReady();

	/*
	Gives the thread a job to process
	*/
	void giveJob(Job*);

	/*
	Signal the thread to die after the current job completes
	*/
	void kill();

private:
	static void _start(WorkerThread*); //entry point for the thread. starts the main loop

	struct WorkerThreadPrivateData * priv; //Stores all private data for the thread

	void main(); //main loop for the thread
};
