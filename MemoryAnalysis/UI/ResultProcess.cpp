// ResultProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "ResultProcess.h"
#include "afxdialogex.h"

#include <process.h>
#include "AfxTempl.h"



// CResultProcess 对话框

IMPLEMENT_DYNAMIC(CResultProcess, CDialog)

	CResultProcess::CResultProcess(CWnd* pParent /*=NULL*/)
	: CDialog(CResultProcess::IDD, pParent)
	, imageFilePath(_T(""))
{
	//pProInfoArray->RemoveAll();
	pProInfoArray=NULL;
	m_strProName = _T("");
}


CResultProcess::~CResultProcess()
{
	if (NULL!=pProInfoArray)
	{
		delete pProInfoArray;
		pProInfoArray=NULL;
	}
	

	//delete pProCtrl;
}

void CResultProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PROCESSES, m_ListProcesses);
	DDX_Control(pDX, IDC_LIST_CTRL_PROCESS, m_ListCtrProcesses);
	DDX_Text(pDX, IDC_EDIT_PROCESS_NAME, m_strProName);
}


BEGIN_MESSAGE_MAP(CResultProcess, CDialog)
	ON_BN_CLICKED(IDC_SHOW, &CResultProcess::OnBnClickedShow)
	ON_LBN_DBLCLK(IDC_LIST_PROCESSES, &CResultProcess::OnLbnDblclkListProcesses)
	ON_BN_CLICKED(IDC_SAVE, &CResultProcess::OnBnClickedSave)
	ON_WM_CONTEXTMENU()
	//	ON_BN_CLICKED(IDC_CHROME_MEMORY_EXTRACT, &CResultProcess::OnBnClickedChromeMemoryExtract)
	ON_BN_CLICKED(IDC_BUTTON4, &CResultProcess::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_NOTEPAD, &CResultProcess::OnClickedNotepad)
END_MESSAGE_MAP()


// CResultProcess 消息处理程序


BOOL CResultProcess::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// CListCtrl 列表视图控件——显示进程
	m_ListCtrProcesses.SetExtendedStyle(LVS_LIST|LVS_EX_FLATSB|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE);

	m_ListCtrProcesses.InsertColumn(0,"Num",LVCFMT_LEFT,60,0);
	m_ListCtrProcesses.InsertColumn(1,"Pid",LVCFMT_LEFT,60,1);
	m_ListCtrProcesses.InsertColumn(2,"PPid",LVCFMT_LEFT,60,2);
	m_ListCtrProcesses.InsertColumn(3,"Process",LVCFMT_LEFT,100,3);
	m_ListCtrProcesses.InsertColumn(4,"CR3",LVCFMT_LEFT,100,4);

	//OnBnClickedShow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CResultProcess::OnBnClickedShow()
{
	// TODO: 在此添加控件通知处理程序代码
	//

	int mProCount;
	mProCount=mProInfoArray.GetCount();
	PROCESS tmpProcess;
	CString tmpPid;
	CString tmpPPid;
	CString tmpCR3;

	//显示CListCtrl

	m_ListCtrProcesses.DeleteAllItems();
	for (int nIndex=0;nIndex<mProCount;nIndex++)
	{

		CString strCount;//计数
		strCount.Format("%d",nIndex+1);

		tmpProcess=mProInfoArray.GetAt(nIndex);
		tmpPid.Format("%d",tmpProcess.Pid);
		tmpPPid.Format("%d",tmpProcess.PPid);
		tmpCR3.Format("%d",tmpProcess.DirectoryTableBase);
		m_ListCtrProcesses.InsertItem(nIndex,strCount);
		m_ListCtrProcesses.SetItemText(nIndex,0,strCount);
		m_ListCtrProcesses.SetItemText(nIndex,1,tmpPid);
		m_ListCtrProcesses.SetItemText(nIndex,2,tmpPPid);
		m_ListCtrProcesses.SetItemText(nIndex,3,tmpProcess.strName);
		m_ListCtrProcesses.SetItemText(nIndex,4,tmpCR3);
		UpdateData(FALSE);
	}
	GetDlgItem(IDC_SHOW)->EnableWindow(FALSE);
}

void CResultProcess::SetProcessInfo(PROCESS process)
{
	mProInfoArray.Add(process);

}

void CResultProcess::SetProcessInfo(CArray<PROCESS,PROCESS>* pProcessArray)
{
	//pProInfoArray=pProcessArray;
}

