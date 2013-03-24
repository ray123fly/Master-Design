// PDE PTE��ص����ݺͲ���
#include "stdafx.h"
#include "MemoryManagement.h"

// �ж�pae
BOOL isPae(DWORD cr3,CString imagefile)
{
	char mTemp;

	CFile fImage(imagefile,CFile::modeRead|CFile::shareDenyNone);
	fImage.Seek(cr3,CFile::begin);
	fImage.Read(&mTemp,1);
	fImage.Close();
	//char *p=(char *)cr3;
	if(0x01==mTemp)    // pae
		return TRUE;
	else             // non-pae
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// no-pae

// �õ���־λ��Ϣ
void GetAddrFlags(DWORD addr,VirtualAddrFlags &stat)
{
	//stat.validbit=addr&1;
	stat.validbit = (addr<<31)>>31;
	stat.prototypebit = (addr>>10)&1;
	stat.transitionbit = (addr>>11)&1;
}

PTE_TYPE GetPdeOrPteType(DWORD addr)
{
	VirtualAddrFlags state;
	GetAddrFlags(addr,state);

	if (1 == state.validbit)
	{
		return valide;
	}
	else
	{
		if (1 == state.transitionbit)
		{
			return transition;
		}
		else
		{
			if (1 == state.prototypebit)
			{
				return prototype;
			}
			else
			{
				return other;
			}
		}
	}
}

// �ֱ��ж� PDE �� PTE �ĸ�����־λ����Ϣ
BOOL isLargePageMode(DWORD pde)
{
	return (pde&0x80)>>7;
}

//pte   pdeOri
BOOL IsValid(DWORD addr)
{
	VirtualAddrFlags state;
	GetAddrFlags(addr,state);

	if (0 == state.validbit)
	{
		return FALSE;
	} 
	else
		return TRUE;
	
}

BOOL IsTransition(DWORD addr)
{
	VirtualAddrFlags state;
	GetAddrFlags(addr,state);

	if (0 == state.validbit)
	{
		return FALSE;
	}
	else
	{
		if (0 == state.transitionbit)
		{
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL IsPrototype(DWORD addr)
{
	return TRUE;
}

// �õ�ҳĿ¼
void GetPageDirTable(DWORD baseAddrOri,int pageDirSize,CFile &ImgFile,CArray<DWORD,DWORD> *pPageDir)
{
	pPageDir->RemoveAll();
	DWORD *tmpAddress;
	DWORD PDE;
	char* pTempPageDir=new char[pageDirSize];//
	char* pPointer=pTempPageDir;

	DWORD baseAddr=PteToData(baseAddrOri);//���Կ�������������

	ImgFile.Seek(baseAddr,CFile::begin);
	ImgFile.Read(pTempPageDir,pageDirSize);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPDE   ��ҳĿ¼��Ҫ��Ҫλ�ƣ�

	char TrueValue[4];
	for (int Index=0;Index<1024;Index++)
	{
		// �����ڣ���С��ת��������ط�����Ҫ
		TrueValue[0] = pPointer[4*Index+0];
		TrueValue[1] = pPointer[4*Index+1];
		TrueValue[2] = pPointer[4*Index+2];
		TrueValue[3] = pPointer[4*Index+3];

		tmpAddress = (DWORD*)TrueValue;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�

		PDE = *tmpAddress;			// ҳĿ¼��

		//��PDE����������
		pPageDir->Add(PDE);
	}
	pPointer=NULL;
	//delete[] pTempPageDir;//����ͳ�������
	delete[] pTempPageDir;
	tmpAddress=NULL;
	//return pPageDir;
}

// �õ�ҳ��
void GetPageTable(DWORD pdeOri,int pageTableSize,CFile &ImgFile,CArray<DWORD,DWORD>* pPageTab)
{
	pPageTab->RemoveAll();
	DWORD *tmpAddress;
	DWORD PTE;
	char* pTmpPageTab = new char[pageTableSize];

	DWORD pde=PdeToPageTable(pdeOri);

	ImgFile.Seek(pde,CFile::begin);
	ImgFile.Read(pTmpPageTab,pageTableSize);

	//CArray<DWORD,DWORD>* pPageTab;
	//pPageTab = new CArray<DWORD,DWORD>;

	char TrueValue[4];
	for (int Index = 0;Index < 1024;Index++)
	{
		TrueValue[0] = pTmpPageTab[4*Index+0];
		TrueValue[1] = pTmpPageTab[4*Index+1];
		TrueValue[2] = pTmpPageTab[4*Index+2];
		TrueValue[3] = pTmpPageTab[4*Index+3];
		tmpAddress = (DWORD *)TrueValue;
		PTE = *tmpAddress;

		//��PTE����������
		pPageTab->Add(PTE);
	}
	delete pTmpPageTab;
	tmpAddress=NULL;

	//delete[] pTmpPageTab;
	//return pPageTab;
}

void GetPhysicalPageData(DWORD PdeOrPteOri,BOOL isLarge,CFile &ImgFile,char* pTmpData)
{
	//DWORD pte=PdeOrPteToReal(PdeOrPteOri);
	DWORD PdeOrPteReal(0);
	if (TRUE==isLarge)
	{
		PdeOrPteReal=PdeToData(PdeOrPteOri);
		ImgFile.Seek(PdeOrPteReal,CFile::begin);
		ImgFile.Read(pTmpData,PAGE_SIZE_LARGE);
	} 
	else
	{
		PdeOrPteReal=PteToData(PdeOrPteOri);
		ImgFile.Seek(PdeOrPteReal,CFile::begin);
		ImgFile.Read(pTmpData,PAGE_SIZE);
	}
}

DWORD PdeToPageTable(DWORD pde)
{
	DWORD PageTab(0);
	PageTab=(pde>>12)<<12;
	return PageTab;
}

DWORD PdeToData(DWORD pde)
{
	DWORD realDataPage(0);
	realDataPage=(pde>>22)<<22;
	return realDataPage;
}

DWORD PteToData(DWORD PdeOrPte)
{
	DWORD realDataPage(0);
	realDataPage=(PdeOrPte>>12)<<12;
	return realDataPage;
}

// ��ȡ�����ڴ�
void NoPaeDumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//AfxMessageBox("Go!");
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	DWORD PDE(0),PTE(0);

	CArray<DWORD,DWORD>*pPageDirectory,*pPageTable;//�ֱ���ҳĿ¼���ҳ��
	pPageDirectory = new CArray<DWORD,DWORD>;
	pPageTable = new CArray<DWORD,DWORD>;

	pPageDirectory->RemoveAll();
	pPageTable->RemoveAll();

	//����ҳ�浼�����õ�����ʱ�ռ�
	char* pPageDataBuf = new char[PAGE_SIZE];
	char* pLargePageDataBuf = new char[PAGE_SIZE_LARGE];

	//�����ļ�һ���򿪣��ദʹ��
	CFile ImgFile(imgFilePath,CFile::modeReadWrite);

	//����Pro���ڴ浼���ļ�
	CString strPid;
	strPid.Format("%d",pro.Pid);
	CString ProDmpFilePath = ".\\Mem-PageTable-"+strPid+"-"+pro.strName+".raw";
	CFile ProcDmpFile(ProDmpFilePath,CFile::modeCreate|CFile::modeReadWrite);

	//һ�λ�ȡһ��ҳ���ҳĿ¼��
	GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,ImgFile,pPageDirectory);

	for( int DirIndex=0; DirIndex < 512 ; DirIndex++)
	{
	
		//pProgressDlg->ShowWindow(SW_SHOW);
		//��һ�ַ���:����������
		PDE=pPageDirectory->GetAt(DirIndex);

		if (valide !=GetPdeOrPteType(PDE))
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ʱ�ж������ַת���Ƿ��ô�ҳģʽ
		isLargePage = isLargePageMode(PDE);	// �õ�8λ(��0λ��ʼ����������)�Ƿ�Ϊ0�ж��Ƿ��Ǵ�ҳģʽ�����Ǵ�ҳģʽ��ֵΪ1��TRUE�����Ǵ�ҳģʽֵΪ0��FALSE

		if (isLargePage == TRUE)
		{
			//��ҳ��
			//PDEת��
			GetPhysicalPageData(PDE,isLargePage,ImgFile,pLargePageDataBuf);//һ�ζ�ȡһ��ҳ������ݣ�Ȼ��д��ProcessFile��
			//ProcDmpFile.SeekToEnd();
			ProcDmpFile.Seek(0,CFile::end);
			ProcDmpFile.Write(pLargePageDataBuf,PAGE_SIZE_LARGE);
		}
		else//�Ǵ�ҳģʽ
		{
			//��ҳ��
			GetPageTable(PDE,PAGE_SIZE,ImgFile,pPageTable);//һ�λ�ȡһ��ҳ�����ݣ���ʹ1024��ҳ����

			for (int TableIndex = 0;TableIndex < PTE_COUNT;TableIndex++)
			{

				PTE = pPageTable->GetAt(TableIndex);

				if (valide == GetPdeOrPteType(PTE))//ҳ����Ч�Ͷ�ȡ����ҳ������
				{
					GetPhysicalPageData(PTE,isLargePage,ImgFile,pPageDataBuf);
					ProcDmpFile.SeekToEnd();
					ProcDmpFile.Write(pPageDataBuf,PAGE_SIZE);
				}
			}
		}
	}

	delete[] pPageDataBuf;
	delete[] pLargePageDataBuf;
	delete pPageDirectory;
	delete pPageTable;

	ProcDmpFile.Close();
	ImgFile.Close();

}

//////////////////////////////////////////////////////////////////////////
//pae 

// �ֱ��ж� PDE �� PTE �ĸ�����־λ����Ϣ
//pte   pde

void PaeGetAddrFlags(DWORD addr,VirtualAddrFlags &state)
{
	// pass
}

BOOL PaeIsLargePageMode(INT64 pde)
{
	return (pde&0x80)>>7;
}

BOOL PaeIsValid(INT64 addr)
{
	if (addr&0x01)
	{
		return TRUE;
	}
	else
		return FALSE;
}

BOOL PaeIsTransition(INT64 addr)
{
	return TRUE;
}

BOOL PaeIsPrototype(INT64 addr)
{
	return TRUE;
}

PTE_TYPE PaeGetPdeOrPteType(DWORD addr)
{
	return transition;
}


DWORD PaeCR3ToPDPTBase(DWORD cr3)
{
	DWORD realPDPTBase(0);
	realPDPTBase=cr3&0xfffff000;
	return realPDPTBase;
}

inline INT64 strToi64(char* str)
{
	char *tmp=new char [9];
	strcpy(tmp,str);
	CString t=strrev(tmp);
	INT64 n = _atoi64(t); 
	delete[] tmp;
	return n;
}
void PaeGetPageDirPointerTable(DWORD cr3,int tableSize,CFile &imgFile,CArray<INT64,INT64> *pPageDirPointerTab)
{
	pPageDirPointerTab->RemoveAll();

	char* pTempPageDirPointerTab=new char[tableSize];// ҳĿ¼ָ���
	INT64 baseAddr=PaeCR3ToPDPTBase(cr3);//ҳĿ¼ָ������ַ

	imgFile.Seek(baseAddr,CFile::begin);
	imgFile.Read(pTempPageDirPointerTab,tableSize);  // ��ҳĿ¼ָ����ȡ��pTempPageDirTab��

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char* pPointer=pTempPageDirPointerTab;

	INT64 *tmpAddress; // ��С��ת��
	INT64 PDPTE;

	for (int Index=0;Index<4;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�

		PDPTE= *tmpAddress;       		// ҳĿ¼ָ�����

		//PDPTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		//����
		CString strPdpte;
		strPdpte.Format("PDPTE: %x",PDPTE);
		AfxMessageBox(strPdpte);

		// ��PDPT����������
		pPageDirPointerTab->Add(PDPTE);
		pPointer+=8;
	}
	
	// delete[] pTempPageDir;//����ͳ�������
	delete[] pTempPageDirPointerTab;
	delete[] TrueValue;
	tmpAddress=NULL;
	pPointer=NULL;
}

INT64 PaePDPTEToPageDirBase(INT64 pdpte)
{
	INT64 realPageDirBase;
	realPageDirBase=pdpte&0xffffff000;
	return realPageDirBase;
}

void PaeGetPageDirTable(INT64 baseAddr,int pageDirSize,CFile &ImgFile,CArray<INT64,INT64> *pPageDir)
{
	pPageDir->RemoveAll();

	char* pTempPageDir=new char[pageDirSize];// ҳĿ¼ָ���
	INT64 base=PaePteToDataBase(baseAddr);//ҳĿ¼�����ַ

	ImgFile.Seek(base,CFile::begin);
	ImgFile.Read(pTempPageDir,pageDirSize);  // ��ҳĿ¼ָ����ȡ��pTempPageDirTab��

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char* pPointer=pTempPageDir;

	INT64 *tmpAddress; // ��С��ת��
	INT64 PDTE;

	for (int Index=0;Index<512;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�

		PDTE= *tmpAddress;       		// ҳĿ¼����
		//PDTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		//����
	/*	CString strPdte;
		strPdte.Format("PDTE: %x",PDTE);
		AfxMessageBox(strPdte);
    */
		// ��PDPT����������
		pPageDir->Add(PDTE);
		pPointer+=8;
	}
	
	// delete[] pTempPageDir;//����ͳ�������
	delete[] pTempPageDir;
	delete[] TrueValue;
	tmpAddress=NULL;
	pPointer=NULL;
}

INT64 PaePdeToPageTableBase(INT64 pde)
{
	INT64 realPageTableBase;
	realPageTableBase=pde&0xffffff000;
	return realPageTableBase;
}

// ��ҳģʽ�������ݣ�2M��
INT64 PaePdeToDataBase(INT64 pde,BOOL isLarge)
{
	INT64 realPte;
	if (isLarge)// ��ҳ35-21
	{
		realPte=pde&0xfffe00000;
	} 
	else// Сҳ35-12
	{
		realPte=pde&0xffffff000;
	}
	return realPte;
}

void PaeGetPageTable(INT64 pdeOri,int pageTableSize,CFile &ImgFile,CArray<INT64,INT64> *pPageTab)
{
	pPageTab->RemoveAll();

	char* pTempPageTable=new char[pageTableSize];// ҳĿ¼ָ���
	INT64 base=PaePdeToPageTableBase(pdeOri);//ҳĿ¼�����ַ

	ImgFile.Seek(base,CFile::begin);
	ImgFile.Read(pTempPageTable,pageTableSize);  // ��ҳ���ȡ��pTempPageTab��

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char *p=pTempPageTable;
	char* pPointer=pTempPageTable;

	INT64 *tmpAddress; // ��С��ת��
	INT64 PTE;

	for (int Index=0;Index<512;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
		PTE= *tmpAddress;       		// ҳĿ¼����
		//PTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		 //����
	/*	CString strPte;
		strPte.Format("PTE: %x",PTE);
		AfxMessageBox(strPte);
	*/
		// ��PDPT����������
		pPageTab->Add(PTE);
		pPointer+=8;
	}

	delete[] pTempPageTable;
	delete[] TrueValue;
	tmpAddress=NULL;
	pPointer=NULL;
}

// Сҳģʽ�������ݣ�4K��
INT64 PaePteToDataBase(INT64 pte)
{
	INT64 realPte=pte&0xffffff000;
	return realPte;
}

void PaeGetPhysicalPageData(INT64 PdeOrPteOri,BOOL isLarge,CFile &ImgFile,char* pTmpData)
{
	DWORD PdeOrPteReal(0);
	PdeOrPteReal=PaePdeToDataBase(PdeOrPteOri,isLarge);
	ImgFile.Seek(PdeOrPteReal,CFile::begin);
	if (TRUE==isLarge)
	{
		ImgFile.Read(pTmpData,PAE_PAGE_SIZE_LARGE);
	} 
	else
	{
		ImgFile.Read(pTmpData,PAE_PAGE_SIZE);
	}
}

// ��ȡ�����ڴ�
void PaeDumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//AfxMessageBox("Go!");
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	INT64 PDPTE(0),PDE(0),PTE(0);

	CArray<INT64,INT64> *pPageDirectoryPointerTable,*pPageDirectory,*pPageTable;//�ֱ���ҳĿ¼ָ���ҳĿ¼����ҳ��

	pPageDirectoryPointerTable=new CArray<INT64,INT64>;
	pPageDirectory = new CArray<INT64,INT64>;
	pPageTable = new CArray<INT64,INT64>;

	//����ҳ�浼�����õ�����ʱ�ռ�
	char* pPageDataBuf = new char[PAE_PAGE_SIZE];
	char* pLargePageDataBuf = new char[PAE_PAGE_SIZE_LARGE];

	//�����ļ�һ���򿪣��ദʹ��
	CFile ImgFile(imgFilePath,CFile::modeReadWrite);

	//����Pro���ڴ浼���ļ�
	CString strPid;
	strPid.Format("%d",pro.Pid);
	CString ProDmpFilePath = ".\\Mem-PageTable-"+strPid+"-"+pro.strName+".raw";
	CFile ProcDmpFile(ProDmpFilePath,CFile::modeCreate|CFile::modeReadWrite);

	//////////////////////////////////////////////////////////////////////////
	//��ʼ��ȡ

	//AfxMessageBox(pro.DirectoryTableBase);
	// 1��һ��ȡ���ĸ�ҳĿ¼ָ��
	PaeGetPageDirPointerTable(pro.DirectoryTableBase,32,ImgFile,pPageDirectoryPointerTable);

	// ѭ������ĸ�ҳĿ¼�ռ������
	//for (int i=0;i<2;i++) //�û��ռ�
	for (int i=0;i<4;i++)
	{
		// ���λ�ȡÿһ��ҳĿ¼�����ַ����ҳĿ¼ָ����
		PDPTE=pPageDirectoryPointerTable->GetAt(i);
	/*	if (!PaeIsValid(PDPTE))// ҳĿ¼ָ�������ָ���ҳĿ¼�����ڴ�
		{
			continue;
		}
    */
		// ҳĿ¼ָ�������ָ���ҳĿ¼���ڴ�

		// 2��һ�λ�ȡһ��ҳ���ҳĿ¼��
		PaeGetPageDirTable(PDPTE,PAE_PAGE_SIZE,ImgFile,pPageDirectory);

		for( int DirIndex=0; DirIndex < PAE_PDE_COUNT ; DirIndex++)
		{
			PDE=pPageDirectory->GetAt(DirIndex);

			if (!PaeIsValid(PDE))// ҳĿ¼����ָ���ҳ�����ڴ�
			{
				continue;
			}

			//////////////////////////////////////////////////////////////////////////
			// ��ʱ�ж������ַת���Ƿ��ô�ҳģʽ
			isLargePage = PaeIsLargePageMode(PDE);	// �õ�8λ(��0λ��ʼ����������)�Ƿ�Ϊ0�ж��Ƿ��Ǵ�ҳģʽ�����Ǵ�ҳģʽ��ֵΪ1��TRUE�����Ǵ�ҳģʽֵΪ0��FALSE

			if (isLargePage == TRUE) // ��ҳģʽ
			{
				//������ҳ
				//PDEת��
				// 3�����ҳ����
				PaeGetPhysicalPageData(PDE,isLargePage,ImgFile,pLargePageDataBuf);//һ�ζ�ȡһ��ҳ������ݣ�Ȼ��д��ProcessFile��
				//ProcDmpFile.SeekToEnd();
				ProcDmpFile.Seek(0,CFile::end);
				ProcDmpFile.Write(pLargePageDataBuf,PAE_PAGE_SIZE_LARGE);
			}
			else // Сҳģʽ
			{
				// 3����ҳ��
				PaeGetPageTable(PDE,PAE_PAGE_SIZE,ImgFile,pPageTable);//һ�λ�ȡһ��ҳ�����ݣ���ʹ1024��ҳ����

				for (int TableIndex = 0;TableIndex < PAE_PTE_COUNT;TableIndex++)
				{
					PTE = pPageTable->GetAt(TableIndex);

					if (PaeIsValid(PTE))//ҳ����Ч�Ͷ�ȡ����ҳ������
					{
						// 4����Сҳ����
						PaeGetPhysicalPageData(PTE,isLargePage,ImgFile,pPageDataBuf);
						ProcDmpFile.SeekToEnd();
						ProcDmpFile.Write(pPageDataBuf,PAE_PAGE_SIZE);
					}
				}
			}
		}
	}

	delete[] pPageDirectoryPointerTable;
	delete[] pPageDirectory;
	delete[] pPageTable;

	delete[] pPageDataBuf;
	delete[] pLargePageDataBuf;

	ImgFile.Close();
	ProcDmpFile.Close();
}

//////////////////////////////////////////////////////////////////////////
// �����ڴ浼������ͳһ�ӿ�

// ��ȡ�����ڴ�
void DumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//�ж���pae��no-pae
	if (isPae(pro.DirectoryTableBase,imgFilePath))  // pae
	{
		AfxMessageBox("pae mode!");
		PaeDumpProcessMemory(pro,imgFilePath);
	}
	else // no-pae
	{
		AfxMessageBox("no-pae mode!");
		NoPaeDumpProcessMemory(pro,imgFilePath);
	}
}

