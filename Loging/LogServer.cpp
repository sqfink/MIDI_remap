#include "LogServer.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <list>
#include <ctime>
#include <cstdarg>
#include <stdio.h>
#include <cstdio>

/*
At runtime start default to not running
*/
bool LogServer::alreadyExists = false;

/*
Contains all data to be printed by the log thread
*/
struct LogMessage{
	char * message; //baked version of the format string and associated data

	LogLevels severity; //The severity of the message 
	
	const char * alias; //The alias of the message sender

	const char * timestamp; //The timestamp of compilation
	
	const char * file; //The file that 
	
	const char * function; //The function the message was sent from

	time_t sendTime; //the timestamp for when themessage was sent
	
	int line; //The line number of the source file
};

/*
LogServer attributes that are hidden from global acessavility to prevent clr breaking imports in header
*/
struct LogServerPrivateData{
	volatile bool die; //For signaling the thread to end main loop

	std::thread * logThread; //Pointer to the logging thread

	std::condition_variable_any cv; //Eventing system for the log thread

	std::mutex mtx; //Mutex for use by the condition variable

	std::mutex logQueueLock; //Mutext for locking the log queue for adds and removes

	std::list<LogMessage *> logQueue; //The queue of messages to print

	unsigned long queuedMessages; //The number of messages in the queue
};

LogServer::LogServer() {
	if (alreadyExists)
		throw new std::exception("Log server already running");

	priv = NULL;
	priv = new LogServerPrivateData(); //creates the private data container

	if (priv == NULL)
		throw new std::exception("Error allocating the LogServer private data");

	priv->logThread = NULL; //null the thread pointer

	alreadyExists = true; //flag to not allow any more intializations

	fd = stdout;
}

void LogServer::setLogFile(FILE* fd){
	if (fd == NULL)
		throw new std::exception("Cannot output to NULL file");

	this->fd = fd;
}

void LogServer::setLogFile(const char* outfile){
	fd = NULL;
	fd = fopen(outfile, "a+");
	if (fd == NULL)
		throw new std::exception("Error opening output file for write");

}

LogServer::~LogServer() {
	if (!alreadyExists) //nothing to do if the log server is not runing
		return;

	if (priv){
		priv->die = true; //signal thread to die
		priv->logThread->join(); //wait for it to die
		delete priv->logThread; //free the mem
		if (priv->queuedMessages){ //if there are messages
			for (auto m : priv->logQueue){ //foreach message
				if (!m) //message was null
					continue;
				if (m->message)
					free(m->message); //release the memory used by the message text
				m->message = NULL; //delete the refrence to the text
				free(m); //free the message
			}
			priv->queuedMessages = 0; //no more messages exist
			priv->logQueue.clear(); //remove any refrences in the list
		}
	}
}

void LogServer::_start(LogServer * ptr){
	if (ptr == NULL){
		throw new std::exception("Thread cannot be started without a self refrence");
	}

	ptr->priv->die = false; //dont instantly die on start

	printf("Log thread started\n");
	ptr->main(); //do the main loop
	printf("Log thread terminated\n"); //thread is now dead
}

void LogServer::main(){
	time_t rawtime;
	time(&rawtime); //get the time
	char* timeval = ctime(&rawtime); //get a time string from the current time
	fprintf(fd, "Log opened at %s", timeval);
	//free(timeval); //free the time string

	priv->queuedMessages = 0; //we can't have mesages yet

	fprintf(fd, "Log sender,Message send time,Severity,Filename,Compliation timestamp,Function,Line number,Message\n");
	while (!priv->die){
		if (priv->queuedMessages){
			LogMessage * message = NULL;

			priv->logQueueLock.lock(); //lock the message queue
			message = priv->logQueue.front(); //get the next message in the queue
			priv->logQueue.pop_front(); //remove the message from the queue
			priv->queuedMessages--; //decrease pending message count
			priv->logQueueLock.unlock(); //unlock the message queue

			if (message == NULL) //handle null messages gracefully
				continue;

			char* timeval = ctime(&message->sendTime); //get a time string from the message
			timeval[strlen(timeval) - 1] = '\0'; //remove the null terminator

			fprintf(fd, "%s,%s,%d,%s,%s,%s,%d,%s\n", //do the print
				message->alias,
				timeval,
				message->severity,
				message->file,
				message->timestamp,
				message->function,
				message->line,
				message->message);

			//free(timeval); //free the time string //FIXME: should this really not be freed?
			timeval = NULL; //remove the refrence
		}
		else{
			fflush(fd); //not busy so flush the file
			priv->mtx.try_lock(); //wait for the event
			priv->cv.wait_for(priv->mtx, std::chrono::milliseconds(250)); //every 1/4 sec check die condition TODO: should this be shorter?
		}
	}
	fclose(fd); //close the file
	if (priv) //prevent deleting the mutex while it is locked
		priv->mtx.unlock();	
}

void LogServer::write(LogLevels severity, const char * alias, const char * timestamp, const char * file, const char * function, int line, const char* format, ...){
	if (!priv->logThread){ //if the thread is not running start it
		priv->logThread = new std::thread(_start, this); //start the thread
		if (priv->logThread == NULL){ //make sure is started
			throw new std::exception("Error creating log thread");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));  //sleep for 1/4 sec before proceeding to allow thread to get set up
	}

	LogMessage * mes = NULL;
	mes = new LogMessage(); //create a message

	mes->alias = alias;		//copy data
	mes->file = file;
	mes->function = function;
	mes->line = line;
	mes->severity = severity;
	mes->timestamp = timestamp;
	
	time(&mes->sendTime); //load the current time

	mes->message = NULL; 
	mes->message = (char*)calloc(1025, sizeof(char)); //allocate space for 1024 char message with an end \0

	va_list args;
	va_start(args, format); //load the ... args
	vsnprintf(mes->message, 1024, format, args); //printf it into message->message
	va_end(args); //close the args

	priv->logQueueLock.lock(); //lock the queue for add
	priv->logQueue.push_back(mes); //add the message to the queue
	priv->queuedMessages++; //increment the message counter
	priv->logQueueLock.unlock(); //unlock the queue

	priv->cv.notify_one(); //proc the event on the thread
}

