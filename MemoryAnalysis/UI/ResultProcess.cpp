// ResultProcess.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "ResultProcess.h"
#include "afxdialogex.h"

#include <process.h>
#include "AfxTempl.h"



// CResultProcess �Ի���

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


// CResultProcess ��Ϣ�������


BOOL CResultProcess::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	// CListCtrl �б���ͼ�ؼ�������ʾ����
	m_ListCtrProcesses.SetExtendedStyle(LVS_LIST|LVS_EX_FLATSB|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE);

	m_ListCtrProcesses.InsertColumn(0,"Num",LVCFMT_LEFT,60,0);
	m_ListCtrProcesses.InsertColumn(1,"Pid",LVCFMT_LEFT,60,1);
	m_ListCtrProcesses.InsertColumn(2,"PPid",LVCFMT_LEFT,60,2);
	m_ListCtrProcesses.InsertColumn(3,"Process",LVCFMT_LEFT,100,3);
	m_ListCtrProcesses.InsertColumn(4,"CR3",LVCFMT_LEFT,100,4);

	//OnBnClickedShow();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CResultProcess::OnBnClickedShow()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//

	int mProCount;
	mProCount=mProInfoArray.GetCount();
	PROCESS tmpProcess;
	CString tmpPid;
	CString tmpPPid;
	CString tmpCR3;

	//��ʾCListCtrl

	m_ListCtrProcesses.DeleteAllItems();
	for (int nIndex=0;nIndex<mProCount;nIndex++)
	{

		CString strCount;//����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//�ȵõ�Ŀǰ���̵� Eprocess
	//Ȼ�������Ӧ�ĺ��������ڴ����ȡ
	PROCESS process;
	int curIndex=m_ListProcesses.GetCurSel();
	process=mProInfoArray.GetAt(curIndex);
	AfxMessageBox(process.strName);

	AfxMessageBox("Readay!");
	ExtractProcMemXP(process,imageFilePath);
}


void CResultProcess::OnBnClickedSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBox("����ɹ���");
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CResultProcess::ExtractProcMemXP(PROCESS pro,CString imgFilePath)
{
	//ExtractProcessMemoryPageTableXP(pro,imgFilePath);
	CString tempImgPath=imgFilePath;
	AfxMessageBox("Start");
	MemInfoExtraction::ExtractProcessMemoryPageTableXP(pro,tempImgPath);
	
	//mMemInfo.ExtractProcessMemoryVadTreeXP(pro,tempImgPath);
	AfxMessageBox("�����ڴ�ռ���ȡ�ɹ���");
}


void CResultProcess::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������

	// ��ȡ����

	//PROCESS process;
	//int curIndex=m_ListProcesses.GetCurSel();
	//process=mProInfoArray.GetAt(curIndex);

	// �Ҽ��˵�
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
// TODO: �ڴ���������������
PROCESS process;
int curIndex=m_ListProcesses.GetCurSel();
process=mProInfoArray.GetAt(curIndex);
AfxMessageBox(process.strName);

AfxMessageBox("Readay!");

// ��ʼ
AfxMessageBox("PageTable");
mMemInfo.ExtractProcessMemoryPageTableXP(process,imageFilePath);

AfxMessageBox("�����ڴ�ռ���ȡ�ɹ���PageTable����");

}


void CResultProcess::OnProceememoryVadtree()
{
// TODO: �ڴ���������������
PROCESS process;
int curIndex=m_ListProcesses.GetCurSel();
process=mProInfoArray.GetAt(curIndex);
AfxMessageBox(process.strName);

//AfxMessageBox("Readay!");

// ��ʼ
AfxMessageBox("VadTree");

mMemInfo.ExtractProcessMemoryVadTreeXP(process,imageFilePath);
AfxMessageBox("�����ڴ�ռ���ȡ�ɹ���VadTree����");

}
*/


// PageTable �߳�
UINT ThreadPageTableFunc(LPVOID pParam)
{
	/*
	AfxMessageBox("Start");
	CResultProcess* pDlg;
	pDlg=(CResultProcess*)pParam;

	pDlg->mMemInfo.ExtractProcessMemoryPageTableXP(pDlg->mSeletedPro,pDlg->imageFilePath);
	AfxMessageBox("�����ڴ�ռ���ȡ�ɹ���");
	*/
	return 0;
}

// Extract google chrome processes memory
// ͨ��pagetable�ķ�����ȡ�����йȸ�������Ľ���
//void CResultProcess::OnBnClickedChromeMemoryExtract()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
//	//�����������飬����chrome.exe����
//	for (int nIndex=0;nIndex<count;nIndex++)
//	{
//		pro=mProInfoArray.GetAt(nIndex);
//		if (pro.strName==strChrome)
//		{
//			// ��ʼ��ȡ
//			mMemInfo.ExtractProcessMemoryPageTableXP(pro,imageFilePath);
//
//			//���̡߳���ʹ���̳߳ؼ���
//			//mSeletedPro=pro;
//			//AfxBeginThread(ThreadPageTableFunc,this,THREAD_PRIORITY_ABOVE_NORMAL);
//
//			// ��־����
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
//		MessageBox("û�з���chrome���̣�");
//	}
//	else
//		MessageBox("���е�chrome������ȡ��ɣ�");
//
//	process=mProInfoArray.GetAt(curIndex);
//}

