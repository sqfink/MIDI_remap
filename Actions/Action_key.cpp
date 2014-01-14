#include "Action_key.h"
#include <Windows.h>
#include <stdio.h>
#include <exception>
#include "LogServer.h"

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
	if(vk == 0)
		return; //0 key code is not valid. don't do anything

	INPUT tmp;
	
	tmp.ki.wVk = vk;
	tmp.ki.dwFlags = state ? 0 : KEYEVENTF_KEYUP;
	tmp.ki.dwExtraInfo = 0;
	tmp.ki.time = 0;
	tmp.ki.wScan = 0;

	tmp.type = INPUT_KEYBOARD;

	SendInput(1, &tmp, sizeof(INPUT));

	LOG(DEBUG, "Action Key", "Key:%d state:%d",  tmp.ki.wVk,  state ? 0 : KEYEVENTF_KEYUP);
}

Action_key::~Action_key()
{
	vk = 0;
	state = false;
}

#define tryKeyCode(INPUT_STRING, CODE_VALUE) {if(strcmp(INPUT_STRING, keyString) == 0){ vk = CODE_VALUE;	return;	}};

Action_key::Action_key(const char * keyString, bool state){
	this->state = state;
	tryKeyCode("VK_LBUTTON",VK_LBUTTON);
	tryKeyCode("VK_RBUTTON",VK_RBUTTON);
	tryKeyCode("VK_CANCEL",VK_CANCEL);
	tryKeyCode("VK_MBUTTON",VK_MBUTTON);
	tryKeyCode("VK_XBUTTON1",VK_XBUTTON1);
	tryKeyCode("VK_XBUTTON2",VK_XBUTTON2);
	tryKeyCode("VK_BACK",VK_BACK);
	tryKeyCode("VK_TAB",VK_TAB);
	tryKeyCode("VK_CLEAR",VK_CLEAR);
	tryKeyCode("VK_RETURN",VK_RETURN);
	tryKeyCode("VK_SHIFT",VK_SHIFT);
	tryKeyCode("VK_CONTROL",VK_CONTROL);
	tryKeyCode("VK_MENU",VK_MENU);
	tryKeyCode("VK_ALT",VK_MENU);
	tryKeyCode("VK_PAUSE",VK_PAUSE);
	tryKeyCode("VK_CAPITAL",VK_CAPITAL);
	tryKeyCode("VK_CAPSLOCK",VK_CAPITAL);
	tryKeyCode("VK_ESCAPE",VK_ESCAPE);
	tryKeyCode("VK_SPACE",VK_SPACE);
	tryKeyCode("VK_PRIOR",VK_PRIOR);
	tryKeyCode("VK_PAGEUP",VK_PRIOR);
	tryKeyCode("VK_NEXT",VK_NEXT);
	tryKeyCode("VK_PAGEDN",VK_NEXT);
	tryKeyCode("VK_END",VK_END);
	tryKeyCode("VK_HOME",VK_HOME);
	tryKeyCode("VK_LEFT",VK_LEFT);
	tryKeyCode("VK_RIGHT",VK_RIGHT);
	tryKeyCode("VK_UP",VK_UP);
	tryKeyCode("VK_DOWN",VK_DOWN);
	tryKeyCode("VK_SELECT",VK_SELECT);
	tryKeyCode("VK_PRINT",VK_PRINT);
	tryKeyCode("VK_PRNTSCRN",VK_SNAPSHOT);
	tryKeyCode("VK_SNAPSHOT",VK_SNAPSHOT);
	tryKeyCode("VK_INSERT",VK_INSERT);
	tryKeyCode("VK_DELETE",VK_DELETE);
	tryKeyCode("VK_LWIN",VK_LWIN);
	tryKeyCode("VK_RWIN",VK_RWIN);
	tryKeyCode("VK_WIN",VK_LWIN);
	tryKeyCode("VK_SLEEP",VK_SLEEP);
	tryKeyCode("VK_NUMPAD0",VK_NUMPAD0);
	tryKeyCode("VK_NUMPAD1",VK_NUMPAD1);
	tryKeyCode("VK_NUMPAD2",VK_NUMPAD2);
	tryKeyCode("VK_NUMPAD3",VK_NUMPAD3);
	tryKeyCode("VK_NUMPAD4",VK_NUMPAD4);
	tryKeyCode("VK_NUMPAD5",VK_NUMPAD5);
	tryKeyCode("VK_NUMPAD6",VK_NUMPAD6);
	tryKeyCode("VK_NUMPAD7",VK_NUMPAD7);
	tryKeyCode("VK_NUMPAD8",VK_NUMPAD8);
	tryKeyCode("VK_NUMPAD9",VK_NUMPAD9);
	tryKeyCode("VK_MULTIPLY",VK_MULTIPLY);
	tryKeyCode("VK_ADD",VK_ADD);
	tryKeyCode("VK_SEPARATOR",VK_SEPARATOR);
	tryKeyCode("VK_SUBTRACT",VK_SUBTRACT);
	tryKeyCode("VK_DECIMAL",VK_DECIMAL);
	tryKeyCode("VK_DIVIDE",VK_DIVIDE);
	tryKeyCode("VK_F1",VK_F1);
	tryKeyCode("VK_F2",VK_F2);
	tryKeyCode("VK_F3",VK_F3);
	tryKeyCode("VK_F4",VK_F4);
	tryKeyCode("VK_F5",VK_F5);
	tryKeyCode("VK_F6",VK_F6);
	tryKeyCode("VK_F7",VK_F7);
	tryKeyCode("VK_F8",VK_F8);
	tryKeyCode("VK_F9",VK_F9);
	tryKeyCode("VK_F10",VK_F10);
	tryKeyCode("VK_F11",VK_F11);
	tryKeyCode("VK_F12",VK_F12);
	tryKeyCode("VK_NUMLOCK",VK_NUMLOCK);
	tryKeyCode("VK_SCROLL",VK_SCROLL);
	tryKeyCode("VK_LSHIFT",VK_LSHIFT);
	tryKeyCode("VK_RSHIFT",VK_LSHIFT);
	tryKeyCode("VK_SHIFT",VK_LSHIFT);
	tryKeyCode("VK_LCONTROL",VK_LCONTROL);
	tryKeyCode("VK_RCONTROL",VK_RCONTROL);
	tryKeyCode("VK_CONTROL",VK_LCONTROL);
	tryKeyCode("VK_LMENU",VK_LMENU);
	tryKeyCode("VK_RMENU",VK_RMENU);
	tryKeyCode("VK_MENU",VK_LMENU);
	tryKeyCode("VK_BROWSER_BACK",VK_BROWSER_BACK);
	tryKeyCode("VK_BROWSER_FORWARD",VK_BROWSER_FORWARD);
	tryKeyCode("VK_BROWSER_REFRESH",VK_BROWSER_REFRESH);
	tryKeyCode("VK_BROWSER_STOP",VK_BROWSER_STOP);
	tryKeyCode("VK_BROWSER_SEARCH",VK_BROWSER_SEARCH);
	tryKeyCode("VK_BROWSER_FAVORITES",VK_BROWSER_FAVORITES);
	tryKeyCode("VK_BROWSER_HOME",VK_BROWSER_HOME);
	tryKeyCode("VK_VOLUME_MUTE",VK_VOLUME_MUTE);
	tryKeyCode("VK_VOLUME_DOWN",VK_VOLUME_DOWN);
	tryKeyCode("VK_VOLUME_UP",VK_VOLUME_UP);
	tryKeyCode("VK_MEDIA_NEXT_TRACK",VK_MEDIA_NEXT_TRACK);
	tryKeyCode("VK_MEDIA_NEXT",VK_MEDIA_NEXT_TRACK);
	tryKeyCode("VK_MEDIA_PREV_TRACK",VK_MEDIA_PREV_TRACK);
	tryKeyCode("VK_MEDIA_PREV",VK_MEDIA_PREV_TRACK);
	tryKeyCode("VK_MEDIA_STOP",VK_MEDIA_STOP);
	tryKeyCode("VK_MEDIA_PLAY_PAUSE",VK_MEDIA_PLAY_PAUSE);
	tryKeyCode("VK_MEDIA_PLAY",VK_MEDIA_PLAY_PAUSE);
	tryKeyCode("VK_MEDIA_PAUSE",VK_MEDIA_PLAY_PAUSE);
	tryKeyCode("VK_LAUNCH_MAIL",VK_LAUNCH_MAIL);
	tryKeyCode("VK_LAUNCH_MEDIA_SELECT",VK_LAUNCH_MEDIA_SELECT);
	tryKeyCode("VK_LAUNCH_APP1",VK_LAUNCH_APP1);
	tryKeyCode("VK_LAUNCH_APP2",VK_LAUNCH_APP2);
	tryKeyCode("VK_OEM_1",VK_OEM_1);
	tryKeyCode("VK_OEM_PLUS",VK_OEM_PLUS);
	tryKeyCode("VK_OEM_COMMA",VK_OEM_COMMA);
	tryKeyCode("VK_OEM_MINUS",VK_OEM_MINUS);
	tryKeyCode("VK_OEM_PERIOD",VK_OEM_PERIOD);
	tryKeyCode("VK_OEM_2",VK_OEM_2);
	tryKeyCode("VK_OEM_3",VK_OEM_3);
	tryKeyCode("VK_OEM_4",VK_OEM_4);
	tryKeyCode("VK_OEM_5",VK_OEM_5);
	tryKeyCode("VK_OEM_6",VK_OEM_6);
	tryKeyCode("VK_OEM_7",VK_OEM_7);
	tryKeyCode("VK_OEM_8",VK_OEM_8);
	tryKeyCode("VK_OEM_102",VK_OEM_102);
	tryKeyCode("VK_ATTN",VK_ATTN);
	tryKeyCode("VK_CRSEL",VK_CRSEL);
	tryKeyCode("VK_EXSEL",VK_EXSEL);
	tryKeyCode("VK_EREOF",VK_EREOF);
	tryKeyCode("VK_PLAY",VK_PLAY);
	tryKeyCode("VK_ZOOM",VK_ZOOM);
	tryKeyCode("VK_OEM_CLEAR",VK_OEM_CLEAR);

	LOG(WARN, "Action key parser", "Command string %s not found", keyString);
	vk = 0; //key not found
}