void CResultProcess::TransferPara(CString imgFilePath,MemInfoExtraction memInfo)
{
	imageFilePath=imgFilePath;
	//mMemInfo=memInfo;//
}

void CResultProcess::OnLbnDblclkListProcesses()
{
	// TODO: 在此添加控件通知处理程序代码

	//先得到目前进程的 Eprocess
	//然后调用相应的函数进行内存的提取
	PROCESS process;
	int curIndex=m_ListProcesses.GetCurSel();
	process=mProInfoArray.GetAt(curIndex);
	AfxMessageBox(process.strName);

	AfxMessageBox("Readay!");
	ExtractProcMemXP(process,imageFilePath);
}


void CResultProcess::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("保存成功！");
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CResultProcess::ExtractProcMemXP(PROCESS pro,CString imgFilePath)
{
	//ExtractProcessMemoryPageTableXP(pro,imgFilePath);
	CString tempImgPath=imgFilePath;
	AfxMessageBox("Start");
	MemInfoExtraction::ExtractProcessMemoryPageTableXP(pro,tempImgPath);
	
	//mMemInfo.ExtractProcessMemoryVadTreeXP(pro,tempImgPath);
	AfxMessageBox("进程内存空间提取成功！");
}


void CResultProcess::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码

	// 获取进程

	//PROCESS process;
	//int curIndex=m_ListProcesses.GetCurSel();
	//process=mProInfoArray.GetAt(curIndex);

	// 右键菜单
	CMenu TheMenu;   
	TheMenu.LoadMenu(IDR_MENU_RIGHT); 
	CMenu * pMenu = TheMenu.GetSubMenu(0); 
	CPoint pos;
	GetCursorPos(&pos);
	int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD,pos.x,pos.y,this);  
	switch(nRet)
	{ 
	case ID_VADTREE:
		MessageBox("vadtree");
		break;
	case ID_PAGE_TABLE:
		MessageBox("page-table");
		break;
	default:
		break;
	} 
}

/*
void CResultProcess::OnProceememoryPagetable()
{
// TODO: 在此添加命令处理程序代码
PROCESS process;
int curIndex=m_ListProcesses.GetCurSel();
process=mProInfoArray.GetAt(curIndex);
AfxMessageBox(process.strName);

AfxMessageBox("Readay!");

// 开始
AfxMessageBox("PageTable");
mMemInfo.ExtractProcessMemoryPageTableXP(process,imageFilePath);

AfxMessageBox("进程内存空间提取成功（PageTable）！");

}


void CResultProcess::OnProceememoryVadtree()
{
// TODO: 在此添加命令处理程序代码
PROCESS process;
int curIndex=m_ListProcesses.GetCurSel();
process=mProInfoArray.GetAt(curIndex);
AfxMessageBox(process.strName);

//AfxMessageBox("Readay!");

// 开始
AfxMessageBox("VadTree");

mMemInfo.ExtractProcessMemoryVadTreeXP(process,imageFilePath);
AfxMessageBox("进程内存空间提取成功（VadTree）！");

}
*/


// PageTable 线程
UINT ThreadPageTableFunc(LPVOID pParam)
{
	/*
	AfxMessageBox("Start");
	CResultProcess* pDlg;
	pDlg=(CResultProcess*)pParam;

	pDlg->mMemInfo.ExtractProcessMemoryPageTableXP(pDlg->mSeletedPro,pDlg->imageFilePath);
	AfxMessageBox("进程内存空间提取成功！");
	*/
	return 0;
}

// Extract google chrome processes memory
// 通过pagetable的方法提取出所有谷歌浏览器的进程
//void CResultProcess::OnBnClickedChromeMemoryExtract()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString strChrome("chrome.exe");
//	PROCESS pro;
//	int count=mProInfoArray.GetCount();
//	int num(0);
//
//
//	//CString strlog0("start extracting chrome memory......");
//	//m_ListProcesses.InsertString(num++,strlog0);
//	//UpdateData(FALSE);
//
//	//遍历进程数组，查找chrome.exe进程
//	for (int nIndex=0;nIndex<count;nIndex++)
//	{
//		pro=mProInfoArray.GetAt(nIndex);
//		if (pro.strName==strChrome)
//		{
//			// 开始提取
//			mMemInfo.ExtractProcessMemoryPageTableXP(pro,imageFilePath);
//
//			//多线程——使用线程池技术
//			//mSeletedPro=pro;
//			//AfxBeginThread(ThreadPageTableFunc,this,THREAD_PRIORITY_ABOVE_NORMAL);
//
//			// 日志更新
//			CString strpid;
//			strpid.Format("%d",pro.Pid);
//			CString strlog=strpid+"_"+pro.strName;
//			m_ListProcesses.InsertString(num++,strlog);
//			UpdateData(FALSE);
//		}
//
//	}
//
//	if (num==0)
//	{
//		MessageBox("没有发现chrome进程！");
//	}
//	else
//		MessageBox("所有的chrome进程提取完成！");
//
//	process=mProInfoArray.GetAt(curIndex);
//}

