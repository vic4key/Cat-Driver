/*
	@Name	: CatEngine 1.0
	@Author	: Vic aka vic4key
	@Date	: Update day by day
*/

#ifndef _CATENGINE_H_
#define _CATENGINE_H_

#include <windows.h>
#include <psapi.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>

#ifdef _WIN64
	#error CatEngine only support x86 compiler, did not support x64 compiler
#endif

#ifdef _MSC_VER
	#define CATENGINE
	#pragma pack(1)
	#pragma warning(disable: 4311)
	#pragma warning(disable: 4312)
	#pragma warning(disable: 4996) /* Tun off the warning: _CRT_SECURE_NO_DEPRECATE */
	#pragma warning(disable: 4518) /* Turn off the warning: storage-class or type specifier(s) unexpected here; ignored */
	#pragma warning(disable: 4230) /* Turn off the warning: anachronism used : modifiers/qualifiers interspersed, qualifier ignored */
	#define catnaked __declspec(naked)
#endif

#ifdef __BORLANDC__
	#define CATENGINE
	#pragma option -a1
	#pragma warn -8059 /* Turn off the warning: Structure packing size has changed */
	#define catnaked __declspec(naked)
#endif

#ifdef __MINGW32__
	#define CATENGINE
	#pragma pack(1)
	#define catnaked
#endif

#ifdef CATENGINE
	#pragma comment(lib,"psapi.lib")
	#define catapi __stdcall
#else
	#error CatEngine did not support the current compiler.
#endif

#define _A_CAT_ENGINE	 "CatEngine"
#define _W_CAT_ENGINE	L"CatEngine"

#define T	TEXT

/* ----------------------- INITIALIZE TLHELP32 ----------------------- */

#define MAX_MODULE_NAME32	255

#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPMODULE32 0x00000010
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000

typedef struct _PROCESSENTRY32A
{
	DWORD   dwSize;
	DWORD   cntUsage;
	DWORD   th32ProcessID;          // this process
	ULONG	th32DefaultHeapID;
	DWORD   th32ModuleID;           // associated exe
	DWORD   cntThreads;
	DWORD   th32ParentProcessID;    // this process's parent process
	LONG    pcPriClassBase;         // Base priority of process's threads
	DWORD   dwFlags;
	CHAR    szExeFile[MAX_PATH];    // Path
} TProcessEntry32A;
typedef TProcessEntry32A *PProcessEntry32A;

typedef struct _PROCESSENTRY32W
{
	DWORD   dwSize;
	DWORD   cntUsage;
	DWORD   th32ProcessID;          // this process
	ULONG	th32DefaultHeapID;
	DWORD   th32ModuleID;           // associated exe
	DWORD   cntThreads;
	DWORD   th32ParentProcessID;    // this process's parent process
	LONG    pcPriClassBase;         // Base priority of process's threads
	DWORD   dwFlags;
	WCHAR   szExeFile[MAX_PATH];    // Path
} TProcessEntry32W;
typedef TProcessEntry32W *PProcessEntry32W;

typedef struct _MODULEENTRY32A
{
	DWORD   dwSize;
	DWORD   th32ModuleID;       // This module
	DWORD   th32ProcessID;      // owning process
	DWORD   GlblcntUsage;       // Global usage count on the module
	DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
	BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
	DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
	HMODULE hModule;            // The hModule of this module in th32ProcessID's context
	char    szModule[MAX_MODULE_NAME32 + 1];
	char    szExePath[MAX_PATH];
} TModuleEntry32A;
typedef TModuleEntry32A *PModuleEntry32A;

typedef struct _MODULEENTRY32W
{
	DWORD   dwSize;
	DWORD   th32ModuleID;       // This module
	DWORD   th32ProcessID;      // owning process
	DWORD   GlblcntUsage;       // Global usage count on the module
	DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
	BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
	DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
	HMODULE hModule;            // The hModule of this module in th32ProcessID's context
	WCHAR   szModule[MAX_MODULE_NAME32 + 1];
	WCHAR   szExePath[MAX_PATH];
} TModuleEntry32W;
typedef TModuleEntry32W *PModuleEntry32W;

typedef HANDLE (WINAPI *PCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);

typedef BOOL (WINAPI *PProcess32FirstA)(HANDLE hSnapshot, PProcessEntry32A lppe);
typedef BOOL (WINAPI *PProcess32NextA)(HANDLE hSnapshot, PProcessEntry32A lppe);
typedef BOOL (WINAPI *PProcess32FirstW)(HANDLE hSnapshot, PProcessEntry32W lppe);
typedef BOOL (WINAPI *PProcess32NextW)(HANDLE hSnapshot, PProcessEntry32W lppe);

typedef BOOL (WINAPI *PModule32FirstA)(HANDLE hSnapshot, PModuleEntry32A lpme);
typedef BOOL (WINAPI *PModule32NextA)(HANDLE hSnapshot, PModuleEntry32A lpme);
typedef BOOL (WINAPI *PModule32FirstW)(HANDLE hSnapshot, PModuleEntry32W lpme);
typedef BOOL (WINAPI *PModule32NextW)(HANDLE hSnapshot, PModuleEntry32W lpme);

int __stdcall InitTlHelp32();

/* ------------------------------ DATA ------------------------------ */

/* --- Data: catLenInstruction --- */

