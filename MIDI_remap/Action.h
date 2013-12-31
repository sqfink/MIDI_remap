#pragma once

/*
Abstract type for implementable actions
When the action occurs in the job action execution queue doAction is called
*/
class Action {
public:
	/*
	Called when the action is to be preformed
	*/
	virtual void doAction() = 0;
};

