#ifndef MCATPROCEDURES_H
#define MCATPROCEDURES_H

#include <Windows.h>
#include "../CatEngine/CatEngine.h"

/************************************************************************/
/* // Usage CatCaller!!

// bool CatLoadDriver()
// bool CatUnloadDriver)()
// bool CatStartDriver()
// bool CatStopDriver()
// bool CatGeneral(LPVOID, ULONG);
// bool CatOpenProcess(ULONG, HANDLE&);
// bool CatReadProcessMemory(HANDLE, ULONG, LPVOID, ULONG, ULONG&);
// bool CatWriteProcessMemory(HANDLE, ULONG, const LPVOID, ULONG, ULONG&);

extern PfnCatLoadDriver CatLoadDriver;
extern PfnCatUnloadDriver CatUnloadDriver;
extern PfnCatStartDriver CatStartDriver;
extern PfnCatStopDriver CatStopDriver;
extern PfnCatGeneral CatGeneral;
extern PfnCatOpenProcess CatOpenProcess;
extern PfnCatReadProcessMemory CatReadProcessMemory;
extern PfnCatWriteProcessMemory CatWriteProcessMemory;

if (CatInitProcedures() == true) {
	CATCALLER_PRINT_PROCEDURES
	CATCALLER_VERIFY_PROCEDURES(NOTHING)

	// Load & start own driver
	CatLoadDriver();
	CatStartDriver();

	Your code here ...

	// Unload & stop own driver
	CatStopDriver();
	CatUnloadDriver();
}
*/
/************************************************************************/

#define CALLER_NAME	TEXT("CatCaller.dll")

typedef bool (*PfnCatLoadDriver)();
typedef bool (*PfnCatUnloadDriver)();
typedef bool (*PfnCatStartDriver)();
typedef bool (*PfnCatStopDriver)();
typedef bool (*PfnCatGeneral)(LPVOID, ULONG);
typedef bool (*PfnCatOpenProcess)(ULONG PID, HANDLE& Handle);
typedef bool (*PfnCatReadProcessMemory)(HANDLE Handle, ULONG Address, LPVOID Buffer, ULONG nSize, ULONG& ReadBytes);
typedef bool (*PfnCatWriteProcessMemory)(HANDLE Handle, ULONG Address, const LPVOID Buffer, ULONG nSize, ULONG& WrittenBytes);

#ifndef NOTHING
#define NOTHING
#endif

#define CATCALLER_VERIFY_PROCEDURES(code) \
if (!CatLoadDriver) { \
catMsg(T("CatLoadDriver could not be located!")); \
code; \
}\
if (!CatUnloadDriver) { \
catMsg(T("CatUnloadDriver could not be located!")); \
code; \
}\
if (!CatStartDriver) { \
catMsg(T("CatStartDriver could not be located!")); \
code; \
}\
if (!CatStopDriver) { \
catMsg(T("CatStopDriver could not be located!")); \
code; \
}\
if (!CatGeneral) { \
catMsg(T("CatGeneral could not be located!")); \
code; \
}\
if (!CatOpenProcess) { \
catMsg(T("CatOpenProcess could not be located!")); \
code; \
}\
if (!CatReadProcessMemory) { \
catMsg(T("CatReadProcessMemory could not be located!")); \
code; \
}\
if (!CatWriteProcessMemory) { \
catMsg(T("CatWriteProcessMemory could not be located!")); \
code; \
}

#define CATCALLER_PRINT_PROCEDURES \
catMsg(T("**********************************************")); \
catMsg(T("CatLoadDriver = %#0.8lx"), CatLoadDriver); \
catMsg(T("CatUnloadDriver = %#0.8lx"), CatUnloadDriver); \
catMsg(T("CatStartDriver = %#0.8lx"), CatStartDriver); \
catMsg(T("CatStopDriver = %#0.8lx"), CatStopDriver); \
catMsg(T("CatGeneral = %#0.8lx"), CatGeneral); \
catMsg(T("CatOpenProcess = %#0.8lx"), CatOpenProcess); \
catMsg(T("CatReadProcessMemory = %#0.8lx"), CatReadProcessMemory); \
catMsg(T("CatWriteProcessMemory = %#0.8lx"), CatWriteProcessMemory); \
catMsg(T("**********************************************"));

bool CatInitProcedures();
bool CatDestroyProcedures();

#endif // MCATPROCEDURES_H