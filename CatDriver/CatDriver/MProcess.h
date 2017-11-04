#ifndef MPROCESS_H
#define MPROCESS_H

#include "MGetSysRoutines.h"
#include "MGeneral.h"
#include "MTypes.h"
#include <Ntifs.h>

#define CAT_STACK_COPY_BYTES	0x00000200
#define CAT_POOL_COPY_BYTES		0x00010000
#define CAT_MAPPED_COPY_PAGES	0x0000000F
#define CAT_POOL_COPY_THRESHOLD	0x000003FF

ULONG fnGetCopyExceptionInfo(
	__in PEXCEPTION_POINTERS ExceptionInfo,
	__out PBOOLEAN HaveBadAddress,
	__out PULONG_PTR BadAddress
);

NTSTATUS CATNTAPI CatOpenProcess(
	__in ULONG PID,
	HANDLE& ProcessHandle);

NTSTATUS CATNTAPI CatOpenProcess(
	__in ULONG PID,
	__out PHANDLE ProcessHandle);

NTSTATUS CATNTAPI CatReadVirtualMemory(
	__in HANDLE Handle,
	__in ULONG Address,
	__out PVOID Buffer,
	__in ULONG BufferSize,
	__out PULONG BytesDone
);

NTSTATUS CATNTAPI CatWriteVirtualMemory(
	__in HANDLE Handle,
	__in ULONG Address,
	__in PVOID Buffer,
	__in ULONG BufferSize,
	__out PULONG BytesDone
);

NTSTATUS CATNTAPI CatCopyVirtualMemory(
	__in PEPROCESS FromProcess,
	__in PVOID FromAddress,
	__in PEPROCESS ToProcess,
	__in PVOID ToAddress,
	__in SIZE_T BufferLength,
	__in KPROCESSOR_MODE AccessMode,
	__out SIZE_T ReturnLength
);

NTSTATUS CATNTAPI CatReadVirtualMemory(
	__in HANDLE ProcessHandle,
	__in PVOID BaseAddress,
	__out_bcount(BufferSize) PVOID Buffer,
	__in SIZE_T BufferSize,
	__out_opt PSIZE_T NumberOfBytesRead,
	__in KPROCESSOR_MODE AccessMode
);

NTSTATUS CATNTAPI CatWriteVirtualMemory(
	__in HANDLE ProcessHandle,
	__in_opt PVOID BaseAddress,
	__in_bcount(BufferSize) PVOID Buffer,
	__in SIZE_T BufferSize,
	__out_opt PSIZE_T NumberOfBytesWritten,
	__in KPROCESSOR_MODE AccessMode
);

#endif // MPROCESS_H