#define C_ERROR         0xFFFFFFFFL
#define C_PREFIX        0x00000001L
#define C_66            0x00000002L
#define C_67            0x00000004L
#define C_DATA66        0x00000008L
#define C_DATA1         0x00000010L
#define C_DATA2         0x00000020L
#define C_DATA4         0x00000040L
#define C_MEM67         0x00000080L
#define C_MEM1          0x00000100L
#define C_MEM2          0x00000200L
#define C_MEM4          0x00000400L
#define C_MODRM         0x00000800L
#define C_DATAW0        0x00001000L
#define C_FUCKINGTEST   0x00002000L
#define C_TABLE_0F      0x00004000L

const unsigned long table_1[256] = {
	/* 00 */   C_MODRM
	/* 01 */,  C_MODRM
	/* 02 */,  C_MODRM
	/* 03 */,  C_MODRM
	/* 04 */,  C_DATAW0
	/* 05 */,  C_DATAW0
	/* 06 */,  0x00000000L
	/* 07 */,  0x00000000L
	/* 08 */,  C_MODRM
	/* 09 */,  C_MODRM
	/* 0A */,  C_MODRM
	/* 0B */,  C_MODRM
	/* 0C */,  C_DATAW0
	/* 0D */,  C_DATAW0
	/* 0E */,  0x00000000L
	/* 0F */,  C_TABLE_0F
	/* 10 */,  C_MODRM
	/* 11 */,  C_MODRM
	/* 12 */,  C_MODRM
	/* 13 */,  C_MODRM
	/* 14 */,  C_DATAW0
	/* 15 */,  C_DATAW0
	/* 16 */,  0x00000000L
	/* 17 */,  0x00000000L
	/* 18 */,  C_MODRM
	/* 19 */,  C_MODRM
	/* 1A */,  C_MODRM
	/* 1B */,  C_MODRM
	/* 1C */,  C_DATAW0
	/* 1D */,  C_DATAW0
	/* 1E */,  0x00000000L
	/* 1F */,  0x00000000L
	/* 20 */,  C_MODRM
	/* 21 */,  C_MODRM
	/* 22 */,  C_MODRM
	/* 23 */,  C_MODRM
	/* 24 */,  C_DATAW0
	/* 25 */,  C_DATAW0
	/* 26 */,  C_PREFIX
	/* 27 */,  0x00000000L
	/* 28 */,  C_MODRM
	/* 29 */,  C_MODRM
	/* 2A */,  C_MODRM
	/* 2B */,  C_MODRM
	/* 2C */,  C_DATAW0
	/* 2D */,  C_DATAW0
	/* 2E */,  C_PREFIX
	/* 2F */,  0x00000000L
	/* 30 */,  C_MODRM
	/* 31 */,  C_MODRM
	/* 32 */,  C_MODRM
	/* 33 */,  C_MODRM
	/* 34 */,  C_DATAW0
	/* 35 */,  C_DATAW0
	/* 36 */,  C_PREFIX
	/* 37 */,  0x00000000L
	/* 38 */,  C_MODRM
	/* 39 */,  C_MODRM
	/* 3A */,  C_MODRM
	/* 3B */,  C_MODRM
	/* 3C */,  C_DATAW0
	/* 3D */,  C_DATAW0
	/* 3E */,  C_PREFIX
	/* 3F */,  0x00000000L
	/* 40 */,  0x00000000L
	/* 41 */,  0x00000000L
	/* 42 */,  0x00000000L
	/* 43 */,  0x00000000L
	/* 44 */,  0x00000000L
	/* 45 */,  0x00000000L
	/* 46 */,  0x00000000L
	/* 47 */,  0x00000000L
	/* 48 */,  0x00000000L
	/* 49 */,  0x00000000L
	/* 4A */,  0x00000000L
	/* 4B */,  0x00000000L
	/* 4C */,  0x00000000L
	/* 4D */,  0x00000000L
	/* 4E */,  0x00000000L
	/* 4F */,  0x00000000L
	/* 50 */,  0x00000000L
	/* 51 */,  0x00000000L
	/* 52 */,  0x00000000L
	/* 53 */,  0x00000000L
	/* 54 */,  0x00000000L
	/* 55 */,  0x00000000L
	/* 56 */,  0x00000000L
	/* 57 */,  0x00000000L
	/* 58 */,  0x00000000L
	/* 59 */,  0x00000000L
	/* 5A */,  0x00000000L
	/* 5B */,  0x00000000L
	/* 5C */,  0x00000000L
	/* 5D */,  0x00000000L
	/* 5E */,  0x00000000L
	/* 5F */,  0x00000000L
	/* 60 */,  0x00000000L
	/* 61 */,  0x00000000L
	/* 62 */,  C_MODRM
	/* 63 */,  C_MODRM
	/* 64 */,  C_PREFIX
	/* 65 */,  C_PREFIX
	/* 66 */,  C_PREFIX+C_66
	/* 67 */,  C_PREFIX+C_67
	/* 68 */,  C_DATA66
	/* 69 */,  C_MODRM+C_DATA66
	/* 6A */,  C_DATA1
	/* 6B */,  C_MODRM+C_DATA1
	/* 6C */,  0x00000000L
	/* 6D */,  0x00000000L
	/* 6E */,  0x00000000L
	/* 6F */,  0x00000000L
	/* 70 */,  C_DATA1
	/* 71 */,  C_DATA1
	/* 72 */,  C_DATA1
	/* 73 */,  C_DATA1
	/* 74 */,  C_DATA1
	/* 75 */,  C_DATA1
	/* 76 */,  C_DATA1
	/* 77 */,  C_DATA1
	/* 78 */,  C_DATA1
	/* 79 */,  C_DATA1
	/* 7A */,  C_DATA1
	/* 7B */,  C_DATA1
	/* 7C */,  C_DATA1
	/* 7D */,  C_DATA1
	/* 7E */,  C_DATA1
	/* 7F */,  C_DATA1
	/* 80 */,  C_MODRM+C_DATA1
	/* 81 */,  C_MODRM+C_DATA66
	/* 82 */,  C_MODRM+C_DATA1
	/* 83 */,  C_MODRM+C_DATA1
	/* 84 */,  C_MODRM
	/* 85 */,  C_MODRM
	/* 86 */,  C_MODRM
	/* 87 */,  C_MODRM
	/* 88 */,  C_MODRM
	/* 89 */,  C_MODRM
	/* 8A */,  C_MODRM
	/* 8B */,  C_MODRM
	/* 8C */,  C_MODRM
	/* 8D */,  C_MODRM
	/* 8E */,  C_MODRM
	/* 8F */,  C_MODRM
	/* 90 */,  0x00000000L
	/* 91 */,  0x00000000L
	/* 92 */,  0x00000000L
	/* 93 */,  0x00000000L
	/* 94 */,  0x00000000L
	/* 95 */,  0x00000000L
	/* 96 */,  0x00000000L
	/* 97 */,  0x00000000L
	/* 98 */,  0x00000000L
	/* 99 */,  0x00000000L
	/* 9A */,  C_DATA66+C_MEM2
	/* 9B */,  0x00000000L
	/* 9C */,  0x00000000L
	/* 9D */,  0x00000000L
	/* 9E */,  0x00000000L
	/* 9F */,  0x00000000L
	/* A0 */,  C_MEM67
	/* A1 */,  C_MEM67
	/* A2 */,  C_MEM67
	/* A3 */,  C_MEM67
	/* A4 */,  0x00000000L
	/* A5 */,  0x00000000L
	/* A6 */,  0x00000000L
	/* A7 */,  0x00000000L
	/* A8 */,  C_DATA1
	/* A9 */,  C_DATA66
	/* AA */,  0x00000000L
	/* AB */,  0x00000000L
	/* AC */,  0x00000000L
	/* AD */,  0x00000000L
	/* AE */,  0x00000000L
	/* AF */,  0x00000000L
	/* B0 */,  C_DATA1
	/* B1 */,  C_DATA1
	/* B2 */,  C_DATA1
	/* B3 */,  C_DATA1
	/* B4 */,  C_DATA1
	/* B5 */,  C_DATA1
	/* B6 */,  C_DATA1
	/* B7 */,  C_DATA1
	/* B8 */,  C_DATA66
	/* B9 */,  C_DATA66
	/* BA */,  C_DATA66
	/* BB */,  C_DATA66
	/* BC */,  C_DATA66
	/* BD */,  C_DATA66
	/* BE */,  C_DATA66
	/* BF */,  C_DATA66
	/* C0 */,  C_MODRM+C_DATA1
	/* C1 */,  C_MODRM+C_DATA1
	/* C2 */,  C_DATA2
	/* C3 */,  0x00000000L
	/* C4 */,  C_MODRM
	/* C5 */,  C_MODRM
	/* C6 */,  C_MODRM+C_DATA66
	/* C7 */,  C_MODRM+C_DATA66
	/* C8 */,  C_DATA2+C_DATA1
	/* C9 */,  0x00000000L
	/* CA */,  C_DATA2
	/* CB */,  0x00000000L
	/* CC */,  0x00000000L
	/* CD */,  C_DATA1+C_DATA4
	/* CE */,  0x00000000L
	/* CF */,  0x00000000L
	/* D0 */,  C_MODRM
	/* D1 */,  C_MODRM
	/* D2 */,  C_MODRM
	/* D3 */,  C_MODRM
	/* D4 */,  0x00000000L
	/* D5 */,  0x00000000L
	/* D6 */,  0x00000000L
	/* D7 */,  0x00000000L
	/* D8 */,  C_MODRM
	/* D9 */,  C_MODRM
	/* DA */,  C_MODRM
	/* DB */,  C_MODRM
	/* DC */,  C_MODRM
	/* DD */,  C_MODRM
	/* DE */,  C_MODRM
	/* DF */,  C_MODRM
	/* E0 */,  C_DATA1
	/* E1 */,  C_DATA1
	/* E2 */,  C_DATA1
	/* E3 */,  C_DATA1
	/* E4 */,  C_DATA1
	/* E5 */,  C_DATA1
	/* E6 */,  C_DATA1
	/* E7 */,  C_DATA1
	/* E8 */,  C_DATA66
	/* E9 */,  C_DATA66
	/* EA */,  C_DATA66+C_MEM2
	/* EB */,  C_DATA1
	/* EC */,  0x00000000L
	/* ED */,  0x00000000L
	/* EE */,  0x00000000L
	/* EF */,  0x00000000L
	/* F0 */,  C_PREFIX
	/* F1 */,  0x00000000L                       // 0xF1
	/* F2 */,  C_PREFIX
	/* F3 */,  C_PREFIX
	/* F4 */,  0x00000000L
	/* F5 */,  0x00000000L
	/* F6 */,  C_FUCKINGTEST
	/* F7 */,  C_FUCKINGTEST
	/* F8 */,  0x00000000L
	/* F9 */,  0x00000000L
	/* FA */,  0x00000000L
	/* FB */,  0x00000000L
	/* FC */,  0x00000000L
	/* FD */,  0x00000000L
	/* FE */,  C_MODRM
	/* FF */,  C_MODRM
}; // table_1

