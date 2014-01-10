#include "WorkerThreadPool.h"
#include <thread>
#include <mutex>

WorkerThreadPool* WorkerThreadPool::instance = NULL;

/*
Protects the stdc++11 functions from the global header to allow importing glocal header from managed(or clr) projects
*/
struct WorkerThreadPoolPriv {
	std::mutex processQueueLock;
	std::mutex sysLock; //protects access to all lists
};

WorkerThreadPool::WorkerThreadPool(){
	int hardwareThreads = std::thread::hardware_concurrency(); //get the number of parallel threads that can exist on the hardware
	priv = new WorkerThreadPoolPriv; //init the private data struct
	instance = this;

	readyThreadCount = 0;

	if (hardwareThreads == 0){
		numThreads = 4;
		printf("Failed to detect number of hardware threads. Using %d worker threads.\n", numThreads);
	}
	else{
		numThreads = (int)ceil((double)hardwareThreads * 1.5); //1.5 * number of hardware threads rounded up

		numThreads = (numThreads < 4) ? 4 : numThreads; //4 threads minimum

		printf("%d hardware threads detected. Creating %d worker threads\n", hardwareThreads, numThreads);
	}

	for (int i = 0; i < numThreads; i++){
		WorkerThread * tmp = NULL;
		tmp = new WorkerThread();
		
		if (tmp == NULL){
			throw new std::exception("Error creating worker thread");
		}

		allThreads.push_back(tmp); //add it to the thread list

		tmp = NULL;
	}
}

WorkerThreadPool* WorkerThreadPool::getThreadPool(){
	if (!instance)
		instance = new WorkerThreadPool();
	return instance;
}

void WorkerThreadPool::destroy(){

	if (instance){
		delete instance;
	}

	instance = NULL; 
	return;
}

WorkerThreadPool::~WorkerThreadPool(){
	if (numThreads){
		numThreads = 0;
		readyThreadCount = 0;
		
		/*
		destroy all threads
		*/
		for (auto thread : allThreads){
			if (thread){
				thread->kill();
				delete thread;
			}
			thread = NULL;
		}
		priv->sysLock.lock(); { //lock for deleting all queues
			while (readyThreads.size()){ //clear the ready thread list
				readyThreads.pop();
			}

			busyThreads.clear(); //clear the busy thread list
		
			allThreads.clear(); //remove all items from thread list

			while (jobQueue.size()){ //remove all jobs from queue
				jobQueue.pop(); 
			}
		} priv->sysLock.unlock(); //unlock deleting all queues
	}
}

void WorkerThreadPool::submitJob(Job* newJob){
	if(newJob == NULL){
		throw new std::exception("Cannot start NULL job");
	}

	priv->sysLock.lock(); { //lock for adding the job to the queue
		jobQueue.push(newJob); //add the job to the queue
	}priv->sysLock.unlock(); //unlock adding the job to the queue

	tryProcessQueue();
}


void WorkerThreadPool::tryProcessQueue(){
	priv->processQueueLock.lock(); //take lock so only one atempt can be made concurrently

	if(jobQueue.size() == 0){
		priv->processQueueLock.unlock(); //release lock
		return;					//no jobs to process
	}

	if(readyThreadCount <= 0){
		priv->processQueueLock.unlock(); //release lock
		return;					//no threads are ready to process
	}
	
	int readyCount = readyThreadCount; //copy the number of threads that were ready

	for(int i = 0; i < readyCount; i++){
		WorkerThread* nextThread = NULL;
		priv->sysLock.lock();{ //lock access to the ready thread list
			nextThread = readyThreads.front();
			readyThreads.pop();
			readyThreadCount--; //decrement the number of threads in the ready list
		}priv->sysLock.unlock(); //unlock access to the ready thread list

		if(nextThread == NULL){
			priv->processQueueLock.unlock(); //release lock
			throw new std::exception("Got a NULL thread pointer");
		}

		Job* nextJob = NULL;
		priv->sysLock.lock();{ //lock for getting the next job in the job queue
			nextJob = jobQueue.front();
			jobQueue.pop();
		}priv->sysLock.unlock(); //unlock getting the next job in the queue

		if(nextJob == NULL){
			priv->processQueueLock.unlock(); //release lock
			throw new std::exception("Got a NULL job pointer");
		}

		nextThread->giveJob(nextJob); //submit the job to the thread
	}

	priv->processQueueLock.unlock(); //release lock
	return;
}

void WorkerThreadPool::_jobCompleted(WorkerThread* thread){
	priv->sysLock.lock(); { //Lock for adding the thread to the idle thread list
		busyThreads.remove(thread); //remove from the busy thread list
		readyThreads.push(thread); //add to the ready thread list
		readyThreadCount++; //update ready thread counter
	} priv->sysLock.unlock(); //release lock for adding thread to idle list
}

void WorkerThreadPool::_jobStarted(WorkerThread* thread){
	priv->sysLock.lock(); { //Lock for adding the thread to the busy thread list
		busyThreads.push_back(thread);
	} priv->sysLock.unlock(); //release lock for adding thread to busy list
}