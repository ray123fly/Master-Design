#pragma once


// CDialogWebAnalysis �Ի���

class CDialogWebAnalysis : public CDialog
{
	DECLARE_DYNAMIC(CDialogWebAnalysis)

public:
	CDialogWebAnalysis(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogWebAnalysis();

// �Ի�������
	enum { IDD = IDD_DIALOG_WEB_ANALYSIS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenRawFile();
	afx_msg void OnBnClickedAnalysis();
//	CString m_WebResult;
	CString strTmpMemFile;
	CListCtrl m_ListMemFile;
	virtual BOOL OnInitDialog();

	VOID WebAnalysis(const CString& file);
	int mMemFileCount;
	CListBox m_Log;
	char **ppLogFilePath;

	// test
	CString m_TestRegex;
	CString strDefultRegex;
	CString strTmpRegex;
	CRichEditCtrl m_WebContent;
//	afx_msg void OnSetfocusWebResultLog();
	afx_msg void OnDblclkWebResultLog();

	// //��ʾ�����Ľ��
	VOID ShowWebContent(const CString& file);

	afx_msg void OnBnClickedTest();

	afx_msg void OnBnClickedButtonSelectAll();
	afx_msg void OnBnClickedButtonNotSelectAll();
	afx_msg void OnBnClickedCodeTest();
};
