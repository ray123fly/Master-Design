#include "stdafx.h"
#include "VAtoPAXP.h"
#include <fstream>
using namespace std;

void ExtractProcessMemoryXP(PROCESS pro,CString imgFilePath)
{
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	DWORD PDE(0),PTE(0);

	CArray<DWORD,DWORD>*pPageDirectory,*pPageTable;
	pPageDirectory = new CArray<DWORD,DWORD>;
	pPageTable = new CArray<DWORD,DWORD>;

	char* pageBuf = new char[PAGE_SIZE];
	char* largePageBuf = new char[PAGE_SIZE_LARGE];

	CString ProFilePath = ".\\Mem_"+pro.strName+".raw";

	CFile ImgFile(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	CFile ProcessFile(ProFilePath,CFile::modeCreate|CFile::modeWrite);

	pPageDirectory = GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,imgFilePath);//一次获取一个页面的页目录项

	for( int DirIndex=0; DirIndex < PDE_COUNT ; DirIndex++)
	{
		//另一种方法:利用数组类
		PDE=pPageDirectory->GetAt(DirIndex);

		if (valide !=GetPdeOrPteType(PDE))
		{
			continue;

		}

		//////////////////////////////////////////////////////////////////////////
		// 此时判断这个地址转换是否用大页模式
		isLargePage = (PDE&0x80)>>7;		// 用第8位(从0位开始，从右至左)是否为0判断是否是大页模式，若是大页模式则值为1即TRUE，不是大页模式值为0即FALSE

		if (isLargePage == TRUE)
		{
			//提页表
			largePageBuf = GetPhysicalPageData(PDE,PAGE_SIZE_LARGE,imgFilePath);//一次读取一个页面的数据，人后写到ProcessFile中
			ProcessFile.Write(largePageBuf,PAGE_SIZE_LARGE);
		}
		else//非大页模式
		{
			
		//提页表
			pPageTable = GetPageTable(PDE,PAGE_SIZE,imgFilePath);//一次获取一个页面数据，即使1024个页表项
		
			for (int TableIndex = 0;TableIndex < PTE_COUNT;TableIndex++)
			{
				
				PTE = pPageTable->GetAt(TableIndex);
				if (valide == GetPdeOrPteType(PTE))//页面有效就读取整个页面数据
				{
					pageBuf = GetPhysicalPageData(PTE,PAGE_SIZE,imgFilePath);
					ProcessFile.Write(pageBuf,PAGE_SIZE);
				}
			}
		}
	}
	free(pageBuf);
	free(largePageBuf);
	ProcessFile.Close();
	ImgFile.Close();
}
