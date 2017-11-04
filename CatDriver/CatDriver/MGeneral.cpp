#include "MGeneral.h"

extern SYSTEM_MODULE CatDriverInfo;
extern SYSTEM_MODULE NtoskrnlInfo;
extern Pfn_snprintf fn_snprintf;

int fnExceptionFilter(PEXCEPTION_POINTERS ep)
{
	PEXCEPTION_RECORD er = ep->ExceptionRecord;
	PCONTEXT ct = ep->ContextRecord;
	char* exc_message = catGetMessageDescription(er->ExceptionCode, MT_EXCEPTION);

	ULONG exc_va_address = (ULONG)er->ExceptionAddress, exc_rva_address = 0;

	/*CatPrintf("CatDriverImageBase = %#0.8lx, NtoskrnlImageBase = %#0.8lx",
		CatDriverImageBase,
		NtoskrnlImageBase
	);*/

	char* excMod = (char*)"Unknown";

	struct {
		ULONG begin;
		ULONG end;
		ULONG size;
		char* name;
	} a = {0}, b = {0};

	// CatDriver.sys
	a.begin = (ULONG)CatDriverInfo.ImageBaseAddress;
	a.size = CatDriverInfo.ImageSize;
	a.end = a.begin + a.size;
	a.name = (char*)CatDriverInfo.Name;

	// Ntoskrnl.exe
	b.begin = (ULONG)NtoskrnlInfo.ImageBaseAddress;
	b.size = NtoskrnlInfo.ImageSize;
	b.end = b.begin + b.size;
	b.name = (char*)NtoskrnlInfo.Name;

	if (exc_va_address >= a.begin && exc_va_address <= a.end) {
		exc_rva_address = exc_va_address - a.begin;
		excMod = catExtractFileNameA(a.name);
	}
	else if (exc_va_address >= b.begin && exc_va_address <= b.end) {
		exc_rva_address = exc_va_address - (ULONG)b.begin;
		excMod = catExtractFileNameA(b.name);
	}

	CatPrintf(
		"Exception %#0.8lx (%s) in '%s' at %#0.8lx (rva: %#0.8lx)!\n\
		\t\t\t\t\t\t\t\t\t\t\tEAX = %#0.8lx, EBX = %#0.8lx, EBX = %#0.8lx, EBX = %#0.8lx\n\
		\t\t\t\t\t\t\t\t\t\t\tESI = %#0.8lx, EDI = %#0.8lx, EBP = %#0.8lx, ESP = %#0.8lx",
		er->ExceptionCode,
		exc_message,
		excMod,
		exc_va_address,
		exc_rva_address,
		ct->Eax, ct->Ebp, ct->Ecx, ct->Edx,
		ct->Esi, ct->Edi, ct->Ebp, ct->Esp
	);

	return EXCEPTION_EXECUTE_HANDLER;
}

char* CATNTAPI catGetMessageDescription(const int msgId, TMessateType msgType)
{
	CAT_TRY(NOTHING)

	char* message = "Unknown";
	TMessageInfo* ListMessage = NULL;
	int ListMessageLength = 0;

	#define FIND_MESSAGE(lst) for (int i = 0; i < (int)(sizeof(lst)/sizeof(lst[0])); i++) { \
		if (msgId == lst[i].Id) { \
			message = lst[i].Desc; \
			break; \
		} \
	}

	switch(msgType) {
		case MT_STATUS:
			FIND_MESSAGE(ListStatusMessage)
			break;
		case MT_EXCEPTION:
			FIND_MESSAGE(ListExceptionMessage)
			break;
		default:;
	}

	return message;

	CAT_EXCEPT(return "")
}

NTSTATUS CATNTAPI catReadSystemBuffer(PVOID ioBuffer, unsigned long inputSize, PVOID pData, unsigned long dataSize)
{
	CAT_TRY(NOTHING)

	NTSTATUS status = STATUS_SUCCESS;

	if (inputSize && (!pData || !dataSize)) {
		status = STATUS_UNSUCCESSFUL;
	}

	if (inputSize <= dataSize) {
		RtlCopyMemory(pData, ioBuffer, inputSize);
	}
	else {
		CatPrintf("Buffer too small, couldn't read!");
		status = STATUS_BUFFER_TOO_SMALL;
	}

	return status;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}