void CResultProcess::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	CString strProName=m_strProName;
	PROCESS pro;
	int count=mProInfoArray.GetCount();
	int num(0);

	//�����������飬����chrome.exe����
	for (int nIndex=0;nIndex<count;nIndex++)
	{
		pro=mProInfoArray.GetAt(nIndex);
		if (pro.strName==strProName)
		{
			// ��ʼ��ȡ
			//mMemInfo.ExtractProcessMemoryPageTableXP(pro,imageFilePath);
			DumpProcessMemory(pro,imageFilePath);
			//���̡߳���ʹ���̳߳ؼ���
			//mSeletedPro=pro;
			//AfxBeginThread(ThreadPageTableFunc,this,THREAD_PRIORITY_ABOVE_NORMAL);

			// ��־����
			CString strpid;
			strpid.Format("%d",pro.Pid);
			CString strlog=strpid+"_"+pro.strName;
			m_ListProcesses.InsertString(num++,strlog);
			UpdateData(FALSE);
		}

	}

	if (num==0)
	{
		MessageBox("û�з��ָý��̣�");
	}
	else
		MessageBox("���еĽ�����ȡ��ɣ�");
}


//////////////////////////////////////////////////////////////////////////
// ��ȡnotepad

//////////////////////////////////////////////////////////////////////////
// ���룺ҳĿ¼����ַ�������ַ�������ļ�·��
// ����������ַ
static DWORD XPVToP(DWORD dirTableBase,DWORD virtualAdd,CString filePath)  
{
	//////////////////////////////////////////////////////////////////////////
	// ���ж��Ƿ�Ϊ��ҳģʽ  ����PDE��ֵ���ж�

	DWORD OffsetNoLargePage = virtualAdd&0xfff;			// ���Ǵ�ҳģʽʱ��ҳ��ƫ����  ��ʮ��λ��0xfff==(00000000000000000000111111111111)2
	DWORD Table = ((virtualAdd >> 12)&0x3ff)<<2;	// �м�ʮλ��0x3ff==(1111111111)2��ҳ����ƫ����  ������λ���൱�ڳ���4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// ȡ��22λ

	DWORD Dir = (virtualAdd >> 22)<<2;				// ǰʮλ  ҳĿ¼����ƫ����  ������λ���൱�ڳ���4	
	DWORD result(0);
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // �洢�Ӿ�������ȡ�����ַ�
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);

	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	file.Seek((dirTableBase+Dir),CFile::begin);  // PDE��λ��ΪdirTableBase+Dir
	file.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPDE   ��ҳĿ¼��Ҫ��Ҫλ�ƣ�
	trueValue[0] = tmp[3];		
	trueValue[1] = tmp[2];
	trueValue[2] = tmp[1];
	trueValue[3] = tmp[0];
	tmpAddress = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	PDE = *tmpAddress;			// ҳĿ¼���ֵ


	//////////////////////////////////////////////////////////////////////////
	// ��ʱ�ж������ַת���Ƿ��ô�ҳģʽ
	isLargePage = (PDE&0x80)>>7;		// �õ�8λ(��0λ��ʼ����������)�Ƿ�Ϊ0�ж��Ƿ��Ǵ�ҳģʽ�����Ǵ�ҳģʽ��ֵΪ1��TRUE�����Ǵ�ҳģʽֵΪ0��FALSE

	if (isLargePage == TRUE)
	{
		PA = ((PDE>>22)<<22) + OffsetLargePage;	// ȡPDE��ǰ10λ����22λ����  PAΪ��ֵ����λ����OffsetLargePage
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
		PDE = ((*tmpAddress)>>12)<<12;	// ҳ��СΪ4KBʱҳĿ¼���12λ����
		file.Seek((((PDE>>12)<<12)+Table),CFile::begin);	// PDE��12λ���㣬PTE��λ��Ϊ (PDE��12λ�����)+Table
		file.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPTE
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	// ҳ�����12λ����

		PA = ((PTE>>12)<<12) + OffsetNoLargePage;	// ȡPTE��ǰ20λ����12λ����  PAΪ��ֵ����λ����OffsetNoLargePage
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
// ��ȡ�����ڴ�
// pNum:�������  filePath:�����ļ�·��
// ����LDR�������vad������
//void MemInfoExtraction::XPMemoryExtract(int processNum,CString filePath)
static void XPMemoryExtract(PROCESS pro,CString filePath)
{
	// 1��notepad ����

	// �����ļ�
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
		// 2��ҳĿ¼����ַdirTableBase
		DWORD mDtb=(pro.DirectoryTableBase>>12)<<12;
		DWORD PhAdd=XPVToP(mDtb,virtualAdd,filePath);
		Tempfile.Seek(PhAdd,CFile::begin);
		Tempfile.Read(tmpPageDataBuf,PAGE_SIZE);

		// д���ļ�
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
		// ƥ���ַ���
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
							file1.Write(buf,2);// д��־

							// д�ı�
							Tempfile.Seek(8*s+24,CFile::begin);//�ҵ��ı�λ��
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

// �ص�����
void CResultProcess::OnClickedNotepad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strProName("notepad.exe");
	CString strProName1("NOTEPAD.EXE");
	PROCESS pro;
	int count=mProInfoArray.GetCount();

	for (int nIndex=0;nIndex<count;nIndex++)
	{
		pro=mProInfoArray.GetAt(nIndex);
		if (pro.strName==strProName||pro.strName==strProName1)
		{
			// ��ȡnotepad
			XPMemoryExtract(pro,imageFilePath);
		}
	}
}
