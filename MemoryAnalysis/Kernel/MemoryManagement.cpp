// PDE PTE相关的数据和操作
#include "stdafx.h"
#include "MemoryManagement.h"

// 判断pae
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

// 得到标志位信息
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

// 分别判断 PDE 和 PTE 的各个标志位的信息
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

// 得到页目录
void GetPageDirTable(DWORD baseAddrOri,int pageDirSize,CFile &ImgFile,CArray<DWORD,DWORD> *pPageDir)
{
	pPageDir->RemoveAll();
	DWORD *tmpAddress;
	DWORD PDE;
	char* pTempPageDir=new char[pageDirSize];//
	char* pPointer=pTempPageDir;

	DWORD baseAddr=PteToData(baseAddrOri);//可以考虑用内联函数

	ImgFile.Seek(baseAddr,CFile::begin);
	ImgFile.Read(pTempPageDir,pageDirSize);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PDE   ？页目录项要不要位移？

	char TrueValue[4];
	for (int Index=0;Index<1024;Index++)
	{
		// 区别于：大小端转化，这个地方不需要
		TrueValue[0] = pPointer[4*Index+0];
		TrueValue[1] = pPointer[4*Index+1];
		TrueValue[2] = pPointer[4*Index+2];
		TrueValue[3] = pPointer[4*Index+3];

		tmpAddress = (DWORD*)TrueValue;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒

		PDE = *tmpAddress;			// 页目录项

		//将PDE放入数组中
		pPageDir->Add(PDE);
	}
	pPointer=NULL;
	//delete[] pTempPageDir;//问题就出在这里
	delete[] pTempPageDir;
	tmpAddress=NULL;
	//return pPageDir;
}

// 得到页表
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

		//将PTE放入数组中
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

