#pragma once

#include "MemoryAnalysis.h"

// CProgressDlg �Ի���

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROGRESS };

	int lower;
	int upper;

	int pos;//���������ζ�λ��

public:
	void SetProgressRange(int begin,int end);
	void UpdateProgressCtrl(int curPos);

	virtual BOOL OnInitDialog();
	BOOL Create();

	VOID Close();
	virtual BOOL DestroyWindow();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProgressCtrl;
};
