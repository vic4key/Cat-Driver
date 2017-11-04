// CatCaller.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CatCaller.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		catMsg(T("************ CAT_CALLER -> LOADED ************"));
		break;
	case DLL_PROCESS_DETACH:
		catMsg(T("*********** CAT_CALLER -> UNLOADED ***********"));
		break;
	}

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/

CATAPI bool CatLoadDriver()
{
	return (myService.catLoadService(DRIVER_NAME) == true);
}

CATAPI bool CatUnloadDriver()
{
	return (myService.catUnLoadService() == true);
}

CATAPI bool CatStartDriver()
{
	return (myFile.catInit(DRIVER_SYMB, fmOpenExisting, fsNone, faUnknown) == true);
}

CATAPI bool CatStopDriver()
{
	return (myFile.catClose() == true);
}

CATAPI bool CatGeneral(LPVOID p, ULONG z)
{
	/*------- CAT_IOCTL_GENERAL -------*/
	return (myFile.catIOControl(CAT_IOCTL_GENERAL, p, (unsigned long)z, NULL, 0) == true);
}

CATAPI bool CatOpenProcess(ULONG PID, HANDLE& Handle)
{
	/*------- CAT_IOCTL_OPENPROCESS -------*/
	if (PID == (ULONG)INVALID_HANDLE_VALUE) {
		return false;
	}

	HANDLE hProcess = 0;
	bool result = false;

	PID *= MAGIC_NUMBER;

	result = (myFile.catIOControl(CAT_IOCTL_OPENPROCESS, &PID, sizeof(PID), &hProcess, sizeof(Handle)) == true);

	ULONG uHandle = ((ULONG)hProcess / MAGIC_NUMBER);
	Handle = (HANDLE)uHandle;

	return result;
}

CATAPI bool CatReadProcessMemory(HANDLE Handle, ULONG Address, LPVOID Buffer, ULONG nSize, ULONG& ReadBytes)
{
	/*------- CAT_IOCTL_READMEMORY -------*/
	if (Handle == 0 || Handle == INVALID_HANDLE_VALUE || !Address || !nSize || !Buffer) {
		return false;
	}

	TReadWriteMemoryInfo rmi = {0};
	bool result = false;

	ULONG uProcess = (ULONG)Handle * MAGIC_NUMBER;

	rmi.Handle = (HANDLE)uProcess;
	rmi.Address = (PVOID)Address;
	rmi.Buffer = Buffer;
	rmi.BufferSize = nSize;
	rmi.BytesDone = 0;

	result = (myFile.catIOControl(CAT_IOCTL_READMEMORY, &rmi, sizeof(rmi), &rmi, sizeof(rmi)) == true);

	Buffer = rmi.Buffer;

	ReadBytes = rmi.BytesDone;

	return result;
}

CATAPI bool CatWriteProcessMemory(HANDLE Handle, ULONG Address, const LPVOID Buffer, ULONG nSize, ULONG& WrittenBytes)
{
	/* ------- CAT_IOCTL_WRITEMEMORY ------- */
	TReadWriteMemoryInfo wmi = {0};
	bool result = false;

	if (Handle == 0 || Handle == INVALID_HANDLE_VALUE || !Address || !nSize || !Buffer) {
		return false;
	}

	ULONG hProcess = (ULONG)Handle * MAGIC_NUMBER;

	wmi.Handle = (HANDLE)hProcess;
	wmi.Address = (PVOID)Address;
	wmi.Buffer = Buffer;
	wmi.BufferSize = nSize;
	wmi.BytesDone = 0;

	result = (myFile.catIOControl(CAT_IOCTL_WRITEMEMORY, &wmi, sizeof(wmi), &wmi.BytesDone, sizeof(wmi.BytesDone)) == true);

	WrittenBytes = wmi.BytesDone;
	
	return result;
}