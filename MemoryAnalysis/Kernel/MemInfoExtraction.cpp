#include "StdAfx.h"
#include "MemInfoExtraction.h"

#include "MemoryManagement.h"

#include <iostream>
#include <fstream>

using namespace std;

#define KPCR_LENGTH 0xd70   //kpcr��С  ��
#define KPCR_SEARCH_SIZE 8

//#define	XPEPROCESSLENGTH 0x25c  //XP�µ�eprocess��С
//#define	XPEPROCESSLENGTH 0x25c  //XP�µ�eprocess��С

const DWORD EPROCESS_LENGTH_XP = 0x25c;
const DWORD EPROCESS_LENGTH_WIN7 = 0x2c0;
//const DWORD PAGE_SIZE = 4096;



MemInfoExtraction::MemInfoExtraction(void)
{
	//////////////////////////////////////////////////////////////////////////
	// XP��
	pKpcrResultXP = new char[KPCR_LENGTH];
	bKpcrExistXP = FALSE;
	
	// m_pXPEprocessResult��ά���������ڴ�
	ppEprocessResultXP = new char*[100]; // �����һ�ٸ�
	for(int i=0;i<100;i++)
	{
		ppEprocessResultXP[i] = new char[EPROCESS_LENGTH_XP];
	}
	pProcessMemoryXP = NULL;

	mEprocessActiveAmountXP = 0;		// ���̵�������ʼ��Ϊ����

	//////////////////////////////////////////////////////////////////////////
	// WIN7��

	

	pKpcrResultWin7 = new char[KPCR_LENGTH];
	// m_pWIN7EprocessResult��ά���������ڴ�

	ppEprocessResultWin7 = new char*[100]; // �����һ�ٸ�
	for(int i=0;i<100;i++)
	{
		ppEprocessResultWin7[i] = new char[EPROCESS_LENGTH_WIN7];
	}
	pProcessMemoryWin7 = NULL;

	
	//////////////////////////////////////////////////////////////////////////
	pPageDataBuf = new char[PAGE_SIZE];
}

MemInfoExtraction::~MemInfoExtraction(void)
{
	//////////////////////////////////////////////////////////////////////////
	// XP��
	delete[] pKpcrResultXP;
	pKpcrResultXP = NULL;
	//////////////////////////////////////////////////////////////////////////
	// m_pXPEprocessResult��ά�����ͷ��ڴ�
	//delete[] ppEprocessResultXP;
	for (int i=0;i<100;i++)
	{
		delete[] ppEprocessResultXP[i];
		ppEprocessResultXP[i] = NULL;
	}
	delete[] ppEprocessResultXP;
	ppEprocessResultXP = NULL;

	delete[] pProcessMemoryXP;

	//////////////////////////////////////////////////////////////////////////
	// WIN7��
	delete[] pKpcrResultWin7;
	pKpcrResultWin7 = NULL;
	//////////////////////////////////////////////////////////////////////////
	// m_pWIN7EprocessResult��ά�����ͷ��ڴ�
	//delete[] ppEprocessResultWin7;
	for (int i=0;i<100;i++)
	{
		delete[] ppEprocessResultWin7[i];
		ppEprocessResultWin7[i] = NULL;
	}
	delete[] ppEprocessResultWin7;
	ppEprocessResultWin7 = NULL;

	delete[] pProcessMemoryWin7;

	//////////////////////////////////////////////////////////////////////////
	delete[] pPageDataBuf;
}

//////////////////////////////////////////////////////////////////////////
// pMemBlock:������ַ�����ͷָ��
// mBlockSize:������ַ����ĳ���
void MemInfoExtraction::ExtractKpcrXP(const char* pMemBlock,ULONGLONG  mBlockSize)
{
	if(!(pMemBlock&&mBlockSize))// ��ֹ�������������������һ��Ϊ���򷵻�
		return;
	//pKpcrResultXP = new char[KPCRLENGTH];

	DWORD loops(0);
	if(mBlockSize>4096)		//��ֹ����С��������
		loops = mBlockSize - 4096; //kpcr�����Ե�ַΪ0xffdff000������һ��ҳ�ĵ�һ�
	                               //���Լ��������һҳ��ͷ���ɣ�4k
	bKpcrExistXP = FALSE;  // ��ʼ��ΪFALSE��λ���ǲ���Ӧ�÷��ڹ��캯�����أ���
	
	//////////////////////////////////////////////////////////////////////////
	// Ҫƥ����ַ�
	char pattern[] = {0x00,0xf0,0xdf,0xff,0x20,0xf1,0xdf,0xff}; 

	int matchAmount = 0;// ����ƥ�������

	//////////////////////////////////////////////////////////////////////////
	// ƥ���㷨��4k,4k(4k=4096)��ƥ�䣬������ֵ��ҳ��ƫ��0x1c��ͬʱ��KPCR��ƫ��0X1c������ʮ����28��
	for(DWORD i=0; i < loops; i+=4096)
	{
		int j = 0;
		for(; j < KPCR_SEARCH_SIZE;j++ )
		{
			if(pMemBlock[i+0x1c+j] == pattern[j])
				++matchAmount;    // ƥ��һ����ĸj�Լ�һ
			else
				break;
		}
		if(matchAmount == KPCR_SEARCH_SIZE) // ����һ��ƥ��Ķ���			
		{
			for(int k=0;k<KPCR_LENGTH;++k)
			{
				pKpcrResultXP[k]=pMemBlock[i+k];  //00f0dfff20f1dfff��λ�ü�ȥ0x01c��ʮ����28,�õ�KPCR��λ��
			}
			bKpcrExistXP = TRUE;
			return;
		}
		matchAmount = 0;//��matchAmount����
	}
	
}


