/**************************************************
* Provides a simple interface for devices to provide to be loaded by the core engine
* 
**************************************************/

namespace DeviceDLL{

	/*
	Provides the name of the device that the DLL handles as reported by the system when queried
	*/
	const wchar_t* __declspec(dllexport) __stdcall getDeviceName();



}
