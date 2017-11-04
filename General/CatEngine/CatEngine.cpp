//
// CatEngine 1.0
// CatEngine.cpp: Contain source code for engine
//

#include "CatEngine.h"

/* ---------------------- Initialize ToolHelp32 ---------------------- */

PCreateToolhelp32Snapshot fnCreateToolhelp32Snapshot = NULL;

PProcess32FirstA fnProcess32FirstA = NULL;
PProcess32NextA fnProcess32NextA = NULL;
PProcess32FirstW fnProcess32FirstW = NULL;
PProcess32NextW fnProcess32NextW = NULL;

PModule32FirstA fnModule32FirstA = NULL;
PModule32NextA fnModule32NextA = NULL;
PModule32FirstW fnModule32FirstW = NULL;
PModule32NextW fnModule32NextW = NULL;

/* ------------------------- FUNCTIONS PUBLIC ------------------------- */

int __stdcall InitTlHelp32()
{
	HMODULE hKernel32 = LoadLibrary(TEXT("kernel32.dll"));
	if (!hKernel32) {
		return 1;
	}

	fnCreateToolhelp32Snapshot = (PCreateToolhelp32Snapshot)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	if (fnCreateToolhelp32Snapshot == NULL) {
		return 2;
	}

	fnProcess32FirstA = (PProcess32FirstA)GetProcAddress(hKernel32, "Process32First");
	if (fnProcess32FirstA == NULL) {
		return 3;
	}

	fnProcess32NextA = (PProcess32NextA)GetProcAddress(hKernel32, "Process32Next");
	if (fnProcess32NextA == NULL) {
		return 4;
	}

	fnProcess32FirstW = (PProcess32FirstW)GetProcAddress(hKernel32, "Process32FirstW");
	if (fnProcess32FirstA == NULL) {
		return 5;
	}

	fnProcess32NextW = (PProcess32NextW)GetProcAddress(hKernel32, "Process32NextW");
	if (fnProcess32NextW == NULL) {
		return 6;
	}

	fnModule32FirstA = (PModule32FirstA)GetProcAddress(hKernel32, "Module32First");
	if (fnModule32FirstA == NULL) {
		return 7;
	}

	fnModule32NextA = (PModule32NextA)GetProcAddress(hKernel32, "Module32Next");
	if (fnModule32NextA == NULL) {
		return 8;
	}

	fnModule32FirstW = (PModule32FirstW)GetProcAddress(hKernel32, "Module32FirstW");
	if (fnModule32FirstW == NULL) {
		return 9;
	}

	fnModule32NextW = (PModule32NextW)GetProcAddress(hKernel32, "Module32NextW");
	if (fnModule32NextW == NULL) {
		return 10;
	}

	return 0;
}

/* --- Cat: Base Working --- */

#ifdef __BORLANDC__
	#pragma warn -8070 /* Turn off the warning: Function should return a value in function */
#endif
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4731) /* Turn off the warning: Pointer register '...' modified by assembly code */
	#pragma warning(disable: 4035) /* Turn off the warning: '...': no return value */
	#pragma warning(disable: 4716) /* Turn off the warning: '...': must return a value */
#endif
unsigned long catapi catGetBit(unsigned long Value, unsigned char position)
{
	#ifdef __MINGW32__
	__asm__(
		".intel_syntax noprefix\n\t"
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"pushf\n\t"
		"clc\n\t"
		"mov ebx, [ebp+0x8]\n\t"
		"mov cl, [ebp+0xC]\n\t"
		"inc cl\n\t"
		"rcr ebx, cl\n\t"
		"jc LB_CENABLED\n\t"
		"xor eax, eax\n\t"
		"JMP LB_CDISABLED\n\t"
		"LB_CENABLED:\n\t"
		"mov eax, 1\n\t"
		"LB_CDISABLED:\n\t"
		"rcl ebx, cl\n\t"
		"popf\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"
		".att_syntax"
	);
	#endif

	#if _MSC_VER || __BORLANDC__
	__asm {
		push ecx
		push edx
		push ebx
		push esi
		push edi
		pushf
		clc
		mov ebx, Value
		mov cl, position
		inc cl
		rcr ebx, cl
		jc LB_CENABLED
		xor eax, eax
		JMP LB_CDISABLED
		LB_CENABLED:
		mov eax, 1
		LB_CDISABLED:
		rcl ebx, cl
		popf
		pop edi
		pop esi
		pop ebx
		pop edx
		pop ecx
	}
	#endif
}
#ifdef _MSC_VER
	#pragma warning(pop)
#endif
#ifdef __BORLANDC__
	#pragma warn +8070 /* Turn on the warning: Function should return a value in function */
#endif

/* --- Cat: Math Working --- */

int catapi catGCD(unsigned long count, ...) /* Greatest Common Divisor -> BCNN */
{
	va_list args;
	int min, pro, ret = 0, *array;
	unsigned long i;

	array = new int[count];

	va_start(args, count);

	for (i = 0; i < count; i++) {
		array[i] = va_arg(args, int);
	}

	va_end(args);

	min = array[0];
	for (i = 0; i < count - 1; i++) {
		if (min > array[i + 1]) {
			min = array[i + 1];
		}
	}

	pro = 1;
	for (i = 0; i < count; i++) {
		pro *= array[i];
	}

	if (pro == 0) {
		return 0;
	}

	unsigned long j;
	for (int m = min; m <= pro; m++) {
		for (j = 0; j < count; j++) {
			if (m % array[j] != 0) {
				break;
			}
		}
		if (j == count) {
			ret = m;
			break;
		}
	}

	delete[] array;

	return ret;
}

int catapi catLCM(unsigned long count, ...) /* Least Common Multiples  -> UCLN = |a1*a2*...*an|/GCD(a1, a2, ..., an) */
{
	va_list args;
	int *array, min, ret = 0;
	unsigned long i;

	array = new int[count];

	va_start(args, count);

	for (i = 0; i < count; i++) {
		array[i] = va_arg(args, int);
		if (array[i] == 0) {
			return 0;
		}
	}

	va_end(args);

	min = array[0];
	for (i = 0; i < count - 1; i++) {
		if (min > array[i + 1]) {
			min = array[i + 1];
		}
	}

	unsigned long j;
	for (int m = min - 1; m > 1; m--) {
		for (j = 0; j < count; j++) {
			if (array[j] % m != 0) {
				break;
			}
		}
		if (j == count) {
			ret = m;
			break;
		}
	}

	return ret;
}

/* --- Cat: String Formatting --- */

char* catapi catFormatA(const char *lpcszFormat, ...)
{
	va_list args;
	static char Buffer[MAXBYTE] = {0};

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return (char*)Buffer;
}

