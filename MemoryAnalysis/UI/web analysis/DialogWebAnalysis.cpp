// DialogWebAnalysis.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "DialogWebAnalysis.h"
#include "afxdialogex.h"

#include "DialogUtf8Test.h"
#include "Search.h"

#include <boost/regex.hpp>
using namespace boost;
// CDialogWebAnalysis �Ի���

IMPLEMENT_DYNAMIC(CDialogWebAnalysis, CDialog)

CDialogWebAnalysis::CDialogWebAnalysis(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogWebAnalysis::IDD, pParent)
{

	//  m_WebResult = _T("");
	strTmpMemFile = _T("");
	mMemFileCount=0;

	ppLogFilePath=new char*[200];
	for (int i=0;i<200;i++)
	{
		ppLogFilePath[i]=new char[100];
		ppLogFilePath[i][0]='\0';
	}
	m_TestRegex = _T("");

	//strDefultRegex="<html.*";
	strDefultRegex="<TITLE[^>]*>(.*?)</TITLE>";
}

CDialogWebAnalysis::~CDialogWebAnalysis()
{
	for (int i=0;i<200;i++)
	{
		delete[] ppLogFilePath[i];
	}
}

BOOL CDialogWebAnalysis::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ListMemFile.DeleteAllItems();
	m_ListMemFile.SetExtendedStyle(LVS_LIST|LVS_EX_CHECKBOXES|LVS_EX_FLATSB|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE);

	m_ListMemFile.InsertColumn(0,"No.",LVCFMT_LEFT,30,0);
	m_ListMemFile.InsertColumn(1,"File",LVCFMT_LEFT,200,1);
	m_ListMemFile.InsertColumn(2,"Path",LVCFMT_LEFT,600,2);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDialogWebAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_LBString(pDX, IDC_WEB_RESULT, m_WebResult);
	DDX_Control(pDX, IDC_LIST_MEMORY_FILE, m_ListMemFile);
	DDX_Control(pDX, IDC_WEB_RESULT, m_Log);
	//	DDX_Control(pDX, IDC_EDIT_WEBCONTENT, m_WebContent);
	DDX_Control(pDX, IDC_RICHEDIT_WEBCONTENT, m_WebContent);
	DDX_Text(pDX, IDC_EDIT_TEST_REGEX, m_TestRegex);
}

BEGIN_MESSAGE_MAP(CDialogWebAnalysis, CDialog)
	ON_BN_CLICKED(IDC_OPEN_RAW_FILE, &CDialogWebAnalysis::OnBnClickedOpenRawFile)
	ON_BN_CLICKED(IDC_ANALYSIS, &CDialogWebAnalysis::OnBnClickedAnalysis)
//	ON_LBN_SETFOCUS(IDC_WEB_RESULT, &CDialogWebAnalysis::OnSetfocusWebResultLog)
	ON_LBN_DBLCLK(IDC_WEB_RESULT, &CDialogWebAnalysis::OnDblclkWebResultLog)
	ON_BN_CLICKED(IDC_TEST, &CDialogWebAnalysis::OnBnClickedTest)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, &CDialogWebAnalysis::OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_NOT_SELECT_ALL, &CDialogWebAnalysis::OnBnClickedButtonNotSelectAll)
	ON_BN_CLICKED(IDC_CODE_TEST, &CDialogWebAnalysis::OnBnClickedCodeTest)
END_MESSAGE_MAP()


// CDialogWebAnalysis ��Ϣ�������


