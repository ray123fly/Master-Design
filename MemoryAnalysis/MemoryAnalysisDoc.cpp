// MemoryAnalysisDoc.cpp : CMemoryAnalysisDoc 类的实现
//

#include "stdafx.h"
#include "MemoryAnalysis.h"

#include "MemoryAnalysisDoc.h"
#include "MemoryAnalysisView.h"
#include "DialogWebAnalysis.h"
#include "SearchKeywords.h"

#include "CodeTranster.h"
#include "CharacterSetCode.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <afx.h>


//#include "ProgressDlg.h"

//#include "MemInfoExtraction.h"
//#include "MemoryManagement.h"




//#include "Process.h"




using namespace std;

const DWORD KPCR_LENGTH_XP = 0xd70;   //kpcr大小  ？？
const DWORD KSIZE = 1024;
const DWORD EPROCESS_LENGTH_XP = 0x25c;  //XP下的eprocess大小
const DWORD EPROCESS_LENGTH_WIN7 = 0x25c;  //WIN7下的eprocess大小



MemInfoExtraction mMemInfo;

const DWORD bufSize = 12*1024*1024;  // 12M 临时数组大小



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMemoryAnalysisDoc

IMPLEMENT_DYNCREATE(CMemoryAnalysisDoc, CDocument)

	BEGIN_MESSAGE_MAP(CMemoryAnalysisDoc, CDocument)
//		ON_COMMAND(ID_32771, &CMemoryAnalysisDoc::OnKpcrExtractionXP)
		ON_COMMAND(ID_32772, &CMemoryAnalysisDoc::OnEprocessExtractionXP)
		ON_COMMAND(ID_32773, &CMemoryAnalysisDoc::OnEprocessExtractionWin7)
//		ON_COMMAND(ID_32774, &CMemoryAnalysisDoc::OnKpcrExtractionWin7)
//		ON_COMMAND(ID_32775, &CMemoryAnalysisDoc::OnProcessMemoryExtractionXP)
//		ON_COMMAND(IDM_WEB_WEBANALYSIS_VIEW, &CMemoryAnalysisDoc::OnWebWebanalysisView)
//		ON_COMMAND(IDM_WEB_ANALYSIS, &CMemoryAnalysisDoc::OnWebAnalysis)
		ON_COMMAND(IDM_WEB_SEARCH_KEYWORD, &CMemoryAnalysisDoc::OnWebSearchKeyword)
		ON_COMMAND(IDM_WEB_ANALYSIS, &CMemoryAnalysisDoc::OnWebAnalysis)
		ON_COMMAND(IDM_NOTEPAD_RECOVERY, &CMemoryAnalysisDoc::OnNotepadRecovery)
	END_MESSAGE_MAP()



	// CMemoryAnalysisDoc 构造/析构

	CMemoryAnalysisDoc::CMemoryAnalysisDoc()
	{
		// TODO: 在此添加一次性构造代码
		pcBuf = NULL;
		dwBufSize = KSIZE*KSIZE;  //*736分配的空间最大为736M  737M及以上会出错    ？？
		pcBuf = (char*)malloc(dwBufSize);	
		pTmpBuf = (char*)malloc(bufSize);	// 缓冲区大小为4M


		pProcArray=new CArray<PROCESS,PROCESS>;
		pProcArray->RemoveAll();

		pProgressDlg=new CProgressDlg;
		bForNotrpad=false;
	}

	CMemoryAnalysisDoc::~CMemoryAnalysisDoc()
	{
		//delete[] pcBuf;
		delete pProcArray;

		if (pProgressDlg!=NULL)
		{
			delete pProgressDlg;
			pProgressDlg=NULL;
		}
	}

	BOOL CMemoryAnalysisDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: 在此添加重新初始化代码

		// (SDI 文档将重用该文档)
		//sprintf(pcBuf, "大家好呀有什么问题？aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		//
		((CMemoryAnalysisView *)m_viewList.GetHead())->SetText(pcBuf, strlen(pcBuf));//这个地方是什么意思啊？？？？

		//////////////////////////////////////////////////////////////////////////
		// open a new image
		/*
		CFile fileOpen()
		OnOpenDocument()

		*/
		return TRUE;
	}




	// CMemoryAnalysisDoc 序列化

	void CMemoryAnalysisDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: 在此添加存储代码
		}
		else
		{
			// TODO: 在此添加加载代码
		}
	}


	// CMemoryAnalysisDoc 诊断

