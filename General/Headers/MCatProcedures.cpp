#include "MCatProcedures.h"

PfnCatLoadDriver CatLoadDriver;
PfnCatUnloadDriver CatUnloadDriver;
PfnCatStartDriver CatStartDriver;
PfnCatStopDriver CatStopDriver;
PfnCatGeneral CatGeneral;
PfnCatOpenProcess CatOpenProcess;
PfnCatReadProcessMemory CatReadProcessMemory;
PfnCatWriteProcessMemory CatWriteProcessMemory;

bool CatInitProcedures()
{
	CcatLibrary catLib(CALLER_NAME);

	if (catLib.IsLibraryAvailable() == false) {
		return false;
	}

	CatLoadDriver = (PfnCatLoadDriver)catLib.catGetRoutineAddress(TEXT("CatLoadDriver"));
	CatUnloadDriver = (PfnCatUnloadDriver)catLib.catGetRoutineAddress(TEXT("CatUnloadDriver"));
	CatStartDriver = (PfnCatStartDriver)catLib.catGetRoutineAddress(TEXT("CatStartDriver"));
	CatStopDriver = (PfnCatStopDriver)catLib.catGetRoutineAddress(TEXT("CatStopDriver"));
	CatGeneral = (PfnCatGeneral)catLib.catGetRoutineAddress(TEXT("CatGeneral"));
	CatOpenProcess = (PfnCatOpenProcess)catLib.catGetRoutineAddress(TEXT("CatOpenProcess"));
	CatReadProcessMemory = (PfnCatReadProcessMemory)catLib.catGetRoutineAddress(TEXT("CatReadProcessMemory"));
	CatWriteProcessMemory = (PfnCatWriteProcessMemory)catLib.catGetRoutineAddress(TEXT("CatWriteProcessMemory"));

	CATCALLER_VERIFY_PROCEDURES(return false)

	return true;
}

bool CatDestroyProcedures()
{
	return true;
}