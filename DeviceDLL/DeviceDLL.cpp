#include "DeviceDLL.h"
#include "LogServer.h"

std::map<const wchar_t *, DeviceDLL*> DeviceDLL::loadedDevices;

DeviceDLL::DeviceDLL(const wchar_t * fname) {
	isDevice = false; //not a valid device until confirmed
	getDevHandleFunc = NULL; //init the loaded fnction pointers to NULL 
	getDevNameFunc = NULL;
	module = NULL; //intit module to not exist
	dev = NULL; //init device to not exist

	if (fname == NULL)
		throw new std::exception("Cannot load NULL DeviceDLL"); //syntax error. tell the programmer


	if (wcsstr(fname, L".dll") == NULL){
		return; //this is not a dll
	}

	module = LoadLibrary(fname);
	if (module == NULL){	//not a valid module
		LOG(ERR, "DeviceDLL", "Error opening DLL %ls", fname);
		return; //exit constructor
	}
	


	getDevNameFunc = (getDeviceNameFunc) GetProcAddress(module, "getDeviceName"); //try to retrieve the getDeviceName function pointer from the DLL
	getDevHandleFunc = (getNewDeviceHandleFunc) GetProcAddress(module, "getNewDeviceHandle"); //try to retrieve the getNewDeviceHandle function pointer from the DLL

	if (getDevHandleFunc == NULL || getDevNameFunc == NULL){ //if either function was not loaded
		getDevHandleFunc = NULL; //clear any loaded function pointers
		getDevNameFunc = NULL;

		FreeLibrary(module); //unload the DLL

		module = NULL; //delete the module refrence
		LOG(WARN, "DeviceDLL", "%ls is not a device DLL", fname);
		return; //exit constructor
	}

	isDevice = true; //all loaded correctly
	loadedDevices[getDevNameFunc()] = this; //add to the map of loaded device modules
}

MIDI_Device* DeviceDLL::getDeviceHandle(){
	if (isDevice){ //ensure it is a valid device
		if (dev) //if there is alread a device handle present return that
			return dev;

		dev = getDevHandleFunc(); //Device does not yet exist, create it
		if (dev){
			LOG(INFO, "DeviceDLL", "Handle to %s device created", getDevNameFunc());
		}
		else{
			LOG(WARN, "DeviceDLL", "Handle to %s device creation failed", getDevNameFunc());
		}
		return dev;
	}
	return NULL;
}

const wchar_t * DeviceDLL::getDeviceName(){
	if (isDevice){ //ensure it is a valid device
		if (getDevNameFunc){ //make sure the get name function pointer exists
			return getDevNameFunc();
		}
		LOG(WARN, "DeviceDLL", "device exists but getDevNameFunc does not")
	}
	return NULL;
}

DeviceDLL::~DeviceDLL() {
	if (isDevice){
		if (module){
			loadedDevices.erase(loadedDevices.find(getDevNameFunc())); //remove from the map of loaded devices

			getDevHandleFunc = NULL; //clear the function pointers
			getDevNameFunc = NULL;

			FreeLibrary(module); //unload the DLL
		}
		module = NULL;
	}
	isDevice = false;
}

bool DeviceDLL::isDeviceDLL(){
	return isDevice;
}

std::list<const wchar_t*> DeviceDLL::listLoadedDeviceModules(){
	std::list<const wchar_t*> lst;
	for (auto a : loadedDevices){ //pull names of all loaded devices
		lst.push_back(a.first); //add them to the device list
	}
	return lst;
}

int DeviceDLL::loadDevicesFromFolder(const wchar_t * filename){
	if (filename == NULL)
		throw new std::exception("Cannot load modules from NULL");

	WIN32_FIND_DATA FindFileData;
	int i = 0;
	HANDLE hFind;
	hFind = FindFirstFile(filename, &FindFileData); //find the first file in the directory
	if (hFind == INVALID_HANDLE_VALUE) {
		LOG(WARN, "DeviceDLL", "Error opening folder");
		return 0;
	}

	do { //for each file in directory
		DeviceDLL * d = new DeviceDLL(FindFileData.cFileName); //load the device into the loaded map
		if (d->isDeviceDLL())
			i++;
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);

	LOG(INFO, "DeviceDLL", "Loaded %d device DLLs from %ls", i, filename);
	for (auto a : listLoadedDeviceModules()){
		LOG(FINE, "DeviceDLL", "Device DLL %ls loaded", a);
	}
	return i;
}

DeviceDLL * DeviceDLL::getLoadedDeviceDLL(const wchar_t * devName){
	for (auto a : loadedDevices){ //loop over all loaded devices
		if (wcscmp(a.first, devName) == 0){ 
			return a.second; //return the refrence if the names are equal
		}
	}
	return NULL; //device not loaded, return NULL
}
