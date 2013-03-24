// SearchKeywords.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "SearchKeywords.h"
#include "afxdialogex.h"

#include "Search.h"
#include "deelx.h"

//#define _WIN32_WINNT
// CSearchKeywords �Ի���
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


// CSearchKeywords ��Ϣ�������


void CSearchKeywords::OnBnClickedOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"All Files(*.*)|*.*||",AfxGetMainWnd());
	if(fileDlg.DoModal()==IDOK)
	{
		//�����ģ���ѡ��ľ����ļ���Ϣ������memoryImage��
		m_FileOfSearch=fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_FILES)->SetWindowText(m_FileOfSearch);
		//MessageBox(m_FileOfSearch.GetString());
	}
}



extern void SearchKeysInFile(const char * file,const char * keys,const char * resultFile);

void CSearchKeywords::OnBnClickedSearch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	//	MessageBox(m_Keyword);
	//	MessageBox(m_FileOfSearch);

	// ��ʼ����
	//boost::regex mExpression(m_Keyword.GetString());
	//SearchFile(m_FileOfSearch.GetString(),mExpression);

	//////////////////////////////////////////////////////////////////////////
	// ʹ��deelx.h
	char* resultfile=".\\keywordResultsFile.txt";
	SearchKeysInFile(m_FileOfSearch.GetBuffer(),m_Keyword.GetBuffer(),resultfile);


	//////////////////////////////////////////////////////////////////////////
	// ��ʾ���
	ShowResultContent(resultfile);
	UpdateData(FALSE);
	MessageBox("OK!");
}


// �ص�����
static DWORD CALLBACK MyStreamingCallback(DWORD dwCookie,LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;
	ASSERT_KINDOF(CFile, pFile);

	*pcb = pFile->Read(pbBuff, cb);

	return 0;
	//	return !(*pcb > 0);
}

// ��ʾ����ļ�
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