// 提取进程内存
void NoPaeDumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//AfxMessageBox("Go!");
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	DWORD PDE(0),PTE(0);

	CArray<DWORD,DWORD>*pPageDirectory,*pPageTable;//分别存放页目录表和页表
	pPageDirectory = new CArray<DWORD,DWORD>;
	pPageTable = new CArray<DWORD,DWORD>;

	pPageDirectory->RemoveAll();
	pPageTable->RemoveAll();

	//物理页面导出是用到的临时空间
	char* pPageDataBuf = new char[PAGE_SIZE];
	char* pLargePageDataBuf = new char[PAGE_SIZE_LARGE];

	//镜像文件一处打开，多处使用
	CFile ImgFile(imgFilePath,CFile::modeReadWrite);

	//进程Pro的内存导出文件
	CString strPid;
	strPid.Format("%d",pro.Pid);
	CString ProDmpFilePath = ".\\Mem-PageTable-"+strPid+"-"+pro.strName+".raw";
	CFile ProcDmpFile(ProDmpFilePath,CFile::modeCreate|CFile::modeReadWrite);

	//一次获取一个页面的页目录项
	GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,ImgFile,pPageDirectory);

	for( int DirIndex=0; DirIndex < 512 ; DirIndex++)
	{
	
		//pProgressDlg->ShowWindow(SW_SHOW);
		//另一种方法:利用数组类
		PDE=pPageDirectory->GetAt(DirIndex);

		if (valide !=GetPdeOrPteType(PDE))
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// 此时判断这个地址转换是否用大页模式
		isLargePage = isLargePageMode(PDE);	// 用第8位(从0位开始，从右至左)是否为0判断是否是大页模式，若是大页模式则值为1即TRUE，不是大页模式值为0即FALSE

		if (isLargePage == TRUE)
		{
			//提页表
			//PDE转化
			GetPhysicalPageData(PDE,isLargePage,ImgFile,pLargePageDataBuf);//一次读取一个页面的数据，然后写到ProcessFile中
			//ProcDmpFile.SeekToEnd();
			ProcDmpFile.Seek(0,CFile::end);
			ProcDmpFile.Write(pLargePageDataBuf,PAGE_SIZE_LARGE);
		}
		else//非大页模式
		{
			//提页表
			GetPageTable(PDE,PAGE_SIZE,ImgFile,pPageTable);//一次获取一个页面数据，即使1024个页表项

			for (int TableIndex = 0;TableIndex < PTE_COUNT;TableIndex++)
			{

				PTE = pPageTable->GetAt(TableIndex);

				if (valide == GetPdeOrPteType(PTE))//页面有效就读取整个页面数据
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

// 分别判断 PDE 和 PTE 的各个标志位的信息
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

	char* pTempPageDirPointerTab=new char[tableSize];// 页目录指针表
	INT64 baseAddr=PaeCR3ToPDPTBase(cr3);//页目录指针表基地址

	imgFile.Seek(baseAddr,CFile::begin);
	imgFile.Read(pTempPageDirPointerTab,tableSize);  // 将页目录指针表读取到pTempPageDirTab中

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char* pPointer=pTempPageDirPointerTab;

	INT64 *tmpAddress; // 大小端转换
	INT64 PDPTE;

	for (int Index=0;Index<4;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒

		PDPTE= *tmpAddress;       		// 页目录指针表项

		//PDPTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		//测试
		CString strPdpte;
		strPdpte.Format("PDPTE: %x",PDPTE);
		AfxMessageBox(strPdpte);

		// 将PDPT放入数组中
		pPageDirPointerTab->Add(PDPTE);
		pPointer+=8;
	}
	
	// delete[] pTempPageDir;//问题就出在这里
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

	char* pTempPageDir=new char[pageDirSize];// 页目录指针表
	INT64 base=PaePteToDataBase(baseAddr);//页目录表基地址

	ImgFile.Seek(base,CFile::begin);
	ImgFile.Read(pTempPageDir,pageDirSize);  // 将页目录指针表读取到pTempPageDirTab中

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char* pPointer=pTempPageDir;

	INT64 *tmpAddress; // 大小端转换
	INT64 PDTE;

	for (int Index=0;Index<512;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒

		PDTE= *tmpAddress;       		// 页目录表项
		//PDTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		//测试
	/*	CString strPdte;
		strPdte.Format("PDTE: %x",PDTE);
		AfxMessageBox(strPdte);
    */
		// 将PDPT放入数组中
		pPageDir->Add(PDTE);
		pPointer+=8;
	}
	
	// delete[] pTempPageDir;//问题就出在这里
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

// 大页模式——数据（2M）
INT64 PaePdeToDataBase(INT64 pde,BOOL isLarge)
{
	INT64 realPte;
	if (isLarge)// 大页35-21
	{
		realPte=pde&0xfffe00000;
	} 
	else// 小页35-12
	{
		realPte=pde&0xffffff000;
	}
	return realPte;
}

void PaeGetPageTable(INT64 pdeOri,int pageTableSize,CFile &ImgFile,CArray<INT64,INT64> *pPageTab)
{
	pPageTab->RemoveAll();

	char* pTempPageTable=new char[pageTableSize];// 页目录指针表
	INT64 base=PaePdeToPageTableBase(pdeOri);//页目录表基地址

	ImgFile.Seek(base,CFile::begin);
	ImgFile.Read(pTempPageTable,pageTableSize);  // 将页表读取到pTempPageTab中

	char* TrueValue=new char[8];
	//TrueValue[8]='\0';
	char *p=pTempPageTable;
	char* pPointer=pTempPageTable;

	INT64 *tmpAddress; // 大小端转换
	INT64 PTE;

	for (int Index=0;Index<512;Index++)
	{
		strncpy(TrueValue,pPointer,8);
		tmpAddress = (INT64*)TrueValue;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
		PTE= *tmpAddress;       		// 页目录表项
		//PTE=strToi64(TrueValue);
		//////////////////////////////////////////////////////////////////////////
		 //测试
	/*	CString strPte;
		strPte.Format("PTE: %x",PTE);
		AfxMessageBox(strPte);
	*/
		// 将PDPT放入数组中
		pPageTab->Add(PTE);
		pPointer+=8;
	}

	delete[] pTempPageTable;
	delete[] TrueValue;
	tmpAddress=NULL;
	pPointer=NULL;
}

// 小页模式——数据（4K）
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

// 提取进程内存
void PaeDumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//AfxMessageBox("Go!");
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	INT64 PDPTE(0),PDE(0),PTE(0);

	CArray<INT64,INT64> *pPageDirectoryPointerTable,*pPageDirectory,*pPageTable;//分别存放页目录指针表，页目录，和页表

	pPageDirectoryPointerTable=new CArray<INT64,INT64>;
	pPageDirectory = new CArray<INT64,INT64>;
	pPageTable = new CArray<INT64,INT64>;

	//物理页面导出是用到的临时空间
	char* pPageDataBuf = new char[PAE_PAGE_SIZE];
	char* pLargePageDataBuf = new char[PAE_PAGE_SIZE_LARGE];

	//镜像文件一处打开，多处使用
	CFile ImgFile(imgFilePath,CFile::modeReadWrite);

	//进程Pro的内存导出文件
	CString strPid;
	strPid.Format("%d",pro.Pid);
	CString ProDmpFilePath = ".\\Mem-PageTable-"+strPid+"-"+pro.strName+".raw";
	CFile ProcDmpFile(ProDmpFilePath,CFile::modeCreate|CFile::modeReadWrite);

	//////////////////////////////////////////////////////////////////////////
	//开始提取

	//AfxMessageBox(pro.DirectoryTableBase);
	// 1、一次取出四个页目录指针
	PaeGetPageDirPointerTable(pro.DirectoryTableBase,32,ImgFile,pPageDirectoryPointerTable);

	// 循环提出四个页目录空间的数据
	//for (int i=0;i<2;i++) //用户空间
	for (int i=0;i<4;i++)
	{
		// 依次获取每一个页目录表基地址（即页目录指针表项）
		PDPTE=pPageDirectoryPointerTable->GetAt(i);
	/*	if (!PaeIsValid(PDPTE))// 页目录指针表项所指向的页目录不在内存
		{
			continue;
		}
    */
		// 页目录指针表项所指向的页目录在内存

		// 2、一次获取一个页面的页目录项
		PaeGetPageDirTable(PDPTE,PAE_PAGE_SIZE,ImgFile,pPageDirectory);

		for( int DirIndex=0; DirIndex < PAE_PDE_COUNT ; DirIndex++)
		{
			PDE=pPageDirectory->GetAt(DirIndex);

			if (!PaeIsValid(PDE))// 页目录项所指向的页表不在内存
			{
				continue;
			}

			//////////////////////////////////////////////////////////////////////////
			// 此时判断这个地址转换是否用大页模式
			isLargePage = PaeIsLargePageMode(PDE);	// 用第8位(从0位开始，从右至左)是否为0判断是否是大页模式，若是大页模式则值为1即TRUE，不是大页模式值为0即FALSE

			if (isLargePage == TRUE) // 大页模式
			{
				//提物理页
				//PDE转化
				// 3、提大页数据
				PaeGetPhysicalPageData(PDE,isLargePage,ImgFile,pLargePageDataBuf);//一次读取一个页面的数据，然后写到ProcessFile中
				//ProcDmpFile.SeekToEnd();
				ProcDmpFile.Seek(0,CFile::end);
				ProcDmpFile.Write(pLargePageDataBuf,PAE_PAGE_SIZE_LARGE);
			}
			else // 小页模式
			{
				// 3、提页表
				PaeGetPageTable(PDE,PAE_PAGE_SIZE,ImgFile,pPageTable);//一次获取一个页面数据，即使1024个页表项

				for (int TableIndex = 0;TableIndex < PAE_PTE_COUNT;TableIndex++)
				{
					PTE = pPageTable->GetAt(TableIndex);

					if (PaeIsValid(PTE))//页面有效就读取整个页面数据
					{
						// 4、提小页数据
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
// 进程内存导出——统一接口

// 提取进程内存
void DumpProcessMemory(PROCESS pro,CString imgFilePath)
{
	//判断是pae？no-pae
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
//镜像操作

// 将敬凯的函数进行修改即可
DWORD VirtualToPhysicalAddr(DWORD mPageDirectoryBase,DWORD virtualAdd,CString filePath)
{
	DWORD physicaAddr(0);

	//////////////////////////////////////////////////////////////////////////
	// 先判断是否为大页模式——根据PDE的值来判断

	// 一下情况适合于页大小为4KB，非PAE模式，不能适用于大页模式，内核变量好像都是用大页模式


	// 1、准备工作
	DWORD OffsetNoLargePage = virtualAdd&0xfff;		// 不是大页模式时的页内偏移量  后十二位，0xfff==(00000000000000000000111111111111)2
	DWORD OffsetOfPageTab = ((virtualAdd >> 12)&0x3ff)<<2;	// 中间十位，0x3ff==(1111111111)2，页表内偏移量  左移两位，
	//相当于乘以4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// 取后22位

	DWORD Dir = (virtualAdd >> 22)<<2;				// 前十位  页目录表内偏移量  左移两位，相当于乘以4	
	DWORD result(0);                             //返回值
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // 存储从镜像中提取出的字符
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);	

	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 2、读取PDE
	file.Seek((mPageDirectoryBase+Dir),CFile::begin);  // PDE的位置为dirTableBase+Dir
	file.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PDE   ？页目录项要不要位移？
	/*
	trueValue[0] = tmp[3];		
	trueValue[1] = tmp[2];
	trueValue[2] = tmp[1];
	trueValue[3] = tmp[0];
	*/
	tmpAddress = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	PDE = *tmpAddress;			// 页目录项的值



	// 3、根据PDE判断此地址转换是否大页模式
	isLargePage = (PDE&0x80)>>7;		// 用第8位(从0位开始，从右至左)是否为0判断是否是大页模式，若是大页模式则值为1即TRUE，不是大页模式值为0即FALSE

	// 4、根据分页模式来确定下一步的工作
	if (isLargePage == TRUE)
	{
		// 4.1、大页模式

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
		//  4.2、小页模式

		// 4.2.1 得到页表基地址
		PDE = ((*tmpAddress)>>12)<<12;	// 页大小为4KB时页目录项，后12位置零

		// 4.2.2 得到PTE
		file.Seek((((PDE>>12)<<12)+OffsetOfPageTab),CFile::begin);	// PDE后12位置零，PTE的位置为 (PDE后12位置零后)+Table
		file.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PTE
		/*		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		*/
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	 // 页表项，后12位置零
		/*file.Seek((PTE+Offset),CFile::begin);	// PA的位置为PTE+Offset
		file.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PTE
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
		tmpAddress = (DWORD*)trueValue;*/

		// 4.2.3 得到物理地址
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

