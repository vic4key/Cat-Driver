#ifndef MCTRLCODES_H
#define MCTRLCODES_H

#ifndef _DEVIOCTL_
	#define CTL_CODE( DeviceType, Function, Method, Access ) ( \
		((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
	)

	#define METHOD_BUFFERED                 0
	#define METHOD_IN_DIRECT                1
	#define METHOD_OUT_DIRECT               2
	#define METHOD_NEITHER                  3

	#define FILE_ANY_ACCESS                 0
	#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
	#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
	#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe
#endif

#define CAT_DEVICE_TYPE 0x1609

#define CAT_CTL_CODE(x) CTL_CODE(CAT_DEVICE_TYPE, 0x800 + x, METHOD_BUFFERED, FILE_ANY_ACCESS)

// General
#define CAT_IOCTL_GENERAL CAT_CTL_CODE(100)

// Processes
#define CAT_IOCTL_OPENPROCESS		CAT_CTL_CODE(101)

#define CAT_IOCTL_READMEMORY		CAT_CTL_CODE(102)
#define CAT_IOCTL_WRITEMEMORY		CAT_CTL_CODE(103)

#define CAT_IOCTL_SUSPENDPROCESS	CAT_CTL_CODE(104)
#define CAT_IOCTL_RESUMEPROCESS		CAT_CTL_CODE(105)

//#define CAT_IOCTL_WRITEMEMORY		CAT_CTL_CODE(106)

#endif