const unsigned long table_0F[256] = {
	/* 00 */   C_MODRM
	/* 01 */,  C_MODRM
	/* 02 */,  C_MODRM
	/* 03 */,  C_MODRM
	/* 04 */,  0xFFFFFFFFL
	/* 05 */,  0xFFFFFFFFL
	/* 06 */,  0x00000000L
	/* 07 */,  0xFFFFFFFFL
	/* 08 */,  0x00000000L
	/* 09 */,  0x00000000L
	/* 0A */,  0x00000000L
	/* 0B */,  0x00000000L
	/* 0C */,  0xFFFFFFFFL
	/* 0D */,  0xFFFFFFFFL
	/* 0E */,  0xFFFFFFFFL
	/* 0F */,  0xFFFFFFFFL
	/* 10 */,  0xFFFFFFFFL
	/* 11 */,  0xFFFFFFFFL
	/* 12 */,  0xFFFFFFFFL
	/* 13 */,  0xFFFFFFFFL
	/* 14 */,  0xFFFFFFFFL
	/* 15 */,  0xFFFFFFFFL
	/* 16 */,  0xFFFFFFFFL
	/* 17 */,  0xFFFFFFFFL
	/* 18 */,  0xFFFFFFFFL
	/* 19 */,  0xFFFFFFFFL
	/* 1A */,  0xFFFFFFFFL
	/* 1B */,  0xFFFFFFFFL
	/* 1C */,  0xFFFFFFFFL
	/* 1D */,  0xFFFFFFFFL
	/* 1E */,  0xFFFFFFFFL
	/* 1F */,  0xFFFFFFFFL
	/* 20 */,  0xFFFFFFFFL
	/* 21 */,  0xFFFFFFFFL
	/* 22 */,  0xFFFFFFFFL
	/* 23 */,  0xFFFFFFFFL
	/* 24 */,  0xFFFFFFFFL
	/* 25 */,  0xFFFFFFFFL
	/* 26 */,  0xFFFFFFFFL
	/* 27 */,  0xFFFFFFFFL
	/* 28 */,  0xFFFFFFFFL
	/* 29 */,  0xFFFFFFFFL
	/* 2A */,  0xFFFFFFFFL
	/* 2B */,  0xFFFFFFFFL
	/* 2C */,  0xFFFFFFFFL
	/* 2D */,  0xFFFFFFFFL
	/* 2E */,  0xFFFFFFFFL
	/* 2F */,  0xFFFFFFFFL
	/* 30 */,  0xFFFFFFFFL
	/* 31 */,  0xFFFFFFFFL
	/* 32 */,  0xFFFFFFFFL
	/* 33 */,  0xFFFFFFFFL
	/* 34 */,  0xFFFFFFFFL
	/* 35 */,  0xFFFFFFFFL
	/* 36 */,  0xFFFFFFFFL
	/* 37 */,  0xFFFFFFFFL
	/* 38 */,  0xFFFFFFFFL
	/* 39 */,  0xFFFFFFFFL
	/* 3A */,  0xFFFFFFFFL
	/* 3B */,  0xFFFFFFFFL
	/* 3C */,  0xFFFFFFFFL
	/* 3D */,  0xFFFFFFFFL
	/* 3E */,  0xFFFFFFFFL
	/* 3F */,  0xFFFFFFFFL
	/* 40 */,  0xFFFFFFFFL
	/* 41 */,  0xFFFFFFFFL
	/* 42 */,  0xFFFFFFFFL
	/* 43 */,  0xFFFFFFFFL
	/* 44 */,  0xFFFFFFFFL
	/* 45 */,  0xFFFFFFFFL
	/* 46 */,  0xFFFFFFFFL
	/* 47 */,  0xFFFFFFFFL
	/* 48 */,  0xFFFFFFFFL
	/* 49 */,  0xFFFFFFFFL
	/* 4A */,  0xFFFFFFFFL
	/* 4B */,  0xFFFFFFFFL
	/* 4C */,  0xFFFFFFFFL
	/* 4D */,  0xFFFFFFFFL
	/* 4E */,  0xFFFFFFFFL
	/* 4F */,  0xFFFFFFFFL
	/* 50 */,  0xFFFFFFFFL
	/* 51 */,  0xFFFFFFFFL
	/* 52 */,  0xFFFFFFFFL
	/* 53 */,  0xFFFFFFFFL
	/* 54 */,  0xFFFFFFFFL
	/* 55 */,  0xFFFFFFFFL
	/* 56 */,  0xFFFFFFFFL
	/* 57 */,  0xFFFFFFFFL
	/* 58 */,  0xFFFFFFFFL
	/* 59 */,  0xFFFFFFFFL
	/* 5A */,  0xFFFFFFFFL
	/* 5B */,  0xFFFFFFFFL
	/* 5C */,  0xFFFFFFFFL
	/* 5D */,  0xFFFFFFFFL
	/* 5E */,  0xFFFFFFFFL
	/* 5F */,  0xFFFFFFFFL
	/* 60 */,  0xFFFFFFFFL
	/* 61 */,  0xFFFFFFFFL
	/* 62 */,  0xFFFFFFFFL
	/* 63 */,  0xFFFFFFFFL
	/* 64 */,  0xFFFFFFFFL
	/* 65 */,  0xFFFFFFFFL
	/* 66 */,  0xFFFFFFFFL
	/* 67 */,  0xFFFFFFFFL
	/* 68 */,  0xFFFFFFFFL
	/* 69 */,  0xFFFFFFFFL
	/* 6A */,  0xFFFFFFFFL
	/* 6B */,  0xFFFFFFFFL
	/* 6C */,  0xFFFFFFFFL
	/* 6D */,  0xFFFFFFFFL
	/* 6E */,  0xFFFFFFFFL
	/* 6F */,  0xFFFFFFFFL
	/* 70 */,  0xFFFFFFFFL
	/* 71 */,  0xFFFFFFFFL
	/* 72 */,  0xFFFFFFFFL
	/* 73 */,  0xFFFFFFFFL
	/* 74 */,  0xFFFFFFFFL
	/* 75 */,  0xFFFFFFFFL
	/* 76 */,  0xFFFFFFFFL
	/* 77 */,  0xFFFFFFFFL
	/* 78 */,  0xFFFFFFFFL
	/* 79 */,  0xFFFFFFFFL
	/* 7A */,  0xFFFFFFFFL
	/* 7B */,  0xFFFFFFFFL
	/* 7C */,  0xFFFFFFFFL
	/* 7D */,  0xFFFFFFFFL
	/* 7E */,  0xFFFFFFFFL
	/* 7F */,  0xFFFFFFFFL
	/* 80 */,  C_DATA66
	/* 81 */,  C_DATA66
	/* 82 */,  C_DATA66
	/* 83 */,  C_DATA66
	/* 84 */,  C_DATA66
	/* 85 */,  C_DATA66
	/* 86 */,  C_DATA66
	/* 87 */,  C_DATA66
	/* 88 */,  C_DATA66
	/* 89 */,  C_DATA66
	/* 8A */,  C_DATA66
	/* 8B */,  C_DATA66
	/* 8C */,  C_DATA66
	/* 8D */,  C_DATA66
	/* 8E */,  C_DATA66
	/* 8F */,  C_DATA66
	/* 90 */,  C_MODRM
	/* 91 */,  C_MODRM
	/* 92 */,  C_MODRM
	/* 93 */,  C_MODRM
	/* 94 */,  C_MODRM
	/* 95 */,  C_MODRM
	/* 96 */,  C_MODRM
	/* 97 */,  C_MODRM
	/* 98 */,  C_MODRM
	/* 99 */,  C_MODRM
	/* 9A */,  C_MODRM
	/* 9B */,  C_MODRM
	/* 9C */,  C_MODRM
	/* 9D */,  C_MODRM
	/* 9E */,  C_MODRM
	/* 9F */,  C_MODRM
	/* A0 */,  0x00000000L
	/* A1 */,  0x00000000L
	/* A2 */,  0x00000000L
	/* A3 */,  C_MODRM
	/* A4 */,  C_MODRM+C_DATA1
	/* A5 */,  C_MODRM
	/* A6 */,  0xFFFFFFFFL
	/* A7 */,  0xFFFFFFFFL
	/* A8 */,  0x00000000L
	/* A9 */,  0x00000000L
	/* AA */,  0x00000000L
	/* AB */,  C_MODRM
	/* AC */,  C_MODRM+C_DATA1
	/* AD */,  C_MODRM
	/* AE */,  0xFFFFFFFFL
	/* AF */,  C_MODRM
	/* B0 */,  C_MODRM
	/* B1 */,  C_MODRM
	/* B2 */,  C_MODRM
	/* B3 */,  C_MODRM
	/* B4 */,  C_MODRM
	/* B5 */,  C_MODRM
	/* B6 */,  C_MODRM
	/* B7 */,  C_MODRM
	/* B8 */,  0xFFFFFFFFL
	/* B9 */,  0xFFFFFFFFL
	/* BA */,  C_MODRM+C_DATA1
	/* BB */,  C_MODRM
	/* BC */,  C_MODRM
	/* BD */,  C_MODRM
	/* BE */,  C_MODRM
	/* BF */,  C_MODRM
	/* C0 */,  C_MODRM
	/* C1 */,  C_MODRM
	/* C2 */,  0xFFFFFFFFL
	/* C3 */,  0xFFFFFFFFL
	/* C4 */,  0xFFFFFFFFL
	/* C5 */,  0xFFFFFFFFL
	/* C6 */,  0xFFFFFFFFL
	/* C7 */,  0xFFFFFFFFL
	/* C8 */,  0x00000000L
	/* C9 */,  0x00000000L
	/* CA */,  0x00000000L
	/* CB */,  0x00000000L
	/* CC */,  0x00000000L
	/* CD */,  0x00000000L
	/* CE */,  0x00000000L
	/* CF */,  0x00000000L
	/* D0 */,  0xFFFFFFFFL
	/* D1 */,  0xFFFFFFFFL
	/* D2 */,  0xFFFFFFFFL
	/* D3 */,  0xFFFFFFFFL
	/* D4 */,  0xFFFFFFFFL
	/* D5 */,  0xFFFFFFFFL
	/* D6 */,  0xFFFFFFFFL
	/* D7 */,  0xFFFFFFFFL
	/* D8 */,  0xFFFFFFFFL
	/* D9 */,  0xFFFFFFFFL
	/* DA */,  0xFFFFFFFFL
	/* DB */,  0xFFFFFFFFL
	/* DC */,  0xFFFFFFFFL
	/* DD */,  0xFFFFFFFFL
	/* DE */,  0xFFFFFFFFL
	/* DF */,  0xFFFFFFFFL
	/* E0 */,  0xFFFFFFFFL
	/* E1 */,  0xFFFFFFFFL
	/* E2 */,  0xFFFFFFFFL
	/* E3 */,  0xFFFFFFFFL
	/* E4 */,  0xFFFFFFFFL
	/* E5 */,  0xFFFFFFFFL
	/* E6 */,  0xFFFFFFFFL
	/* E7 */,  0xFFFFFFFFL
	/* E8 */,  0xFFFFFFFFL
	/* E9 */,  0xFFFFFFFFL
	/* EA */,  0xFFFFFFFFL
	/* EB */,  0xFFFFFFFFL
	/* EC */,  0xFFFFFFFFL
	/* ED */,  0xFFFFFFFFL
	/* EE */,  0xFFFFFFFFL
	/* EF */,  0xFFFFFFFFL
	/* F0 */,  0xFFFFFFFFL
	/* F1 */,  0xFFFFFFFFL
	/* F2 */,  0xFFFFFFFFL
	/* F3 */,  0xFFFFFFFFL
	/* F4 */,  0xFFFFFFFFL
	/* F5 */,  0xFFFFFFFFL
	/* F6 */,  0xFFFFFFFFL
	/* F7 */,  0xFFFFFFFFL
	/* F8 */,  0xFFFFFFFFL
	/* F9 */,  0xFFFFFFFFL
	/* FA */,  0xFFFFFFFFL
	/* FB */,  0xFFFFFFFFL
	/* FC */,  0xFFFFFFFFL
	/* FD */,  0xFFFFFFFFL
	/* FE */,  0xFFFFFFFFL
	/* FF */,  0xFFFFFFFFL
}; // table_0F

