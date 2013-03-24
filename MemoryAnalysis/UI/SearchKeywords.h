#pragma once
#include "afxwin.h"


// CSearchKeywords 对话框

class CSearchKeywords : public CDialog
{
	DECLARE_DYNAMIC(CSearchKeywords)

public:
	CSearchKeywords(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchKeywords();

// 对话框数据
	enum { IDD = IDD_DIALOG_SEARCH_KEYWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedOpen();

	VOID ShowResultContent(const CString& file);
	// 关键字
//	CEdit mKeyword;
	// 目标搜索文件
//	CEdit m_FileOfSearch;
//	CString m_FielOfSearch;
	CString m_FileOfSearch;
	CString m_Keyword;
	CRichEditCtrl m_ResultCtrl;
};
