// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CATCALLER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CATCALLER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "../General/CatEngine/CatEngine.h"
#include "../General/Headers/MGlobal.h"
#include "../General/Headers/MCtrlCodes.h"

#ifdef CATCALLER_EXPORTS
#define CATAPI __declspec(dllexport)
#else
#define CATAPI __declspec(dllimport)
#endif

#define DRIVER_SYMB (L"\\\\.\\" DRIVER)

//#define CatPrintf(Format, ...) catMsgW(L"CatCaller: " Format L"\n", __VA_ARGS__)

CcatServiceW myService;
CcatFileW myFile;

#ifdef __cplusplus
extern "C" {
	CATAPI bool CatLoadDriver();
	CATAPI bool CatUnloadDriver();
	CATAPI bool CatStartDriver();
	CATAPI bool CatStopDriver();
	CATAPI bool CatGeneral(LPVOID, ULONG);
	CATAPI bool CatOpenProcess(ULONG, HANDLE&);
	CATAPI bool CatReadProcessMemory(HANDLE, ULONG, LPVOID, ULONG, ULONG&);
	CATAPI bool CatWriteProcessMemory(HANDLE, ULONG, const LPVOID, ULONG, ULONG&);
};
#endif