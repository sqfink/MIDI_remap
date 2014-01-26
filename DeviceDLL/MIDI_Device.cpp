#include "MIDI_Device.h"
#include <stdio.h>
#include <exception>
#include <errno.h>
#include "LogServer.h"

int MIDI_Device::instanceCount;

const wchar_t * MIDI_Device::DeviceName = L"Generic MIDI device";

MIDI_Device * MIDI_Device::refrence;

MIDI_Device::MIDI_Device(){
	if (this->instanceCount >= 1){
		throw new std::exception("Device already exists, only one device can exist at a time");
	}

	this->instanceCount++;
	connected = false;
	started = false;
	refrence = this;
}

MIDI_Device::~MIDI_Device(){
	if (instanceCount){ //if the device 
		if (started){
			stop();
		}
		if (connected){
			disconnect();
		}
		this->instanceCount--;
		connected = false;
		started = false;
	}
}

int MIDI_Device::connect(){
	LOG(INFO, "MIDI Device", "Opening connection to %ls...\t", getDeviceName());

	if (getDeviceCount() == 0){
		LOG(INFO,"MIDI Device", "[FAILED]\tNo MIDI devices connected\n");
		return ENODEV;
	}
	
	int devID = getDeviceIdByName(getDeviceName());

	if (devID == -1){
		LOG(INFO,"MIDI Device", "[FAILED]\tDevice not connected");
		return ENODEV;
	}

	MMRESULT retVal = midiInOpen(&dev, devID, (DWORD_PTR)preprocessMIDI, (DWORD_PTR) this, CALLBACK_FUNCTION | MIDI_IO_STATUS);

	if (retVal != MMSYSERR_NOERROR){
		LOG(INFO,"MIDI Device", "[FAILED]\t");
		switch (retVal){
		case MMSYSERR_ALLOCATED:
			printf("MMSYSERR_ALLOCATED\n");
			throw new std::exception("MMSYSERR_ALLOCATED");
		case MMSYSERR_BADDEVICEID:
			printf("MMSYSERR_BADDEVICEID\n");
			throw new std::exception("MMSYSERR_BADDEVICEID");
		case MMSYSERR_INVALFLAG:
			printf("MMSYSERR_INVALFLAG\n");
			throw new std::exception("MMSYSERR_INVALFLAG");
		case MMSYSERR_INVALPARAM:
			printf("MMSYSERR_INVALPARAM\n");
			throw new std::exception("MMSYSERR_INVALPARAM");
		case MMSYSERR_NOMEM:
			printf("MMSYSERR_NOMEM\n");
			throw new std::exception("MMSYSERR_NOMEM");
		default:
			printf("Unknown error\n");
			throw new std::exception("Unknown error occured while opening MIDI connection");
		}
	}

	LOG(INFO,"MIDI Device", "[  OK  ]\n");
	
	connected = true;

	return ERROR_SUCCESS;
}

void MIDI_Device::start(){
	LOG(INFO,"MIDI Device", "Starting MIDI processing for %ls...\t", getDeviceName());

	if (!connected){
		started = false;
		LOG(INFO,"MIDI Device", "[FAILED]\tConnection not oppened\n");
		throw new std::exception("Connection not oppened\n");
	}

	MMRESULT retVal = midiInStart(dev);
	if (retVal != MMSYSERR_NOERROR){
		printf("[FAILED]\t");
		started = false;
		switch (retVal){
		case MMSYSERR_INVALHANDLE:
			printf("MMSYSERR_INVALHANDLE\n");
			throw new std::exception("MMSYSERR_INVALHANDLE");
		default:
			printf("Unknown error\n");
			throw new std::exception("Unknown error while stopping the MIDI listener");
		}
	}

	started = true;
	printf("[  OK  ]\n");
}

void MIDI_Device::stop(){
	LOG(INFO, "MIDI Device", "Stoping MIDI processing for %ls...\t", getDeviceName());

	if (!connected){
		started = false;
		LOG(INFO,"MIDI Device", "[FAILED]\tConnection not oppened\n");
		throw new std::exception("Connection not oppened\n");
	}

	if (!started){
		LOG(INFO,"MIDI Device", "[  OK  ]\n");
		return;
	}

	MMRESULT retVal = midiInStop(dev);
	if (retVal != MMSYSERR_NOERROR){
		LOG(INFO,"MIDI Device", "[FAILED]\t");
		switch (retVal){
		case MMSYSERR_INVALHANDLE:
			printf("MMSYSERR_INVALHANDLE\n");
			throw new std::exception("MMSYSERR_INVALHANDLE");
		default:
			printf("Unknown error\n");
			throw new std::exception("Unknown error while stoping the MIDI listener");
		}
	}

	started = false;
	LOG(INFO,"MIDI Device", "[  OK  ]\n");
}


int MIDI_Device::disconnect(){
	if (connected && started){
		stop();
	}

	LOG(INFO, "MIDI Device", "Closing connection to %ls...\t", getDeviceName());

	if (!connected){
		LOG(INFO,"MIDI Device", "[  OK  ]\n");
		return ERROR_SUCCESS;
	}

	MMRESULT retVal = midiInClose(dev);
	if (retVal != MMSYSERR_NOERROR){
		LOG(INFO,"MIDI Device", "[FAILED]\t");
		switch (retVal){
		case MIDIERR_STILLPLAYING:
			printf("MIDIERR_STILLPLAYING\n");
			throw new std::exception("MIDIERR_STILLPLAYING");
		case MMSYSERR_INVALHANDLE:
			printf("MMSYSERR_INVALHANDLE\n");
			throw new std::exception("MMSYSERR_INVALHANDLE");
		case MMSYSERR_NOMEM:
			printf("MMSYSERR_NOMEM\n");
			throw new std::exception("MMSYSERR_NOMEM");
		default:
			printf("Unknown error\n");
			throw new std::exception("Unknown error occured while closing MIDI connection");
		}
	}
	connected = false;
	LOG(INFO,"MIDI Device", "[  OK  ]\n");
	return ERROR_SUCCESS;
}

