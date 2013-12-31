// midiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "MIDI_Fighter.h"
#include "MIDI_Device.h"

/*
Start of program
*/
int _tmain(int argc, _TCHAR* argv[])
{
	try{
		std::vector<WCHAR*> tmp = MIDI_Device::listDevices();
		if (tmp.size() == 0){
			printf("No MIDI devices detected\n");
		}
		else{
			printf("%d MIDI devices detected\n", tmp.size());
			for (auto item : tmp){
				if (item != NULL)
					printf("\tdevice: %ls\n", item);
				else
					printf("\tItem was NULL\n");
			}
		}
	}
	catch (std::exception * e){
		if (e == NULL){
			printf("NULL ERROR\n");
		}
		else{
			printf("Error: %s\n", e->what());
		}
	}
	system("PAUSE");
	return 0;
}

