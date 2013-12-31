#pragma once
#include "Action.h"
#include <vector>

class Job {
public:
	Job();

	~Job();

	void execute();

	void addAction(Action*);

private:
	
	int actionCount;

	bool mayBlock;

	std::vector<Action*> actions;
};

