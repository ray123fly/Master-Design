
//Process.h : all of the structure information of process 

#ifndef __PROCESS_H__
#define __PROCESS_H__

struct _PROCESS_STRUCT
{
	CString strName;
	DWORD DirectoryTableBase;
	DWORD VadRootVirAddr;

	DWORD Pid;
	DWORD PPid;
};
typedef _PROCESS_STRUCT PROCESS;

#endif


