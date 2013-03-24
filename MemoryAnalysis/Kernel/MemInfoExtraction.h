
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
	// �Ľ��͡�����ȡ�����ڴ�

	static void ExtractProcessMemoryPageTableXP(PROCESS pro,CString imgFilePath);// Page Table
	void ExtractProcessMemoryVadTreeXP(PROCESS pro,CString imgFilePath);  // VadTree

	DWORD VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CFile imgFile);

	//////////////////////////////////////////////////////////////////////////
	// XP��
	void ExtractKpcrXP(const char* Memory,ULONGLONG memLen);
	void ExtractEprocessXP(const char* Memory,DWORD memLen);  // Ѱ��XP�µ�EPROCESS
	void ExtractProcessMemoryVadTreeXP(CString filePath);

	bool IsKpcrExistXP();
	char* GetKpcrResultXP();

	char* GetEprocessResultXP(int i);
	int GetEprocessAmountXP();
	void EprocessAmountToZeroXP(); // ��eprocessAmount����


	DWORD VAtoPAXP(DWORD dirTableBase,DWORD virtualAdd,CString filePath); 
	DWORD GetVAValueXP(DWORD dirTableBase,DWORD virtualAdd,CString filePath);


	//////////////////////////////////////////////////////////////////////////
	// WIN7��
	void ExtractKpcrWin7(char* Memory,DWORD memLen);
	void ExtractEprocessWin7(const char* Memory,DWORD memLen);

	int GetKpcrAmountWin7();

	char* GetEprocessResultWin7(int i);
	int GetEprocessAmountWin7();
	void EprocessAmountToZeroWin7();


	//////////////////////////////////////////////////////////////////////////
	//��Vad�������������

	//void MiddleOrderVadTree(DWORD vadRootPhysicalAdd);
	void MiddleOrderVadTree1(DWORD dirTableBase,DWORD vadNodeVirtuallAdd,CString filePath,int processNum,CString fp);

	//�Ľ���
	void MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath,CString dmpFilePath);

private:
	
	//////////////////////////////////////////////////////////////////////////
	// XP��
	bool bKpcrExistXP;  // kpcr��־λ
	char* pKpcrResultXP;  // ����ڴ澵���е�KPCR

	int mEprocessActiveAmountXP;  // ����̵�����
	char** ppEprocessResultXP; // ��Ż�ȡ�Ľ��̵�EPROCESS�ṹ,�ö�ά��������Ž��
	char* pProcessMemoryXP; // ��Ż�ȡ�Ľ��̵��ڴ�

	//////////////////////////////////////////////////////////////////////////
	// win7 ��
	int mKpcrAmountWin7;
	char* pKpcrResultWin7;  // ����ڴ澵���е�KPCR
	
	int mEprocessActiveAmountWin7;  // ����̵�����
	char** ppEprocessResultWin7;    // ��Ż�ȡ�Ľ��̵�EPROCESS�ṹ, �ö�ά��������Ž��
	char* pProcessMemoryWin7;       // ��Ż�ȡ�Ľ��̵��ڴ�

	//////////////////////////////////////////////////////////////////////////
	char* pPageDataBuf;       //

public:
	//���������
	MemInfoExtraction operator=(MemInfoExtraction &meminfo);
};