char* MemInfoExtraction::GetKpcrResultXP()
{
	if(bKpcrExistXP==TRUE)
		return pKpcrResultXP;
	else
		return NULL;
}

bool MemInfoExtraction::IsKpcrExistXP()
{
	return bKpcrExistXP;
}

// XP ���̽ṹ��ȡ

void MemInfoExtraction::ExtractEprocessXP(const char* MemBlock,DWORD MemBlockSize)
{
	if(!(MemBlock&&MemBlockSize)) // 
		return;

	DWORD loops = 0;
	if(MemBlockSize>EPROCESS_LENGTH_XP)
		loops = MemBlockSize - EPROCESS_LENGTH_XP;
	//mEprocessActiveAmountXP = 0;		// �µĵ��ã����̵���������
	//////////////////////////////////////////////////////////////////////////
	// Ҫƥ����ַ�
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime ���еĽ���Ϊ0x00000000
	char pattern2[] = {0xfd,0x7f};							     // Peb��ַ��ֵ����Ϊ0x7ffdxxxx��ʵ���ڴ��еĴ洢��ʽ��0xxxxxfd7f ��intelС��ģʽ��
	char pattern3[] = {0x03,0x01,0x00,0x00};					 // ExitStatus  ֵΪ0x00000103���ڴ���ʵ�ʴ洢��ֵΪ0x03010000

	int matchAmount = 0;// ����ƥ�������
	
	//////////////////////////////////////////////////////////////////////////
	// ƥ���㷨
	// ERPOCESS�ṹ�϶���һ��ҳ�ڣ�ֻ��һ��ҳ��Ѱ�ң�Ч�ʸ���
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(MemBlockSize>4096)  //��ֹmemLenС��4096Խ�����
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_XP;j++)  //������<����<=�� ��ֹ©����Ӧ����ģ�����������⣺����XPEPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(MemBlock[i+j+0x078+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(MemBlock[i+j+0x1b2+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(MemBlock[i+j+0x24c+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_XP;l++)
					{
						ppEprocessResultXP[mEprocessActiveAmountXP][l] = MemBlock[i+j+l];
					}
					mEprocessActiveAmountXP++;	//ƥ��һ���Լ�һ					
				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}
		else					// memLenС��4096ʱ
		{
			for(DWORD j=0;j<=MemBlockSize-EPROCESS_LENGTH_XP;j++)		//������<����<=�� ��ֹ©����Ӧ����ģ���memLen==XPEPROCESSLENTHʱ
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(MemBlock[i+j+0x078+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(MemBlock[i+j+0x1b2+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(MemBlock[i+j+0x24c+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_XP;l++)
					{
						//ppEprocessResultXP[mEprocessActiveAmountXP][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountXP++;	//ƥ��һ���Լ�һ

				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}

		MemBlockSize = MemBlockSize - 4096; //ƥ����һ���Լ�4k
	}
}

char* MemInfoExtraction::GetEprocessResultXP(int i)
{
	return ppEprocessResultXP[i];
}

int MemInfoExtraction::GetEprocessAmountXP()
{
	return mEprocessActiveAmountXP;
}

void MemInfoExtraction::EprocessAmountToZeroXP()
{
	mEprocessActiveAmountXP = 0;
}



//////////////////////////////////////////////////////////////////////////
// ������WIN7��Ѱ��EPROCESS�ĺ���
/*
void MemInfoExtraction::ExtractEprocessWin7(const char* Memory,DWORD memLen)
{
	if(!(Memory&&memLen)) // ��ֹ�������������������һ��Ϊ���򷵻�
		return;
	//ppEprocessResultWin7 = new char[WIN7EPROCESSLENGTH];

	
	DWORD loops = 0;
	if(memLen>EPROCESS_LENGTH_WIN7)
		loops = memLen - EPROCESS_LENGTH_WIN7;
	mEprocessActiveAmountWin7 = 0;		// �µĵ��ã����̵���������
	
	//////////////////////////////////////////////////////////////////////////
	// Ҫƥ����ַ�
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime ���еĽ���Ϊ0x00000000
	char pattern2[] = {0xfd,0x7f};								// Peb��ַ��ֵ����Ϊ0x7ffdxxxx��ʵ���ڴ��еĴ洢��ʽ��0xxxxxfd7f ��intelС��ģʽ��
	char pattern3[] = {0x03,0x01,0x00,0x00};					// ExitStatus  ֵΪ0x00000103���ڴ���ʵ�ʴ洢��ֵΪ0x03010000


	int matchAmount = 0;		// ����ƥ�������
	
	//////////////////////////////////////////////////////////////////////////
	// ƥ���㷨
	// ERPOCESS�ṹ�϶���һ��ҳ�ڣ�ֻ��һ��ҳ��Ѱ�ң�Ч�ʸ���
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //��ֹmemLenС��4096Խ�����
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_WIN7;j++)  //������<����<=�� ��ֹ©����Ӧ����ģ�����������⣺����WIN7EPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}

					mEprocessActiveAmountWin7++;	//ƥ��һ���Լ�һ
					
				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}
		else// memLenС��4096ʱ
		{
			for(DWORD j=0;j<=memLen-EPROCESS_LENGTH_WIN7;j++)		//������<����<=�� ��ֹ©����Ӧ����ģ���memLen==WIN7EPROCESSLENTHʱ
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountWin7++;	//ƥ��һ���Լ�һ

				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}

		memLen = memLen - 4096; //ƥ����һ���Լ�4k
	}
	//////////////////////////////////////////////////////////////////////////

}
*/

void MemInfoExtraction::ExtractEprocessWin7(const char* Memory,DWORD memLen)
{
	if(!(Memory&&memLen)) // ��ֹ�������������������һ��Ϊ���򷵻�
		return;
	//m_pWIN7EprocessResult = new char[WIN7EPROCESSLENGTH];

	
	DWORD loops = 0;
	if(memLen>EPROCESS_LENGTH_WIN7)
		loops = memLen - EPROCESS_LENGTH_WIN7;
	mEprocessActiveAmountWin7 = 0;		// �µĵ��ã����̵���������
	
	//////////////////////////////////////////////////////////////////////////
	// Ҫƥ����ַ�
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime ���еĽ���Ϊ0x00000000
	char pattern2[] = {0xfd,0x7f};								// Peb��ַ��ֵ����Ϊ0x7ffdxxxx��ʵ���ڴ��еĴ洢��ʽ��0xxxxxfd7f ��intelС��ģʽ��
	char pattern3[] = {0x03,0x01,0x00,0x00};					// ExitStatus  ֵΪ0x00000103���ڴ���ʵ�ʴ洢��ֵΪ0x03010000


	int matchAmount = 0;		// ����ƥ�������
	

	//////////////////////////////////////////////////////////////////////////
	// ƥ���㷨
	// ERPOCESS�ṹ�϶���һ��ҳ�ڣ�ֻ��һ��ҳ��Ѱ�ң�Ч�ʸ���
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //��ֹmemLenС��4096Խ�����
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_WIN7;j++)  //������<����<=�� ��ֹ©����Ӧ����ģ�����������⣺����WIN7EPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l]=Memory[i+j+l];
					}										
					//////////////////////////////////////////////////////////////////////////


					mEprocessActiveAmountWin7++;	//ƥ��һ���Լ�һ
					
				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}
		else					// memLenС��4096ʱ
		{
			for(int j=0;j<=memLen-EPROCESS_LENGTH_WIN7;j++)		//������<����<=�� ��ֹ©����Ӧ����ģ���memLen==WIN7EPROCESSLENTHʱ
			{
				int k;
				for(k=0;k<8;k++)					//pattern1�ĳ���Ϊ8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTimeλ����EPROCESS��+0x078��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Pebָ��λ����EPROCESS��+0x1b0��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatusλ����EPROCESS��+0x24c��
						++matchAmount;		// ƥ��һ����ĸmatchAmount�Լ�һ
					else
						break;
				}
				if (matchAmount==14)  //���������������ֵ��ƥ�䣨�ܹ�ռ14���ֽڣ�
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountWin7++;	//ƥ��һ���Լ�һ

				}
				matchAmount = 0;		// ���㣬��ֹ�ۼ�
			}
		}

		memLen = memLen - 4096; //ƥ����һ���Լ�4k
	}
	//////////////////////////////////////////////////////////////////////////

}



