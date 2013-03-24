// DialogWebAnalysis.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "DialogWebAnalysis.h"
#include "afxdialogex.h"

#include "DialogUtf8Test.h"
#include "Search.h"

#include <boost/regex.hpp>
using namespace boost;
// CDialogWebAnalysis 对话框

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

	// TODO:  在此添加额外的初始化
	m_ListMemFile.DeleteAllItems();
	m_ListMemFile.SetExtendedStyle(LVS_LIST|LVS_EX_CHECKBOXES|LVS_EX_FLATSB|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE);

	m_ListMemFile.InsertColumn(0,"No.",LVCFMT_LEFT,30,0);
	m_ListMemFile.InsertColumn(1,"File",LVCFMT_LEFT,200,1);
	m_ListMemFile.InsertColumn(2,"Path",LVCFMT_LEFT,600,2);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
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


// CDialogWebAnalysis 消息处理程序


void CDialogWebAnalysis::OnBnClickedOpenRawFile()
{
	// TODO: 在此添加控件通知处理程序代码  "All Files(*.*)|*.*||"
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR,"Raw Files(*.raw)|*.raw||",AfxGetMainWnd());
	//OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|

	// 多选任意多个文件
	const int MIN_FILE_NUMBER = 10;        //至少允许选择10个文件
	fileDlg.m_ofn.lpstrFile = new TCHAR[_MAX_PATH * MIN_FILE_NUMBER]; //重新定义缓冲区大小          
	memset(fileDlg.m_ofn.lpstrFile, 0, _MAX_PATH * MIN_FILE_NUMBER);  //初始化定义的缓冲区
	fileDlg.m_ofn.nMaxFile = _MAX_PATH * MIN_FILE_NUMBER;


	if(fileDlg.DoModal()==IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		while (NULL != pos)
		{
			//TRACE(fileDlg.GetNextPathName(pos));                                //获取文件名
			//方法四：将选择的镜像文件信息保存在memoryImage中

			//单选

			//CString strFileName=fileDlg.GetFileName();
			//CString strFilePath=fileDlg.GetPathName();
			

			// 多选

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
	delete[] fileDlg.m_ofn.lpstrFile;     //最后别忘了释放内存
}

// 分析选中的文件——[准备工作]
void CDialogWebAnalysis::OnBnClickedAnalysis()
{
	// TODO: 在此添加控件通知处理程序代码
	
	// 先得到CListCtrl中的文件路径，在进行分析

	// 清空日志

	m_Log.ResetContent();
	UpdateData(FALSE);
	// 开始

	CString strObjectFile;
	CString strLog;
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// 获取选中的文件进行分析

		//if( m_ListMemFile.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED || m_ListMemFile.GetCheck(i))
		if(m_ListMemFile.GetCheck(i))
		{
			strObjectFile=m_ListMemFile.GetItemText(i,2);

			// 分析该文件		
			WebAnalysis(strObjectFile);
			//MessageBox("Next");
		}
	}
}

extern void SearchFile(const std::string& file,boost::regex expression,char** resultFile);
extern void SearchFile_tianya(const std::string& file,boost::regex expression,char** resultFile);
// 分析进程内存文件——[真正的分析]
VOID CDialogWebAnalysis::WebAnalysis(const CString& file)
{

	// 1、确定网站类型，选择网页模板

	// 等待后续升级
	
	//strDefultRegex=

	// 2、 根据模板进行恢复

		//2.1、开始恢复网页内容
	boost::match_flag_type flags=match_default;
	
	//UpdateData(FALSE);
	//////////////////////////////////////////////////////////////////////////
	// 设置regex正则表达式的选项

	CButton* pBtn;
	int nState;

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_ICASE));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("选中了ICASE");
		//flags=flags|regex::icase;
	}

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_NOT_NULL));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("选中了NULL");
		flags=flags|match_not_null;
	}

	pBtn=static_cast<CButton*>(GetDlgItem(IDC_CHECK_NOT_BOB));
	nState=pBtn->GetCheck();
	if (nState==BST_CHECKED)
	{
		::AfxMessageBox("选中了BOB");
		flags=flags|match_not_bob;
	}

	boost::regex expression(strDefultRegex.GetString(),flags);
	//////////////////////////////////////////////////////////////////////////
	// 测试单选按钮:选择sina、天涯

	UINT nSex=GetCheckedRadioButton(IDC_RADIO_SINA,IDC_RADIO_OTHER);//参数分别为第一个、最后一个单选框的ID号
	switch(nSex)
	{
	case IDC_RADIO_SINA:
		::AfxMessageBox("选中了Sina");
		SearchFile(file.GetString(),expression,ppLogFilePath);
		break;
	case IDC_RADIO_TIANYA:
		::AfxMessageBox("选中了天涯");
		SearchFile_tianya(file.GetString(),expression,ppLogFilePath);
		break;
	case IDC_RADIO_OTHER:
		::AfxMessageBox("选中第3个");break; 
	}


	
	// 2.2、更新日志结果

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


// 测试用

void CDialogWebAnalysis::OnBnClickedTest()
{
	// TODO: 在此添加控件通知处理程序代码
/*
	CString str;
	GetDlgItem(IDC_TEST)->GetWindowText(str);
*/

	GetDlgItem(IDC_TEST)->SetWindowText("测试中...");
	GetDlgItem(IDC_TEST)->EnableWindow(FALSE);

	UpdateData(TRUE);
	//MessageBox(m_TestRegex);
	//MessageBox(str);
	strTmpRegex=strDefultRegex;
	strDefultRegex=m_TestRegex;
	OnBnClickedAnalysis();

	

	//////////////////////////////////////////////////////////////////////////
	//复位
	strDefultRegex=strTmpRegex;
	GetDlgItem(IDC_TEST)->SetWindowText("测试");
	GetDlgItem(IDC_TEST)->EnableWindow(TRUE);
}

// 选中所有的进程内存文件

void CDialogWebAnalysis::OnBnClickedButtonSelectAll()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// 设置每一行的状态
		m_ListMemFile.SetCheck(i,1);
	}
}


// 取消选中的进程内存文件

void CDialogWebAnalysis::OnBnClickedButtonNotSelectAll()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<m_ListMemFile.GetItemCount(); i++)
	{
		// 设置每一行的状态
		m_ListMemFile.SetCheck(i,0);
	}
}

// 验证是否是 UTF-8编码的字符串

void CDialogWebAnalysis::OnBnClickedCodeTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogUtf8Test dlg;
	dlg.DoModal();
}


// 选择分析结果

void CDialogWebAnalysis::OnDblclkWebResultLog()
{
	// TODO: 在此添加控件通知处理程序代码

	// 获取文件路径
	int curIndex=m_Log.GetCurSel();
	CString strWeb;
	m_Log.GetText(curIndex,strWeb);

	// 显示
	// 要改动，让右边的显示随着左边的选择而变化？？？？？？？？

	CString strWebContentFile=strWeb;// 将相应的分析结果文件赋给该变量即可
	ShowWebContent(strWebContentFile);
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
VOID CDialogWebAnalysis::ShowWebContent(const CString& file)
{
	//CString fileName("C:\\log.txt");//
	
	//m_WebContent.Clear();
	//CStdioFile mfile; 
	//mfile.Open(file,CFile::modeRead,0);// 将fileName改成file即可

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