/* ------------------------- MACROS PUBLIC ------------------------- */

#define catSetBit(a, x) (a |= (1 << x))
#define catClearBit(a, x) (a &= ~(1 << x))
#define catInvertBit(a, x) (a ^= (1 << x))
//#define catGetBit(a, x) (a & (1 << x))

/* ------------------------- CLASSES PUBLIC ------------------------- */

/* --- Cat: Api Hooking Class --- */
class CcatApiHookingSupport {
	protected:
		typedef struct _REDIRECT {
			unsigned char jmp;
			unsigned long address;
		} TRedirect;
	public:
		CcatApiHookingSupport(){};
		~CcatApiHookingSupport(){};
		unsigned long catapi catLenInstruction(unsigned long ulAddress);
		unsigned long catapi catJumpLen(unsigned long ulSrcAddress, unsigned long ulDestAddress);
		bool catapi catDetour(void* pProc, void* pHookProc, void** pOldProc);
};

class CcatApiHookingA: protected CcatApiHookingSupport {
	public:
		CcatApiHookingA(){};
		~CcatApiHookingA(){};
		bool catapi catAPIAttach(const char* lpszModuleName, const char* lpszProcName, void* lpHookProc, void** lpOldProc);
		bool catapi catAPIDetach(const char* lpszModuleName, const char* lpszProcName, void** lpOldProc);
	protected:
};

