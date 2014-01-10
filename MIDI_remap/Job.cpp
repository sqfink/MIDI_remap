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
	for (auto act : actions){
		if (act)
			act->doAction();
	}
}