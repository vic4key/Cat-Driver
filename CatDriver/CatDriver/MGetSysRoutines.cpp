#include "MGetSysRoutines.h"

/*Routine variables */
Pfn_snprintf fn_snprintf;
PfnPsSuspendProcess fnPsSuspendProcess;
PfnMmCopyVirtualMemory fnMmCopyVirtualMemory;

NTSTATUS CATNTAPI catGetModuleInfomation(wchar_t* ModuleName, PSYSTEM_MODULE ModuleInfo)
{
	CAT_TRY(NOTHING)

		NTSTATUS status = STATUS_UNSUCCESSFUL;

	if ((PVOID)ZwQuerySystemInformation == NULL) {
		return STATUS_PROCEDURE_NOT_FOUND;
	}

	ULONG retLen = 0;
	PSYSTEM_MODULE_INFORMATION o = NULL;

	status = ZwQuerySystemInformation(SystemModuleInformation, 0, 0, (PULONG)&retLen);
	if (!retLen) {
		return status;
	}

	o = (PSYSTEM_MODULE_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, retLen + 1, 'zqsi');
	if (o == NULL) {
		return STATUS_BUFFER_TOO_SMALL;
	}

	status = ZwQuerySystemInformation(SystemModuleInformation, o, retLen, (PULONG)&retLen);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	for (ULONG i = 0; i < o->ModulesCount; i++) {
		PSYSTEM_MODULE m = (PSYSTEM_MODULE)&(o->Modules[i]);
		char* fileName = catExtractFileNameA((char*)m->Name);
		wchar_t* FileName = catPacToPwc(fileName);
		if (!wcscmp(ModuleName, FileName)) {
			RtlCopyMemory(ModuleInfo, m, sizeof(SYSTEM_MODULE));
			break;
		}
	}

	ExFreePoolWithTag(o, 'zqsi');

	return STATUS_SUCCESS;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}

PVOID CATNTAPI catGetSystemRoutineAddress(__in PWSTR SystemRoutineName)
{
	UNICODE_STRING systemRoutineName;
	RtlInitUnicodeString(&systemRoutineName, SystemRoutineName);
	return MmGetSystemRoutineAddress(&systemRoutineName);
}

PVOID CATNTAPI catGetRoutineAddressBySystemCallNumber(ULONG SystemCallNumber)
{
	CAT_TRY(NOTHING)

	PSYSTEM_SERVICE_TABLE SST = KeServiceDescriptorTable;
	if (!SST) {
		return NULL;
	}

	/*CatPrintf("SST: ServiceTableBase = %ph, NumberOfServices = %ph", SST->ServiceTableBase, SST->NumberOfServices);*/

	if (SystemCallNumber >= SST->NumberOfServices) {
		return NULL;
	}

	return *(PVOID*)((ULONG)SST->ServiceTableBase + 4*SystemCallNumber);

	CAT_EXCEPT(return NULL)
}

void CATNTAPI InitDynamicSystemRoutines()
{
	CAT_TRY(NOTHING)

	fn_snprintf = (Pfn_snprintf)catGetSystemRoutineAddress(L"_snprintf");
	if (!fn_snprintf) CatPrintf("**Error: _snprintf could not be found!");

	/*fnPsSuspendProcess = (PfnPsSuspendProcess)catGetSystemRoutineAddress(L"PsSuspendProcess");
	if (!fnPsSuspendProcess) CatPrintf("**Error: PsSuspendProcess could not be found!");

	fnMmCopyVirtualMemory = (PfnMmCopyVirtualMemory)catGetSystemRoutineAddress(L"MmCopyVirtualMemory");
	if (!fnMmCopyVirtualMemory) CatPrintf("**Error: MmCopyVirtualMemory could not be found!");*/

	CAT_EXCEPT(NOTHING)
}