class CcatApiHookingW: protected CcatApiHookingSupport {
	public:
		CcatApiHookingW(){};
		~CcatApiHookingW(){};
		bool catapi catAPIAttach(const wchar_t* lpwszModuleName, const wchar_t* lpwszProcName, void* lpHookProc, void** lpOldProc);
		bool catapi catAPIDetach(const wchar_t* lpwszModuleName, const wchar_t* lpwszProcName, void** lpOldProc);
	protected:
};

/* --- Cat : File Working Class --- */

enum TFileModeFlags {
	fmCreateNew = 1,	// CREATE_NEW = 1,
	fmCreateAlway,		// CREATE_ALWAYS = 2,
	fmOpenExisting,		// OPEN_EXISTING = 3,
	fmOpenAlways,		// OPEN_ALWAYS = 4,
	fmTruncateExisting,	// TRUNCATE_EXISTING = 5,
};

enum TFileAttributeFlags {
	faUnknown 			= 0x00000000,
	faReadOnly 			= 0x00000001,	// FILE_ATTRIBUTE_READONLY             = $00000001;
	faHidden   			= 0x00000002,	// FILE_ATTRIBUTE_HIDDEN               = $00000002;
	faSystem 			= 0x00000004,	// FILE_ATTRIBUTE_SYSTEM               = $00000004;
	//faDirectory 		= 0x00000010,	// FILE_ATTRIBUTE_DIRECTORY            = $00000010;
	faArchive 			= 0x00000020,	// FILE_ATTRIBUTE_ARCHIVE              = $00000020;
	//faDevice 			= 0x00000040,	// FILE_ATTRIBUTE_DEVICE               = $00000040;
	faNormal 			= 0x00000080,	// FILE_ATTRIBUTE_NORMAL               = $00000080;
	faTemporary 		= 0x00000100,	// FILE_ATTRIBUTE_TEMPORARY            = $00000100;
	//faSparseFile 		= 0x00000200,	// FILE_ATTRIBUTE_SPARSE_FILE          = $00000200;
	//faReparsePoint 	= 0x00000400,	// FILE_ATTRIBUTE_REPARSE_POINT        = $00000400;
	faCompressed 		= 0x00000800,	// FILE_ATTRIBUTE_COMPRESSED           = $00000800;
	faOffline 			= 0X00001000,	// FILE_ATTRIBUTE_OFFLINE              = $00001000;
	//faNotContentIndexed = 0x00002000,	// FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  = $00002000;
	//faEncrypted 		= 0x00004000,	// FILE_ATTRIBUTE_ENCRYPTED            = $00004000;
};

