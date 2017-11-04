#ifndef MGLOBAL_H
#define MGLOBAL_H

#define DRIVER		L"CatDriver"
#define DRIVER_NAME	(DRIVER L".sys")

#define CATNTAPI NTAPI

#define MAGIC_NUMBER 7

typedef struct _RW_MEMORY_INFO {
	HANDLE Handle;
	void*  Address;
	void*  Buffer;
	unsigned long BufferSize;
	unsigned long BytesDone;
} TReadWriteMemoryInfo, *PReadWriteMemoryInfo;

#endif // MGLOBAL_H