char* MemInfoExtraction::GetEprocessResultWin7(int i)
{
	return ppEprocessResultWin7[i];

}

int MemInfoExtraction::GetEprocessAmountWin7()
{
	return mEprocessActiveAmountWin7;
}

void MemInfoExtraction::EprocessAmountToZeroWin7()
{
	mEprocessActiveAmountWin7 = 0;
}





void MemInfoExtraction::ExtractKpcrWin7( char* Memory,DWORD memLen)
{
	if(!(Memory&&memLen)) // ��ֹ�������������������һ��Ϊ���򷵻�
		return;

	DWORD loops = 0;
	if(memLen>0x120)		//��ֹ����С��������
		loops = memLen - 0x120; //


	mKpcrAmountWin7 = 0;		// 
	

	//////////////////////////////////////////////////////////////////////////
	// Ҫƥ����ַ�
	char* original1;	//  �洢ԭʼ���ַ���
	char* original2;
	char trueKpcrVAValue[4] = {0x00,0x00,0x00,0x00};	// �������ڴ��е�ֵת����ϵͳ��ʵ��ֵ  �洢ת������ַ���
	char trueKprcbValue[4] = {0x00,0x00,0x00,0x00};

	long* pattern1(0);	// longռ�ĸ��ֽڣ��洢�ĸ��Լ���ֵ�����а�λ������
	long* pattern2(0);

	bool kpcrVAisRight(FALSE),kprcbVAisRight(FALSE);  //  �����ĸ��ֽڵĸ�λ�͵�λ		8�ֽ���0x80000000��0x7fffffff�������Ϊ1֤�� ����>0x80000000
									// ���ڴ���ʵ�ʴ洢��ֵ��
	int offset(0);					// KPRCB�����KPCR��λ��
	bool offsetRight = FALSE;		// KPRCB�����KPCR��λ�ƴ�СΪ0x120,�����ȷ (32λWindows 7����0x120)

	int matchAmount = 0;		// ����ƥ�������
	

	//////////////////////////////////////////////////////////////////////////
	// ƥ���㷨
	// ERPOCESS�ṹ�϶���һ��ҳ�ڣ�ֻ��һ��ҳ��Ѱ�ң�Ч�ʸ���
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //��ֹmemLenС��4096Խ�����
		{
			for(int j=0;j<=4096;j++)  //
			{
				//pattern1 = (long*)Memory;
				//pattern2 = (long*)(Memory + 4);
				//////////////////////////////////////////////////////////////////////////
				// ��ȡԭʼ�ַ���
				original1 = Memory+i+j;
				original2 = Memory+i+j+ 4;
				//////////////////////////////////////////////////////////////////////////
				// ת����ϵͳ�ڵ�ʵ��ֵ
				trueKpcrVAValue[0] = original1[3];
				trueKpcrVAValue[1] = original1[2];
				trueKpcrVAValue[2] = original1[1];
				trueKpcrVAValue[3] = original1[0];

				trueKprcbValue[0] = original2[3];
				trueKprcbValue[1] = original2[2];
				trueKprcbValue[2] = original2[1];
				trueKprcbValue[3] = original2[0];

				//////////////////////////////////////////////////////////////////////////
				// �ַ�����λ��
				pattern1 = (long*)trueKpcrVAValue;
				pattern2 = (long*)trueKprcbValue;

				kpcrVAisRight = ((*pattern1)&0x80000000)&&((*pattern1)&0x7fffffff);
				kprcbVAisRight = ((*pattern2)&0x80000000)&&((*pattern2)&0x7fffffff);
				
				//////////////////////////////////////////////////////////////////////////
				// �鿴PRCB�����KPCR��λ�ƴ�СΪ0x120
				offset = *pattern2 - *pattern1;
				if (offset == 0x120)
				{
					offsetRight = TRUE;
				}


				//////////////////////////////////////////////////////////////////////////
				// �������ȷ
				if(kpcrVAisRight&&kprcbVAisRight&&offsetRight)
				{
					mKpcrAmountWin7++;	//ƥ��һ���Լ�һ
					/*CString num;
					num.Format("%d",mKpcrAmountWin7);                                                            
					CString result = "find"+num+"times";
					MessageBox(0,result,0,0);*/					
				}				
			}
		}
		else					// memLenС��4096ʱ
		{
			for(int j=0;j<=4096;j++)  //
			{
				//pattern1 = (long*)Memory;
				//pattern2 = (long*)(Memory + 4);
				//////////////////////////////////////////////////////////////////////////
				// ��ȡԭʼ�ַ���
				original1 = Memory+i+j;
				original2 = Memory+i+j+ 4;
				//////////////////////////////////////////////////////////////////////////
				// ת����ϵͳ�ڵ�ʵ��ֵ
				trueKpcrVAValue[0] = original1[3];
				trueKpcrVAValue[1] = original1[2];
				trueKpcrVAValue[2] = original1[1];
				trueKpcrVAValue[3] = original1[0];

				trueKprcbValue[0] = original2[3];
				trueKprcbValue[1] = original2[2];
				trueKprcbValue[2] = original2[1];
				trueKprcbValue[3] = original2[0];

				//////////////////////////////////////////////////////////////////////////
				// �ַ�����λ��
				pattern1 = (long*)trueKpcrVAValue;
				pattern2 = (long*)trueKprcbValue;

				kpcrVAisRight = ((*pattern1)&0x80000000)&&((*pattern1)&0x7fffffff);
				kprcbVAisRight = ((*pattern2)&0x80000000)&&((*pattern2)&0x7fffffff);

				//////////////////////////////////////////////////////////////////////////
				// �鿴PRCB�����KPCR��λ�ƴ�СΪ0x120
				offset = *pattern2 - *pattern1;
				if (offset == 0x120)
				{
					offsetRight = TRUE;
				}


				//////////////////////////////////////////////////////////////////////////
				// �������ȷ
				if(kpcrVAisRight&&kprcbVAisRight&&offsetRight)
				{
					mKpcrAmountWin7++;	//ƥ��һ���Լ�һ
					/*CString num;
					num.Format("%d",mKpcrAmountWin7);
					CString result = "find"+num+"times";
					MessageBox(0,result,0,0);	*/				
				}				
			}
		}

		memLen = memLen - 4096; //ƥ����һ���Լ�4k
	}
	//////////////////////////////////////////////////////////////////////////
}