enum TFileShareFlags {
	fsNone				= 0x00000000,
	fsRead              = 0x00000001,
	fsWrite             = 0x00000002,
	fsDelete            = 0x00000004
};

class CcatFileSupport {
	public:
		CcatFileSupport(){};
		~CcatFileSupport(){};
	protected:
		HANDLE hFile;
		unsigned long ulReadSize, ulWroteSize;

		virtual bool catapi catRead(void* Buffer, unsigned long ulSize);
		virtual bool catapi catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize);
		virtual bool catapi catWrite(const void* cBuffer, unsigned long ulSize);
		virtual bool catapi catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize);
		virtual bool catapi catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize);
		virtual bool catapi catClose();
	private:
};

class CcatFileA: protected CcatFileSupport {
	public:
		CcatFileA(){};
		~CcatFileA(){};
		bool catapi catInit(const char *lpszFilePath, TFileModeFlags fmFlag, TFileShareFlags fsFlag, TFileAttributeFlags faFlag);
		bool catapi catRead(void* Buffer, unsigned long ulSize);
		bool catapi catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize);
		bool catapi catWrite(const void* cBuffer, unsigned long ulSize);
		bool catapi catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize);
		bool catapi catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize);
		bool catapi catClose();
	protected:
};

