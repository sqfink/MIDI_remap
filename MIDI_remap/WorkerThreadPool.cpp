#include "WorkerThreadPool.h"
#include <thread>

WorkerThreadPool* WorkerThreadPool::instance = NULL;

WorkerThreadPool::WorkerThreadPool(){
	int hardwareThreads = std::thread::hardware_concurrency();

	readyThreadCount = 0;

	if (hardwareThreads == 0){
		numThreads = 4;
		printf("Failed to detect number of hardware threads. Using %d worker threads.\n", numThreads);
	}
	else{
		numThreads = (int)ceil((double)hardwareThreads * 1.5); //1.5 * number of hardware threads rounded up

		numThreads = (numThreads < 4) ? 4 : numThreads; //4 threads minimum

		printf("%d hardware threads detected. Creating %d worker threads", hardwareThreads, numThreads);
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
				delete thread;
			}
			thread = NULL;
		}

		while (readyThreads.size()){ //clear the ready thread list
			readyThreads.pop();
		}

		while (busyThreads.size()){ //clear the busy thread list
			busyThreads.pop();
		}
		
		allThreads.clear(); //remove all items from thread list

		while (jobQueue.size()){ //remove all jobs from queue
			jobQueue.pop(); 
		}
		
	}
}


