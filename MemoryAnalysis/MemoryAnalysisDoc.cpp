// MemoryAnalysisDoc.cpp : CMemoryAnalysisDoc ���ʵ��
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

const DWORD KPCR_LENGTH_XP = 0xd70;   //kpcr��С  ����
const DWORD KSIZE = 1024;
const DWORD EPROCESS_LENGTH_XP = 0x25c;  //XP�µ�eprocess��С
const DWORD EPROCESS_LENGTH_WIN7 = 0x25c;  //WIN7�µ�eprocess��С



MemInfoExtraction mMemInfo;

const DWORD bufSize = 12*1024*1024;  // 12M ��ʱ�����С



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



	// CMemoryAnalysisDoc ����/����

	CMemoryAnalysisDoc::CMemoryAnalysisDoc()
	{
		// TODO: �ڴ����һ���Թ������
		pcBuf = NULL;
		dwBufSize = KSIZE*KSIZE;  //*736����Ŀռ����Ϊ736M  737M�����ϻ����    ����
		pcBuf = (char*)malloc(dwBufSize);	
		pTmpBuf = (char*)malloc(bufSize);	// ��������СΪ4M


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

		// TODO: �ڴ�������³�ʼ������

		// (SDI �ĵ������ø��ĵ�)
		//sprintf(pcBuf, "��Һ�ѽ��ʲô���⣿aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		//
		((CMemoryAnalysisView *)m_viewList.GetHead())->SetText(pcBuf, strlen(pcBuf));//����ط���ʲô��˼����������

		//////////////////////////////////////////////////////////////////////////
		// open a new image
		/*
		CFile fileOpen()
		OnOpenDocument()

		*/
		return TRUE;
	}




	// CMemoryAnalysisDoc ���л�

	void CMemoryAnalysisDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: �ڴ���Ӵ洢����
		}
		else
		{
			// TODO: �ڴ���Ӽ��ش���
		}
	}


	// CMemoryAnalysisDoc ���

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


	// CMemoryAnalysisDoc ����

	BOOL CMemoryAnalysisDoc::OnOpenDocument(LPCTSTR lpszPathName)
	{
		if (!CDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		// TODO:  �ڴ������ר�õĴ�������

		mMemInfo.EprocessAmountToZeroXP();  // ��������������
		
		dwCurSize = 0;//��֮ǰ������     ??�������һ�δ��ļ�ʱ���ڴ��ͷŵ�
		dwBufSize = 0;
		//delete[] pcBuf;

		strImageFilePath = lpszPathName;

		CFile fMemImageFile(lpszPathName,CFile::modeReadWrite | CFile::shareDenyWrite);
		dwCurSize = fMemImageFile.GetLength()+2;  //��ʵ���ļ��������ֽ�

		if (dwCurSize > dwBufSize)
		{
			dwBufSize = dwCurSize;
			if (dwCurSize <= 2*KSIZE*KSIZE)		// ��������ʾ  ��ʾ2M
			{
				pcBuf = (char*)realloc(pcBuf,dwCurSize);
			}
			else
			{
				pcBuf = (char*)realloc(pcBuf,2*KSIZE*KSIZE);
			}
			//pcBuf = (char*)realloc(pcBuf,dwCurSize); //���ļ�����1M�����¸�m_pcBuf�������ռ�
			//pcBuf = (char*)malloc(dwCurSize);
		}

		if(dwCurSize <= 2*KSIZE*KSIZE)
		{
			dwCurSize = fMemImageFile.Read(pcBuf,dwCurSize);//���ļ���ȡ��m_pcBuf�����У����ص�ֵΪ��ȡ���ļ��Ĵ�С
		}
		else
		{
			dwCurSize = fMemImageFile.Read(pcBuf,2*KSIZE*KSIZE);
		}


		//pcBuf[dwCurSize] = '/0';
		((CMemoryAnalysisView*)m_viewList.GetHead())->SetText(pcBuf,dwCurSize); //��ʾm_pcBuf�����ŵ�����

		return TRUE;
	}

	// XP 

//	void CMemoryAnalysisDoc::OnKpcrExtractionXP()
//	{
//		// TODO: �ڴ���������������
//		if(pcBuf!=NULL)
//		{
//			mMemInfo.ExtractKpcrXP(pcBuf,dwCurSize);
//		}
//		//////////////////////////////////////////////////////////////////////////
//		// �������ʾ
//		//((CMemoryAnalysisView*)m_viewList.GetHead())->SetText(mMemInfo.GetXPKpcrResult(),KPCR_LENGTH_XP); 
//		char* pKpcrResult = mMemInfo.GetKpcrResultXP();  // û�з���ռ䣬��������������λ�ö�ȡ����������ڲ�������
//
//		//////////////////////////////////////////////////////////////////////////
//		// ���pKpcrResult��Ϊ�գ��򽫽��д�뵽��ǰ�ļ�����KpcrInfo.raw�ļ���
//		if(mMemInfo.IsKpcrExistXP())
//		{
//			ofstream ofile(".\\XPKpcrInfo.raw",ios::binary);
//
//			//���Ը��������ĺ�������write
//			for(int i=0;i<KPCR_LENGTH_XP;i++)
//			{
//				ofile << pKpcrResult[i];
//			}
//		}
//		else
//		{
//			MessageBox(NULL,"cann't search KPCR",NULL,NULL);
//		}
//		pKpcrResult = NULL;   // �ÿ�	
//	}

	void CMemoryAnalysisDoc::OnEprocessExtractionXP()
	{
		// TODO: �ڴ���������������
		CResultProcess proResultDlg;

		// �����Ѿ���ȡ�ɹ���ֱ�ӷ���

		int mAmount=pProcArray->GetCount();
		if (mAmount) //����
		{
			if (bForNotrpad)
			{
				return;
			}
			PROCESS pro;
			// ��ʾ����
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
		BOOL bFindOfEP(FALSE);	// ����EPROCESS����־λ

		int mNewCount(0);
		int mOldCount(0);

		ULONGLONG loffset(0);
		PROCESS mOneProcess;

		//ULONGLONG fileLen = fImageFile.GetLength();
		ULONGLONG fileLen = 264*1024*1024;//(12*22=264)

		
		// ���̵���Ϣ
		char* tmpAddr=new char[4];//������ŴӾ����ж�ȡ��32λ���ݡ���һ����һ����ַ
		//DWORD* tmpValue(NULL);//���������������tmpAddr[4]������ת����ַ���м�ֵ

		char* pEprocessResult=NULL;
		char* pProcessName = new char[16];		// ImageFileName��Ϊ16  ����delete  Ӱ��pEprocessResult �������
		DWORD* pTmpPageDirBase(NULL);
		DWORD* pTmpVadRootVA(NULL);
		DWORD mPid(0);
		DWORD mPPid(0);
		

		//������������ʾ���ȡ���ʹ�÷�ģ̬�Ի���
		int range=22;
		pProgressDlg->SetProgressRange(0,22);
		int curPosition=0;
		pProgressDlg->Create();
		pProgressDlg->CenterWindow();
		pProgressDlg->ShowWindow(SW_SHOW);

		while(loffset < fileLen)		// û���ļ���β�ͼ�����
		{
			fImageFile.Seek(loffset,CFile::begin);
			fImageFile.Read(pTmpBuf,bufSize);

			//���½�����
			curPosition++;
			pProgressDlg->UpdateProgressCtrl(curPosition);

			// ������ǰ��
			mMemInfo.ExtractEprocessXP(pTmpBuf,bufSize);


			mNewCount = mMemInfo.GetEprocessAmountXP();  // ��ǰ���eprocess����

			// �ڵ�ǰ���з���Eprocess������ǰ��õĽ�������������ǰ������
			if(mNewCount>mOldCount)		
			{
				bFindOfEP = TRUE;//EP�ṹ���֣����ñ�־λ
				for(int j=mOldCount;j<mNewCount;j++)
				{
					pEprocessResult = mMemInfo.GetEprocessResultXP(j);  // �õ���j��eprocess

					//////////////////////////////////////////////////////////////////////////
					// �����д�뵽��ǰ�ļ�����EprocessInfo.raw�ļ���

					//�ֱ�õ����̵ĸ�����Ϣ�������и�ʽ��
					pProcessName = pEprocessResult+0x174;//�õ����̵����֣������Է񣿣�������
					
					// ҳĿ¼����ַ
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
					

					// ���ӻ�׼��
					CString strProcessNum;
					CString strProcessName;

					strProcessNum.Format("%d",mOldCount);
					strProcessName.Format("%s",pProcessName);

					// ��Doc�е��½�����Ŀ
					mOldCount++;

					//�ڶԻ�������ʾ 
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
			loffset += bufSize;//��ȡ��һ��
		}
		pEprocessResult=NULL;
		//�������ر�
		pProgressDlg->Close();

		// �ɹ� 
		if(bFindOfEP == FALSE)
		{
			MessageBox(NULL,"No win 7's eprocess!",NULL,NULL);
			return;
		}
		fImageFile.Close();
		//�����ʾ
		//proResultDlg.SetProcessInfo(this->pProcArray);
		
		if (!bForNotrpad)// ��Ϊnotepad��׼�����Ͳ�����ʾ���̽��
		{
			//���ݽ��̸�Dlg
			proResultDlg.TransferPara(strImageFilePath,mMemInfo);
			proResultDlg.DoModal();
		}
	}

	// Win 7

/*������
	void CMemoryAnalysisDoc::OnEprocessExtractionWin7()
	{
		CResultProcess proResultDlg;
		proResultDlg.DoModal();
		// TODO: �ڴ���������������
		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		int mNewCount(0);
		int mOldCount(0);

		DWORD loffset(0);
		ULONGLONG  fileLen = file.GetLength();
		while(loffset < fileLen)		// û���ļ���β�ͼ�����
		{
			file.Seek(loffset,CFile::begin);
			file.Read(pTmpBuf,bufSize);

			mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);
			mNewCount = mMemInfo.GetEprocessAmountWin7();  // ���eprocess����

			if(mNewCount>0)
			{
				for(int j=0;j<mNewCount;j++)
				{
					char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // �õ���j��eprocess

					//////////////////////////////////////////////////////////////////////////
					// �����д�뵽��ǰ�ļ�����EprocessInfo.raw�ļ���
					CString strProcessNum;
					strProcessNum.Format("%d",mOldCount);
					CString ofilePath = ".\\WIN7EprocessInfo"+strProcessNum+".raw";
					//ofstream ofile(".\\EprocessInfo.raw");
					ofstream ofile(ofilePath,ios::binary);

					//��EPROCESS�Ľṹд���ļ�ofilePath��

					for(int i=0;i<EPROCESS_LENGTH_WIN7;i++)
					{
						ofile << pEprocessResult[i];
					}
					mOldCount++; // �������
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
		// TODO: �ڴ���������������
		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		// ������ǰ��ǰ��Ľ�����Ŀ
		int mNewCount(0);
		int mOldCount(0);

		//Eprocess
		BOOL bFindofEP(FALSE);

		DWORD loffset(0);
		ULONGLONG  fileLen = file.GetLength();
		while(loffset < fileLen)		// û���ļ���β�ͼ�����
		{
			file.Seek(loffset,CFile::begin);
			file.Read(pTmpBuf,bufSize);

			// ������ǰ��
			mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);

			mNewCount = mMemInfo.GetEprocessAmountWin7();  // ���eprocess����

			// �ڵ�ǰ���з���Eprocess������ǰ��õĽ�������������ǰ������
			if(mNewCount>mOldCount)
			{

				for(int j=0;j<mNewCount;j++)
				{
					char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // �õ���j��eprocess

					//////////////////////////////////////////////////////////////////////////
					// �����д�뵽��ǰ�ļ�����EprocessInfo.raw�ļ���
					CString strProcessNum;
					strProcessNum.Format("%d",mOldCount);
					CString ofilePath = ".\\WIN7EprocessInfo"+strProcessNum+".raw";
					//ofstream ofile(".\\EprocessInfo.raw");
					ofstream ofile(ofilePath,ios::binary);

					//��EPROCESS�Ľṹд���ļ�ofilePath��

					for(int i=0;i<EPROCESS_LENGTH_WIN7;i++)
					{
						ofile << pEprocessResult[i];
					}
					mOldCount++; // �������
				}
			}

			memset(pTmpBuf,0,bufSize);
			loffset += bufSize;
		}
	}
*/

void CMemoryAnalysisDoc::OnEprocessExtractionWin7()
{
	
	// TODO: �ڴ���������������

	CResultProcess proResultDlg;
	PROCESS mOneProcess;

	// ���̵���Ϣ
	char* pProcessName = new char[16];		// ImageFileName��Ϊ16  ����delete  Ӱ��pEprocessResult �������
	DWORD* pTmpPageDirBase(NULL);
	DWORD* pTmpVadRootVA(NULL);
	DWORD mPid(0);
	DWORD mPPid(0);

	//������ŴӾ����ж�ȡ��32λ���ݡ���һ����һ����ַ
	char* tmpAddr=new char[4];
	
	// ��������
	int mNewCount(0);
	int mOldCount(0);
	BOOL bFindOfEP(FALSE);	// ����EPROCESS����־λ
	
	CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
	DWORD fileLen = file.GetLength();
	DWORD loffset(0);

	// ѭ����ȡ�����ļ������в���
	while(loffset < fileLen)		// û���ļ���β�ͼ�����
	{
		file.Seek(loffset,CFile::begin);
		file.Read(pTmpBuf,bufSize);

		mMemInfo.ExtractEprocessWin7(pTmpBuf,bufSize);
		mNewCount = mMemInfo.GetEprocessAmountWin7();  // ���eprocess����

		// �ڵ�ǰ���з���Eprocess
		if(mNewCount>0)
		{
			if(FALSE==bFindOfEP)
				bFindOfEP=TRUE;
			for(int j=0;j<mNewCount;j++)
			{
				char* pEprocessResult = mMemInfo.GetEprocessResultWin7(j);  // �õ���j��eprocess
				
				//�ֱ�õ����̵ĸ�����Ϣ�������и�ʽ��
				pProcessName = pEprocessResult+0x16c;//�õ����̵����֣������Է񣿣�������

				// ҳĿ¼����ַ
				tmpAddr=pEprocessResult+0x18;//����XPһ������_kprocess�У�
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
				
				// ���ӻ�׼��
				CString strProcessNum;
				CString strProcessName;

				// ��Doc�е��½�����Ŀ
				mOldCount++;

				strProcessNum.Format("%d",mOldCount);
				strProcessName.Format("%s",pProcessName);
				
				//�ڶԻ�������ʾ 
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

	// �ɹ� 
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

	//�����ʾ
	//proResultDlg.SetProcessInfo(this->pProcArray);
	//���ݽ��̸�Dlg
	proResultDlg.TransferPara(strImageFilePath,mMemInfo);
	proResultDlg.DoModal();
	proResultDlg.~CResultProcess();
}

//	void CMemoryAnalysisDoc::OnKpcrExtractionWin7()
//	{
//		// TODO: �ڴ���������������
//		CFile file(strImageFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
//
//
//		int resultCount(0);  //������� 
//
//
//
//		DWORD loffset(0);
//		ULONGLONG  fileLen = file.GetLength();
//		while(loffset < fileLen)		// û���ļ���β�ͼ�����
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
		// TODO: �ڴ���������������
		CSearchKeywords dlg;
		dlg.DoModal();
	}


	void CMemoryAnalysisDoc::OnWebAnalysis()
	{
		// TODO: �ڴ���������������
		//AfxInitRichEdit2();
		CDialogWebAnalysis dlg;
		dlg.DoModal();
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
	// pNum:�������  filePath:�����ļ�·��
	// ����LDR�������vad������
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
	void CMemoryAnalysisDoc::OnNotepadRecovery()
	{
		// TODO: �ڴ���������������

		//��ȡ������Ϣ
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
				// ��ȡnotepad
				XPMemoryExtract(pro,strImageFilePath);
			}
		}
	}
