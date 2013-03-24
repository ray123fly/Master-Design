#pragma once


// CDialogUtf8Test 对话框

class CDialogUtf8Test : public CDialog
{
	DECLARE_DYNAMIC(CDialogUtf8Test)

public:
	CDialogUtf8Test(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogUtf8Test();

// 对话框数据
	enum { IDD = IDD_UTF8_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUtf8;
//	CEdit m_strShow;
	afx_msg void OnBnClickedTransfer();
	CString m_strShow;
};
