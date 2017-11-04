#ifndef MGETSYSROUTINES_H
#define MGETSYSROUTINES_H

#include "stdafx.h"
#include "MTypes.h"
#include "MGeneral.h"
#include "../../General/Headers/MGlobal.h"

// Routines definition
typedef NTSTATUS (NTAPI *PfnPsSuspendProcess)(
	__in PVOID Process
);

typedef int (*Pfn_snprintf)(
	char* buffer,
	size_t count,
	const char *format,
	... 
);

typedef NTSTATUS (NTAPI *PfnMmCopyVirtualMemory)(
	__in PEPROCESS SourceProcess,
	__in PVOID SourceAddress,
	__in PEPROCESS TargetProcess,
	__out PVOID TargetAddress,
	__in SIZE_T BufferSize,
	__in KPROCESSOR_MODE PreviousMode,
	__out PSIZE_T ReturnSize
);

/* Routines linked */
#ifdef __cplusplus
extern "C" PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;

extern "C" NTSTATUS PsLookupProcessThreadByCid(
	__in PCLIENT_ID Cid,
	__out PEPROCESS *Process OPTIONAL,
	__out PETHREAD *Thread
);

extern "C" NTSTATUS ZwQuerySystemInformation(
	__in      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout   PVOID SystemInformation,
	__in      ULONG SystemInformationLength,
	__in_opt PULONG ReturnLength
);
#endif

NTSTATUS CATNTAPI catGetModuleInfomation(wchar_t* ModuleName, PSYSTEM_MODULE ModuleInfo);
PVOID CATNTAPI catGetSystemRoutineAddress(__in PWSTR SystemRoutineName);
PVOID CATNTAPI catGetRoutineAddressBySystemCallNumber(ULONG SystemCallNumber);;

void CATNTAPI InitDynamicSystemRoutines();

#endif // MGETSYSROUTINES_H