// midiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "MIDI_Fighter.h"
#include "MIDI_Device.h"
#include "WorkerThreadPool.h"
#include "LogServer.h"

/*
Start of program
*/
int _tmain(int argc, _TCHAR* argv[])
{
	try{
		Log.setLogFile(stdout);
		WorkerThreadPool * w = WorkerThreadPool::getThreadPool();
		std::vector<WCHAR*> tmp = MIDI_Device::listDevices();
		MIDI_Device * dev = new MIDI_Fighter();
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
			if(MIDI_Device::getDeviceIdByName(MIDI_Fighter::getName()) != -1){ //check if the device is present
				dev->connect();
				dev->start();
			}
		}
		system("PAUSE");
		dev->stop();
		dev->disconnect();
		w->destroy();
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