#ifdef _DEBUG
	void CMemoryAnalysisDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CMemoryAnalysisDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif //_DEBUG


	// CMemoryAnalysisDoc 命令

	BOOL CMemoryAnalysisDoc::OnOpenDocument(LPCTSTR lpszPathName)
	{
		if (!CDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		// TODO:  在此添加您专用的创建代码

		mMemInfo.EprocessAmountToZeroXP();  // 将进程数量清零
		
		dwCurSize = 0;//读之前先清零     ??如何在下一次打开文件时将内存释放掉
		dwBufSize = 0;
		//delete[] pcBuf;

		strImageFilePath = lpszPathName;

		CFile fMemImageFile(lpszPathName,CFile::modeReadWrite | CFile::shareDenyWrite);
		dwCurSize = fMemImageFile.GetLength()+2;  //比实际文件多两个字节

		if (dwCurSize > dwBufSize)
		{
			dwBufSize = dwCurSize;
			if (dwCurSize <= 2*KSIZE*KSIZE)		// 先这样显示  显示2M
			{
				pcBuf = (char*)realloc(pcBuf,dwCurSize);
			}
			else
			{
				pcBuf = (char*)realloc(pcBuf,2*KSIZE*KSIZE);
			}
			//pcBuf = (char*)realloc(pcBuf,dwCurSize); //若文件大于1M，重新给m_pcBuf数组分配空间
			//pcBuf = (char*)malloc(dwCurSize);
		}

		if(dwCurSize <= 2*KSIZE*KSIZE)
		{
			dwCurSize = fMemImageFile.Read(pcBuf,dwCurSize);//将文件读取到m_pcBuf数组中，返回的值为读取的文件的大小
		}
		else
		{
			dwCurSize = fMemImageFile.Read(pcBuf,2*KSIZE*KSIZE);
		}


		//pcBuf[dwCurSize] = '/0';
		((CMemoryAnalysisView*)m_viewList.GetHead())->SetText(pcBuf,dwCurSize); //显示m_pcBuf数组存放的数据

		return TRUE;
	}

	// XP 

//	void CMemoryAnalysisDoc::OnKpcrExtractionXP()
//	{
//		// TODO: 在此添加命令处理程序代码
//		if(pcBuf!=NULL)
//		{
//			mMemInfo.ExtractKpcrXP(pcBuf,dwCurSize);
//		}
//		//////////////////////////////////////////////////////////////////////////
//		// 将结果显示
//		//((CMemoryAnalysisView*)m_viewList.GetHead())->SetText(mMemInfo.GetXPKpcrResult(),KPCR_LENGTH_XP); 
//		char* pKpcrResult = mMemInfo.GetKpcrResultXP();  // 没有分配空间，若结果存在在这个位置读取，如果不存在不做操作
//
//		//////////////////////////////////////////////////////////////////////////
//		// 如果pKpcrResult不为空，则将结果写入到当前文件夹下KpcrInfo.raw文件中
//		if(mMemInfo.IsKpcrExistXP())
//		{
//			ofstream ofile(".\\XPKpcrInfo.raw",ios::binary);
//
//			//可以改用其他的函数，如write
//			for(int i=0;i<KPCR_LENGTH_XP;i++)
//			{
//				ofile << pKpcrResult[i];
//			}
//		}
//		else
//		{
//			MessageBox(NULL,"cann't search KPCR",NULL,NULL);
//		}
//		pKpcrResult = NULL;   // 置空	
//	}

	void CMemoryAnalysisDoc::OnEprocessExtractionXP()
	{
		// TODO: 在此添加命令处理程序代码
		CResultProcess proResultDlg;

		// 进程已经提取成功，直接返回

		int mAmount=pProcArray->GetCount();
		if (mAmount) //出错
		{
			if (bForNotrpad)
			{
				return;
			}
			PROCESS pro;
			// 显示进程
			for (int i=0;i<mAmount;i++)
			{
				pro=pProcArray->GetAt(i);
				proResultDlg.SetProcessInfo(pro);
			}
			proResultDlg.TransferPara(strImageFilePath,mMemInfo);
			proResultDlg.DoModal();

			return;
		}

		

		//CFile fImageFile(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
		CFile fImageFile(strImageFilePath,CFile::modeReadWrite);
		BOOL bFindOfEP(FALSE);	// 发现EPROCESS，标志位

		int mNewCount(0);
		int mOldCount(0);

		ULONGLONG loffset(0);
		PROCESS mOneProcess;

		//ULONGLONG fileLen = fImageFile.GetLength();
		ULONGLONG fileLen = 264*1024*1024;//(12*22=264)

		
		// 进程的信息
		char* tmpAddr=new char[4];//用来存放从镜像中读取的32位数据——一般是一个地址
		//DWORD* tmpValue(NULL);//用来将上面的数组tmpAddr[4]的内容转化地址的中间值

		char* pEprocessResult=NULL;
		char* pProcessName = new char[16];		// ImageFileName域长为16  不能delete  影响pEprocessResult 程序崩溃
		DWORD* pTmpPageDirBase(NULL);
		DWORD* pTmpVadRootVA(NULL);
		DWORD mPid(0);
		DWORD mPPid(0);
		

		//创建进度条显示进度——使用非模态对话框
		int range=22;
		pProgressDlg->SetProgressRange(0,22);
		int curPosition=0;
		pProgressDlg->Create();
		pProgressDlg->CenterWindow();
		pProgressDlg->ShowWindow(SW_SHOW);

		while(loffset < fileLen)		// 没到文件结尾就继续读
		{
			fImageFile.Seek(loffset,CFile::begin);
			fImageFile.Read(pTmpBuf,bufSize);

			//更新进度条
			curPosition++;
			pProgressDlg->UpdateProgressCtrl(curPosition);

			// 搜索当前块
			mMemInfo.ExtractEprocessXP(pTmpBuf,bufSize);


			mNewCount = mMemInfo.GetEprocessAmountXP();  // 当前获得eprocess数量

			// 在当前块中发现Eprocess——当前获得的进程数量大于以前的数量
			if(mNewCount>mOldCount)		
			{
				bFindOfEP = TRUE;//EP结构发现，设置标志位
				for(int j=mOldCount;j<mNewCount;j++)
				{
					pEprocessResult = mMemInfo.GetEprocessResultXP(j);  // 得到第j个eprocess

					//////////////////////////////////////////////////////////////////////////
					// 将结果写入到当前文件夹下EprocessInfo.raw文件中

					//分别得到进程的各项信息，并进行格式化
					pProcessName = pEprocessResult+0x174;//得到进程的名字，方法对否？？？？？
					
					// 页目录基地址
					tmpAddr=pEprocessResult+0x18;
					pTmpPageDirBase=(DWORD*)tmpAddr;
					
					// pid
					tmpAddr=pEprocessResult+0x084;
					mPid=*((DWORD*)tmpAddr);

					// ppid
					tmpAddr=pEprocessResult+0x14c;
					mPPid=*((DWORD*)tmpAddr);

					// VadRootVA
					tmpAddr= pEprocessResult + 0x11c;//0x11c	
					pTmpVadRootVA= (DWORD*)tmpAddr;
					

					// 可视化准备
					CString strProcessNum;
					CString strProcessName;

					strProcessNum.Format("%d",mOldCount);
					strProcessName.Format("%s",pProcessName);

					// 更Doc中的新进程数目
					mOldCount++;

					//在对话框中显示 
					mOneProcess.strName=strProcessName;
					mOneProcess.Pid=mPid;
					mOneProcess.PPid=mPPid;
					mOneProcess.DirectoryTableBase=*pTmpPageDirBase;
					mOneProcess.VadRootVirAddr=*pTmpVadRootVA;
					
					pProcArray->Add(mOneProcess);	
					proResultDlg.SetProcessInfo(mOneProcess);
				}
			}
			memset(pTmpBuf,0,bufSize);
			loffset += bufSize;//读取下一块
		}
		pEprocessResult=NULL;
		//进度条关闭
		pProgressDlg->Close();

		// 成功 
		if(bFindOfEP == FALSE)
		{
			MessageBox(NULL,"No win 7's eprocess!",NULL,NULL);
			return;
		}
		fImageFile.Close();
		//结果显示
		//proResultDlg.SetProcessInfo(this->pProcArray);
		
		if (!bForNotrpad)// 是为notepad作准备，就不在显示进程结果
		{
			//传递进程给Dlg
			proResultDlg.TransferPara(strImageFilePath,mMemInfo);
			proResultDlg.DoModal();
		}
	}

	// Win 7

/*敬凯的
	void CMemoryAnalysisDoc::OnEprocessExtractionWin7()
	{
		CResultProcess proResultDlg;
		proResultDlg.DoModal();
		// TODO: 在此添加命令处理程序代码
		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		int mNewCount(0);
		int mOldCount(0);

		DWORD loffset(0);
		ULONGLONG  fileLen = file.GetLength();
		while(loffset < fileLen)		// 没到文件结尾就继续读
		{
			file.Seek(loffset,CFile::begin);
			file.Read(pTmpBuf,bufSize);

			mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);
			mNewCount = mMemInfo.GetEprocessAmountWin7();  // 获得eprocess数量

			if(mNewCount>0)
			{
				for(int j=0;j<mNewCount;j++)
				{
					char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // 得到第j个eprocess

					//////////////////////////////////////////////////////////////////////////
					// 将结果写入到当前文件夹下EprocessInfo.raw文件中
					CString strProcessNum;
					strProcessNum.Format("%d",mOldCount);
					CString ofilePath = ".\\WIN7EprocessInfo"+strProcessNum+".raw";
					//ofstream ofile(".\\EprocessInfo.raw");
					ofstream ofile(ofilePath,ios::binary);

					//将EPROCESS的结构写入文件ofilePath中

					for(int i=0;i<EPROCESS_LENGTH_WIN7;i++)
					{
						ofile << pEprocessResult[i];
					}
					mOldCount++; // 进程序号
				}
			}

			memset(pTmpBuf,0,bufSize);
			loffset += bufSize;
		}

		mOldCount = 0;
		//MessageBox("cheng gong!");
		//MessageBox()
	}

*/
/*
	void CMemoryAnalysisDoc::OnEprocessExtractionWin7()
	{
		CResultProcess proResultDlg;
		proResultDlg.DoModal();
		// TODO: 在此添加命令处理程序代码
		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		// 搜索当前块前后的进程数目
		int mNewCount(0);
		int mOldCount(0);

		//Eprocess
		BOOL bFindofEP(FALSE);

		DWORD loffset(0);
		ULONGLONG  fileLen = file.GetLength();
		while(loffset < fileLen)		// 没到文件结尾就继续读
		{
			file.Seek(loffset,CFile::begin);
			file.Read(pTmpBuf,bufSize);

			// 搜索当前块
			mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);

			mNewCount = mMemInfo.GetEprocessAmountWin7();  // 获得eprocess数量

			// 在当前块中发现Eprocess——当前获得的进程数量大于以前的数量
			if(mNewCount>mOldCount)
			{

				for(int j=0;j<mNewCount;j++)
				{
					char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // 得到第j个eprocess

					//////////////////////////////////////////////////////////////////////////
					// 将结果写入到当前文件夹下EprocessInfo.raw文件中
					CString strProcessNum;
					strProcessNum.Format("%d",mOldCount);
					CString ofilePath = ".\\WIN7EprocessInfo"+strProcessNum+".raw";
					//ofstream ofile(".\\EprocessInfo.raw");
					ofstream ofile(ofilePath,ios::binary);

					//将EPROCESS的结构写入文件ofilePath中

					for(int i=0;i<EPROCESS_LENGTH_WIN7;i++)
					{
						ofile << pEprocessResult[i];
					}
					mOldCount++; // 进程序号
				}
			}

			memset(pTmpBuf,0,bufSize);
			loffset += bufSize;
		}
	}
*/

void CMemoryAnalysisDoc::OnEprocessExtractionWin7()
{
	
	// TODO: 在此添加命令处理程序代码

	CResultProcess proResultDlg;
	PROCESS mOneProcess;

	// 进程的信息
	char* pProcessName = new char[16];		// ImageFileName域长为16  不能delete  影响pEprocessResult 程序崩溃
	DWORD* pTmpPageDirBase(NULL);
	DWORD* pTmpVadRootVA(NULL);
	DWORD mPid(0);
	DWORD mPPid(0);

	//用来存放从镜像中读取的32位数据——一般是一个地址
	char* tmpAddr=new char[4];
	
	// 搜索辅助
	int mNewCount(0);
	int mOldCount(0);
	BOOL bFindOfEP(FALSE);	// 发现EPROCESS，标志位
	
	CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
	DWORD fileLen = file.GetLength();
	DWORD loffset(0);

	// 循环读取镜像文件，进行查找
	while(loffset < fileLen)		// 没到文件结尾就继续读
	{
		file.Seek(loffset,CFile::begin);
		file.Read(pTmpBuf,bufSize);

		mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);
		mNewCount = mMemInfo.GetEprocessAmountWin7();  // 获得eprocess数量

		// 在当前块中发现Eprocess
		if(mNewCount>0)
		{
			if(FALSE==bFindOfEP)
				bFindOfEP=TRUE;
			for(int j=0;j<mNewCount;j++)
			{
				char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // 得到第j个eprocess
				
				//分别得到进程的各项信息，并进行格式化
				pProcessName = pEprocessResult+0x16c;//得到进程的名字，方法对否？？？？？

				// 页目录基地址
				tmpAddr=pEprocessResult+0x18;//（和XP一样，在_kprocess中）
				pTmpPageDirBase=(DWORD*)tmpAddr;

				// pid
				tmpAddr=pEprocessResult+0x0b4;
				mPid=*((DWORD*)tmpAddr);

				// ppid
				tmpAddr=pEprocessResult+0x140;
				mPPid=*((DWORD*)tmpAddr);

				// VadRootVA
				tmpAddr= pEprocessResult + 0x278;	
				pTmpVadRootVA= (DWORD*)tmpAddr;
				
				// 可视化准备
				CString strProcessNum;
				CString strProcessName;

				// 更Doc中的新进程数目
				mOldCount++;

				strProcessNum.Format("%d",mOldCount);
				strProcessName.Format("%s",pProcessName);
				
				//在对话框中显示 
				mOneProcess.strName=strProcessName;
				mOneProcess.Pid=mPid;
				mOneProcess.PPid=mPPid;
				mOneProcess.DirectoryTableBase=*pTmpPageDirBase;
				mOneProcess.VadRootVirAddr=*pTmpVadRootVA;


				proResultDlg.SetProcessInfo(mOneProcess);
				pProcArray->Add(mOneProcess);		
			}
		}
		memset(pTmpBuf,0,bufSize);
		loffset += bufSize;
	}

	// 成功 
	if(bFindOfEP == FALSE)
	{
		MessageBox(NULL,"No win 7's eprocess!",NULL,NULL);
	}
	/*else
	{
		CString num;
		num.Format("%d",mOldCount);
		MessageBox(NULL,num,NULL,NULL);
	}
	*/
	file.Close();

	//结果显示
	//proResultDlg.SetProcessInfo(this->pProcArray);
	//传递进程给Dlg
	proResultDlg.TransferPara(strImageFilePath,mMemInfo);
	proResultDlg.DoModal();
	proResultDlg.~CResultProcess();
}

