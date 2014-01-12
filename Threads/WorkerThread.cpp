#include "WorkerThread.h"
#include "WorkerThreadPool.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "LogServer.h"

LogServer Log; //for the extern

/*
Stores private thread data to prevent threads being included elsewhere
*/
struct WorkerThreadPrivateData{
	volatile bool die; //for signaling the thread to end main loop

	std::condition_variable_any cv; //for signaling job to run processing

	std::mutex * mtx; 

	std::thread * worker; //the thread pointer

	Job * currentJob; //the job to execute

	WorkerThreadPool * pool; //Link to the owning pool
};

WorkerThread::WorkerThread()
{
	priv = NULL;
	priv = new WorkerThreadPrivateData(); //initialize hidden vars

	priv->worker = NULL;
	priv->mtx = NULL;

	priv->currentJob = NULL;
	priv->die = false;
	priv->pool = NULL;

	priv->pool = WorkerThreadPool::getThreadPool();
	if (priv->pool == NULL){
		throw new std::exception("Error getting handle to the thread pool");
	}

	priv->mtx = new std::mutex();
	if (priv->mtx == NULL)
		throw new std::exception("Error allocating mutex");

	priv->worker = new std::thread(_start, this); //create the thread
	if (priv->worker == NULL){ //make sure is started
		throw new std::exception("Error creating worker thread");
	}
}


WorkerThread::~WorkerThread()
{
	if (priv){ //if we are initialized
		if (priv->worker){ //make sure the thread is not still active
			throw new std::exception("Thread must be killed before it is delted");
		}
		priv->pool = NULL; //clear the link to the owner pool
		delete priv; //delete private data
	}
	priv = NULL; //clear refrence to private data
}

void WorkerThread::_start(WorkerThread* ptr){
	if (ptr == NULL){
		throw new std::exception("Thread cannot be started without a self refrence");
	}
	ptr->priv->die = false;
	ptr->priv->currentJob = NULL;
	
	ptr->main(); //do the main loop

}

void WorkerThread::main(){
	LOG(INFO, "WorkerThreads", "Worker thread %d created", std::this_thread::get_id());
	while (! priv->die){
		if (priv->currentJob){
			priv->currentJob->execute();
			priv->currentJob = NULL;
			priv->pool->_jobCompleted(this); //signal that job was completed
		}
		else{
			priv->mtx->try_lock();
			priv->cv.wait_for(*priv->mtx,std::chrono::milliseconds(250)); //every 1/4 sec check die condition TODO: should this be shorter?
		}
	}
	if (priv) //prevent deleting the mutex while it is locked
		priv->mtx->unlock();

	LOG(INFO, "WorkerThreads", "Worker thread %d ended", std::this_thread::get_id());
}

void WorkerThread::giveJob(Job* newJob){
	if (!priv->pool){ //get the thread pool the first time we recieve a job
		WorkerThreadPool * pool = WorkerThreadPool::getThreadPool();

		if (pool == NULL){
			throw new std::exception("Error getting thread pool handle");
			return;
		}
	}
	
	if (priv->currentJob) //protect from job overwrite
		throw new std::exception("Thread busy");

	priv->currentJob = newJob; //set the job 

	priv->cv.notify_all(); //trigger event in worker thread
}

void WorkerThread::kill(){ 
	priv->die = true; //flag the thread to die next cycle
	if (priv->worker->joinable())
		priv->worker->join(); //wait for the thread to die
	delete priv->worker; //free the mem
	priv->worker = NULL; //delete refrence
}