#pragma once
#include "afxcmn.h"
#include "AfxTempl.h"



#include "MemoryManagement.h"

#include "MemInfoExtraction.h"


// CResultProcess 对话框

class CResultProcess : public CDialog
{
	DECLARE_DYNAMIC(CResultProcess)

public:
	CResultProcess(CWnd* pParent = NULL);   // 标准构造函数
	//CResultProcess(CArray* pProcessArray);
	virtual ~CResultProcess();

// 对话框数据
	enum { IDD = IDD_DIALOG_RESULT_PROCESS };

private:
	CArray <PROCESS,PROCESS> mProInfoArray;//进程的EProcess结构信息
	CArray <PROCESS,PROCESS>* pProInfoArray;//进程的EProcess结构信息
	CString imageFilePath;//镜像路径
	MemInfoExtraction mMemInfo;//



private:
	//控件
	//CProgressCtrl *pProCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//CListCtrl m_ProcessList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedShow();

	void SetProcessInfo(PROCESS process);//需要修改
	void SetProcessInfo(CArray<PROCESS,PROCESS>* pProcessArray);
	void TransferPara(CString imgFilePath,MemInfoExtraction memInfo);
	void ExtractProcMemXP(PROCESS pro,CString imgFilePath);



	CListBox m_ListProcesses;
	afx_msg void OnLbnDblclkListProcesses();
//	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedSave();
	


	CListCtrl m_ListCtrProcesses;
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
//	afx_msg void OnBnClickedChromeMemoryExtract();


	afx_msg void OnBnClickedButton4();
	CString m_strProName;
	afx_msg void OnClickedNotepad();
};

UINT ThreadPageTableFunc(LPVOID pParam);