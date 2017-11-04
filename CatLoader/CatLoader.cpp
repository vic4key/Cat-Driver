// CatLoader.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdlib>
#include <cstdio>
#include <Windows.h>
#include "../General/CatEngine/CatEngine.h"
#include "../General/Headers/MCatProcedures.h"

#define LOADER_NAME		L"CatLoader"
#define TARGET_NAME		T("Audition.exe")

#define CatPrintf(Format, ...) wprintf(L"[+] %s: " Format, LOADER_NAME, __VA_ARGS__)

extern PfnCatLoadDriver CatLoadDriver;
extern PfnCatUnloadDriver CatUnloadDriver;
extern PfnCatStartDriver CatStartDriver;
extern PfnCatStopDriver CatStopDriver;
extern PfnCatGeneral CatGeneral;
extern PfnCatOpenProcess CatOpenProcess;
extern PfnCatReadProcessMemory CatReadProcessMemory;
extern PfnCatWriteProcessMemory CatWriteProcessMemory;

#pragma warning(disable: 4430)
#pragma warning(disable: 4554)

enum _FEELING{
	kPerfect = 0,
	kGreat = 1,
	kCool = 2,
	kBad = 3,
	kMiss = 4,
	kNone = 5
} TFeeling;

enum _POSITION {
	LOC_DANCING_ROOM = 400
} TPosition;

template<typename X> X RPM(HANDLE h, ULONG a)
{
	X b = (X)0;
	ULONG r = 0;
	CatReadProcessMemory(h, a, &b, sizeof(b), r);
	return b;
}

template<typename X> void WPM(HANDLE h, ULONG a, X v)
{
	X b = v;
	ULONG r = 0;
	CatWriteProcessMemory(h, a, &b, sizeof(b), r);
}

template<typename X> X RPMEX(HANDLE h, ULONG a, ULONG c, ...)
{
	if (c == 0) return RPM<X>(h, a);

	va_list args;
	ULONG *array = new ULONG[c];
	memset(array, 0, c);

	va_start(args, c);

	for (ULONG i = 0; i < c; i++) {
		array[i] = va_arg(args, ULONG);
	}

	va_end(args);

	ULONG r = RPM<ULONG>(h, a);
	for (ULONG i = 0; i < c; ++i) r = RPM<ULONG>(h, r + array[i]);

	delete []array;

	return (X)r;
}

template<typename X> void WPMEX(HANDLE h, ULONG a, X v, ULONG c, ...)
{
	if (c == 0) {
		WPM<X>(h, a, v);
		return;
	}

	va_list args;
	ULONG *array = new ULONG[c];
	memset(array, 0, c);

	va_start(args, c);

	for (ULONG i = 0; i < c; i++) {
		array[i] = va_arg(args, ULONG);
	}

	va_end(args);

	ULONG r = (X)0;

	r = RPM<ULONG>(h, a);
	for (ULONG i = 0; i < c - 1; ++i) r = RPM<ULONG>(h, r + array[i]);

	WPM<X>(h, r + array[c - 1], v);

	delete []array;
}

#define KEYSTATE(vkkey)((ULONG)(GetKeyState(vkkey) & 0x80) == 0x80)
#define KEYSTATEEX(vkkey1, vkkey2)((KEYSTATE(vkkey1)) && (((ULONG)GetAsyncKeyState(vkkey2) & 0x8000) == 0x8000))

class CAutoKey {
public:
	CAutoKey(){};
	CAutoKey(HANDLE Handle, ULONG AutoAddress, ULONG LocAddress);
	~CAutoKey(){};

	void doIt();
	void smartSpace();

	bool getActivation();
	void setActivation(bool state);
	void invertActivation();
private:
	HANDLE h;
	bool activated;
	ULONG a, uLocAddress, uLevel, uSleep;

	UCHAR getLevel();
	bool dancable();
	UCHAR nextKey();
	UCHAR getKey(UCHAR i);
	void pressKey(UCHAR i, UCHAR v);
	void pressedKey(UCHAR v);
};

CAutoKey::CAutoKey(HANDLE Handle, ULONG AutoAddress, ULONG LocAddress)
{
	this->h = Handle;
	this->a = AutoAddress;
	this->uLocAddress = LocAddress;
	this->uLevel = 0;
	this->uSleep = 10;
	this->activated = false;
}

bool CAutoKey::getActivation()
{
	return this->activated;
}

void CAutoKey::setActivation(bool state)
{
	this->activated = state;
}

void CAutoKey::invertActivation()
{
	this->activated = !this->activated;
}

UCHAR CAutoKey::getLevel()
{
	return RPMEX<UCHAR>(this->h, this->a, 4, 0x1c, 0x14, 0x39c, 0x0);
}

bool CAutoKey::dancable()
{
	return (RPMEX<UCHAR>(this->h, this->a, 3, 0x1c, 0x14, 0x674c) == 0);
}

UCHAR CAutoKey::nextKey()
{
	UCHAR r = RPMEX<UCHAR>(this->h, this->a, 4, 0x1c, 0x14, 0x3a0, 0x0);
	return (r == 0 ? 1 : r);
}

UCHAR CAutoKey::getKey(UCHAR i)
{
	return RPMEX<UCHAR>(this->h, this->a, 4, 0x1c, 0x14, 0x39c, 4*i);
}