class CcatFileW: protected CcatFileSupport {
	public:
		CcatFileW(){};
		~CcatFileW(){};
		bool catapi catInit(const wchar_t *lpwszFilePath, TFileModeFlags fmFlag, TFileShareFlags fsFlag, TFileAttributeFlags faAttributeFlag);
		bool catapi catRead(void* Buffer, unsigned long ulSize);
		bool catapi catRead(unsigned long ulOffset, void* Buffer, unsigned long ulSize);
		bool catapi catWrite(const void* cBuffer, unsigned long ulSize);
		bool catapi catWrite(unsigned long ulOffset, const void* cBuffer, unsigned long ulSize);
		bool catapi catIOControl(unsigned long ulControlCode, void* lpSendBuffer, unsigned long ulSendSize, void* lpReveiceBuffer, unsigned long ulReveiceSize);
		bool catapi catClose();
	protected:
};

/* --- Cat : Service Working Class --- */
class CcatServiceSupport {
	protected:
		SC_HANDLE hSCManager, hService;
    	SERVICE_STATUS ss;
    	virtual bool catapi catUnLoadService();
	public:
		CcatServiceSupport(){};
		~CcatServiceSupport(){};
	private:
};

class CcatServiceA: protected CcatServiceSupport {
	public:
		CcatServiceA(){};
		~CcatServiceA(){};
		bool catapi catLoadService(const char* lpszServiceName);
		bool catapi catUnLoadService();
	protected:
};

class CcatServiceW: protected CcatServiceSupport {
	public:
		CcatServiceW(){};
		~CcatServiceW(){};
		bool catapi catLoadService(const wchar_t* lpwszServiceName);
		bool catapi catUnLoadService();
	protected:
};

/* --- Cat : Process In/Ejection Class --- */
class CcatProcessA {
	public:
		CcatProcessA(){};
		~CcatProcessA(){};
		bool catapi catInjectionLibrary(unsigned long PID, const char* lpcszLibraryPath);
		bool catapi catEjectionLibrary(unsigned long PID, const char* lpcszModuleName);
	protected:
};

class CcatProcessW {
	public:
		CcatProcessW(){};
		~CcatProcessW(){};
		bool catapi catInjectionLibrary(unsigned long PID, const wchar_t* lpcwszLibraryPath);
		bool catapi catEjectionLibrary(unsigned long PID, const wchar_t* lpcwszModuleName);
	protected:
};

/* --- Cat : Library Class --- */
class CcatLibraryA {
	public:
		CcatLibraryA();
		CcatLibraryA(const char *lpcszModuleName);
		CcatLibraryA(const char *lpcszModuleName, const char *lpcszRoutineName);
		~CcatLibraryA();

		bool catapi IsLibraryAvailable();
		void* catapi catGetRoutineAddress();
		void* catapi catGetRoutineAddress(const char *lpcszRoutineName);
		void* catapi catGetRoutineAddress(const char *lpcszModuleName, const char *lpcszRoutineName);
	private:
		char *lpszModuleName, *lpszRoutineName;
		void* lpRoutine;
	protected:
};

class CcatLibraryW {
public:
	CcatLibraryW();
	CcatLibraryW(const wchar_t *lpcszModuleName);
	CcatLibraryW(const wchar_t *lpcszModuleName, const wchar_t *lpcszRoutineName);
	~CcatLibraryW();

	bool catapi IsLibraryAvailable();
	void* catapi catGetRoutineAddress();
	void* catapi catGetRoutineAddress(const wchar_t *lpcwszRoutineName);
	void* catapi catGetRoutineAddress(const wchar_t *lpcwszModuleName, const wchar_t *lpcwszRoutineName);
private:
	wchar_t *lpwszModuleName, *lpwszRoutineName;
	void* lpRoutine;
protected:
};

/* -------- Re-define Classes with non A/W suffix -------- */
/*
	CcatService
*/
#ifndef UNICODE
	#define CcatHookAPI CcatApiHookingA
	#define CcatService CcatServiceA
	#define CcatFile CcatFileA
	#define CcatProcess CcatProcessA
	#define CcatLibrary CcatLibraryA
#else
	#define CcatHookAPI CcatApiHookingW
	#define CcatService CcatServiceW
	#define CcatFile CcatFileW
	#define CcatProcess CcatProcessW
	#define CcatLibrary CcatLibraryW
#endif

/* ------------------------- FUNCTIONS PUBLIC ------------------------- */

