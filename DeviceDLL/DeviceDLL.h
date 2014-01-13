#pragma once
#include "MIDI_Device.h"
#include "iDeviceDLL.h"
#include <list>
#include <map>

/*
Provides an interface to handle DLL loading and interaction
*/
class DeviceDLL {
public:

	/*
	Attempts to load all dlls in the specified folder as devices
	valid DLLs will be added to the loaded device list
	returns the number of new devices loaded
	*/
	static int loadDevicesFromFolder(const wchar_t * filename);

	/*
	If the device with the name provided has been loaded gets the pointer to the DeviceDLL
	*/
	static DeviceDLL * getLoadedDeviceDLL(const wchar_t * devName);

	/*
	Gets a list of the currently loaded DeviceDLLs by the Device name strings they work with
	*/
	static std::list<const wchar_t*> listLoadedDeviceModules();

	/*
	Opens the DLL at the path specified by fname
	If the DLL is not of type DeviceDLL the DLL is unloaded
	*/
	DeviceDLL(const wchar_t* fname);
	
	/*
	Checks if the DLL that was loaded by the constructor was a DeviceDLL
	*/
	bool isDeviceDLL();
	
	/*
	Unloads the DLL if it was still loaded 
	*/
	~DeviceDLL();

	/*
	Gets a handle to the MIDI_Device of this DLL or returns NULL if device not present
	*/
	MIDI_Device * getDeviceHandle();

	/*
	Gets a LPWCSTR to the name of the device handled by this DLL
	*/
	const wchar_t * getDeviceName();

private:
	static std::map<const wchar_t *, DeviceDLL*> loadedDevices; //A map of the currently loaded device names and a refrence to the DeviceDLL

	MIDI_Device * dev; //pointer to the device opened by getDeviceHandle

	HMODULE module; //The DLL module that has been loaded

	getNewDeviceHandleFunc getDevHandleFunc; //pointer to the DLL funcrtion to get a new device handle

	getDeviceNameFunc getDevNameFunc; //pointer to the DLL function to get the device name
		
	bool isDevice; //flag for if the dll is a DeviceDLL to be checked before any
};