int MemInfoExtraction::GetKpcrAmountWin7()
{
	return mKpcrAmountWin7;
}

//////////////////////////////////////////////////////////////////////////
//  ��ȡ�����ڴ�---Page Table

void MemInfoExtraction::ExtractProcessMemoryPageTableXP(PROCESS pro,CString imgFilePath)
{
	//�ж���pae��no-pae

	/*
	if (isPae(pro.DirectoryTableBase,imgFilePath))
	{
	AfxMessageBox("pae mode!");
	}
	else
	AfxMessageBox("no-pae mode!");
	*/

	// ����ͳһ�Ľӿ�
	DumpProcessMemory(pro,imgFilePath);
/*
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

	//������������ʾ���ȡ���ʹ�÷�ģ̬�Ի���
	CProgressDlg *pProgressDlg=new CProgressDlg;
	pProgressDlg->SetProgressRange(0,512);//�����أ�
	int curPosition=0;
	pProgressDlg->Create();
	pProgressDlg->CenterWindow();
	pProgressDlg->ShowWindow(SW_SHOW);

	//һ�λ�ȡһ��ҳ���ҳĿ¼��
	GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,ImgFile,pPageDirectory);

	for( int DirIndex=0; DirIndex < 512 ; DirIndex++)
	{
		pProgressDlg->UpdateProgressCtrl(DirIndex);
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

	pProgressDlg->Close();
	delete pProgressDlg;
	pProgressDlg=NULL;
	delete[] pPageDataBuf;
	delete[] pLargePageDataBuf;
	delete pPageDirectory;
	delete pPageTable;

	ProcDmpFile.Close();
	ImgFile.Close();

*/
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ�����ڴ�----VadTree

// �Ľ���

void MemInfoExtraction::ExtractProcessMemoryVadTreeXP(PROCESS pro,CString imgFilePath)
{
	// �õ�EP[porcessNum]�ڵ�ҳĿ¼����ַ����vadRoot��ֵ��Ȼ���ڽ����������traverse

	DWORD dirTableBase(0);	// ��12λΪ��־λ����dirTableBaseOri��12λ����
	DWORD vadRootVA(0);

	// 1���õ���i�����̵�ҳĿ¼����ַ
	dirTableBase=(pro.DirectoryTableBase>>12)<<12;// ���ã�����12λ����

	// 2���õ���i�����̵�vadRoot
	vadRootVA=pro.VadRootVirAddr;

	// 3��׼������ļ�

	// 3.1��Ϊ�ڴ浼���ļ�׼������
	CString mDumpFilePath = ".\\Mem-"+pro.strName+".raw";	// mDumpFilePath:�������ڴ��ļ�Ŀ¼

	// 3.2����������ļ�
	CFile fDumpFile;
	fDumpFile.Open(mDumpFilePath,CFile::modeReadWrite|CFile::modeCreate);
	fDumpFile.Close();

	// 4���������Vad��������Ҫ�ڸĽ�
	MiddleOrderVadTree(pro,vadRootVA,imgFilePath,mDumpFilePath);
}

void MemInfoExtraction::ExtractProcessMemoryVadTreeXP(CString filePath)
{
	//////////////////////////////////////////////////////////////////////////
	// �õ�EP[porcessNum]�ڵ�ҳĿ¼����ַ����vadRoot��ֵ

	char* tmpStr= new char[4];	// �ַ�����ʱ����
	DWORD* tmpValue(0);	
	DWORD dirTableBaseOri(0);	// dirTableBase,ҳĿ¼����ַԭ�ͣ�������12λ��ֵ
	DWORD dirTableBase(0);	// ��12λΪ��־λ����dirTableBaseOri��12λ����
	DWORD vadRootVA(0);
	DWORD vadRootPA(0);
	int processNum(0);

	//vadRootPA = VAtoPAXP(dirTableBase,vadRootVA,filePath);
	CString num;
	

	processNum = mEprocessActiveAmountXP;

	

	for(int i=0;i<processNum;i++)
	{
		//////////////////////////////////////////////////////////////////////////
		// Ϊ��ȡ��׼������

		// 1���õ���i�����̵�ҳĿ¼����ַ

		tmpStr = ppEprocessResultXP[i] + 0x18;	// 0x18ΪҳĿ¼����ַDirectoryTableBase����λ�� 
		tmpValue = (DWORD*)tmpStr;  // char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
		dirTableBaseOri = *tmpValue;
		dirTableBase = (dirTableBaseOri>>12)<<12;	// ���ã�����12λ����
		//dirTableBase=

		// 2���õ���i�����̵�vadRoot

		tmpStr = ppEprocessResultXP[i] + 0x11c;	// 0x11cΪvadrootλ�� 
		tmpValue = (DWORD*)tmpStr;
		vadRootVA = *tmpValue;

		// 3��׼������ļ�

		// 3.1��Ϊ�ڴ浼���ļ�׼������
		
		char* procName = new char[16];		// ImageFileName��Ϊ16  ����delete  Ӱ��pEprocessResult �������
		procName = ppEprocessResultXP[i]+0X174;

		num.Format("%d",i);

		CString filePath1 = ".\\Mem"+num+procName+".raw";	// filePath1:�������ڴ��ļ�Ŀ¼
		
		// 3.1����������ļ�

		CFile file;
		file.Open(filePath1,CFile::modeReadWrite|CFile::modeCreate);
		file.Close();

		// 4���������Vad��
		MiddleOrderVadTree1(dirTableBase,vadRootVA,filePath,i,filePath1);
	}
}

//////////////////////////////////////////////////////////////////////////
// Middle-Order-Traverse the VadTree
void MemInfoExtraction::MiddleOrderVadTree1(DWORD dirTableBase,DWORD vadNodeVirtuallAdd,CString filePath,int processNum,CString fp)
{
	// ��VA->PA  ������������ڵ�leftchild,rightchildָ���ֵ  ����ָ���ֵת���������ַ
	DWORD vadNodeVirtuallAddL(0),vadNodeVirtuallAddR(0);
	DWORD vadNodePhysicalAdd(0);

	DWORD startingVpn(0),endingVpn(0);
	DWORD startingVA(0),endingVA(0);
	DWORD startingPA(0),endingPA(0);
	DWORD space(0);
	DWORD tmpVA(0);		// ��ʱ�洢�����ַ
	DWORD tmpPA(0);		// ��ʱ�洢�����ַ
	

	char* tmp = new char[4];  // �洢�Ӿ�������ȡ�����ַ�
	DWORD* tmpValue(0);

	CString num;
	num.Format("%d",processNum);
	
	

	// ��ȡ��vadRootVirtuallAddL(0),vadRootVirtuallAddR��ֵ
	vadNodePhysicalAdd = VAtoPAXP(dirTableBase,vadNodeVirtuallAdd,filePath);
	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	file.Seek(vadNodePhysicalAdd+12,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	vadNodeVirtuallAddL = *tmpValue;

	file.Seek(vadNodePhysicalAdd+16,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	vadNodeVirtuallAddR = *tmpValue;

	file.Close();

	//
	if ((vadNodeVirtuallAdd >> 31)==1)	// ���˽ڵ����
	{
		MiddleOrderVadTree1(dirTableBase,vadNodeVirtuallAddL,filePath,processNum,fp);

		// �������
		

		CString filePath1 = ".\\Mem"+num+".raw";
		//ofstream outFile(mDumpFilePath,ios::app);	// д������ݽ���׷�����ļ���ĩβ���˷�ʽʹ��ios::out
		ofstream outFile;
		//outFile.open(mDumpFilePath,ios::app);
		outFile.open(fp,ios::app);
		outFile.seekp(0,ios::end);				// ���ļ���ĩβд�����ݣ��Ƚ��ļ���ָ���Ƶ�ĩβ

		CFile file1(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);
		file1.Seek(vadNodePhysicalAdd,CFile::begin);		// ��ô�Ѳ����ڽ�������Ū�����ˣ���
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		startingVpn = *tmpValue;
		startingVA = startingVpn<<12;

		file1.Seek(vadNodePhysicalAdd+4,CFile::begin);
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		endingVpn = *tmpValue;
		endingVA = endingVpn<<12;

		file1.Close();

		startingPA = VAtoPAXP(dirTableBase,startingVA,filePath);
		tmpVA = startingVA;
		tmpPA = startingPA;
		space = endingVA - startingVA;
		
		while(tmpVA<=endingVA)
		{
			CFile file2(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);
			file2.Seek(tmpPA,CFile::begin);
			file2.Read(pPageDataBuf,PAGE_SIZE);
			file2.Close();

			// д���ļ�
			outFile.write(pPageDataBuf,PAGE_SIZE);
			outFile.seekp(0,ios::end);

			tmpVA += PAGE_SIZE;		// ��4096
			tmpPA = VAtoPAXP(dirTableBase,tmpVA,filePath);
		}
		int a = 0;
		outFile.close();

		// ��ȡ�ڴ�
		MiddleOrderVadTree1(dirTableBase,vadNodeVirtuallAddR,filePath,processNum,fp);
	}
}


//�Ľ��汾
//MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath);

void MemInfoExtraction::MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath,CString dmpFilePath)
{
	// ��VA->PA  ������������ڵ�leftchild,rightchildָ���ֵ  ����ָ���ֵת���������ַ
	DWORD vadNodeVirtuallAddL(0),vadNodeVirtuallAddR(0);
	DWORD vadNodePhysicalAdd(0);

	DWORD startingVpn(0),endingVpn(0);
	DWORD startingVA(0),endingVA(0);
	DWORD startingPA(0),endingPA(0);
	DWORD space(0);
	DWORD tmpVA(0);		// ��ʱ�洢�����ַ
	DWORD tmpPA(0);		// ��ʱ�洢�����ַ

	char* tmp = new char[4];  // �洢�Ӿ�������ȡ�����ַ�
	DWORD* tmpValue(0);

	DWORD dirTableBase=(pro.DirectoryTableBase>>12)<<12;


	// 1����ȡ��vadRootVirtuallAddL��vadRootVirtuallAddR��ֵ
	vadNodePhysicalAdd = VAtoPAXP(dirTableBase,vadNodeVirtuallAdd,imgFilePath);
	CFile file(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 1.1 vadRootVirtuallAddL
	file.Seek(vadNodePhysicalAdd+12,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	vadNodeVirtuallAddL = *tmpValue;

	// 1.2��vadRootVirtuallAddR
	file.Seek(vadNodePhysicalAdd+16,CFile::begin);
	file.Read(tmp,4);
	tmpValue = (DWORD*)tmp;	// char����ת��DWORD*ʱҲ��С�˷�ʽ�������˳���ã����Բ����ٽ�ԭʼ����˳��ߵ�
	vadNodeVirtuallAddR = *tmpValue;

	file.Close();

	// 2����ʼ������

	if ((vadNodeVirtuallAdd >> 31)==1)	// ���˽ڵ����
	{
		// 2.1������������
		MiddleOrderVadTree(pro,vadNodeVirtuallAddL,imgFilePath,dmpFilePath);

		// 2.2����ȡ��ǰ�Ľڵ�����

		
//		CString mDumpFilePath = ".\\Mem"+num+".raw";
		//ofstream dmpFile(mDumpFilePath,ios::app);	// д������ݽ���׷�����ļ���ĩβ���˷�ʽʹ��ios::out

		// 2.2.1��׼������ļ�
		ofstream dmpFile;
		dmpFile.open(dmpFilePath,ios::app);
		dmpFile.seekp(0,ios::end);				// ���ļ���ĩβд�����ݣ��Ƚ��ļ���ָ���Ƶ�ĩβ

		// 2.2.2�� ׼���������

		// ��ʼ����ҳ�� startingVpn
		
		CFile file1(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		file1.Seek(vadNodePhysicalAdd,CFile::begin);		// ��ô�Ѳ����ڽ�������Ū�����ˣ���
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		startingVpn = *tmpValue;
		startingVA = startingVpn<<12;

		// ��ֹ����ҳ��			endingVpn
		file1.Seek(vadNodePhysicalAdd+4,CFile::begin);
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		endingVpn = *tmpValue;
		endingVA = endingVpn<<12;

		file1.Close();

		// ��ʼ����ҳ��ַ startingPA
		startingPA = VAtoPAXP(dirTableBase,startingVA,imgFilePath);
		tmpVA = startingVA;
		tmpPA = startingPA;
		space = endingVA - startingVA;	//һ��Vad�ڵ���һ�������������ַ��

		// 2.2.3����ʼ���ڵ����ݴ�file2 д���ļ�dmpFile

		char* pDataBuf=new char[PAGE_SIZE];
		while(tmpVA<=endingVA)
		{
			CFile file2(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
			file2.Seek(tmpPA,CFile::begin);
			int readLen=file2.Read(pDataBuf,PAGE_SIZE);
			file2.Close();

			// д���ļ���������������ﰡ
			dmpFile.write(pDataBuf,PAGE_SIZE);
			dmpFile.seekp(0,ios::end);

			tmpVA += PAGE_SIZE;	// ��4096
			tmpPA = VAtoPAXP(dirTableBase,tmpVA,imgFilePath);
		}
		delete[] pDataBuf;
		pDataBuf=NULL;

		dmpFile.close();
	

		// 2.3������������
		MiddleOrderVadTree(pro,vadNodeVirtuallAddR,imgFilePath,dmpFilePath);
	}
//	tmpValue=NULL;
//  delete[] tmp;
}


//////////////////////////////////////////////////////////////////////////
// �����ַת��Ϊ�����ַ
// mPageDirectoryBase:ҳĿ¼����ַ
// virtualAdd:�����ַ
// ���������ַ


// ���룺ҳĿ¼����ַ�������ַ�������ļ�·��
// ����������ַ

DWORD MemInfoExtraction::VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CString filePath)  
{
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

		// �õ������ַ
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
	tmpAddress=NULL;
	delete[] tmp;
	return result;
}

// �Ľ���

// �����ľ����ļ������� CFile ����

DWORD MemInfoExtraction::VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CFile imgFile)  
{
	//////////////////////////////////////////////////////////////////////////
	// ���ж��Ƿ�Ϊ��ҳģʽ��������PDE��ֵ���ж�

	// ��������ʺ���ҳ��СΪ4KB����PAEģʽ�����������ڴ�ҳģʽ���ں˱����������ô�ҳģʽ


	// 1��׼������
	DWORD OffsetNoLargePage = virtualAdd&0xfff;		// ���Ǵ�ҳģʽʱ��ҳ��ƫ����  ��ʮ��λ��0xfff==(00000000000000000000111111111111)2
	DWORD OffsetOfPageTab = ((virtualAdd >> 12)&0x3ff)<<2;	// �м�ʮλ��0x3ff==(1111111111)2��ҳ����ƫ����  ������λ��
	                                                // �൱�ڳ���4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// ȡ��22λ

	DWORD Dir = (virtualAdd >> 22)<<2;				// ǰʮλ  ҳĿ¼����ƫ����  ������λ���൱�ڳ���4	
	DWORD result(0);                            // ����ֵ
	BOOL isLargePage(FALSE);					// �Ƿ�Ϊ��ҳģʽ��ȱʡֵΪ��

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // �洢�Ӿ�������ȡ�����ַ�
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);

//	CFile imgFile(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 2����ȡPDE
	imgFile.Seek((mPageDirectoryBase+Dir),CFile::begin);  // PDE��λ��ΪdirTableBase+Dir
	imgFile.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPDE   ��ҳĿ¼��Ҫ��Ҫλ�ƣ�
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

		// �õ������ַ
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
		imgFile.Seek((((PDE>>12)<<12)+OffsetOfPageTab),CFile::begin);	// PDE��12λ���㣬PTE��λ��Ϊ (PDE��12λ�����)+Table
		imgFile.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPTE
/*
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
*/
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	 // ҳ�����12λ����
		/*imgFile.Seek((PTE+Offset),CFile::begin);	// PA��λ��ΪPTE+Offset
		imgFile.Read(tmp,4);  // ��4���ֽڵ�tmp��  �ٽ�tmp���ĸ��ֽڵ�ֵ��Ϊ DWORD�ͣ�ֵΪPTE
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
//	imgFile.Close();
	tmpAddress=NULL;
	delete[] tmp;
	return result;
}


//////////////////////////////////////////////////////////////////////////
// ��������ַ����ֵ

DWORD MemInfoExtraction::GetVAValueXP(DWORD dirTableBase,DWORD virtualAdd,CString filePath)
{
	DWORD result(0);
	return result;
}

MemInfoExtraction MemInfoExtraction::operator=(MemInfoExtraction &meminfo)
{
	if (this==&meminfo)
	{
		return *this;
	}
	//����ط��Ǹ������Ⱑ��
//	delete pProgressDlg;
//pProgressDlg=new CProgressDlg;
	return *this;
}
