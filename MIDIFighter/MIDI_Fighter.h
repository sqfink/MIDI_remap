#pragma once
#include "MIDI_Device.h"
#include <Windows.h>
#include "Job.h"

struct Bank{
	UINT ID;
	Job * btn[16];
};

/*
Representation of the MIDI fighter spectra controller
*/
class MIDI_Fighter : public MIDI_Device {
public:
	MIDI_Fighter();

protected:

	void loadConfig();

	/*
	Device specific implementation of midi information handling
	*/
	virtual int Impl_PreprocessMIDI(UINT msg, BYTE state, BYTE firstByte, BYTE secondByte, DWORD timestamp);

private:

	Bank banks[4]; //4 selectable button banks

	UINT selected_bank; //the currently selected button bank
};

