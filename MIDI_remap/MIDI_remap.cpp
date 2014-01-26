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
	LOG_TRY //catch errors
		Log.setLogFile("log.csv");
		LogServer::setLogLevel(DEBUG);
		LogServer::showInfoLogsInStdout(true);
		LogServer::showErrInStderr(true);

		WorkerThreadPool * w = WorkerThreadPool::getThreadPool();
		std::vector<WCHAR*> tmp = MIDI_Device::listDevices();
		std::vector<MIDI_Device*> runningDevices;
		DeviceDLL::loadDevicesFromFolder(L".\\*");
		if (tmp.size() == 0){
			LOG(WARN, "Core", "No MIDI devices detected");
		}
		else{
			LOG(INFO, "Core", "%d MIDI devices detected", tmp.size());
			for (auto item : tmp){
				if (item != NULL){
					LOG(INFO, "Core","MIDI device %ls detected", item);
					DeviceDLL* module = DeviceDLL::getLoadedDeviceDLL(item); //gets the loaded module for the device
					if (module == NULL){
						LOG(WARN, "Core", "No module found for MIDI device %ls", item);
						continue;		//device module not found
					}
					MIDI_Device * dev = module->getDeviceHandle();	//get a handle to the device
					if (dev == NULL){
						LOG(ERR, "Core", "Error opening device handle to %ls", item);
						continue;		//error opening device handle
					}
					runningDevices.push_back(dev); //add to the list of runnign devices
					if (dev->connect() != ERROR_SUCCESS){
						LOG(ERR, "Core", "Error connecting to MIDI device %ls", item);
						continue; //error connecting to device
					}
					dev->start(); //start listening to the device
					LOG(INFO, "Core", "MIDI device %ls listening",item);
				}
				else
					LOG(ERR, "Core", "Connected MIDI device returned NULL name");
			}
		}
		system("PAUSE"); //wait unitl user is ready to exit
		LOG(INFO, "Core", "Stoping all running MIDI devices");
		for (auto d : runningDevices){ //for all devices
			if (d){
				LOG(FINE, "Core", "Stopping MIDI device %ls", d->getName());
				delete d; //destroy the device
			}
			d = NULL;
		}
		LOG(FINE, "Core", "All MIDI devices terminated");
		LOG(FINE, "Core", "Destroying thread pool");
		w->destroy(); //destroy the thread pool
		LOG(FINE, "Core", "Thread pool terminated");

	LOG_BEGIN_CATCH(CRITICAL, "Core") //Do logged catch
	LOG_END_CATCH//end log catch with no special processing

	return 0;
}

