#include "MProcess.h"

ULONG fnGetCopyExceptionInfo(
	__in PEXCEPTION_POINTERS ExceptionInfo,
	__out PBOOLEAN HaveBadAddress,
	__out PULONG_PTR BadAddress
)
{
	PEXCEPTION_RECORD exceptionRecord;

	*HaveBadAddress = false;
	exceptionRecord = ExceptionInfo->ExceptionRecord;

	CatPrintf("Occured an exception %#0.8lx (%s) at %#0.8lx!",
		exceptionRecord->ExceptionCode,
		catGetMessageDescription(exceptionRecord->ExceptionCode, MT_EXCEPTION),
		exceptionRecord->ExceptionAddress);

	if ((exceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION) ||
		(exceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) ||
		(exceptionRecord->ExceptionCode == STATUS_IN_PAGE_ERROR))
	{
		if (exceptionRecord->NumberParameters > 1) {
			*HaveBadAddress = true;
			*BadAddress = exceptionRecord->ExceptionInformation[1];
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

NTSTATUS CATNTAPI CatOpenProcess(
	__in ULONG PID,
	HANDLE& ProcessHandle
)
{
	CAT_TRY(NOTHING)

	if (!PID || PID == (ULONG)-1) {
		ProcessHandle = 0;
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = STATUS_SUCCESS;
	ProcessHandle = 0;

	CLIENT_ID cid = {0};
	OBJECT_ATTRIBUTES oa = {0};

	RtlZeroMemory(&cid, sizeof(cid));
	cid.UniqueProcess = (HANDLE)PID;

	RtlZeroMemory(&oa, sizeof(oa));
	oa.Length = sizeof(oa);

	ProcessHandle = 0;
	status = ZwOpenProcess(&ProcessHandle, 0x001F0FFF, &oa, &cid);

	return status;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}

NTSTATUS CATNTAPI CatOpenProcess(
	__in ULONG PID,
	__out PHANDLE ProcessHandle
)
{
	CAT_TRY(NOTHING)

	if (!PID || PID == (ULONG)-1) {
		ProcessHandle = 0;
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	CLIENT_ID cid = {0};
	PEPROCESS process;
	PETHREAD thread;
	HANDLE hProcess;

	if (!(ULONG)PsLookupProcessThreadByCid) {
		return STATUS_NOT_SUPPORTED;
	}

	RtlZeroMemory(&cid, sizeof(cid));
	cid.UniqueProcess = (HANDLE)PID;
	if (cid.UniqueThread != 0) {
		status = PsLookupProcessThreadByCid(&cid, &process, &thread);
		if (NT_SUCCESS(status)) {
			ObDereferenceObject(thread);
		}
	}
	else {
		status = PsLookupProcessByProcessId(cid.UniqueProcess, &process);
	}

	if (!NT_SUCCESS(status)) {
		return status;
	}

	status = ObOpenObjectByPointer(process, 0, NULL, PROCESS_ALL_ACCESS, *PsProcessType, KernelMode, &hProcess);
	ObDereferenceObject(process);
	if (NT_SUCCESS(status)) {
		*ProcessHandle = hProcess;
	}

	return status;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}

NTSTATUS CATNTAPI CatCopyVirtualMemory(
	__in PEPROCESS FromProcess,
	__in PVOID FromAddress,
	__in PEPROCESS ToProcess,
	__in PVOID ToAddress,
	__in SIZE_T BufferLength,
	__in KPROCESSOR_MODE AccessMode,
	__out PSIZE_T ReturnLength
)
{
	UCHAR stackBuffer[CAT_STACK_COPY_BYTES];
	PVOID buffer;
	PFN_NUMBER mdlBuffer[(sizeof(MDL) / sizeof(PFN_NUMBER)) + CAT_MAPPED_COPY_PAGES + 1];
	PMDL mdl = (PMDL)mdlBuffer;
	PVOID mappedAddress;
	SIZE_T mappedTotalSize;
	SIZE_T blockSize;
	SIZE_T stillToCopy;
	KAPC_STATE apcState;
	PVOID sourceAddress;
	PVOID targetAddress;
	BOOLEAN doMappedCopy;
	BOOLEAN pagesLocked;
	BOOLEAN copyingToTarget = false;
	BOOLEAN probing = false;
	BOOLEAN mapping = false;
	BOOLEAN haveBadAddress;
	ULONG_PTR badAddress;

	sourceAddress = FromAddress;
	targetAddress = ToAddress;

	// We don't check if buffer == NULL when freeing. If buffer doesn't need
	// to be freed, set to stackBuffer, not NULL.
	buffer = stackBuffer;

	mappedTotalSize = (CAT_MAPPED_COPY_PAGES - 2) * PAGE_SIZE;
	if (mappedTotalSize > BufferLength) {
		mappedTotalSize = BufferLength;
	}

	stillToCopy = BufferLength;
	blockSize = mappedTotalSize;

	while (stillToCopy) {
		// If we're at the last copy block, copy the remaining bytes instead
		// of the whole block size.
		if (blockSize > stillToCopy) {
			blockSize = stillToCopy;
		}

		// Choose the best method based on the number of bytes left to copy.
		if (blockSize > CAT_POOL_COPY_THRESHOLD) {
			doMappedCopy = true;
		}
		else {
			doMappedCopy = false;

			if (blockSize <= CAT_STACK_COPY_BYTES) {
				if (buffer != stackBuffer) {
					ExFreePoolWithTag(buffer, 'RWM');
				}

				buffer = stackBuffer;
			}
			else {
				// Don't allocate the buffer if we've done so already.
				// Note that the block size never increases, so this allocation
				// will always be OK.
				if (buffer == stackBuffer) {
					// Keep trying to allocate a buffer.
					while (true) {
						buffer = ExAllocatePoolWithTag(NonPagedPool, blockSize, 'RWM');

						// Stop trying if we got a buffer.
						if (buffer) break;

						blockSize /= 2;

						// Use the stack buffer if we can.
						if (blockSize <= CAT_STACK_COPY_BYTES) {
							buffer = stackBuffer;
							break;
						}
					}
				}
			}
		}

		// Reset state.
		mappedAddress = NULL;
		pagesLocked = false;
		copyingToTarget = false;

		KeStackAttachProcess((PRKPROCESS)FromProcess, (PRKAPC_STATE)&apcState);

		__try {
			// Probe only if this is the first time.
			if (sourceAddress == FromAddress && AccessMode != KernelMode) {
				probing = true;
				ProbeForRead(sourceAddress, BufferLength, sizeof(UCHAR));
				probing = false;
			}

			if (doMappedCopy) {
				// Initialize the MDL.
				MmInitializeMdl(mdl, sourceAddress, blockSize);
				MmProbeAndLockPages(mdl, AccessMode, IoReadAccess);
				pagesLocked = true;

				// Map the pages.
				mappedAddress = MmMapLockedPagesSpecifyCache(
					mdl,
					KernelMode,
					MmCached,
					NULL,
					false,
					HighPagePriority
					);
				if (!mappedAddress) {
					// Insufficient resources; exit.
					mapping = true;
					ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
				}
			}
			else {
				memcpy(buffer, sourceAddress, blockSize);
			}

			KeUnstackDetachProcess(&apcState);

			// Attach to the target process and copy the contents out.
			KeStackAttachProcess((PRKPROCESS)ToProcess, (PRKAPC_STATE)&apcState);

			// Probe only if this is the first time.
			if (targetAddress == ToAddress && AccessMode != KernelMode) {
				probing = true;
				ProbeForWrite(targetAddress, BufferLength, sizeof(UCHAR));
				probing = false;
			}

			copyingToTarget = true;

			PHYSICAL_ADDRESS pa = MmGetPhysicalAddress((PVOID)targetAddress);
			PVOID ma = MmMapIoSpace(pa, blockSize, MmWriteCombined);
			__try {
				if (doMappedCopy) {
					memcpy(ma, mappedAddress, blockSize);
				}
				else {
					memcpy(ma, buffer, blockSize);
				}
			}
			__finally {
				MmUnmapIoSpace(ma, blockSize);
			}

		}
		__except (fnGetCopyExceptionInfo(GetExceptionInformation(), &haveBadAddress, &badAddress)) {
			KeUnstackDetachProcess(&apcState);

			// If we mapped the pages, unmap them.
			if (mappedAddress) {
				MmUnmapLockedPages(mappedAddress, mdl);
			}

			// If we locked the pages, unlock them.
			if (pagesLocked) {
				MmUnlockPages(mdl);
			}

			// If we allocated pool storage, free it.
			if (buffer != stackBuffer) {
				ExFreePoolWithTag(buffer, 'RWM');
			}

			// If we failed when probing or mapping, return the error status.
			if (probing || mapping) {
				return GetExceptionCode();
			}

			// Determine which copy failed.
			if (copyingToTarget && haveBadAddress) {
				*ReturnLength = (ULONG)(badAddress - (ULONG_PTR)sourceAddress);
			}
			else {
				*ReturnLength = BufferLength - stillToCopy;
			}

			return STATUS_PARTIAL_COPY;
		}

		KeUnstackDetachProcess(&apcState);

		if (doMappedCopy) {
			MmUnmapLockedPages(mappedAddress, mdl);
			MmUnlockPages(mdl);
		}

		stillToCopy -= blockSize;
		sourceAddress = (PVOID)((ULONG_PTR)sourceAddress + blockSize);
		targetAddress = (PVOID)((ULONG_PTR)targetAddress + blockSize);
	}

	if (buffer != stackBuffer) {
		ExFreePoolWithTag(buffer, 'RWM');
	}

	*ReturnLength = BufferLength;

	return STATUS_SUCCESS;
}

NTSTATUS CATNTAPI CatReadVirtualMemory(
	__in HANDLE ProcessHandle,
	__in PVOID BaseAddress,
	__out_bcount(BufferSize) PVOID Buffer,
	__in SIZE_T BufferSize,
	__out_opt PSIZE_T NumberOfBytesRead,
	__in KPROCESSOR_MODE AccessMode
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PEPROCESS process = NULL;
	SIZE_T numberOfBytesRead = 0;

	if (AccessMode != KernelMode) {
		if (
			(ULONG_PTR)BaseAddress + BufferSize < (ULONG_PTR)BaseAddress ||
			(ULONG_PTR)Buffer + BufferSize < (ULONG_PTR)Buffer ||
			(ULONG_PTR)BaseAddress + BufferSize > (ULONG_PTR)MmHighestUserAddress
			) {
			return STATUS_ACCESS_VIOLATION;
		}
	}

	if (BufferSize != 0) {
		status = ObReferenceObjectByHandle(
			ProcessHandle,
			0,
			*PsProcessType,
			AccessMode,
			(PVOID*)&process,
			NULL
		);

		if (NT_SUCCESS(status)) {
			status = CatCopyVirtualMemory(
				process,
				BaseAddress,
				PsGetCurrentProcess(),
				Buffer,
				BufferSize,
				AccessMode,
				&numberOfBytesRead);
			ObDereferenceObject(process);
		}
	}
	else {
		status = STATUS_SUCCESS;
	}

	if (NumberOfBytesRead) {
		if (AccessMode != KernelMode) {
			CAT_TRY(NOTHING)
				*NumberOfBytesRead = numberOfBytesRead;
			CAT_EXCEPT(NOTHING)
		}
		else {
			*NumberOfBytesRead = numberOfBytesRead;
		}
	}

	return status;
}

NTSTATUS CATNTAPI CatWriteVirtualMemory(
	__in HANDLE ProcessHandle,
	__in_opt PVOID BaseAddress,
	__in_bcount(BufferSize) PVOID Buffer,
	__in SIZE_T BufferSize,
	__out_opt PSIZE_T NumberOfBytesWritten,
	__in KPROCESSOR_MODE AccessMode
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PEPROCESS process = NULL;
	SIZE_T numberOfBytesWritten = 0;

	if (AccessMode != KernelMode) {
		if (
			(ULONG_PTR)BaseAddress + BufferSize < (ULONG_PTR)BaseAddress ||
			(ULONG_PTR)Buffer + BufferSize < (ULONG_PTR)Buffer ||
			(ULONG_PTR)BaseAddress + BufferSize > (ULONG_PTR)MmHighestUserAddress
		)
		{
			return STATUS_ACCESS_VIOLATION;
		}
	}

	if (BufferSize != 0) {
		status = ObReferenceObjectByHandle(
			ProcessHandle,
			0,
			*PsProcessType,
			AccessMode,
			(PVOID*)&process,
			NULL
		);

		if (NT_SUCCESS(status)) {
			status = CatCopyVirtualMemory(
				PsGetCurrentProcess(),
				Buffer,
				process,
				BaseAddress,
				BufferSize,
				AccessMode,
				&numberOfBytesWritten
			);

			ObDereferenceObject(process);
		}
	}
	else {
		numberOfBytesWritten = 0;
		status = STATUS_SUCCESS;
	}

	if (NumberOfBytesWritten) {
		if (AccessMode != KernelMode) {
			CAT_TRY(NOTHING)
				*NumberOfBytesWritten = numberOfBytesWritten;
			CAT_EXCEPT(NOTHING)
		}
		else {
			*NumberOfBytesWritten = numberOfBytesWritten;
		}
	}

	return status;
}
