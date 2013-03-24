
#pragma once
#define PROCESS_H


#include "Process.h"
#include "ProgressDlg.h"

#include "MemoryAnalysis.h"

class MemInfoExtraction
{
public:
	MemInfoExtraction(void);
	~MemInfoExtraction(void);

	//////////////////////////////////////////////////////////////////////////
	// 改进型——提取进程内存

	static void ExtractProcessMemoryPageTableXP(PROCESS pro,CString imgFilePath);// Page Table
	void ExtractProcessMemoryVadTreeXP(PROCESS pro,CString imgFilePath);  // VadTree

	DWORD VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CFile imgFile);

	//////////////////////////////////////////////////////////////////////////
	// XP下
	void ExtractKpcrXP(const char* Memory,ULONGLONG memLen);
	void ExtractEprocessXP(const char* Memory,DWORD memLen);  // 寻找XP下的EPROCESS
	void ExtractProcessMemoryVadTreeXP(CString filePath);

	bool IsKpcrExistXP();
	char* GetKpcrResultXP();

	char* GetEprocessResultXP(int i);
	int GetEprocessAmountXP();
	void EprocessAmountToZeroXP(); // 将eprocessAmount清零


	DWORD VAtoPAXP(DWORD dirTableBase,DWORD virtualAdd,CString filePath); 
	DWORD GetVAValueXP(DWORD dirTableBase,DWORD virtualAdd,CString filePath);


	//////////////////////////////////////////////////////////////////////////
	// WIN7下
	void ExtractKpcrWin7(char* Memory,DWORD memLen);
	void ExtractEprocessWin7(const char* Memory,DWORD memLen);

	int GetKpcrAmountWin7();

	char* GetEprocessResultWin7(int i);
	int GetEprocessAmountWin7();
	void EprocessAmountToZeroWin7();


	//////////////////////////////////////////////////////////////////////////
	//对Vad树进行中序遍历

	//void MiddleOrderVadTree(DWORD vadRootPhysicalAdd);
	void MiddleOrderVadTree1(DWORD dirTableBase,DWORD vadNodeVirtuallAdd,CString filePath,int processNum,CString fp);

	//改进型
	void MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath,CString dmpFilePath);

private:
	
	//////////////////////////////////////////////////////////////////////////
	// XP下
	bool bKpcrExistXP;  // kpcr标志位
	char* pKpcrResultXP;  // 存放内存镜像中的KPCR

	int mEprocessActiveAmountXP;  // 活动进程的数量
	char** ppEprocessResultXP; // 存放获取的进程的EPROCESS结构,用二维数组来存放结果
	char* pProcessMemoryXP; // 存放获取的进程的内存

	//////////////////////////////////////////////////////////////////////////
	// win7 下
	int mKpcrAmountWin7;
	char* pKpcrResultWin7;  // 存放内存镜像中的KPCR
	
	int mEprocessActiveAmountWin7;  // 活动进程的数量
	char** ppEprocessResultWin7;    // 存放获取的进程的EPROCESS结构, 用二维数组来存放结果
	char* pProcessMemoryWin7;       // 存放获取的进程的内存

	//////////////////////////////////////////////////////////////////////////
	char* pPageDataBuf;       //

public:
	//运算符重载
	MemInfoExtraction operator=(MemInfoExtraction &meminfo);
};
