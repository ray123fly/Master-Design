#include "stdafx.h"
#include "VAtoPAXP.h"
#include <fstream>
using namespace std;

void ExtractProcessMemoryXP(PROCESS pro,CString imgFilePath)
{
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	DWORD PDE(0),PTE(0);

	CArray<DWORD,DWORD>*pPageDirectory,*pPageTable;
	pPageDirectory = new CArray<DWORD,DWORD>;
	pPageTable = new CArray<DWORD,DWORD>;

	char* pageBuf = new char[PAGE_SIZE];
	char* largePageBuf = new char[PAGE_SIZE_LARGE];

	CString ProFilePath = ".\\Mem_"+pro.strName+".raw";

	CFile ImgFile(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	CFile ProcessFile(ProFilePath,CFile::modeCreate|CFile::modeWrite);

	pPageDirectory = GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,imgFilePath);//һ�λ�ȡһ��ҳ���ҳĿ¼��

	for( int DirIndex=0; DirIndex < PDE_COUNT ; DirIndex++)
	{
		//��һ�ַ���:����������
		PDE=pPageDirectory->GetAt(DirIndex);

		if (valide !=GetPdeOrPteType(PDE))
		{
			continue;

		}

		//////////////////////////////////////////////////////////////////////////
		// ��ʱ�ж������ַת���Ƿ��ô�ҳģʽ
		isLargePage = (PDE&0x80)>>7;		// �õ�8λ(��0λ��ʼ����������)�Ƿ�Ϊ0�ж��Ƿ��Ǵ�ҳģʽ�����Ǵ�ҳģʽ��ֵΪ1��TRUE�����Ǵ�ҳģʽֵΪ0��FALSE

		if (isLargePage == TRUE)
		{
			//��ҳ��
			largePageBuf = GetPhysicalPageData(PDE,PAGE_SIZE_LARGE,imgFilePath);//һ�ζ�ȡһ��ҳ������ݣ��˺�д��ProcessFile��
			ProcessFile.Write(largePageBuf,PAGE_SIZE_LARGE);
		}
		else//�Ǵ�ҳģʽ
		{
			
		//��ҳ��
			pPageTable = GetPageTable(PDE,PAGE_SIZE,imgFilePath);//һ�λ�ȡһ��ҳ�����ݣ���ʹ1024��ҳ����
		
			for (int TableIndex = 0;TableIndex < PTE_COUNT;TableIndex++)
			{
				
				PTE = pPageTable->GetAt(TableIndex);
				if (valide == GetPdeOrPteType(PTE))//ҳ����Ч�Ͷ�ȡ����ҳ������
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