void CAutoKey::pressKey(UCHAR i, UCHAR v)
{
	WPMEX<UCHAR>(this->h, this->a, v, 4, 0x1c, 0x14, 0x3a0, 4*i);
}

void CAutoKey::pressedKey(UCHAR v)
{
	WPMEX<UCHAR>(this->h, this->a, v, 4, 0x1c, 0x14, 0x3a0, 0x0);
}

void CAutoKey::smartSpace()
{
	/*ULONG loc = rd(this->uLocAddress);
	if (loc == LOC_DANCING_ROOM && this->dancable()) {
		ULONG r = RPMEX<ULONG>(this->h, this->a, 2, 0x1c, 0x08);
		if (r >= kNone) {
			CatPrintf(L"%#.8lx - Couldn't space!!\n", time(NULL));
		}
		else {
			CatPrintf(L"%#0.8lx - Spacable!!\n", time(NULL));
		}
	}*/
}

void CAutoKey::doIt()
{
	ULONG loc = RPM<ULONG>(this->h, this->uLocAddress);
	if (loc == LOC_DANCING_ROOM && this->dancable()) {
		this->uLevel = this->getLevel();
		if (this->nextKey() < this->uLevel) {
			for (UCHAR i = this->nextKey(); i <= this->uLevel; i++) {
				Sleep(this->uSleep);
				this->pressKey(i, getKey(i));
				this->pressedKey(i);
			}
		}
	}
}

void fnThread(void* arg)
{
	static ULONG auto_addr	= 0x0125959CL; // [85 C9 74 0C A1 ?? ?? ?? ?? 85 C0 74 03 89 48 14] + 5
	static ULONG loc_addr	= 0x0125B97CL; // [81 3D ?? ?? ?? ?? 90 01 00 00 75 23 80 3D] + 2
	static ULONG spam_addr	= 0x012752E4L; // [3B 05 ?? ?? ?? ?? 0F 83 B4 00 00 00 8D 4C 24 ?? 51 B9] + 2

	CatPrintf(L"Thread created!\n");

	HANDLE Handle = *(HANDLE*)arg;

	// Free spame message board
	WPM<ULONG>(Handle, spam_addr, 0);
	CatPrintf(L"Now you can spam message board!\n");

	CatPrintf(L"Now you're in auto mode!\n");
	CAutoKey autoKey(Handle, auto_addr, loc_addr);

	while (true) {
		if (!Handle) break;
		
		//autoKey.smartSpace();

		if (KEYSTATEEX(VK_SHIFT, VK_F5)) {
			autoKey.invertActivation();
			if (autoKey.getActivation()) {
				CatPrintf(L"Auto key mode has started!\n");
			}
			else {
				CatPrintf(L"Auto key mode has stopped!\n");
			}
		}

		if (autoKey.getActivation()) {
			autoKey.doIt();
		}

		if (KEYSTATE(VK_LMENU)) {
			autoKey.doIt();
		}

		Sleep(100);
	}
}

bool go(HANDLE hProcess)
{
	if (!hProcess) {
		return false;
	}

	ULONG threadID = 0;
	HANDLE hThread = CreateThread(
		NULL, 0, 
		(LPTHREAD_START_ROUTINE)fnThread, 
		&hProcess, 
		0, &threadID
	);

	if (!hThread) {
		return false;
	}

	system("PAUSE");

	if (CloseHandle(hThread)) {
		CatPrintf(L"Thread closed!\n");
	}
	else {
		CatPrintf(L"Thread couldn't be closed! %s\n", catLastErrorW());
	}

	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwConsole = catGetConsoleWindow();

	if (catIsWow64() != 0) {
		catBoxW(hwConsole, LOADER_NAME, MB_ICONWARNING, L"Sorry. This program doesn't support x64 Operating System!");
		ExitProcess(0);
	}

	if (catBoxW(0, LOADER_NAME, MB_YESNO, L"Would you like to load driver?") == IDNO) {
		ExitProcess(0);
	}

	if (CatInitProcedures() == true) {
		CATCALLER_PRINT_PROCEDURES
		CATCALLER_VERIFY_PROCEDURES(NOTHING)

		/* Load & start own driver */
		if (!CatLoadDriver()) CatPrintf(L"Load driver failed! %s\n", catLastError());
		if (!CatStartDriver()) CatPrintf(L"Start driver failed! %s\n", catLastError());
		
		ULONG PID = catNameToPid(TARGET_NAME);
		HANDLE hProcess = 0;
		if (PID != (ULONG)-1) {
			if (CatOpenProcess(PID, hProcess)) {
				CatPrintf(L"Process opened!\n");
			}
			else {
				CatPrintf(L"Process couldn't be openned! %s\n", catLastErrorW());
			}
		}
		else {
			CatPrintf(L"Audition isn't running!\n");
		}

		if (hProcess) {
			go(hProcess);
			if (CloseHandle(hProcess)) {
			CatPrintf(L"Process closed!\n");
			}
			else {
				CatPrintf(L"Process couldn't be closed! %s\n", catLastErrorW());
			}
		}

		/* Unload & stop own driver */
		if (!CatStopDriver()) CatPrintf(L"Stop driver failed! %s\n", catLastError());
		if (!CatUnloadDriver()) CatPrintf(L"Unload driver failed! %s\n", catLastError());
	}

	CloseHandle(hwConsole);

	system("PAUSE");

	return 0;
}

