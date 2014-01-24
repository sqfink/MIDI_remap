#include "MIDI_Fighter.h"
#include "WorkerThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include "Action_key.h"
#include "LogServer.h"

#define DEV_NAME  L"Midi Fighter Spectra"

//const wchar_t * MIDI_Fighter::DeviceName =  L"Midi Fighter Spectra";

const wchar_t * getDeviceName(){
	return DEV_NAME;
}

MIDI_Device * getNewDeviceHandle(){
	if (MIDI_Fighter::getInstance())
		return MIDI_Fighter::getInstance();
	return (MIDI_Device*) new MIDI_Fighter();
}

void MIDI_Fighter::loadConfig(){
	FILE * fd = NULL;
	fd = fopen(CONFIG_FILE, "r"); 
	if (fd != NULL){
		//TODO: Load the file and do things
	}
	else{
		LOG(ERR, "MIDIFighter", "Error loading MIDIFighter config file. Reverting to hex codes");
		for (int j = 0; j < NUM_BANKS; j++){
			for (int i = 0; i < BTNS_PER_BANK; i++){
				banks[j].btn[i]->addAction(new Action_key(decToHex(i), true));
				banks[j].btn[i]->addAction(new Action_key(decToHex(i), false));
			}
		}
		return;
	}
}

MIDI_Fighter::MIDI_Fighter(){
	this->DeviceName = DEV_NAME; //device name string
	selected_bank = 0; //default to bank 0 unitl changed
	loadConfig(); 
}

int MIDI_Fighter::Impl_PreprocessMIDI(UINT msg, BYTE state, BYTE firstByte, BYTE secondByte, DWORD timestamp){
	if (msg == 963 && state == 130){
		LOG(DEBUG, "MIDIFighter", "Button %d released", firstByte % 16);
	}
	else if (msg == 963 && state == 146){
		LOG(DEBUG, "MIDIFighter", "Button %d pressed", firstByte % 16);
		WorkerThreadPool::getThreadPool()->submitJob(banks[selected_bank].btn[firstByte % 16]);
	}
	else if (msg == 963 && state == 147){
		LOG(DEBUG, "MIDIFighter", "Side button pressed. Now in bank %d", firstByte);
		selected_bank = firstByte;
	}
	return 0;
}