wchar_t* catapi catFormatW(const wchar_t *lpcwszFormat, ...)
{
	va_list args;
	static wchar_t Buffer[MAXBYTE] = {0};

	va_start(args, lpcwszFormat);
	wsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return (wchar_t*)Buffer;
}

void catapi catMsgA(const char *lpcszFormat, ...)
{
	static char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	OutputDebugStringA((const char*)Buffer);
}

void catapi catMsgW(const wchar_t *lpcwszFormat, ...)
{
	static wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	OutputDebugStringW((const wchar_t*)Buffer);
}

/* --- Cat: Message Box Formatting --- */

HWND catapi catGetConsoleWindow()
{
	typedef HWND (WINAPI *PfnGetConsoleWindow)();

	HWND hwConsole = (HWND)0;

	CcatLibrary myLib(TEXT("kernel32.dll"), TEXT("GetConsoleWindow"));
	PfnGetConsoleWindow fnGetConsoleWindow = (PfnGetConsoleWindow)myLib.catGetRoutineAddress();
	if (fnGetConsoleWindow) {
		hwConsole = fnGetConsoleWindow();
	}

	return hwConsole;
}

int catapi catBoxA(HWND hwnd, const char *lpcszTitle, unsigned long uType, const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(hwnd, (const char*)Buffer, lpcszTitle, uType);
}

int catapi catBoxA(HWND hwnd, const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(hwnd, (const char*)Buffer, (const char*)_A_CAT_ENGINE, MB_OK);
}

int catapi catBoxA(const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(GetActiveWindow(), (const char*)Buffer, (const char*)_A_CAT_ENGINE, MB_ICONINFORMATION);
}

int catapi catBoxA(unsigned long ulType, const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(GetActiveWindow(), (const char*)Buffer, (const char*)_A_CAT_ENGINE, ulType);
}

int catapi catBoxA(const char *lpcszTitle, const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(GetActiveWindow(), (const char*)Buffer, lpcszTitle, MB_ICONINFORMATION);
}

int catapi catBoxA(const char *lpcszTitle, unsigned long ulType, const char *lpcszFormat, ...)
{
	char Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcszFormat);
	wvsprintfA((char*)Buffer, lpcszFormat, args);
	va_end(args);

	return MessageBoxA(GetActiveWindow(), (const char*)Buffer, lpcszTitle, ulType);
}

int catapi catBoxW(HWND hwnd, const wchar_t *lpcwszTitle, unsigned long uType, const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(hwnd, (const wchar_t*)Buffer, lpcwszTitle, uType);
}

int catapi catBoxW(HWND hwnd, const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(hwnd, (const wchar_t*)Buffer, (const wchar_t*)_W_CAT_ENGINE, MB_OK);
}

int catapi catBoxW(const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(GetActiveWindow(), (const wchar_t*)Buffer, (const wchar_t*)_W_CAT_ENGINE, MB_ICONINFORMATION);
}

int catapi catBoxW(unsigned long ulType, const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(GetActiveWindow(), (const wchar_t*)Buffer, (const wchar_t*)_W_CAT_ENGINE, ulType);
}

int catapi catBoxW(const wchar_t *lpcwszTitle, const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(GetActiveWindow(), (const wchar_t*)Buffer, lpcwszTitle, MB_ICONINFORMATION);
}

int catapi catBoxW(const wchar_t *lpcwszTitle, unsigned long ulType, const wchar_t *lpcwszFormat, ...)
{
	wchar_t Buffer[MAXBYTE] = {0};
	va_list args;

	va_start(args, lpcwszFormat);
	wvsprintfW((wchar_t*)Buffer, lpcwszFormat, args);
	va_end(args);

	return MessageBoxW(GetActiveWindow(), (const wchar_t*)Buffer, lpcwszTitle, ulType);
}

char* catapi catLastErrorA()
{
	unsigned long ulErrorCode = GetLastError();

	char* lpszErrorMessage = (char*)malloc(2*MAXBYTE);
	ZeroMemory((void*)lpszErrorMessage, MAXBYTE);

	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		ulErrorCode,
		LANG_USER_DEFAULT,
		lpszErrorMessage,
		MAXBYTE,
		NULL);

	static char Buffer[MAXBYTE] = {0};
	wsprintfA((char*)Buffer, "%s", lpszErrorMessage);

	return (char*)Buffer;
}

wchar_t* catapi catLastErrorW()
{
	unsigned long ulErrorCode = GetLastError();

	wchar_t *lpwszErrorMessage = (wchar_t*)malloc(2*MAXBYTE);
	ZeroMemory((void*)lpwszErrorMessage, 2*MAXBYTE);

	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		ulErrorCode,
		LANG_USER_DEFAULT,
		lpwszErrorMessage,
		MAXBYTE,
		NULL);

	static wchar_t Buffer[MAXBYTE] = {0};
	wsprintfW((wchar_t*)Buffer, L"%s", lpwszErrorMessage);

	return (wchar_t*)Buffer;
}

/* --- Cat: String Working --- */

char* catapi catLowerStringA(const char *lpcszString)
{
	int i = 0, len = lstrlenA(lpcszString) + 1;
	char *lpszLower = (char*)malloc(len);
	memset((void*)lpszLower, 0, len);

	while (i < (len - 1)) {
	    if ((lpcszString[i] >= 'A') && ((lpcszString[i] <= 'Z'))) {
	    	lpszLower[i] = lpcszString[i] + 0x20;
	    }
	    else {
	    	lpszLower[i] = lpcszString[i];
		}
	    i++;
	}

	return lpszLower;
}

wchar_t* catapi catLowerStringW(const wchar_t *lpcwszString)
{
	int i = 0, len = lstrlenW(lpcwszString) + 1;
	wchar_t *lpwszLower = (wchar_t*)malloc(2*len);
	memset((void*)lpwszLower, 0, 2*len);

	while (i < (len - 1)) {
	    if ((lpcwszString[i] >= (wchar_t)'A') && ((lpcwszString[i] <= (wchar_t)'Z'))) {
			lpwszLower[i] = lpcwszString[i] + 0x20;
	    }
	    else {
	    	lpwszLower[i] = lpcwszString[i];
	    }
	    i++;
	}

	return lpwszLower;
}

char* catapi catUpperStringA(const char *lpcszString)
{
	int i = 0, len = lstrlenA(lpcszString) + 1;
	char *lpszUpper = (char*)malloc(len);
	memset((void*)lpszUpper, 0, len);

	while (i < (len - 1)) {
	    if ((lpcszString[i] >= 'a') && ((lpcszString[i] <= 'z'))) {
	    	lpszUpper[i] = lpcszString[i] - 0x20;
	    }
	    else {
	    	lpszUpper[i] = lpcszString[i];
		}
	    i++;
	}

	return lpszUpper;
}

