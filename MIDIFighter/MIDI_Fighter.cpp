#include "MIDI_Fighter.h"
#include "WorkerThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include "Action_key.h"

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

}

char decToHex(unsigned int dec){
	dec = dec % 16;
	if (dec < 10)
		return '0' + dec;

	return 'A' + (dec - 10);
}

MIDI_Fighter::MIDI_Fighter(){
	this->DeviceName = DEV_NAME; //device name string
	selected_bank = 0; //default to bank 0 unitl changed
	banks[0].btn[0] = new Job();
	for (int j = 0; j < NUM_BANKS; j++){
		for (int i = 0; i < BTNS_PER_BANK; i++){
			banks[j].btn[i]->addAction(new Action_key(decToHex(i), true));
			banks[j].btn[i]->addAction(new Action_key(decToHex(i), false));
		}
	}
}

int MIDI_Fighter::Impl_PreprocessMIDI(UINT msg, BYTE state, BYTE firstByte, BYTE secondByte, DWORD timestamp){
	if (msg == 963 && state == 130){
		printf("Button %d ", firstByte % 16);
		printf(" released\n");
	}
	else if (msg == 963 && state == 146){
		printf("Button %d ", firstByte % 16);
		printf(" pressed\n");
		WorkerThreadPool::getThreadPool()->submitJob(banks[selected_bank].btn[firstByte % 16]);
	}
	else if (msg == 963 && state == 147){
		printf("Side button pressed. Now in bank %d\n", firstByte);
		selected_bank = firstByte;
	}
	return 0;
}


