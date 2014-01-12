#include "Job.h"


Job::Job(){ 
	actionCount = 0;
}


Job::~Job() {
	for (auto act : actions){
		if (act)
			delete act;
		act = NULL;
	}
	actions.clear();
	actionCount = 0;
}

void Job::execute(){
	if(actionCount){
		for (auto act : actions){
			if (act)
				act->doAction();
		}
	}
}

void Job::addAction(Action * a){
	if(! a)
		throw new std::exception("Action cannot be NULL");
	actions.push_back(a);
}