wchar_t* catapi catUpperStringW(const wchar_t *lpcwszString)
{
	int i = 0, len = lstrlenW(lpcwszString) + 1;
	wchar_t *lpwszUpper = (wchar_t*)malloc(2*len);
	memset((void*)lpwszUpper, 0, 2*len);

	while (i < (len - 1)) {
	    if ((lpcwszString[i] >= (wchar_t)'a') && ((lpcwszString[i] <= (wchar_t)'z'))) {
			lpwszUpper[i] = lpcwszString[i] - 0x20;
	    }
	    else {
	    	lpwszUpper[i] = lpcwszString[i];
	    }
	    i++;
	}

	return lpwszUpper;
}

char* catapi catPwcToPac(const wchar_t* lpcwszString)
{
	int len = lstrlenW(lpcwszString) + 1;
	char *lpszChar = (char*)malloc(len);
	memset((void*)lpszChar, 0, len);

	WideCharToMultiByte(
		CP_ACP,
		WC_COMPOSITECHECK,
		lpcwszString,
		-1, // The terminating null character
		(char*)lpszChar,
		len,
		NULL,
		NULL);
	return (char*)lpszChar;
}

wchar_t* catapi catPacToPwc(const char* lpcszString)
{
	int len = lstrlenA(lpcszString) + 1;
	wchar_t *lpszWideChar = (wchar_t*)malloc(2*len);
	memset((void*)lpszWideChar, 0, 2*len);

	MultiByteToWideChar(
		CP_ACP,
		0,
		lpcszString,
		len,
		lpszWideChar,
		2*len);
	return (wchar_t*)lpszWideChar;
}

/* --- Cat: Process Working --- */

int catapi catIsWow64()
{
	typedef BOOL (WINAPI *PfnisWow64Process)(HANDLE, PBOOL);
	PfnisWow64Process fnIsWow64Process;
	BOOL bWow64 = false;

	CcatLibrary myLib(TEXT("kernel32.dll"), TEXT("IsWow64Process"));
	fnIsWow64Process = (PfnisWow64Process)myLib.catGetRoutineAddress();
	if (fnIsWow64Process == NULL) {
		return -1;
	}

	if (!fnIsWow64Process(GetCurrentProcess(), &bWow64) == true) {
		return -1;
	}

	return bWow64;
}

unsigned long catapi catGetParentPID(unsigned long ulChildPID)
{
	TProcessEntry32A pe;
	HANDLE hSnapshot;
	BOOL bNext;

	if (InitTlHelp32 != 0) {
		return (unsigned long)INVALID_HANDLE_VALUE;
	}

	hSnapshot = fnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return (unsigned long)INVALID_HANDLE_VALUE;
	}

	pe.dwSize = sizeof(TProcessEntry32A);

	bNext = fnProcess32FirstA(hSnapshot, &pe);

	while (bNext) {
		if (pe.th32ProcessID == ulChildPID) {
			return pe.th32ParentProcessID;
		}
		bNext = fnProcess32NextA(hSnapshot, &pe);
		pe.dwSize = sizeof(TProcessEntry32A);
	}
	return (unsigned long)INVALID_HANDLE_VALUE;
}

unsigned long catapi catNameToPidA(const char* lpcszProcessName)
{
	TProcessEntry32A pe;
	HANDLE hSnapshot;
	BOOL bNext;

	if (InitTlHelp32() != 0) {
		return (unsigned long)-1;
	}

	hSnapshot = fnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return (unsigned long)-1;
	}

	pe.dwSize = sizeof(TProcessEntry32A);

	bNext = fnProcess32FirstA(hSnapshot, &pe);

	while (bNext) {
		if (lstrcmpA((const char*)pe.szExeFile, lpcszProcessName) == 0) {
			return pe.th32ProcessID;
		}
		bNext = fnProcess32NextA(hSnapshot, &pe);
		pe.dwSize = sizeof(TProcessEntry32A);
	}

	return (unsigned long)-1;
}

unsigned long catapi catNameToPidW(const wchar_t* lpcwszProcessName)
{
	TProcessEntry32W pe;
	HANDLE hSnapshot;
	BOOL bNext;

	if (InitTlHelp32() != 0) {
		return (unsigned long)-1;
	}

	hSnapshot = fnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return (unsigned long)-1;
	}

	pe.dwSize = sizeof(TProcessEntry32W);

	bNext = fnProcess32FirstW(hSnapshot, &pe);

	while (bNext) {
		if (lstrcmpW((const wchar_t*)pe.szExeFile, lpcwszProcessName) == 0) {
			return pe.th32ProcessID;
		}
		bNext = fnProcess32NextW(hSnapshot, &pe);
		pe.dwSize = sizeof(TProcessEntry32W);
	}

	return (unsigned long)-1;
}

char* catapi catPidToNameA(unsigned long ulPID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
	if (!hProcess) {
		return 0;
	}

	HMODULE hModule = 0;
	unsigned long cbNeeded = 0;
	static char szProcessName[MAXBYTE] = {0};
	ZeroMemory((void*)szProcessName, MAXBYTE);

    if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
        GetModuleBaseNameA(hProcess, hModule, (char*)szProcessName, sizeof(szProcessName));
    }

	return (char*)szProcessName;
}

wchar_t* catapi catPidToNameW(unsigned long ulPID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
	if (!hProcess) {
		return 0;
	}

	HMODULE hModule = 0;
	unsigned long cbNeeded = 0;
	static wchar_t wszProcessName[MAXBYTE] = {0};
	ZeroMemory((void*)wszProcessName, 2*MAXBYTE);

    if ( EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
        GetModuleBaseNameW(hProcess, hModule, (wchar_t*)wszProcessName, sizeof(wszProcessName)/2);
    }

	return (wchar_t*)wszProcessName;
}