NTSTATUS CATNTAPI catWriteSystemBuffer(PVOID ioBuffer, unsigned long outputSize, PVOID pData, unsigned long dataSize)
{
	CAT_TRY(NOTHING)

	NTSTATUS status = STATUS_SUCCESS;

	if (!outputSize || !pData || !dataSize) {
		return STATUS_SUCCESS;
	}

	if (outputSize >= dataSize) {
		RtlZeroMemory(ioBuffer, outputSize);
		RtlCopyMemory(ioBuffer, pData, dataSize);
	}
	else {
		CatPrintf("Buffer too small, couldn't write!");
		status = STATUS_BUFFER_TOO_SMALL;
	}

	return status;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}

char* CATNTAPI catExtractFileNameA(const char* lpcszFilePath)
{
	CAT_TRY(NOTHING)

	int i, j, x = 0;
	static char szFilePath[255] = {0};

	i = strlen(lpcszFilePath);
	RtlZeroMemory((void*)&szFilePath, 255);

	while ((i > 0) && (lpcszFilePath[i] != 0x5C)) {
		i--;
	}

	if (lpcszFilePath[i] == 0x5C) {
		for (j = i + 1; j <= (int)strlen(lpcszFilePath); j++) {
			szFilePath[x] = lpcszFilePath[j];
			x++;
		}
	}
	else {
		return (char*)lpcszFilePath;
	}

	return (char*)szFilePath;

	CAT_EXCEPT(return "")
}

wchar_t* CATNTAPI catPacToPwc(const char* s)
{
	CAT_TRY(NOTHING)

	static wchar_t S[255] = {0};
	RtlZeroMemory(S, 255);

	for (int i = 0; i < (int)strlen(s); i++) {
		S[i] = (wchar_t)s[i];
	}

	return (wchar_t*)S;

	CAT_EXCEPT(return L"")
}

char* CATNTAPI catGetVersion()
{
	/*
		About Windows versions can be read here:
		https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
	*/

	CAT_TRY(NOTHING)

	RTL_OSVERSIONINFOEXW v = {0};
	v.dwOSVersionInfoSize = sizeof(v);
	
	RtlGetVersion((PRTL_OSVERSIONINFOW)&v);

	/*CatPrintf("major = %u, minor = %u, build = %u, procId = %u, platId = %u, smajor = %u, sminor = %u, csd = '%S'",
		v.dwMajorVersion,
		v.dwMinorVersion,
		v.dwBuildNumber,
		v.wProductType,
		v.dwPlatformId,
		v.wServicePackMajor,
		v.wServicePackMinor,
		v.szCSDVersion
	);*/

	static char *version = "Unknown";
	switch(v.dwMajorVersion) {
		case 5:
			{
				if (v.dwMinorVersion == 0) {
					version = "Windows 2000";
				}
				else if (v.dwMinorVersion == 1) {
					version = "Windows XP";
				}
				else if (v.dwMinorVersion == 2) {
					version = "Windows XP 64-Bit Edition or Windows Server 2003 or Windows Server 2003 R2";
				}
			}
			break;
		case 6:
			{
				if (v.dwMinorVersion == 0) {
					if (v.wProductType == VER_NT_WORKSTATION) {
						version = "Windows Vista";
					}
					else {
						version = "Windows Server 2008";
					}
				}
				else if (v.dwMinorVersion == 1) {
					if (v.wProductType == VER_NT_WORKSTATION) {
						version = "Windows 7";
					}
					else {
						version = "Windows Server 2008 R2";
					}
				}
				else if (v.dwMinorVersion == 2) {
					if (v.wProductType == VER_NT_WORKSTATION) {
						version = "Windows 8";
					}
					else {
						version = "Windows Server 2012";
					}
				}
				else if (v.dwMinorVersion == 3) {
					if (v.wProductType == VER_NT_WORKSTATION) {
						version = "Windows 8.1";
					}
					else {
						version = "Windows Server 2012 R2";
					}
				}
			}
			break;
		case 10:
			{
				if (v.dwMinorVersion == 0) {
					if (v.wProductType == VER_NT_WORKSTATION) {
						version = "Windows 10 Insider Preview";
					}
					else {
						version = "Windows Server Technical Preview";
					}
				}
			}
			break;
		default:;
	}
	
	if (fn_snprintf) {
		static full_version[255] = {0};

		fn_snprintf(
			(char*)full_version,
			255,
			"%s Service Pack %u Build %u",
			version,
			v.wServicePackMajor,
			v.dwBuildNumber
		);

		return (char*)full_version;
	}
	else {
		return version;
	}

	CAT_EXCEPT(return "Unknown")
}

