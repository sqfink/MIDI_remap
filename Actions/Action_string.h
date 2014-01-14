#pragma once
#include "Action.h"
#include "Action_key.h"
#include <vector>


#define BEGIN_CONTROL_SEQ_CHAR '\\'

#define END_CONTROL_SEQ_CHAR ';'

class Action_string : public Action {
public:
	/*
	Executes the action set
	*/
	void doAction();
	
	/*
	Parses a null terminated string into keyboard actions
	
	Parsing will read all of the text between BEGIN_CONTROL_SEQ_CHAR and END_CONTROL_SEQ_CHAR as a virtual key
	See http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx for virtual key codes
	The first incedence of the control string will trigger the key to be depressed a second must be sent to trigger a release
	If no release is seen by the end of the control stream the key will be released at the end of the string

	END_CONTROL_SEQ_CHAR not required for \\

	WARNING: If the shift or capslock keys are depressed it will interfere with any normal characters that are being sent
	*/
	Action_string(const char *);

	/*
	Destroys the object
	*/
	~Action_string();

private:
	std::vector<Action_key*> keys;
};