//	void CMemoryAnalysisDoc::OnKpcrExtractionWin7()
//	{
//		// TODO: 在此添加命令处理程序代码
//		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
//
//
//		int resultCount(0);  //结果数量 
//
//
//
//		DWORD loffset(0);
//		ULONGLONG  fileLen = file.GetLength();
//		while(loffset < fileLen)		// 没到文件结尾就继续读
//		{
//			file.Seek(loffset,CFile::begin);
//			file.Read(pTmpBuf,bufSize);
//
//			mMemInfo.ExtractKpcrWin7(pTmpBuf,bufSize);
//
//
//			memset(pTmpBuf,0,bufSize);
//			loffset += bufSize;
//		}
//
//		resultCount = mMemInfo.GetKpcrAmountWin7();
//
//		CString num;
//		num.Format("%d",resultCount);
//		CString result = "find"+num+"times";
//		MessageBox(0,result,0,0);
//	}

	void CMemoryAnalysisDoc::OnWebSearchKeyword()
	{
		// TODO: 在此添加命令处理程序代码
		CSearchKeywords dlg;
		dlg.DoModal();
	}


	void CMemoryAnalysisDoc::OnWebAnalysis()
	{
		// TODO: 在此添加命令处理程序代码
		//AfxInitRichEdit2();
		CDialogWebAnalysis dlg;
		dlg.DoModal();
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
	// pNum:进程序号  filePath:镜像文件路径
	// 采用LDR中序遍历vad二叉树
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
	void CMemoryAnalysisDoc::OnNotepadRecovery()
	{
		// TODO: 在此添加命令处理程序代码

		//提取进程信息
		bForNotrpad=true;
		OnEprocessExtractionXP();
		bForNotrpad=false;

		CString strProName("notepad.exe");
		CString strProName1("NOTEPAD.EXE");
		PROCESS pro;

		int count=pProcArray->GetCount();
		for (int nIndex=0;nIndex<count;nIndex++)
		{
			pro=pProcArray->GetAt(nIndex);
			if (pro.strName==strProName||pro.strName==strProName1)
			{
				// 提取notepad
				XPMemoryExtract(pro,strImageFilePath);
			}
		}
	}