void CDialogWebAnalysis::OnBnClickedOpenRawFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������  "All Files(*.*)|*.*||"
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR,"Raw Files(*.raw)|*.raw||",AfxGetMainWnd());
	//OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|

	// ��ѡ�������ļ�
	const int MIN_FILE_NUMBER = 10;        //��������ѡ��10���ļ�
	fileDlg.m_ofn.lpstrFile = new TCHAR[_MAX_PATH * MIN_FILE_NUMBER]; //���¶��建������С          
	memset(fileDlg.m_ofn.lpstrFile, 0, _MAX_PATH * MIN_FILE_NUMBER);  //��ʼ������Ļ�����
	fileDlg.m_ofn.nMaxFile = _MAX_PATH * MIN_FILE_NUMBER;


	if(fileDlg.DoModal()==IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		while (NULL != pos)
		{
			//TRACE(fileDlg.GetNextPathName(pos));                                //��ȡ�ļ���
			//�����ģ���ѡ��ľ����ļ���Ϣ������memoryImage��

			//��ѡ

			//CString strFileName=fileDlg.GetFileName();
			//CString strFilePath=fileDlg.GetPathName();
			

			// ��ѡ

			CString strFilePath=fileDlg.GetNextPathName(pos);
		
			CString strFileName;
			strFileName=strFilePath.Mid(strFilePath.ReverseFind('\\')+1); 
			strFileName=strFileName.Left(strFileName.GetLength()-4); 

			CString strCount;
			strCount.Format("%d",mMemFileCount+1);
			m_ListMemFile.InsertItem(mMemFileCount,strCount);
			m_ListMemFile.SetItemText(mMemFileCount,0,strCount);
			m_ListMemFile.SetItemText(mMemFileCount,1,strFileName);
			m_ListMemFile.SetItemText(mMemFileCount,2,strFilePath);
			UpdateData(FALSE);
			mMemFileCount++;
		
		}
	}
	delete[] fileDlg.m_ofn.lpstrFile;     //���������ͷ��ڴ�
}

// ����ѡ�е��ļ�����[׼������]
void CDialogWebAnalysis::OnBnClickedAnalysis()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	// �ȵõ�CListCtrl�е��ļ�·�����ڽ��з���

	// �����־

	m_Log.ResetContent();
	UpdateData(FALSE);
	// ��ʼ

	CString strObjectFile;
	CString strLog;
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// ��ȡѡ�е��ļ����з���

		//if( m_ListMemFile.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED || m_ListMemFile.GetCheck(i))
		if(m_ListMemFile.GetCheck(i))
		{
			strObjectFile=m_ListMemFile.GetItemText(i,2);

			// �������ļ�		
			WebAnalysis(strObjectFile);
			//MessageBox("Next");
		}
	}
}

extern void SearchFile(const std::string& file,boost::regex expression,char** resultFile);
extern void SearchFile_tianya(const std::string& file,boost::regex expression,char** resultFile);
// ���������ڴ��ļ�����[�����ķ���]
VOID CDialogWebAnalysis::WebAnalysis(const CString& file)
{

	// 1��ȷ����վ���ͣ�ѡ����ҳģ��

	// �ȴ���������
	
	//strDefultRegex=

	// 2�� ����ģ����лָ�

		//2.1����ʼ�ָ���ҳ����
	boost::match_flag_type flags=match_default;
	
	//UpdateData(FALSE);
	//////////////////////////////////////////////////////////////////////////
	// ����regex������ʽ��ѡ��

	CButton* pBtn;
	int nState;

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_ICASE));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("ѡ����ICASE");
		//flags=flags|regex::icase;
	}

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_NOT_NULL));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("ѡ����NULL");
		flags=flags|match_not_null;
	}

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_NOT_BOB));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("ѡ����BOB");
		flags=flags|match_not_bob;
	}

	boost::regex expression(strDefultRegex.GetString(),flags);
	//////////////////////////////////////////////////////////////////////////
	// ���Ե�ѡ��ť:ѡ��sina������

	UINT nSex=GetCheckedRadioButton(IDC_RADIO_SINA,IDC_RADIO_OTHER);//�����ֱ�Ϊ��һ�������һ����ѡ���ID��
	switch(nSex)
	{
	case IDC_RADIO_SINA:
		::AfxMessageBox("ѡ����Sina");
		SearchFile(file.GetString(),expression,ppLogFilePath);
		break;
	case IDC_RADIO_TIANYA:
		::AfxMessageBox("ѡ��������");
		SearchFile_tianya(file.GetString(),expression,ppLogFilePath);
		break;
	case IDC_RADIO_OTHER:
		::AfxMessageBox("ѡ�е�3��");break; 
	}


	
	// 2.2��������־���

	for (int i=0;i<200;i++)
	{
		if (ppLogFilePath[i][0]!='\0')
		{
			CString strTmpLog(ppLogFilePath[i]);
			m_Log.AddString(strTmpLog);
			UpdateData(FALSE);
		}
	}
}


