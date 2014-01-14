#pragma once
#include "Action.h"
/*
Sends a keyboard signal to the system
Note: state=true == pressed
	The key will remain in the pressed state until a action of state=false of the same key is sent
	To send a capital letter the shift key must be in the pressed state
*/
class Action_key : public Action {
public:
	void doAction();

	/*
	Sends the key code to the system in the state specified
	Use this for sending special keys like shift, meta, alt, ctrl, etc...
	*/
	Action_key(int vk, bool state);

	/*
	Sends the letter specified to the system in the state provided
	Only chars between A and Z , 0 - 9 will be accepted
	*/
	Action_key(char key, bool state);

	Action_key(char*, bool);

	/*
	Creates a new key press(or release) action from a special key string
	*/
	Action_key(const char *, bool state);

	/*
	Frees resources
	*/
	~Action_key();

private:

	int vk; //the virtual keycode
	bool state; //the pressed state of the key
};

