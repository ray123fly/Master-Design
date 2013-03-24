#include <afx.h>
//#include "Process.h"
#include "MemoryManagement.h"
#include "stdafx.h"

#define PDE_COUNT       1024
#define PDE_COUNT_LARGE 512
#define PTE_COUNT_LARGE 4*1024*1024
#define PTE_COUNT       1024
#define PAGE_SIZE       4*1024
#define PAGE_SIZE_LARGE 4*1024*1024

void ExtractProcessMemoryXP(PROCESS pro,CString imgFilePath);