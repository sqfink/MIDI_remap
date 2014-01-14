#include "Action_string.h"
#include <list>
#include <Windows.h> //winuser.h causes tons of errors without this
#include <Winuser.h>
#include "LogServer.h"

#define ADD_ACTION(KEY) {keys.push_back(new Action_key(KEY, true)); keys.push_back(new Action_key(KEY, false)); }
#define ADD_SHIFTED_ACTION(KEY) {keys.push_back(new Action_key(VK_LSHIFT, true)); keys.push_back(new Action_key(KEY, true)); keys.push_back(new Action_key(KEY, false)); keys.push_back(new Action_key(VK_LSHIFT, false));}

Action_string::Action_string(const char * inputString) {
	//TODO: error checking
	//FIXME: Add remaining possible input chars to parser

	char * beginControlStr = NULL;
	char * currentIndex = (char*) inputString; //start at the begining of the string
	
	std::list<char*> currentlyDepressedControlKeys; //a LIFO queue for the control keys that are active

	while (currentIndex < inputString + strlen(inputString)){ //while not at end of parse
		switch (*currentIndex){ //parse the current char
		case BEGIN_CONTROL_SEQ_CHAR: //A control sequence has started
			currentIndex++;
			beginControlStr = currentIndex; //mark the begining of the control sequence string
			if (*currentIndex = '\\'){ //leave the \\ char for processing by normal parse
				beginControlStr = NULL; //clear current control sequence start position
				continue;	//continue with switch
			}
			else{
				while (currentIndex < inputString + strlen(inputString) && *currentIndex != END_CONTROL_SEQ_CHAR){ //while the string still has info and the end control sequence char has not been seen
					currentIndex++; //increment current position
				}
				if (*currentIndex == END_CONTROL_SEQ_CHAR){ //The control sequence ended correctly
					char * tmp = (char*)calloc(beginControlStr - currentIndex + 1, sizeof(char)); //allocate a new string the size of the control string + \0
					memcpy(tmp, beginControlStr, beginControlStr - currentIndex); //copy the control string 

					bool wasAlreadyPressed = false; //create flag for if the key is already pressed
					for (auto a : currentlyDepressedControlKeys){ //check if the key is already pressed
						if (strcmp(a, tmp) == 0){ //the key exists
							wasAlreadyPressed = true; //indicate so in the flag so it is not added again
							currentlyDepressedControlKeys.remove(a); //remove the old key from the pressed key list
							keys.push_back(new Action_key(tmp, false)); //release the specified control key
							LOG(DEBUG, "Action string parser", "Control key %s released");
							break; //exit the search loop
						}
					}

					if (!wasAlreadyPressed){ //it is a new control key
						keys.push_back(new Action_key(tmp, true)); //press the specified control key
						currentlyDepressedControlKeys.push_front(tmp); //add the pressed key to the currently pressed keys LIFO queue
						LOG(DEBUG, "Action string parser", "Control key %s pressed");
					}

				} else{
					LOG(WARN, "Action string parser", "The input string ended unexpectedly while processing a control string");
				}
				beginControlStr = NULL; //clear current control sequence start position
				break; //Not a \\ sequence so the result does not need to passed down for second parse attempt
			}
			//intentionally no break here to allow the second \ of the escaped sequence to be processed lower down
		case '`':
			ADD_ACTION(VK_OEM_3);
			break;
		case '~':
			ADD_SHIFTED_ACTION(VK_OEM_3);
			break;
		default:
			LOG(WARN, "Action string parser", "No parse exists for %c", *currentIndex);
			break;
		}
		currentIndex++; //move to next index for next parse loop
	}

	if (currentlyDepressedControlKeys.size())
		LOG(WARN, "Action string parser", "%d control keys were not released properly before end of string");
	while (currentlyDepressedControlKeys.size()){ //while there are still control keys pressed
		char * key = currentlyDepressedControlKeys.front(); //get the key code
		currentlyDepressedControlKeys.pop_front(); //remove from the queue
		keys.push_back(new Action_key(key, false)); //release the key
		free(key); //free the temp key string
		key = NULL; //remove refrence
	}
}


Action_string::~Action_string() {
	
}

void Action_string::doAction(){

}