#pragma once


// CDialogUtf8Test �Ի���

class CDialogUtf8Test : public CDialog
{
	DECLARE_DYNAMIC(CDialogUtf8Test)

public:
	CDialogUtf8Test(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogUtf8Test();

// �Ի�������
	enum { IDD = IDD_UTF8_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUtf8;
//	CEdit m_strShow;
	afx_msg void OnBnClickedTransfer();
	CString m_strShow;
};