NTSTATUS CATNTAPI CatGeneral(PVOID p, ULONG z)
{
	CAT_TRY(NOTHING)

	/*

	nt!PsIdleProcess
	nt!PsInitialSystemProcess
	nt!PsActiveProcessHead

	kd> dd PsInitialSystemProcess L1
	80560cd4  823c89c8
	kd> db 823c89c8+0x174 L10
	823c8b3c  53 79 73 74 65 6d 00 00-00 00 00 00 00 00 00 00  System..........

	kd> dd 823c89c8+0x88 L2
	823c8a50  821260a8 80560bd8

	kd> dd 80560bd8 L2
	80560bd8  823c8a50 821aba00

	kd> dd 823c8a50 L2
	823c8a50  821260a8 80560bd8

	kd> db 8212c0a8-0x88+0x174 L10
	8212c194  63 73 72 73 73 2e 65 78-65 00 00 00 00 00 00 00  csrss.exe.......

	kd> dd 821260a8 L2
	821260a8  8212c0a8 823c8a50

	kd> dd 8212c0a8 L2
	8212c0a8  821750a8 821260a8

	kd> db 821750a8-0x88+0x174 L10
	82175194  77 69 6e 6c 6f 67 6f 6e-2e 65 78 65 00 00 00 00  winlogon.exe....
	821751a4  00 00 00 00 00 00 */

	NTSTATUS status = STATUS_SUCCESS;

	if (!p) {
		return STATUS_UNSUCCESSFUL;
	}

	ULONG PID = *(ULONG*)p;
	if (!PID || PID == (ULONG)-1) {
		return STATUS_UNSUCCESSFUL;
	}

	CatPrintf("PID = %uu", PID);

	ULONG System = (ULONG)PsInitialSystemProcess;
	if (!System) {
		return STATUS_UNSUCCESSFUL;
	}

	CatPrintf("PsInitialSystemProcess = %#0.8lx -> '%s'", System, (char*)(System + 0x174));

	PLIST_ENTRY PsActiveProcessHead = (PLIST_ENTRY)((System + 0x88) + 4);
	if (!PsActiveProcessHead) {
		return STATUS_UNSUCCESSFUL;
	}

	CatPrintf("PsActiveProcessHead = %#0.8lx", (ULONG)PsActiveProcessHead->Flink);

	#define GET_NAME_BY_LE(le) ((char*)((PVOID)((ULONG)le - 0x88 + 0x174)))
	#define GET_PID_BY_LE(le) (*(ULONG*)((PVOID)((ULONG)le - 0x88 + 0x84)))
	#define GET_EP_BY_LE(le) ((ULONG)(((ULONG)le - 0x88)))

	static PLIST_ENTRY LE = PsActiveProcessHead->Flink;
	ULONG start = (ULONG)LE, stop = 0, EP = 0;
	while (start != stop) {
		char* Name = GET_NAME_BY_LE(LE);
		ULONG ProcessID = GET_PID_BY_LE(LE);
		CatPrintf("%#0.8lx - [%u] '%s'", (ULONG)LE, ProcessID, Name);
		if (ProcessID == PID) {
			EP = GET_EP_BY_LE(LE);
			break;
		}
		LE = LE->Flink;
		stop = (ULONG)LE;
	}

	CatPrintf("EP = %#0.8lx, PID = %u, Name = '%s'", EP, *(ULONG*)(EP + 0x84), (char*)(EP + 0x174));

	return status;

	CAT_EXCEPT(return STATUS_UNSUCCESSFUL)
}