void CResultProcess::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString strProName=m_strProName;
	PROCESS pro;
	int count=mProInfoArray.GetCount();
	int num(0);

	//遍历进程数组，查找chrome.exe进程
	for (int nIndex=0;nIndex<count;nIndex++)
	{
		pro=mProInfoArray.GetAt(nIndex);
		if (pro.strName==strProName)
		{
			// 开始提取
			//mMemInfo.ExtractProcessMemoryPageTableXP(pro,imageFilePath);
			DumpProcessMemory(pro,imageFilePath);
			//多线程——使用线程池技术
			//mSeletedPro=pro;
			//AfxBeginThread(ThreadPageTableFunc,this,THREAD_PRIORITY_ABOVE_NORMAL);

			// 日志更新
			CString strpid;
			strpid.Format("%d",pro.Pid);
			CString strlog=strpid+"_"+pro.strName;
			m_ListProcesses.InsertString(num++,strlog);
			UpdateData(FALSE);
		}

	}

	if (num==0)
	{
		MessageBox("没有发现该进程！");
	}
	else
		MessageBox("所有的进程提取完成！");
}


//////////////////////////////////////////////////////////////////////////
// 提取notepad

//////////////////////////////////////////////////////////////////////////
// 输入：页目录基地址，虚拟地址，镜像文件路径
// 输出：物理地址
static DWORD XPVToP(DWORD dirTableBase,DWORD virtualAdd,CString filePath)  
{
	//////////////////////////////////////////////////////////////////////////
	// 先判断是否为大页模式  根据PDE的值来判断

	DWORD OffsetNoLargePage = virtualAdd&0xfff;			// 不是大页模式时的页内偏移量  后十二位，0xfff==(00000000000000000000111111111111)2
	DWORD Table = ((virtualAdd >> 12)&0x3ff)<<2;	// 中间十位，0x3ff==(1111111111)2，页表内偏移量  左移两位，相当于乘以4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// 取后22位

	DWORD Dir = (virtualAdd >> 22)<<2;				// 前十位  页目录表内偏移量  左移两位，相当于乘以4	
	DWORD result(0);
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // 存储从镜像中提取出的字符
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);

	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	file.Seek((dirTableBase+Dir),CFile::begin);  // PDE的位置为dirTableBase+Dir
	file.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PDE   ？页目录项要不要位移？
	trueValue[0] = tmp[3];		
	trueValue[1] = tmp[2];
	trueValue[2] = tmp[1];
	trueValue[3] = tmp[0];
	tmpAddress = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	PDE = *tmpAddress;			// 页目录项的值


	//////////////////////////////////////////////////////////////////////////
	// 此时判断这个地址转换是否用大页模式
	isLargePage = (PDE&0x80)>>7;		// 用第8位(从0位开始，从右至左)是否为0判断是否是大页模式，若是大页模式则值为1即TRUE，不是大页模式值为0即FALSE

	if (isLargePage == TRUE)
	{
		PA = ((PDE>>22)<<22) + OffsetLargePage;	// 取PDE的前10位，后22位置零  PA为此值加上位移量OffsetLargePage
		if (PDE==0)
		{
			result = 0;
		} 
		else
		{
			result = PA;
		}
	}
	else
	{
		PDE = ((*tmpAddress)>>12)<<12;	// 页大小为4KB时页目录项，后12位置零
		file.Seek((((PDE>>12)<<12)+Table),CFile::begin);	// PDE后12位置零，PTE的位置为 (PDE后12位置零后)+Table
		file.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PTE
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	// 页表项，后12位置零

		PA = ((PTE>>12)<<12) + OffsetNoLargePage;	// 取PTE的前20位，后12位置零  PA为此值加上位移量OffsetNoLargePage
		if ((PDE==0)||(PTE==0))
		{
			result = 0;
		} 
		else
		{
			result = PA;
		}
	}
	file.Close();
	return result;
}


