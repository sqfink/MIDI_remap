#pragma once
#include <Windows.h>
#include <vector>

typedef unsigned char BYTE;

/*
Base type for MIDI control devices
*/
class MIDI_Device
{
public:

	/*
	Gets the device name string
	*/
	static const wchar_t * getName();

	/*
	Gets the list of connected MIDI device names
	*/
	static std::vector<WCHAR*> listDevices();

	/*
	Called on instance creation
	*/
	MIDI_Device();

	/*
	Called on instance deletion
	*/
	~MIDI_Device();

	/*
	Connects to the device
	*/
	int connect();

	/*
	Disconnects the device
	*/
	int disconnect();

	/*
	Starts listening for MIDI commands
	*/
	void start();

	/*
	Stops listening for MIDI commands
	*/
	void stop();

	/*
	Gets the system device id number for the MIDI device
	*/
	static int getDeviceIdByName(const WCHAR * name);

protected:

	/*
	Device specific implementation of midi information handling
	*/
	virtual int Impl_PreprocessMIDI(UINT msg, BYTE state, BYTE firstByte, BYTE secondByte, DWORD timestamp) = 0;

	static const WCHAR * DeviceName; //the name of the device for matching

	HMIDIIN dev; //system device handle

	/*
	Gets the number of connected MIDI devices
	*/
	static int getDeviceCount();

private:

	static int instanceCount;//The number of instances that currently exist

	bool connected; //the current connection state

	bool started; //the current listening state

	/*
	Gets the raw MIDI data from the system
	*/
	static void CALLBACK preprocessMIDI(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
};

