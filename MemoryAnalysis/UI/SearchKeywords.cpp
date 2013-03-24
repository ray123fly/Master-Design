// SearchKeywords.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "SearchKeywords.h"
#include "afxdialogex.h"

#include "Search.h"
#include "deelx.h"

//#define _WIN32_WINNT
// CSearchKeywords 对话框
#include<boost/regex.hpp>

IMPLEMENT_DYNAMIC(CSearchKeywords, CDialog)

CSearchKeywords::CSearchKeywords(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchKeywords::IDD, pParent)
{

	//  m_FielOfSearch = _T("");
	m_FileOfSearch = _T("");
	m_Keyword = _T("");
}

CSearchKeywords::~CSearchKeywords()
{

}

void CSearchKeywords::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_KEYWORD, mKeyword);
	//  DDX_Control(pDX, IDC_EDIT_FILES, m_FileOfSearch);
	DDX_Text(pDX, IDC_EDIT_FILES, m_FileOfSearch);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_Keyword);
	DDX_Control(pDX, IDC_RICHEDIT_SEARCH_RESULT, m_ResultCtrl);
}


BEGIN_MESSAGE_MAP(CSearchKeywords, CDialog)
	ON_BN_CLICKED(IDC_SEARCH, &CSearchKeywords::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_OPEN, &CSearchKeywords::OnBnClickedOpen)
END_MESSAGE_MAP()


// CSearchKeywords 消息处理程序


void CSearchKeywords::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"All Files(*.*)|*.*||",AfxGetMainWnd());
	if(fileDlg.DoModal()==IDOK)
	{
		//方法四：将选择的镜像文件信息保存在memoryImage中
		m_FileOfSearch=fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_FILES)->SetWindowText(m_FileOfSearch);
		//MessageBox(m_FileOfSearch.GetString());
	}
}



extern void SearchKeysInFile(const char * file,const char * keys,const char * resultFile);

void CSearchKeywords::OnBnClickedSearch()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//	MessageBox(m_Keyword);
	//	MessageBox(m_FileOfSearch);

	// 开始搜索
	//boost::regex mExpression(m_Keyword.GetString());
	//SearchFile(m_FileOfSearch.GetString(),mExpression);

	//////////////////////////////////////////////////////////////////////////
	// 使用deelx.h
	char* resultfile=".\\keywordResultsFile.txt";
	SearchKeysInFile(m_FileOfSearch.GetBuffer(),m_Keyword.GetBuffer(),resultfile);


	//////////////////////////////////////////////////////////////////////////
	// 显示结果
	ShowResultContent(resultfile);
	UpdateData(FALSE);
	MessageBox("OK!");
}


// 回调函数
static DWORD CALLBACK MyStreamingCallback(DWORD dwCookie,LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	ASSERT_KINDOF(CFile, pFile);

	*pcb = pFile->Read(pbBuff, cb);

	return 0;
	//	return !(*pcb > 0);
}

// 显示结果文件
VOID CSearchKeywords::ShowResultContent(const CString& file)
{
	CFile* pInputFile = NULL;

	try
	{
		/*pInputFile = new CFile(str, CFile::modeRead | CFile::shareDenyNone);*/

		pInputFile = new CFile(file, CFile::modeRead | CFile::shareDenyNone);
		EDITSTREAM strm;
		strm.dwCookie = (DWORD) pInputFile;
		strm.pfnCallback = MyStreamingCallback;

		long lResult = m_ResultCtrl.StreamIn(SF_TEXT, strm);

		pInputFile->Close();
	}
	catch (CFileException* pEx)
	{
		pEx->Delete();
	}

	delete pInputFile;
}