/* --- Cat: Math Working --- */
int catapi catGCD(unsigned long count,...);
int catapi catLCM(unsigned long count,...);
/* --- Cat: Base Working --- */
unsigned long catapi catGetBit(unsigned long Value, unsigned char position);
/* --- Cat: String Formatting --- */
char* catapi catLastErrorA();
wchar_t* catapi catLastErrorW();
char* catapi catFormatA(const char *lpcszFormat,...);
wchar_t* catapi catFormatW(const wchar_t *lpcwszFormat,...);
void catapi catMsgA(const char* lpcszFormat,...);
void catapi catMsgW(const wchar_t *lpcwszFormat,...);
/* --- Cat: String Working --- */
char* catapi catLowerStringA(const char *lpcszString);
wchar_t* catapi catLowerStringW(const wchar_t *lpcwszString);
char* catapi catUpperStringA(const char *lpcszString);
wchar_t* catapi catUpperStringW(const wchar_t *lpcwszString);
char* catapi catPwcToPac(const wchar_t* lpcwszString);
wchar_t* catapi catPacToPwc(const char* lpcszString);
/* --- Cat: Message Box Formatting --- */
HWND catapi catGetConsoleWindow();
int catapi catBoxA(const char *lpcszTitle, unsigned long ulType, const char *lpcszFormat,...);
int catapi catBoxA(HWND hwnd, const char *lpcszTitle, unsigned long uType, const char *lpcszFormat,...);
int catapi catBoxW(HWND hwnd, const wchar_t *lpcwszTitle, unsigned long uType, const wchar_t *lpcwszFormat,...);
int catapi catBoxA(HWND hwnd, const char *lpcszFormat, ...);
int catapi catBoxW(HWND hwnd, const wchar_t *lpcwszFormat, ...);
int catapi catBoxA(const char *lpcszFormat,...);
int catapi catBoxW(const wchar_t *lpcwszFormat,...);
int catapi catBoxA(unsigned long ulType, const char *lpcszFormat,...);
int catapi catBoxW(unsigned long ulType, const wchar_t *lpcwszFormat,...);
int catapi catBoxA(const char *lpcszTitle, unsigned long ulType, const char *lpcszFormat,...);
int catapi catBoxW(const wchar_t *lpcwszTitle, unsigned long ulType, const wchar_t *lpcwszFormat,...);
//void catapi catBoxA(const char *lpcszTitle, const char *lpcszFormat,...);
//void catapi catBoxW(const wchar_t *lpcwszTitle, const wchar_t *lpcwszFormat,...);
/* --- Cat: Process Working --- */
int catapi catIsWow64(); /* -1: Error, 0: False, 1: True */
unsigned long catapi catGetParentPID(unsigned long ulChildPID);
unsigned long catapi catNameToPidA(const char* lpcszProcessName);
unsigned long catapi catNameToPidW(const wchar_t* lpcwszProcessName);
char* catapi catPidToNameA(unsigned long ulPID);
wchar_t* catapi catPidToNameW(unsigned long ulPID);
HMODULE catapi catRemoteGetModuleHandleA(unsigned long ulPID, const char* lpcszModuleName);
HMODULE catapi catRemoteGetModuleHandleW(unsigned long ulPID, const wchar_t* lpwcszModuleName);
/* --- Cat: Memory Working --- */
bool catapi catRPM(unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize);
bool catapi catRPM(HANDLE hProcess, unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize);
bool catapi catRPM(unsigned long ulPID, unsigned long ulAddress, void* lpBuffer, SIZE_T ulSize);
bool catapi catWPM(unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize);
bool catapi catWPM(HANDLE hProcess, unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize);
bool catapi catWPM(unsigned long ulPID, unsigned long ulAddress, void* lpcBuffer, SIZE_T ulSize);
/* --- Cat: File Working --- */
char* catapi catFileTypeA(const char* lpcszFilePath);
wchar_t* catapi catFileTypeW(const wchar_t* lpcwszFilePath);
bool catapi catFileExistsA(const char* lpcszFilePath);
bool catapi catFileExistsW(const wchar_t* lpcwszFilePath);
char* catapi catExtractFilePathA(const char* lpcszFilePath);
wchar_t* catapi catExtractFilePathW(const wchar_t* lpcwszFilePath);
char* catapi catExtractFileNameA(const char* lpcszFilePath);
wchar_t* catapi catExtractFileNameW(const wchar_t* lpcwszFilePath);

/* -------- Re-define Functions with non A/W suffix -------- */
/*
	catIsWow64
	catGetParentPID
	catRPM
	catWPM
	catGetBit
	catSetBit
*/
#ifndef UNICODE
	/* --- Cat: String Formatting --- */
	#define catLastError catLastErrorA
	#define catFormat catFormatA
	#define catMsg catMsgA
	#define catBox catBoxA
	#define catLowerString catLowerStringA
	#define catUpperString catUpperStringA
	/* --- Cat: Process Working --- */
	#define catNameToPid catNameToPidA
	#define catPidToName catPidToNameA
	#define catRemoteGetModuleHandle catRemoteGetModuleHandleA
	/* --- Cat: Memory Working --- */
	//...
	/* --- Cat: File Working --- */
	#define catFileType catFileTypeA
	#define catFileExists catFileExistsA
	#define catExtractFilePath catExtractFilePathA
	#define catExtractFileName catExtractFileNameA
#else
	/* --- Cat: String Formatting --- */
	#define catLastError catLastErrorW
	#define catFormat catFormatW
	#define catMsg catMsgW
	#define catBox catBoxW
	#define catLowerString catLowerStringW
	#define catUpperString catUpperStringW
	/* --- Cat: Process Working --- */
	#define catNameToPid catNameToPidW
	#define catPidToName catPidToNameW
	#define catRemoteGetModuleHandle catRemoteGetModuleHandleW
	/* --- Cat: Memory Working --- */
	// ...
	/* --- Cat: File Working --- */
	#define catFileType catFileTypeW
	#define catFileExists catFileExistsW
	#define catExtractFilePath catExtractFilePathW
	#define catExtractFileName catExtractFileNameW
#endif

#endif // _CATENGINE_H_