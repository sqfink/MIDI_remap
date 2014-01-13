/**************************************************
* Provides a simple interface for devices to provide to be loaded by the core engine
* 
**************************************************/
#include "MIDI_Device.h"

typedef const wchar_t * (*getDeviceNameFunc) ( );

typedef MIDI_Device* (*getNewDeviceHandleFunc) ( );

/*
Provides the name of the device that the DLL handles as reported by the system when queried
*/
extern "C" __declspec(dllexport) const wchar_t * getDeviceName();

/*
Instantiates a new MIDI device and returns a handle to it
*/
extern "C" __declspec(dllexport) MIDI_Device * getNewDeviceHandle();
