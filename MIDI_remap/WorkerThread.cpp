#include "WorkerThread.h"
#include "WorkerThreadPool.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

/*
Stores private thread data to prevent threads being included elsewhere
*/
struct WorkerThreadPrivateData{
	volatile bool die; //for signaling the thread to end main loop

	std::condition_variable_any cv; //for signaling job available

	std::thread * worker; //the thread pointer

	Job* currentJob; //the job to execute
};

WorkerThread::WorkerThread()
{
	priv = NULL;
	priv = new WorkerThreadPrivateData();

	priv->worker = NULL;

	if (WorkerThreadPool::getThreadPool() == NULL){
		throw new std::exception("Error geting thread pool");
	}

	priv->worker = new std::thread(_start, this);
	if (priv->worker == NULL){
		throw new std::exception("Error creating worker thread");
	}

	printf("Worker thread %d created\n", priv->worker->get_id());
}


WorkerThread::~WorkerThread()
{
	if (priv){
		if (priv->worker){
			printf("Destroying thread %d\n", priv->worker->get_id());

			if (priv->worker->joinable()){
				priv->worker->join();
			}
			delete priv->worker;
		}
		priv->worker = NULL;

		delete priv;
	}
	priv = NULL;
}

void WorkerThread::_start(WorkerThread* ptr){

	if (ptr == NULL){
		throw new std::exception("Thread cannot be started without a self refrence");
	}

	ptr->main(); //do the main loop
}

void WorkerThread::main(){
	WorkerThreadPool * pool = WorkerThreadPool::getThreadPool();

	if (pool == NULL){ 
		throw new std::exception("Error getting thread pool handle");
		return;
	}

	while (! priv->die){

	}
}