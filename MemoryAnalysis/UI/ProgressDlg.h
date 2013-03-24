#pragma once

#include "MemoryAnalysis.h"

// CProgressDlg 对话框

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROGRESS };

	int lower;
	int upper;

	int pos;//进度条的游动位置

public:
	void SetProgressRange(int begin,int end);
	void UpdateProgressCtrl(int curPos);

	virtual BOOL OnInitDialog();
	BOOL Create();

	VOID Close();
	virtual BOOL DestroyWindow();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProgressCtrl;
};
