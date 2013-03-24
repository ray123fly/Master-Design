// MemoryAnalysisDoc.h : CMemoryAnalysisDoc ��Ľӿ�
//


#pragma once
#include "ResultProcess.h"

#include "ProgressDlg.h"


class CMemoryAnalysisDoc : public CDocument
{
protected: // �������л�����
	CMemoryAnalysisDoc();
	DECLARE_DYNCREATE(CMemoryAnalysisDoc)

// ����
public:
	ULONGLONG dwBufSize,dwCurSize; //?????
	char* pcBuf;    //?????
	char* pTmpBuf;  // �����ݲ��ϵ����������������
	//char* m_pcBuf1;
	//char* m_pcBuf2;
	//char* m_pcBuf3;


	CString strImageFilePath;//�����ļ�·������������
	CArray<PROCESS,PROCESS>* pProcArray;//�������еĽ���

	//char* KpcrResult;

	// �������Ի���
	CProgressDlg *pProgressDlg;
	bool bForNotrpad;

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CMemoryAnalysisDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
public:
//	afx_msg void OnKpcrExtractionXP();
public:
	afx_msg void OnEprocessExtractionXP();
public:
	afx_msg void OnEprocessExtractionWin7();
public:
//	afx_msg void OnKpcrExtractionWin7();
public:
//	afx_msg void OnProcessMemoryExtractionXP();
//	afx_msg void OnWebWebanalysisView();
//	afx_msg void OnWebAnalysis();
	afx_msg void OnWebSearchKeyword();
	afx_msg void OnWebAnalysis();
	afx_msg void OnNotepadRecovery();
};


