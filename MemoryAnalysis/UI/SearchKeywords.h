#pragma once
#include "afxwin.h"


// CSearchKeywords �Ի���

class CSearchKeywords : public CDialog
{
	DECLARE_DYNAMIC(CSearchKeywords)

public:
	CSearchKeywords(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSearchKeywords();

// �Ի�������
	enum { IDD = IDD_DIALOG_SEARCH_KEYWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedOpen();

	VOID ShowResultContent(const CString& file);
	// �ؼ���
//	CEdit mKeyword;
	// Ŀ�������ļ�
//	CEdit m_FileOfSearch;
//	CString m_FielOfSearch;
	CString m_FileOfSearch;
	CString m_Keyword;
	CRichEditCtrl m_ResultCtrl;
};