// ������

void CDialogWebAnalysis::OnBnClickedTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
/*
	CString str;
	GetDlgItem(IDC_TEST)->GetWindowText(str);
*/

	GetDlgItem(IDC_TEST)->SetWindowText("������...");
	GetDlgItem(IDC_TEST)->EnableWindow(FALSE);

	UpdateData(TRUE);
	//MessageBox(m_TestRegex);
	//MessageBox(str);
	strTmpRegex=strDefultRegex;
	strDefultRegex=m_TestRegex;
	OnBnClickedAnalysis();

	

	//////////////////////////////////////////////////////////////////////////
	//��λ
	strDefultRegex=strTmpRegex;
	GetDlgItem(IDC_TEST)->SetWindowText("����");
	GetDlgItem(IDC_TEST)->EnableWindow(TRUE);
}

// ѡ�����еĽ����ڴ��ļ�

void CDialogWebAnalysis::OnBnClickedButtonSelectAll()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// ����ÿһ�е�״̬
		m_ListMemFile.SetCheck(i,1);
	}
}


// ȡ��ѡ�еĽ����ڴ��ļ�

void CDialogWebAnalysis::OnBnClickedButtonNotSelectAll()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// ����ÿһ�е�״̬
		m_ListMemFile.SetCheck(i,0);
	}
}

// ��֤�Ƿ��� UTF-8������ַ���

void CDialogWebAnalysis::OnBnClickedCodeTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogUtf8Test dlg;
	dlg.DoModal();
}


// ѡ��������

void CDialogWebAnalysis::OnDblclkWebResultLog()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// ��ȡ�ļ�·��
	int curIndex=m_Log.GetCurSel();
	CString strWeb;
	m_Log.GetText(curIndex,strWeb);

	// ��ʾ
	// Ҫ�Ķ������ұߵ���ʾ������ߵ�ѡ����仯����������������

	CString strWebContentFile=strWeb;// ����Ӧ�ķ�������ļ������ñ�������
	ShowWebContent(strWebContentFile);
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
VOID CDialogWebAnalysis::ShowWebContent(const CString& file)
{
	//CString fileName("C:\\log.txt");//
	
	//m_WebContent.Clear();
	//CStdioFile mfile; 
	//mfile.Open(file,CFile::modeRead,0);// ��fileName�ĳ�file����

	//CString strBuf,strText;

	//while(mfile.GetPosition()!=mfile.GetLength())
	//{
	//	mfile.ReadString(strBuf);
	//	strText=strText+strBuf+(char)10;	
	//}
	//mfile.Close(); 
	//m_WebContent.SetWindowText(strText); 
	//UpdateData(TRUE);
	//////////////////////////////////////////////////////////////////////////

		CFile* pInputFile = NULL;

		try
		{
			/*pInputFile = new CFile(str, CFile::modeRead | CFile::shareDenyNone);*/

			pInputFile = new CFile(file, CFile::modeRead | CFile::shareDenyNone);
			EDITSTREAM strm;
			strm.dwCookie = (DWORD) pInputFile;
			strm.pfnCallback = MyStreamingCallback;

			long lResult = m_WebContent.StreamIn(SF_TEXT, strm);

			pInputFile->Close();
		}
		catch (CFileException* pEx)
		{
			pEx->Delete();
		}

		delete pInputFile;
}