int MIDI_Device::getDeviceCount(){
	return midiInGetNumDevs();
}

int MIDI_Device::getDeviceIdByName(const WCHAR * name){
	if (name == NULL || wcslen(name) == 0){
		LOG(INFO,"MIDI Device","Device name cannot be NULL");
		return -1;
	}
	int enumeratedDevices = 0;
	int lastDeviceID = 0;

	MIDIINCAPS caps; //MIDI device capabilites structure

	while (enumeratedDevices < getDeviceCount()){ //while we have not enumerated all devices
		for (int i = lastDeviceID; i < getDeviceCount(); i++){ //incrementally check all device ids after the last valid one
			
			MMRESULT retVal = midiInGetDevCaps(i, &caps, sizeof(caps)); //get the capabilities (name) of the device with this ID

			if (retVal != MMSYSERR_NOERROR){
				if (retVal != MMSYSERR_BADDEVICEID)
					printf("Error while getting device capabilities: ");

				switch (retVal){
				case MMSYSERR_BADDEVICEID:
					continue; //the device ID was not valid try the next one
				case MMSYSERR_INVALPARAM:
					printf("MMSYSERR_INVALPARAM\n");
					throw new std::exception("MMSYSERR_INVALPARAM");
				case MMSYSERR_NODRIVER:
					printf("MMSYSERR_NODRIVER\n");
					throw new std::exception("MMSYSERR_NODRIVER");
				case MMSYSERR_NOMEM:
					printf("MMSYSERR_NOMEM\n");
					throw new std::exception("MMSYSERR_NOMEM");
				default:
					printf("Unknown error\n");
					throw new std::exception("Unknown error occured while enumerating MIDI devices");
				}
			}

			//the device was valid
			if (! wcscmp(caps.szPname, name)){
				return i;//the device matched the search string
			}
			lastDeviceID = i; //update last device ID
		}
		enumeratedDevices++; //increment device count
	}
	//all devices enumerated and string not matched
	return -1;
}

void MIDI_Device::preprocessMIDI(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2){
	if (dwInstance == NULL){
		LOG(ERR, "MIDI Device", "Instance cannot be NULL\n");
		throw new std::exception("Instance cannot be NULL");
	}

	MIDI_Device* MIDIdev = (MIDI_Device*)dwInstance; //pull the device instance back out
	
	BYTE firstByte = (unsigned char)(((DWORD)dwParam1 & 0xFF00) >> 8);

	BYTE secondByte = (unsigned char)(((DWORD)dwParam1 & 0xFF0000) >> 16);

	BYTE statusByte = (unsigned char)((DWORD)dwParam1 & 0xFF);

	ULONG timestamp = (DWORD)dwParam2;

	MIDIdev->Impl_PreprocessMIDI(wMsg, statusByte, firstByte, secondByte, timestamp); //send the MIDI command to the implementation
}

std::vector<WCHAR*> MIDI_Device::listDevices(){
	std::vector<WCHAR*> tmp;
	
	if (getDeviceCount() == 0)
		return tmp; //NO devices connected

	int enumeratedDevices = 0;
	int lastDeviceID = 0;

	MIDIINCAPS caps; //MIDI device capabilites structure


	while (enumeratedDevices < getDeviceCount()){ //while we have not enumerated all devices
		for (int i = lastDeviceID; i < INT_MAX; i++){ //incrementally check all device ids after the last valid one

			MMRESULT retVal = midiInGetDevCaps(i, &caps, sizeof(caps)); //get the capabilities (name) of the device with this ID

			if (retVal != MMSYSERR_NOERROR){
				if (retVal != MMSYSERR_BADDEVICEID)
					printf("Error while getting device capabilities: "); 

				switch (retVal){
				case MMSYSERR_BADDEVICEID:
					continue; //the device ID was not valid try the next one
				case MMSYSERR_INVALPARAM:
					printf("MMSYSERR_INVALPARAM\n");
					throw new std::exception("MMSYSERR_INVALPARAM");
				case MMSYSERR_NODRIVER:
					printf("MMSYSERR_NODRIVER\n");
					throw new std::exception("MMSYSERR_NODRIVER");
				case MMSYSERR_NOMEM:
					printf("MMSYSERR_NOMEM\n");
					throw new std::exception("MMSYSERR_NOMEM");
				default:
					printf("Unknown error\n");
					throw new std::exception("Unknown error occured while enumerating MIDI devices");
				}
			}

			wchar_t * name = (wchar_t*) calloc(32, sizeof(wchar_t));
			lstrcpyW(name, caps.szPname);

			//the device was valid, add it to the list
			tmp.push_back(name);

			lastDeviceID = i; //update last device ID
			break;
		}
		enumeratedDevices++; //increment device count
	}
	return tmp;
}

const wchar_t * MIDI_Device::getName(){
	return DeviceName;
}

MIDI_Device * MIDI_Device::getInstance(){
	return refrence;
}

void MIDI_Device::destroyInstance(){
	if (refrence){
		delete refrence;
	}
	refrence = NULL;
}