//////////////////////////////////////////////////////////////////////////
// 获取进程内存
// pNum:进程序号  filePath:镜像文件路径
// 采用LDR中序遍历vad二叉树
//void MemInfoExtraction::XPMemoryExtract(int processNum,CString filePath)
static void XPMemoryExtract(PROCESS pro,CString filePath)
{
	// 1、notepad 进程

	// 导出文件
	CString strNum;
	strNum.Format("%d",pro.Pid);
	CString filePath2 =  ".\\Mem"+strNum+pro.strName+".txt";
	CString filePath1 = ".\\Mem"+strNum+pro.strName+".raw";

	CFile Tempfile;
	CFile	file;
	CFile  file1;
	Tempfile.Open(filePath,CFile::modeRead|CFile::shareDenyNone);
	file.Open(filePath1,CFile::modeReadWrite|CFile::modeCreate);
	file1.Open(filePath2,CFile::modeReadWrite|CFile::modeCreate);


	DWORD virtualAdd;
	DWORD virtualAddl;

	char*buffer=new char[4];
	char*buffer1=new char[4];

	Tempfile.Close();
	delete buffer1;
	virtualAdd=0x000af000;
	virtualAddl=0x000e7000;

	Tempfile.Open(filePath,CFile::modeRead|CFile::shareDenyNone);
	char *tmpPageDataBuf=new char[PAGE_SIZE];
	while(virtualAdd<=virtualAddl)
	{
		// 2、页目录基地址dirTableBase
		DWORD mDtb=(pro.DirectoryTableBase>>12)<<12;
		DWORD PhAdd=XPVToP(mDtb,virtualAdd,filePath);
		Tempfile.Seek(PhAdd,CFile::begin);
		Tempfile.Read(tmpPageDataBuf,PAGE_SIZE);

		// 写入文件
		file.Write(tmpPageDataBuf,PAGE_SIZE);
		file.Seek(0,CFile::end);
		virtualAdd=virtualAdd+0x00001000;

	}
	Tempfile.Close();
	file.Close();

	char*buffer3=new char[8];
	int s=0;
	Tempfile.Open(filePath1,CFile::modeRead|CFile::shareDenyNone);

	////////////////////////////////////////////////////////////////////////////////////////
	char pattern1[] = {0x01,0x00,0x04,0x00,0x2e,0x00,0x74,0x00};
	char pattern2[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

	for(s=0;s<=4*1024*12;s++)
	{
		Tempfile.Read(buffer3,8);

		///////////////////////////////////////////////////////////
		// 匹配字符串
		if(pattern1[0]==buffer3[0]&&pattern1[1]==buffer3[1]&&pattern1[2]==buffer3[2]&&pattern1[3]==buffer3[3]&&pattern1[4]==buffer3[4])
			if(pattern1[5]==buffer3[5]&&pattern1[6]==buffer3[6]&&pattern1[7]==buffer3[7])
			{ 
				for(int m=0;m<=32*1024;m++)
				{
					Tempfile.Read(buffer3,8);
					if(pattern2[0]==buffer3[0]&&pattern2[1]==buffer3[1]&&pattern2[2]==buffer3[2]&&pattern2[3]==buffer3[3]&&pattern2[4]==buffer3[4])
						if(pattern2[5]==buffer3[5]&&pattern2[6]==buffer3[6]&&pattern2[7]==buffer3[7])
						{
							char buf[2]={0xff,0xfe};
							file1.Write(buf,2);// 写标志

							// 写文本
							Tempfile.Seek(8*s+24,CFile::begin);//找到文本位置
							char*buffer5=new char[8*m];
							//char buffer4[100];
							Tempfile.Read(buffer5,8*m);

							file1.Write(buffer5,8*m);

							Tempfile.Close();
							file1.Close();
							delete buffer3;
							delete buffer5;
							break;
						}
				}
				break;
			}
	}
}

// 回调函数
void CResultProcess::OnClickedNotepad()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strProName("notepad.exe");
	CString strProName1("NOTEPAD.EXE");
	PROCESS pro;
	int count=mProInfoArray.GetCount();

	for (int nIndex=0;nIndex<count;nIndex++)
	{
		pro=mProInfoArray.GetAt(nIndex);
		if (pro.strName==strProName||pro.strName==strProName1)
		{
			// 提取notepad
			XPMemoryExtract(pro,imageFilePath);
		}
	}
}
