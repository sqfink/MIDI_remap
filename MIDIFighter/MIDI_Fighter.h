#pragma once
#include "iDeviceDLL.h"
#include <Windows.h>
#include "Job.h"

#define BTNS_PER_BANK 16
#define NUM_BANKS 4

#define CONFIG_FILE "MIDIFighter.cfg"

struct Bank{
	UINT ID;
	Job * btn[BTNS_PER_BANK];
};

/*
Representation of the MIDI fighter spectra controller
*/
class MIDI_Fighter : public MIDI_Device {
public:

	MIDI_Fighter();
	const wchar_t * getDeviceName();

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

