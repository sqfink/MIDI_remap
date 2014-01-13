// midiTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include "MIDI_Device.h"
#include "WorkerThreadPool.h"
#include "LogServer.h"
#include "DeviceDLL.h"

/*
Start of program
*/
int _tmain(int argc, _TCHAR* argv[])
{
	try{
		Log.setLogFile(stdout);
		WorkerThreadPool * w = WorkerThreadPool::getThreadPool();
		std::vector<WCHAR*> tmp = MIDI_Device::listDevices();
		std::vector<MIDI_Device*> runningDevices;
		DeviceDLL::loadDevicesFromFolder(L".\\*");
		if (tmp.size() == 0){
			printf("No MIDI devices detected\n");
		}
		else{
			printf("%d MIDI devices detected\n", tmp.size());
			for (auto item : tmp){
				if (item != NULL){
					printf("\tdevice: %ls\n", item);
					DeviceDLL* module = DeviceDLL::getLoadedDeviceDLL(item); //gets the loaded module for the device
					if (module == NULL)
						continue;		//device module not found
					MIDI_Device * dev = module->getDeviceHandle();	//get a handle to the device
					if (dev == NULL)
						continue;		//error opening device handle
					runningDevices.push_back(dev); //add to the list of runnign devices
					if (dev->connect() != ERROR_SUCCESS)
						continue; //error connecting to device
					dev->start(); //start listening to the device
				}
				else
					printf("\tItem was NULL\n");
			}
		}
		system("PAUSE");
		for (auto d : runningDevices){ //for all devices
			if (d)
				delete d; //destroy the device
			d = NULL;
		}
		w->destroy(); //destroy the thread pool
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

