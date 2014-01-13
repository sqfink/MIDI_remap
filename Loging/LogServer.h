#pragma once
#include <stdio.h>

/*
Predefinition of LogServer
*/
class LogServer;

/*
Global link to the log server
*/
extern LogServer Log;

/* Use this to do log writes. 
DO NOT INCLUDE \n at end of message
WARNING: Log messages are truncated at 1024 chars
*/
#define LOG(SEVERITY, LOG_ALIAS, LOG_FORMAT, ...) _LOG(SEVERITY, LOG_ALIAS, LOG_FORMAT, __VA_ARGS__)

//Use LOG instead
#define _LOG(SEVERITY, LOG_ALIAS, LOG_FORMAT, ...) { Log.write(SEVERITY, LOG_ALIAS, __TIMESTAMP__, __FILE__, __FUNCTION__, __LINE__, LOG_FORMAT, __VA_ARGS__); }

//Begins a try block
#define LOG_TRY try{

//Ends a try block, starts a catch block, logs a message at specified severity of the message attatched to the ecxeption
#define LOG_BEGIN_CATCH(SEVERITY, LOG_ALIAS) }catch(std::exception * e){ if(e) LOG(SEVERITY, LOG_ALIAS, "Exception occured! message: %s", e->what()) else LOG(SEVERITY, LOG_ALIAS, "Exception occured!") 

//Ends a catch block
#define LOG_END_CATCH }

/*
Possible levels of loging
Log writes are sent with a log level
If the log level is lower than the current setting it will be printed
*/
enum LogLevels{
	DISABLED,	//Disables all loging
	CRITICAL,	//Breaks execution of entire system
	ERR,		//Stops execution of current event but does not caus crash
	WARN,		//Execution of current segment may continue but there was a problem
	INFO,		//No problems but something the user should know	
	FINE,		//Not important enough to always tell the user
	DEBUG,		//Debug information that would flood the user	
};

/*
Provides loging interface
*/
class LogServer
{
public:
	/*
	Gets the singleton refrence to the log server
	*/
	static LogServer* getLogHandle();

	/*Messages with greater logging level index than this will be ignored*/
	static void setLogLevel(LogLevels level);

	/*
	Sets wheter or not messages with an INFO severity will be printed to stdout
	*/
	static void showInfoLogsInStdout(bool state);

	/*
	Sets whether or not to print WARN,ERR,CRITICAL messages to stderr
	*/
	static void showErrInStderr(bool state);

	/*
	Writes the line of text to the log at the specified severity
	Functions as printf
	Newline at end of message not required
	*/
	void write(LogLevels severity, const char * alias, const char * timestamp,  const char * file, const char * function, int line, const char* format, ...);

	/*
	Constructor for the log server
	*/
	LogServer();

	/*
	Closes all open handles and kills the log thread
	*/
	~LogServer();

	/*
	Sets the provided file as the log target
	*/
	void setLogFile(const char * loc);

	/*
	Overload for setting the log file using a file pointer for use in selecting stdout/stderr
	*/
	void setLogFile(FILE* fd); 

private:

	bool logInfoToStdout; //flag for if it should print to stdout

	bool logErrToStderr; //flag for if it should print to stderr

	LogLevels logSeverity; //the minimum severity to print

	struct LogServerPrivateData * priv; //Hides the stuff that would break CLR from the public header

	/*
	Used as entry point for new thread
	Calls _main()
	*/
	static void _start(LogServer *);

	/*
	Main loop for the loging thread
	*/
	void main();

	static bool alreadyExists; //Flag for preventing multiple instances

	FILE * fd; //file pointer to use for writing the log
};

