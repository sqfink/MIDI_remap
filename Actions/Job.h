#pragma once
#include "Action.h"
#include <vector>

class Job {
public:
	/*
	Initialize action counter
	*/
	Job();

	/*
	Frees and deletes all loaded actions
	*/
	~Job();

	/*
	Runs the action sequence
	*/
	void execute();

	/*
	Adds an action to be executed to the end of the action list
	*/
	void addAction(Action*);

	/*
	Gets if the job contains any long runtime actions
	*/
	bool willBlock();

private:
	
	/*
	The number of loaded actions
	*/
	int actionCount;

	/*
	Do any of the actions have long runtimes
	*/
	bool mayBlock;

	/*
	The list of actions to execute
	*/
	std::vector<Action*> actions;
};

