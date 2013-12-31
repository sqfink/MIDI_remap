#include "Action_key.h"
#include <Windows.h>
#include <stdio.h>
#include <exception>

Action_key::Action_key(int vk, bool state) {
	this->vk = vk;
	this->state = state;
}

Action_key::Action_key(char key, bool state) {
	if ((key < 'A' || key > 'Z') && (key < '0' || key > '9')){
		printf("Invalid key code: %c\n", key);
		throw new std::exception("Invalid key code");
	}

	if (key > 'A' && key < 'Z'){ //was a letter
		vk = 0x41 + key - 'A';
	}
	else{ //was a number
		vk = 0x30 + key - '0';
	}

	this->state = state;
}

void Action_key::doAction(){
	INPUT tmp;
	
	tmp.ki.wVk = vk;
	tmp.ki.dwFlags = state ? 0 : KEYEVENTF_KEYUP;
	tmp.ki.dwExtraInfo = 0;
	tmp.ki.time = 0;
	tmp.ki.wScan = 0;

	tmp.type = INPUT_KEYBOARD;

	SendInput(1, &tmp, sizeof(INPUT));
}

Action_key::~Action_key()
{
	vk = 0;
	state = false;
}
