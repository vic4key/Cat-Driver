#ifndef CATDRIVER
#define CATDRIVER

/*****************************************
* Name: CatDriver.sys                    *
* Type: Driver | Sub System              *
* Compiler: Microsoft C++ WDK 7 x86      *
* Author: Vic P.                         *
* Mail: vic4key@gmail.com                *
* Website: http://viclab.biz/            *
*****************************************/

#include "stdafx.h"
#include "MGeneral.h"
#include "MTypes.h"
#include "MProcess.h"
#include "MGetSysRoutines.h"
#include "../../General/Headers/MGlobal.h"
#include "../../General/Headers/MCtrlCodes.h"

#pragma pack(1)

SYSTEM_MODULE CatDriverInfo = {0}, NtoskrnlInfo = {0};

#ifdef __cplusplus
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
#endif

void CatDriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS CatDriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS CatDriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS CatDriverHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath)
{
	DbgPrint("+ %S: ********** LOADED *********** +\n", DRIVER);
	DbgPrint("(******************************************)\n");
	DbgPrint("(* Name: CatDriver.sys                    *)\n");
	DbgPrint("(* Type: Driver | Sub System              *)\n");
	DbgPrint("(* Compiler: Microsoft C++ WDK 7 x86      *)\n");
	DbgPrint("(* Author: Vic P.                         *)\n");
	DbgPrint("(* Mail: vic4key@gmail.com                *)\n");
	DbgPrint("(* Website: http://viclab.biz/            *)\n");
	DbgPrint("(******************************************)\n");

	UNICODE_STRING DeviceName, Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	RtlInitUnicodeString(&DeviceName, DRIVER_SYMBOL);
	RtlInitUnicodeString(&Win32Device, DEVICE_SYMBOL);

	DriverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)CatDriverCreate;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)CatDriverHandler;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)CatDriverClose;
	DriverObject->DriverUnload = (PDRIVER_UNLOAD)CatDriverUnload;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, false, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		CatPrintf("Create device failed!");
		return status;
	}

	if (!DeviceObject) {
		CatPrintf("Create device object failed!");
		return STATUS_UNEXPECTED_IO_ERROR;
	}

	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);
	if (!NT_SUCCESS(status)) {
		CatPrintf("Create symbolic link failed!");
	}

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	DeviceObject->AlignmentRequirement = FILE_BYTE_ALIGNMENT;

	return STATUS_SUCCESS;
}

void CatDriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("+ %S: ********* UNLOADED ********** +\n", DRIVER);
	UNICODE_STRING Win32Device;
	RtlInitUnicodeString(&Win32Device, DEVICE_SYMBOL);
	IoDeleteSymbolicLink(&Win32Device);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS CatDriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint("+ %S: ********* STARTED *********** +\n", DRIVER);

	NTSTATUS stt = STATUS_SUCCESS;

	CAT_TRY(NOTHING)

	InitDynamicSystemRoutines();

	CatPrintf("%s", catGetVersion());

	stt = catGetModuleInfomation(L"ntoskrnl.exe", &NtoskrnlInfo);
	if (NT_SUCCESS(stt)) {
		CatPrintf("Ntoskrnl.exe  at %#0.8lx", NtoskrnlInfo.ImageBaseAddress);
	}
	else {
		CatPrintf("stt = %s", catGetMessageDescription(stt, MT_STATUS));
	}

	stt = catGetModuleInfomation(DRIVER_NAME, &CatDriverInfo);
	if (NT_SUCCESS(stt)) {
		CatPrintf("%S at %#0.8lx", DRIVER_NAME, CatDriverInfo.ImageBaseAddress);
	}
	else {
		CatPrintf("stt = %s", catGetMessageDescription(stt, MT_STATUS));
	}

	PSYSTEM_SERVICE_TABLE ntoskrnl = KeServiceDescriptorTable;
	if (!ntoskrnl) {
		CatPrintf("bNt = %#0.8lx, nNt = %uu", ntoskrnl->ServiceTableBase, ntoskrnl->NumberOfServices);
	}

	CAT_EXCEPT(NOTHING)

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CatDriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint("+ %S: ********** STOPED *********** +\n", DRIVER);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CatDriverHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint("+ %S: ********** HANDLED ********** +\n", DRIVER);

	static PVOID ioBuffer = NULL;
	NTSTATUS status = STATUS_NOT_SUPPORTED;
	ULONG outputSize = 0, inputSize = 0, ioCtrlCode = 0;
	PIO_STACK_LOCATION stackLocation = NULL;

	stackLocation = IoGetCurrentIrpStackLocation(Irp);
	if (stackLocation == NULL) {
		CatPrintf("Stack Location is a null buffer!");
		status = STATUS_INVALID_USER_BUFFER;
		goto LB_HANDLE_END;
	}

	ioBuffer = Irp->AssociatedIrp.SystemBuffer;
	if (ioBuffer == NULL) {
		CatPrintf("System buffer is a null buffer!");
		status = STATUS_BUFFER_ALL_ZEROS;
		goto LB_HANDLE_END;
	}

	inputSize = stackLocation->Parameters.DeviceIoControl.InputBufferLength;
	outputSize = stackLocation->Parameters.DeviceIoControl.OutputBufferLength;

	KPROCESSOR_MODE kpmAccessMode = Irp->RequestorMode;

	CatPrintf("InputSize = %uu, OutputSize = %uu, AccessMode = %s",
		inputSize,
		outputSize,
		kpmAccessMode == KernelMode ? "Ring0" : "Ring3");

	ioCtrlCode = stackLocation->Parameters.DeviceIoControl.IoControlCode;
	switch(ioCtrlCode) {
		case CAT_IOCTL_GENERAL:
			{
				CatPrintf("CAT_IOCTL_GENERAL");
				CAT_TRY(NOTHING)
					ULONG PID;
					CAT_READ_SB(&PID, inputSize)
					CatPrintf("ProcessID = %uu", PID);

					CatGeneral(&PID, sizeof(PID));

					CAT_WRITE_SB(0, 0)
				CAT_EXCEPT(NOTHING)
			}
			break;

		case CAT_IOCTL_OPENPROCESS:
			{
				CatPrintf("CAT_CTLCODE_OPENPROCESS");

				CAT_TRY(NOTHING)
					HANDLE hProcess = 0;
					ULONG32 PID = 0;

					CAT_READ_SB(&PID, inputSize)

					PID /= MAGIC_NUMBER;

					status = CatOpenProcess(PID, hProcess);
					CatPrintf("PID = %du, Handle = %#0.8lx, status = %s",
						PID,
						hProcess,
						catGetMessageDescription(status, MT_STATUS)
					);

					ULONG uHandle = (ULONG)hProcess * MAGIC_NUMBER;
					hProcess = (HANDLE)uHandle;

					CAT_WRITE_SB((HANDLE)&hProcess, sizeof(hProcess))
				CAT_EXCEPT(NOTHING)
			}
			break;

		case CAT_IOCTL_READMEMORY:
			{
				CatPrintf("CAT_IOCTL_READMEMORY");

				CAT_TRY(NOTHING)
					TReadWriteMemoryInfo rmi = {0};

					CAT_READ_SB(&rmi, sizeof(rmi))

					ULONG uHandle = (ULONG)rmi.Handle / MAGIC_NUMBER;
					rmi.Handle = (HANDLE)uHandle;

					RtlZeroMemory(rmi.Buffer, rmi.BufferSize);

					status = CatReadVirtualMemory(
						rmi.Handle,
						rmi.Address,
						rmi.Buffer,
						rmi.BufferSize,
						&rmi.BytesDone,
						kpmAccessMode
					);

					CatPrintf("Handle = %#0.8lx, Address = %#0.8lx, Buffer = '%s', BytesDone = %uu, status = %s",
						rmi.Handle,
						(ULONG)rmi.Address,
						(char*)rmi.Buffer,
						rmi.BytesDone,
						catGetMessageDescription(status, MT_STATUS)
					);

					uHandle = (ULONG)rmi.Handle * MAGIC_NUMBER;
					rmi.Handle = (HANDLE)uHandle;

					CAT_WRITE_SB(&rmi, sizeof(rmi))
				CAT_EXCEPT(NOTHING)
			}
			break;

		case CAT_IOCTL_WRITEMEMORY:
			{
				CatPrintf("CAT_IOCTL_WRITEMEMORY");

				CAT_TRY(NOTHING)
					TReadWriteMemoryInfo wmi = {0};

					CAT_READ_SB(&wmi, sizeof(wmi))

					ULONG Handle = (ULONG)wmi.Handle;
					Handle /= MAGIC_NUMBER;
					wmi.Handle = (HANDLE)Handle;

					status = CatWriteVirtualMemory(
						wmi.Handle,
						wmi.Address,
						wmi.Buffer,
						wmi.BufferSize,
						&wmi.BytesDone,
						kpmAccessMode
					);

					CatPrintf("Handle = %#0.8lx, Address = %#0.8lx, BytesDone = %u, status = %s",
						wmi.Handle,
						(ULONG)wmi.Address,
						wmi.BytesDone,
						catGetMessageDescription(status, MT_STATUS)
					);

					CAT_WRITE_SB(&wmi.BytesDone, sizeof(wmi.BytesDone))
				CAT_EXCEPT(NOTHING)
			}
			break;
		default: status = STATUS_SUCCESS;
	}

	LB_HANDLE_END:
	Irp->IoStatus.Information = outputSize;
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

#endif // CATDRIVER