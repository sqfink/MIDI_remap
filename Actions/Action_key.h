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

	/*
	Creates a new key press(or release) action from a special key string
	See http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx for key codes
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

/*Takes a number 0-15 and converts it to the corresponding hex char(ASCII)*/
static char decToHex(unsigned int dec){
	dec = dec % 16;
	if (dec < 10)
		return '0' + dec;

	return 'a' + (dec - 10);
}
