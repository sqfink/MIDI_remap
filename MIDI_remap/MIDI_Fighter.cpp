#include "MIDI_Fighter.h"
#include "WorkerThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include "Action_key.h"


//const wchar_t * MIDI_Fighter::DeviceName =  L"Midifighter Spectra";


MIDI_Fighter::MIDI_Fighter(){
	this->DeviceName = L"Midi Fighter Spectra"; //device name string
	selected_bank = 0; //default to bank 0 unitl changed
	banks[0].btn[0] = new Job();
	banks[0].btn[0]->addAction(new Action_key('A', true));
	banks[0].btn[0]->addAction(new Action_key('A', false));
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