//////////////////////////////////////////////////////////////////////////
//�������

// �������ĺ��������޸ļ���
DWORD VirtualToPhysicalAddr(DWORD mPageDirectoryBase,DWORD virtualAdd,CString filePath)
{
	DWORD physicaAddr(0);

	//////////////////////////////////////////////////////////////////////////
	// ���ж��Ƿ�Ϊ��ҳģʽ��������PDE��ֵ���ж�

	// һ������ʺ���ҳ��СΪ4KB����PAEģʽ�����������ڴ�ҳģʽ���ں˱����������ô�ҳģʽ


	// 1��׼������
	DWORD OffsetNoLargePage = virtualAdd&0xfff;		// ���Ǵ�ҳģʽʱ��ҳ��ƫ����  ��ʮ��λ��0xfff==(00000000000000000000111111111111)2
	DWORD OffsetOfPageTab = ((virtualAdd >> 12)&0x3ff)<<2;	// �м�ʮλ��0x3ff==(1111111111)2��ҳ����ƫ����  ������λ��
	//�൱�ڳ���4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// ȡ��22λ

	DWORD Dir = (virtualAdd >> 22)<<2;				// ǰʮλ  ҳĿ¼����ƫ����  ������λ���൱�ڳ���4	
	DWORD result(0);                             //����ֵ
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // �洢�Ӿ�������ȡ�����ַ�
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);	

	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 2����ȡPDE
	file.Seek((mPageDirectoryBase+Dir),CFile::begin);  // PDE��λ��ΪdirTableBase+Dir
	file.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPDE   ��ҳĿ¼��Ҫ��Ҫλ�ƣ�
	/*
	trueValue[0] = tmp[3];		
	trueValue[1] = tmp[2];
	trueValue[2] = tmp[1];
	trueValue[3] = tmp[0];
	*/
	tmpAddress = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	PDE = *tmpAddress;			// ҳĿ¼���ֵ



	// 3������PDE�жϴ˵�ַת���Ƿ��ҳģʽ
	isLargePage = (PDE&0x80)>>7;		// �õ�8λ(��0λ��ʼ����������)�Ƿ�Ϊ0�ж��Ƿ��Ǵ�ҳģʽ�����Ǵ�ҳģʽ��ֵΪ1��TRUE�����Ǵ�ҳģʽֵΪ0��FALSE

	// 4�����ݷ�ҳģʽ��ȷ����һ���Ĺ���
	if (isLargePage == TRUE)
	{
		// 4.1����ҳģʽ

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
		//  4.2��Сҳģʽ

		// 4.2.1 �õ�ҳ�����ַ
		PDE = ((*tmpAddress)>>12)<<12;	// ҳ��СΪ4KBʱҳĿ¼���12λ����

		// 4.2.2 �õ�PTE
		file.Seek((((PDE>>12)<<12)+OffsetOfPageTab),CFile::begin);	// PDE��12λ���㣬PTE��λ��Ϊ (PDE��12λ�����)+Table
		file.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPTE
		/*		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		*/
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	 // ҳ�����12λ����
		/*file.Seek((PTE+Offset),CFile::begin);	// PA��λ��ΪPTE+Offset
		file.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPTE
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		tmpAddress = (DWORD*)trueValue;*/

		// 4.2.3 �õ������ַ
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