HMODULE catapi catRemoteGetModuleHandleA(unsigned long ulPID, const char* lpcszModuleName)
{
	HANDLE hModuleSnap;
	TModuleEntry32A me32;
	HMODULE modHandle = 0;

	if (InitTlHelp32() != 0) {
		return (HMODULE)-1;
	}

	hModuleSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ulPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	ZeroMemory(&me32, sizeof(me32));
	me32.dwSize = sizeof(TModuleEntry32W);

	if (!fnModule32FirstA(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);
	}

	do {
		if (lstrcmpiA((const char*)me32.szModule, lpcszModuleName) == 0) {
			modHandle = me32.hModule;
			break;
		};
	} while (fnModule32NextA(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	return modHandle;
}

HMODULE catapi catRemoteGetModuleHandleW(unsigned long ulPID, const wchar_t* lpwcszModuleName)
{
	HANDLE hModuleSnap;
	TModuleEntry32W me32;
	HMODULE modHandle = 0;

	if (InitTlHelp32() != 0) {
		return (HMODULE)-1;
	}

	hModuleSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ulPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	ZeroMemory(&me32, sizeof(me32));
	me32.dwSize = sizeof(TModuleEntry32W);

	if (!fnModule32FirstW(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);
	}

	do {
		if (lstrcmpiW((const wchar_t*)me32.szModule, lpwcszModuleName) == 0) {
			modHandle = me32.hModule;
			break;
		};
	} while (fnModule32NextW(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	return modHandle;
}

/* --- Cat: Memory Working --- */

bool catapi catRPM(unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulRead = 0;

	if (IsBadReadPtr((const void*)ulAddress, ulSize)) {
		return false;
	}

	VirtualProtect((void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	ReadProcessMemory(GetCurrentProcess(), (const void*)ulAddress, lpBuffer, ulSize, &ulRead);

	VirtualProtect((void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	if (ulRead == ulSize)
		return true;
	else
		return false;
}

bool catapi catRPM(HANDLE hProcess, unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulRead = 0;

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	ReadProcessMemory(hProcess, (const void*)ulAddress, lpBuffer, ulSize, &ulRead);

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	if (ulRead == ulSize)
		return true;
	else
		return false;
}

bool catapi catRPM(unsigned long ulPID, unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulRead = 0;

	if (!ulPID) {
		return false;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
	if (!hProcess) {
		return false;
	}

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	ReadProcessMemory(hProcess, (const void*)ulAddress, lpBuffer, ulSize, &ulRead);

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	CloseHandle(hProcess);

	if (ulRead == ulSize)
		return true;
	else
		return false;
}

bool catapi catWPM(unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulWritten = 0;

	if (IsBadWritePtr((void*)ulAddress, ulSize)) {
		return false;
	}

	VirtualProtect((void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	WriteProcessMemory(GetCurrentProcess(), (void*)ulAddress, lpcBuffer, ulSize, &ulWritten);

	VirtualProtect((void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	if (ulWritten == ulSize)
		return true;
	else
		return false;
}

bool catapi catWPM(HANDLE hProcess, unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulWritten = 0;

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	WriteProcessMemory(hProcess, (void*)ulAddress, lpcBuffer, ulSize, &ulWritten);

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	if (ulWritten == ulSize)
		return true;
	else
		return false;
}

bool catapi catWPM(unsigned long ulPID, unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize)
{
	unsigned long ulOldProtect = 0;
	SIZE_T ulWritten = 0;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
	if (!hProcess) {
		return false;
	}

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	WriteProcessMemory(hProcess, (void*)ulAddress, lpcBuffer, ulSize, &ulWritten);

	VirtualProtectEx(hProcess, (void*)ulAddress, ulSize, ulOldProtect, &ulOldProtect);

	CloseHandle(hProcess);

	if (ulWritten == ulSize)
		return true;
	else
		return false;
}

/* --- Cat: File Working --- */

char* catapi catFileTypeA(const char* lpcszFilePath)
{
	SHFILEINFOA SHINFO = {0};
	static char szFileType[MAXBYTE] = {0};

	if (SHGetFileInfoA(lpcszFilePath, 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0) {
		ZeroMemory((void*)szFileType, MAXBYTE);
		lstrcpyA((char*)szFileType, (char*)SHINFO.szTypeName);
	}

	return (char*)szFileType;
}

wchar_t* catapi catFileTypeW(const wchar_t* lpcwszFilePath)
{
	SHFILEINFOW SHINFO = {0};
	static wchar_t szFileType[MAXBYTE] = {0};

	if (SHGetFileInfoW(lpcwszFilePath, 0, &SHINFO, sizeof(SHFILEINFOA), SHGFI_TYPENAME) != 0) {
		ZeroMemory((void*)szFileType, 2*MAXBYTE);
		lstrcpyW((wchar_t*)szFileType, (wchar_t*)SHINFO.szTypeName);
	}

	return (wchar_t*)szFileType;
}

bool catapi catFileExistsA(const char* lpcszFilePath)
{
	bool bResult = false;
	HANDLE hFile;
	WIN32_FIND_DATAA Win32FindData;

	hFile = FindFirstFileA(lpcszFilePath, &Win32FindData);
	if (hFile != INVALID_HANDLE_VALUE) {
		bResult = true;
		FindClose(hFile);
	}
	return bResult;
}

bool catapi catFileExistsW(const wchar_t* lpcwszFilePath)
{
	bool bResult = false;
	HANDLE hFile;
	WIN32_FIND_DATAW Win32FindData;

	hFile = FindFirstFileW(lpcwszFilePath, &Win32FindData);
	if (hFile != INVALID_HANDLE_VALUE) {
		bResult = true;
		FindClose(hFile);
	}
	return bResult;
}

char* catapi catExtractFilePathA(const char* lpcszFilePath)
{
	int i;
	static char szFilePath[MAX_PATH] = {0};

	ZeroMemory((void*)&szFilePath, MAX_PATH);

	lstrcpyA((char*)szFilePath, lpcszFilePath);

	i = lstrlenA(szFilePath);
	while ((i > 0) && (szFilePath[i] != 0x5C)) {
		szFilePath[i] = 0x00;
		i--;
	}
	return (char*)szFilePath;
}

wchar_t* catapi catExtractFilePathW(const wchar_t* lpcwszFilePath)
{
	int i;
	static wchar_t szFilePath[MAX_PATH] = {0};

	ZeroMemory((void*)&szFilePath, 2*MAX_PATH);

	lstrcpyW((wchar_t*)szFilePath, lpcwszFilePath);

	i = lstrlenW(szFilePath);
	while ((i > 0) && (szFilePath[i] != 0x5C)) {
		szFilePath[i] = 0x00;
		i--;
	}
	return (wchar_t*)szFilePath;
}

char* catapi catExtractFileNameA(const char* lpcszFilePath)
{
	int i, j, x = 0;
	static char szFilePath[MAXBYTE] = {0};

	i = lstrlenA(lpcszFilePath);
	ZeroMemory((void*)&szFilePath, MAXBYTE);

	while ((i > 0) && (lpcszFilePath[i] != 0x5C)) {
		i--;
	}

	if (lpcszFilePath[i] == 0x5C) {
		for (j = i + 1; j <= lstrlenA(lpcszFilePath); j++) {
			szFilePath[x] = lpcszFilePath[j];
			x++;
		}
	}
	else {
		return (char*)lpcszFilePath;
	}

	return (char*)szFilePath;
}

wchar_t* catapi catExtractFileNameW(const wchar_t* lpcwszFilePath)
{
	int i, j, x = 0;
	static wchar_t szFilePath[MAXBYTE] = {0};

	i = lstrlenW(lpcwszFilePath);
	ZeroMemory((void*)&szFilePath, 2*MAXBYTE);

	while ((i > 0) && (lpcwszFilePath[i] != 0x5C)) {
		i--;
	}

	if (lpcwszFilePath[i] == 0x5C) {
		for (j = i + 1; j <= lstrlenW(lpcwszFilePath); j++) {
			szFilePath[x] = lpcwszFilePath[j];
			x++;
		}
	}
	else {
		return (wchar_t*)lpcwszFilePath;
	}

	return (wchar_t*)szFilePath;
}

/* ------------------------- CLASSES PUBLIC ------------------------- */

/* --- Cat: API Hooking Class --- */

unsigned long catapi CcatApiHookingSupport::catLenInstruction(unsigned long ulAddress)
{
	unsigned char *iptr0 = (unsigned char*)ulAddress;
	unsigned char *iptr = iptr0;
	unsigned char b, mod, rm;
	unsigned long f = 0;

	prefix:
	b = *iptr++;

	f |= table_1[b];

	if ((f&C_FUCKINGTEST) != 0) {
		if (((*iptr)&0x38) == 0x00) {
			f = C_MODRM+C_DATAW0;
		}
		else {
			f = C_MODRM;
		}
	}

	if ((f&C_TABLE_0F) != 0) {
		b = *iptr++;
		f = table_0F[b];
	}

	if (f == C_ERROR) {
		return 0;
	}

	if ((f & C_PREFIX) != 0) {
		f &= ~C_PREFIX;
		goto prefix;
	}

	if ((f&C_DATAW0) != 0) {
		if (b&0x01) {
			f |= C_DATA66;
		}
		else {
			f |= C_DATA1;
		}
	}

	if ((f & C_MODRM) != 0) {
		b = *iptr++;
		mod = b & 0xC0;
		rm  = b & 0x07;
		if (mod != 0xC0) {
			if ((f&C_67) != 0) {
				if ((mod == 0x00) && (rm == 0x06)) {
					f |= C_MEM2;
				}

				if (mod == 0x40) {
					f |= C_MEM1;
				}

				if (mod == 0x80) {
					f|=C_MEM2;
				}
			}
			else {
				if (mod == 0x40) {
					f |= C_MEM1;
				}

				if (mod == 0x80) {
					f |= C_MEM4;
				}

				if (rm == 0x04) {
					rm = (*iptr++) & 0x07;
				}

				if ((rm == 0x05) && (mod == 0x00)) {
					f |= C_MEM4;
				}
			}
		}
	}

	if ((f & C_MEM67) != 0) {
		if ((f & C_67) != 0) {
			f |= C_MEM2;
		}
		else {
			f |= C_MEM4;
		}
	}

	if ((f & C_DATA66) != 0) {
		if ((f & C_66) != 0) {
			f |= C_DATA2;
		}
		else {
			f |= C_DATA4;
		}
	}

	if ((f & C_MEM1) != 0){
		iptr++;
	}
	if ((f & C_MEM2) != 0) {
		iptr += 2;
	}
	if ((f & C_MEM4) != 0) {
		iptr += 4;
	}

	if ((f & C_DATA1) != 0) {
		iptr++;
	}
	if ((f&C_DATA2) != 0) {
		iptr += 2;
	}
	if ((f & C_DATA4) != 0) {
		iptr += 4;
	}

	return (unsigned long)(iptr - iptr0);
}

unsigned long catapi CcatApiHookingSupport::catJumpLen(unsigned long ulSrcAddress, unsigned long ulDestAddress)
{
	return (ulDestAddress - (ulSrcAddress + 5));
}

bool catapi CcatApiHookingSupport::catDetour(void* pProc, void* pHookProc, void** pOldProc)
{
	TRedirect o2n, c2o;
	const unsigned long minHookSize = 5;
	unsigned long iHookLen = 0;

	do {
		iHookLen += catLenInstruction((unsigned long)pProc + iHookLen);
	} while (iHookLen < minHookSize);

	*pOldProc = (void*)VirtualAlloc(NULL, iHookLen + sizeof(TRedirect), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (*pOldProc == NULL) {
		return false;
	}

	memcpy(*pOldProc, pProc, iHookLen);
	c2o.jmp = 0xE9;
	c2o.address = catJumpLen((unsigned long)*pOldProc + 5, (unsigned long)pProc + 5);
	memcpy((void*)((unsigned long)*pOldProc + iHookLen), (void*)&c2o, sizeof(TRedirect));

	unsigned long ulOldProtect = 0;
	VirtualProtect(pProc, iHookLen, PAGE_EXECUTE_READWRITE, &ulOldProtect);
	o2n.jmp = 0xE9;
	o2n.address = catJumpLen((unsigned long)pProc, (unsigned long)pHookProc);
	memcpy(pProc, (void*)&o2n, sizeof(TRedirect));

	return true;
}

bool catapi CcatApiHookingA::catAPIAttach(const char* lpszModuleName, const char* lpszProcName, void* lpHookProc, void** lpOldProc)
{
	HMODULE hLib = LoadLibraryA(lpszModuleName);
	if (!hLib) {
		return false;
	}

	void* lpProc = (void*)GetProcAddress(hLib, lpszProcName);
	if (lpProc == NULL) {
		return false;
	}

	CcatApiHookingSupport::catDetour(lpProc, lpHookProc, lpOldProc);

	FreeLibrary(hLib);

	return true;
}

bool catapi CcatApiHookingA::catAPIDetach(const char* lpszModuleName, const char* lpszProcName, void** lpOldProc)
{
	const unsigned long minHookSize = 5;

	HMODULE hLib = LoadLibraryA(lpszModuleName);
	if (!hLib) {
		return false;
	}

	void* pProc = (void*)GetProcAddress(hLib, lpszProcName);
	if (pProc == NULL) {
		return false;
	}

	memcpy(pProc, *lpOldProc, minHookSize);

	VirtualFree(*lpOldProc, minHookSize + sizeof(TRedirect), MEM_RELEASE);

	FreeLibrary(hLib);

	return true;
}

bool catapi CcatApiHookingW::catAPIAttach(const wchar_t* lpwszModuleName, const wchar_t* lpwszProcName, void* lpHookProc, void** lpOldProc)
{
	HMODULE hLib = LoadLibraryW(lpwszModuleName);
	if (!hLib) {
		return false;
	}

	void* lpProc = (void*)GetProcAddress(hLib, catPwcToPac(lpwszProcName));
	if (lpProc == NULL) {
		return false;
	}

	CcatApiHookingSupport::catDetour(lpProc, lpHookProc, lpOldProc);

	FreeLibrary(hLib);

	return true;
}

bool catapi CcatApiHookingW::catAPIDetach(const wchar_t* lpwszModuleName, const wchar_t* lpwszProcName, void** lpOldProc)
{
	const unsigned long minHookSize = 5;

	HMODULE hLib = LoadLibraryW(lpwszModuleName);
	if (!hLib) {
		return false;
	}

	void* pProc = (void*)GetProcAddress(hLib, catPwcToPac(lpwszProcName));
	if (pProc == NULL) {
		return false;
	}

	memcpy(pProc, *lpOldProc, minHookSize);

	VirtualFree(*lpOldProc, minHookSize + sizeof(TRedirect), MEM_RELEASE);

	FreeLibrary(hLib);

	return true;
}

/* --- Cat : File Working Class --- */

bool catapi CcatFileSupport::catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize)
{
	SetFilePointer(hFile, ulOffset, NULL, FILE_BEGIN);
	ReadFile(hFile, Buffer, ulSize, (LPDWORD)&ulReadSize, NULL);
	if (ulSize != ulReadSize) {
		return false;
	}
	return true;
}

bool catapi CcatFileSupport::catRead(void* Buffer, unsigned long ulSize)
{
	ReadFile(hFile, Buffer, ulSize, (LPDWORD)&ulReadSize, NULL);
	if (ulSize != ulReadSize) {
		return false;
	}
	return true;
}

bool catapi CcatFileSupport::catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize)
{
	SetFilePointer(hFile, ulOffset, NULL, FILE_BEGIN);
	WriteFile(hFile, cBuffer, ulSize, (LPDWORD)&ulWroteSize, NULL);
	if (ulSize != ulWroteSize) {
		return false;
	}
	return true;
}

bool catapi CcatFileSupport::catWrite(const void* cBuffer, unsigned long ulSize)
{
	WriteFile(hFile, cBuffer, ulSize, (LPDWORD)&ulWroteSize, NULL);
	if (ulSize != ulWroteSize) {
		return false;
	}
	return true;
}

bool catapi CcatFileSupport::catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize)
{
	unsigned long ulReturnedLength = 0;
	//catMsg(TEXT("*lpSendBuffer = %d, size_a = %d"), *(unsigned long*)lpSendBuffer, ulSendSize);
	//unsigned long b = 0, c = 0;
	return (DeviceIoControl(hFile, ulControlCode, lpSendBuffer, ulSendSize, lpReveiceBuffer, ulReveiceSize, &ulReturnedLength, (LPOVERLAPPED)NULL) != 0);
}

bool catapi CcatFileSupport::catClose()
{
	if (!CloseHandle(hFile)) {
		return false;
	}
	return true;
}

bool catapi CcatFileA::catInit(const char *lpszFilePath, TFileModeFlags fmFlag, TFileShareFlags fsFlag, TFileAttributeFlags faFlag)
{
	hFile = CreateFileA(
		lpszFilePath,
		GENERIC_READ | GENERIC_WRITE,
		fsFlag,
		NULL,
		fmFlag,
		faFlag,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool catapi CcatFileA::catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize)
{
	return CcatFileSupport::catRead(ulOffset, Buffer, ulSize);
}

bool catapi CcatFileA::catRead(void* Buffer, unsigned long ulSize)
{
	return CcatFileSupport::catRead(Buffer, ulSize);
}

bool catapi CcatFileA::catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize)
{
	return CcatFileSupport::catWrite(ulOffset, cBuffer, ulSize);
}

bool catapi CcatFileA::catWrite(const void* cBuffer, unsigned long ulSize)
{
	return CcatFileSupport::catWrite(cBuffer, ulSize);
}

bool catapi CcatFileA::catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize)
{
	return CcatFileSupport::catIOControl(ulControlCode, lpSendBuffer, ulSendSize, lpReveiceBuffer, ulReveiceSize);
}

bool catapi CcatFileA::catClose()
{
	return CcatFileSupport::catClose();
}

bool catapi CcatFileW::catInit(const wchar_t *lpwszFilePath, TFileModeFlags fmFlag, TFileShareFlags fsFlag, TFileAttributeFlags faFlag)
{
	hFile = CreateFileW(
		lpwszFilePath,
		GENERIC_READ | GENERIC_WRITE,
		fsFlag,
		NULL,
		fmFlag,
		faFlag,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool catapi CcatFileW::catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize)
{
	return CcatFileSupport::catRead(ulOffset, Buffer, ulSize);
}

bool catapi CcatFileW::catRead(void* Buffer, unsigned long ulSize)
{
	return CcatFileSupport::catRead(Buffer, ulSize);
}

bool catapi CcatFileW::catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize)
{
	return CcatFileSupport::catWrite(ulOffset, cBuffer, ulSize);
}

bool catapi CcatFileW::catWrite(const void* cBuffer, unsigned long ulSize)
{
	return CcatFileSupport::catWrite(cBuffer, ulSize);
}

bool catapi CcatFileW::catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize)
{
	return CcatFileSupport::catIOControl(ulControlCode, lpSendBuffer, ulSendSize, lpReveiceBuffer, ulReveiceSize);
}

bool catapi CcatFileW::catClose()
{
	return CcatFileSupport::catClose();
}

/* --- Cat : Service Working Class --- */

bool catapi CcatServiceSupport::catUnLoadService()
{
	bool bExit = true;

    if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
    	bExit = false;
    }

    if (!(DeleteService(hService) && CloseServiceHandle(hService))) {
    	bExit = false;
    }

    if (!CloseServiceHandle(hSCManager)) {
    	bExit = false;
    }

    return bExit;
}

bool catapi CcatServiceA::catLoadService(const char* lpszServiceName)
{
 	static char szServiceName[MAXBYTE], szDisplayName[MAXBYTE], szBinaryPathName[MAX_PATH];

	memset((void*)szServiceName, 0, sizeof(szServiceName));
	memset((void*)szDisplayName, 0, sizeof(szDisplayName));
	memset((void*)szBinaryPathName, 0, sizeof(szBinaryPathName));

 	GetCurrentDirectoryA(MAX_PATH, (char*)szBinaryPathName);

 	lstrcatA((char*)szBinaryPathName, (char*)"\\");
	lstrcatA((char*)szBinaryPathName, (char*)lpszServiceName);
	//lstrcatA((char*)szBinaryPathName, (char*)TEXT(".sys"));

	if (!catFileExistsA((const char*)szBinaryPathName)) {
		return false;
	}

	const int UCT = 1900;
	time_t rawtime;
	static char crtime[MAXBYTE];
	struct tm *timeinfo;

	memset((void*)crtime, 0, sizeof(crtime));

	time(&rawtime);

	#ifdef _MSC_VER
	#if _MSC_VER <= 1200 // VC++ 6.0 or below
	timeinfo = localtime(&rawtime);
	#else
	timeinfo = NULL;
	//localtime_s(timeinfo, &rawtime);
	timeinfo = localtime(&rawtime);
	#endif
	#endif

	#ifdef __BORLANDC__
	timeinfo = localtime(&rawtime);
	#endif

	#ifdef __MINGW32__
	timeinfo = localtime(&rawtime);
	#endif

	memset((void*)&crtime, 0x00, MAXBYTE);
	wsprintfA((char*)crtime, (const char*)"%.2d:%.2d:%.2d",
	timeinfo->tm_hour,
	timeinfo->tm_min,
	timeinfo->tm_sec);
	lstrcpyA((char*)szServiceName, (char*)lpszServiceName);
	lstrcatA((char*)szServiceName, (char*)" - ");
	lstrcatA((char*)szServiceName, (char*)crtime);

	memset((void*)&crtime, 0x00, MAXBYTE);
	wsprintfA((char*)crtime, "%.2d:%.2d:%.2d - %.2d/%.2d/%.4d",
	timeinfo->tm_hour,
	timeinfo->tm_min,
	timeinfo->tm_sec,
	timeinfo->tm_mday,
	timeinfo->tm_mon,
	UCT + timeinfo->tm_year);
	lstrcpyA((char*)szDisplayName, (char*)lpszServiceName);
	lstrcatA((char*)szDisplayName, (char*)" - ");
	lstrcatA((char*)szDisplayName, (char*)crtime);

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager) {
		return false;
	}

    hService = CreateServiceA(
    	hSCManager,
    	(const char*)szServiceName,
    	(const char*)szDisplayName,
    	SERVICE_START | DELETE | SERVICE_STOP,
    	SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
    	SERVICE_ERROR_IGNORE,
    	(const char*)szBinaryPathName,
    	NULL, NULL, NULL, NULL, NULL);
    if (!hService) {
        hService = OpenServiceA(hSCManager, (char*)lpszServiceName, SERVICE_START | DELETE | SERVICE_STOP);
		return false;
    }

    if (!StartService(hService, 0, NULL)) {
        CloseServiceHandle(hService);
        DeleteService(hService);
    	return false;
    }

    return true;
}

bool catapi CcatServiceA::catUnLoadService()
{
	return CcatServiceSupport::catUnLoadService();
}

bool catapi CcatServiceW::catLoadService(const wchar_t* lpwszServiceName)
{
 	static wchar_t szServiceName[MAXBYTE], szDisplayName[MAXBYTE], szBinaryPathName[MAX_PATH];

	memset((void*)szServiceName, 0, sizeof(szServiceName));
	memset((void*)szDisplayName, 0, sizeof(szDisplayName));
	memset((void*)szBinaryPathName, 0, sizeof(szBinaryPathName));

 	GetCurrentDirectoryW(MAX_PATH, (wchar_t*)szBinaryPathName);

 	lstrcatW((wchar_t*)szBinaryPathName, (wchar_t*)L"\\");
	lstrcatW((wchar_t*)szBinaryPathName, (wchar_t*)lpwszServiceName);
	//lstrcatW((wchar_t*)szBinaryPathName, (wchar_t*)L".sys");

	if (!catFileExistsW((const wchar_t*)szBinaryPathName)) {
		return false;
	}

	const int UCT = 1900;
	time_t rawtime;
	static wchar_t crtime[MAXBYTE];
	struct tm *timeinfo;

	memset((void*)crtime, 0, sizeof(crtime));

	time(&rawtime);

	#ifdef _MSC_VER
	#if _MSC_VER <= 1200 // VC++ 6.0 or below
	timeinfo = localtime(&rawtime);
	#else
	timeinfo = NULL;
	//localtime_s(timeinfo, &rawtime);
	timeinfo = localtime(&rawtime);
	#endif
	#endif

	#ifdef __BORLANDC__
	timeinfo = localtime(&rawtime);
	#endif

	#ifdef __MINGW32__
	timeinfo = localtime(&rawtime);
	#endif

	memset((void*)&crtime, 0x00, 2*MAXBYTE);
	wsprintfW((wchar_t*)crtime, L"%.2d:%.2d:%.2d",
	timeinfo->tm_hour,
	timeinfo->tm_min,
	timeinfo->tm_sec);
	lstrcpyW((wchar_t*)szServiceName, lpwszServiceName);
	lstrcatW((wchar_t*)szServiceName, L" - ");
	lstrcatW((wchar_t*)szServiceName, (wchar_t*)crtime);

	memset((void*)&crtime, 0x00, 2*MAXBYTE);
	wsprintfW((wchar_t*)crtime, L"%.2d:%.2d:%.2d - %.2d/%.2d/%.4d",
	timeinfo->tm_hour,
	timeinfo->tm_min,
	timeinfo->tm_sec,
	timeinfo->tm_mday,
	timeinfo->tm_mon,
	UCT + timeinfo->tm_year);
	lstrcpyW((wchar_t*)szDisplayName, lpwszServiceName);
	lstrcatW((wchar_t*)szDisplayName, L" - ");
	lstrcatW((wchar_t*)szDisplayName, (wchar_t*)crtime);

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager) {
		return false;
	}

    hService = CreateServiceW(
    	hSCManager,
    	(const wchar_t*)szServiceName,
    	(const wchar_t*)szDisplayName,
    	SERVICE_START | DELETE | SERVICE_STOP,
    	SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
    	SERVICE_ERROR_IGNORE,
    	(const wchar_t*)szBinaryPathName,
    	NULL, NULL, NULL, NULL, NULL);
    if (!hService) {
        hService = OpenServiceW(hSCManager, (wchar_t*)lpwszServiceName, SERVICE_START | DELETE | SERVICE_STOP);
		return false;
    }

    if (!StartService(hService, 0, NULL)) {
        CloseServiceHandle(hService);
        DeleteService(hService);
    	return false;
    }

    return true;
}

bool catapi CcatServiceW::catUnLoadService()
{
	return CcatServiceSupport::catUnLoadService();
}

/* --- Cat : Process In/Ejection Class --- */

bool catapi CcatProcessA::catInjectionLibrary(unsigned long PID, const char* lpcszLibraryPath)
{
	HANDLE hProcess;
	HMODULE hKernel32;
	void *pMemAlloc, *pLoadLibraryA;

	if (!PID) {
		return false;
	}

	if (!catFileExistsA((const char*)lpcszLibraryPath)) {
		return false;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (!hProcess) {
		return false;
	}

	hKernel32 = GetModuleHandleA("kernel32.dll");
	if (!hKernel32) {
		return false;
	}

	pLoadLibraryA = (void*)GetProcAddress(hKernel32, "LoadLibraryA");
	if (pLoadLibraryA == NULL) {
    	return false;
	}

	pMemAlloc = (void*)VirtualAllocEx(hProcess, NULL, lstrlenA(lpcszLibraryPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, pMemAlloc, (void*)lpcszLibraryPath, lstrlenA(lpcszLibraryPath), NULL);

	CreateRemoteThread(hProcess, (LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, (void*)pMemAlloc, 0, (LPDWORD)NULL);

	CloseHandle(hProcess);

	return true;
}

bool catapi CcatProcessA::catEjectionLibrary(unsigned long PID, const char* lpcszModuleName)
{
	HANDLE hProcess;
	HMODULE hKernel32;
	void* pFreeLibrary;

	if (!PID) {
		return false;
	}

	HMODULE hModule = catRemoteGetModuleHandleA(PID, lpcszModuleName);
	if (!hModule) {
        return false;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if(!hProcess) {
	  return false;
	}

	hKernel32 = GetModuleHandleA("kernel32.dll");
	if (!hKernel32) {
		return false;
	}

	pFreeLibrary = (void*)GetProcAddress(hKernel32, "FreeLibrary");
	if (pFreeLibrary == NULL) {
    	return false;
	}

	CreateRemoteThread(hProcess, (LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)pFreeLibrary, hModule, 0, NULL);

	CloseHandle(hProcess);

	return true;
}

bool catapi CcatProcessW::catInjectionLibrary(unsigned long PID, const wchar_t* lpcwszLibraryPath)
{
	HANDLE hProcess;
	HMODULE hKernel32;
	void *pMemAlloc, *pLoadLibraryW;

	if (!PID) {
		return false;
	}

	if (!catFileExistsW((const wchar_t*)lpcwszLibraryPath)) {
		return false;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (!hProcess) {
		return false;
	}

	hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if (!hKernel32) {
		return false;
	}

	pLoadLibraryW = (void*)GetProcAddress(hKernel32, "LoadLibraryW");
	if (pLoadLibraryW == NULL) {
    	return false;
	}

	pMemAlloc = (void*)VirtualAllocEx(hProcess, NULL, lstrlenW(lpcwszLibraryPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, pMemAlloc, (void*)lpcwszLibraryPath, 2*lstrlenW(lpcwszLibraryPath), NULL);

	CreateRemoteThread(hProcess, (LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryW, (void*)pMemAlloc, 0, (LPDWORD)NULL);

	CloseHandle(hProcess);

	return true;
}

bool catapi CcatProcessW::catEjectionLibrary(unsigned long PID, const wchar_t* lpcwszModuleName)
{
	HANDLE hProcess;
	HMODULE hKernel32;
	void* pFreeLibrary;

	if (!PID) {
		return false;
	}

	HMODULE hModule = catRemoteGetModuleHandleW(PID, lpcwszModuleName);
	if (!hModule) {
        return false;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if(!hProcess) {
	  return false;
	}

	hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if (!hKernel32) {
		return false;
	}

	pFreeLibrary = (void*)GetProcAddress(hKernel32, "FreeLibrary");
	if (pFreeLibrary == NULL) {
    	return false;
	}

	CreateRemoteThread(hProcess, (LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)pFreeLibrary, hModule, 0, NULL);

	CloseHandle(hProcess);

	return true;
}

/* --- Cat : Library Class --- */

CcatLibraryA::CcatLibraryA()
{
	this->lpszModuleName = NULL;
	this->lpszRoutineName = NULL;
}

CcatLibraryA::~CcatLibraryA()
{
	this->lpszModuleName = NULL;
	this->lpszRoutineName = NULL;
}

CcatLibraryA::CcatLibraryA(const char *lpcszModuleName)
{
	this->lpszModuleName = (char*)lpcszModuleName;
	this->lpszRoutineName = NULL;
}

CcatLibraryA::CcatLibraryA(const char *lpcszModuleName, const char *lpcszRoutineName)
{
	this->lpszModuleName = (char*)lpcszModuleName;
	this->lpszRoutineName = (char*)lpcszRoutineName;
}

bool catapi CcatLibraryA::IsLibraryAvailable()
{
	if (this->lpszModuleName) {
		return (LoadLibraryA(this->lpszModuleName) != NULL);
	}
	else {
		return false;
	}
}

void* catapi CcatLibraryA::catGetRoutineAddress()
{
	if (this->lpszModuleName && this->lpszRoutineName) {
		return this->catGetRoutineAddress(this->lpszModuleName, this->lpszRoutineName);
	}

	return NULL;
}

void* catapi CcatLibraryA::catGetRoutineAddress(const char *lpcszRoutineName)
{
	if (this->lpszModuleName && lpcszRoutineName) {
		return this->catGetRoutineAddress(this->lpszModuleName, lpcszRoutineName);
	}

	return NULL;
}

void* catapi catapi CcatLibraryA::catGetRoutineAddress(const char *lpcszModuleName, const char *lpcszRoutineName)
{
	HMODULE hmod = 0;
	if (lpcszModuleName) hmod = LoadLibraryA(lpcszModuleName);

	void* p = NULL;
	if (hmod && lpcszRoutineName) {
		p = (void*)GetProcAddress(hmod, lpcszRoutineName);
	}

	FreeLibrary(hmod);

	return p;
}

CcatLibraryW::CcatLibraryW()
{
	this->lpwszModuleName = NULL;
	this->lpwszRoutineName = NULL;
}

CcatLibraryW::CcatLibraryW(const wchar_t *lpcwszModuleName)
{
	this->lpwszModuleName = (wchar_t*)lpcwszModuleName;
	this->lpwszRoutineName = NULL;
}

CcatLibraryW::CcatLibraryW(const wchar_t *lpcwszModuleName, const wchar_t *lpwcszRoutineName)
{
	this->lpwszModuleName = (wchar_t*)lpcwszModuleName;
	this->lpwszRoutineName = (wchar_t*)lpwcszRoutineName;
}

CcatLibraryW::~CcatLibraryW()
{
	this->lpwszModuleName = NULL;
	this->lpwszRoutineName = NULL;
}

bool catapi CcatLibraryW::IsLibraryAvailable()
{
	if (this->lpwszModuleName) {
		return (LoadLibraryW(this->lpwszModuleName) != NULL);
	}
	else {
		return false;
	}
}

void* catapi CcatLibraryW::catGetRoutineAddress()
{
	if (this->lpwszModuleName && this->lpwszRoutineName) {
		return this->catGetRoutineAddress(this->lpwszModuleName, this->lpwszRoutineName);
	}

	return NULL;
}

void* catapi CcatLibraryW::catGetRoutineAddress(const wchar_t *lpwcszRoutineName)
{
	if (this->lpwszModuleName && lpwcszRoutineName) {
		return this->catGetRoutineAddress(this->lpwszModuleName, lpwcszRoutineName);
	}

	return NULL;
}

void* catapi CcatLibraryW::catGetRoutineAddress(const wchar_t *lpcwszModuleName, const wchar_t *lpwcszRoutineName)
{
	HMODULE hmod = 0;
	if (lpcwszModuleName) hmod = LoadLibraryW(lpcwszModuleName);

	void* p = NULL;
	if (hmod && lpwcszRoutineName) {
		p = (void*)GetProcAddress(hmod, catPwcToPac(lpwcszRoutineName));
	}

	FreeLibrary(hmod);

	return p;
}