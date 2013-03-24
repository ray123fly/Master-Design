// MemoryAnalysisDoc.h : CMemoryAnalysisDoc 类的接口
//


#pragma once
#include "ResultProcess.h"

#include "ProgressDlg.h"


class CMemoryAnalysisDoc : public CDocument
{
protected: // 仅从序列化创建
	CMemoryAnalysisDoc();
	DECLARE_DYNCREATE(CMemoryAnalysisDoc)

// 属性
public:
	ULONGLONG dwBufSize,dwCurSize; //?????
	char* pcBuf;    //?????
	char* pTmpBuf;  // 将数据不断调入这个缓冲区计算
	//char* m_pcBuf1;
	//char* m_pcBuf2;
	//char* m_pcBuf3;


	CString strImageFilePath;//镜像文件路径？？？？？
	CArray<PROCESS,PROCESS>* pProcArray;//保存所有的进程

	//char* KpcrResult;

	// 进度条对话框
	CProgressDlg *pProgressDlg;
	bool bForNotrpad;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CMemoryAnalysisDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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


