#include "StdAfx.h"
#include "MemInfoExtraction.h"

#include "MemoryManagement.h"

#include <iostream>
#include <fstream>

using namespace std;

#define KPCR_LENGTH 0xd70   //kpcr大小  ？
#define KPCR_SEARCH_SIZE 8

//#define	XPEPROCESSLENGTH 0x25c  //XP下的eprocess大小
//#define	XPEPROCESSLENGTH 0x25c  //XP下的eprocess大小

const DWORD EPROCESS_LENGTH_XP = 0x25c;
const DWORD EPROCESS_LENGTH_WIN7 = 0x2c0;
//const DWORD PAGE_SIZE = 4096;



MemInfoExtraction::MemInfoExtraction(void)
{
	//////////////////////////////////////////////////////////////////////////
	// XP下
	pKpcrResultXP = new char[KPCR_LENGTH];
	bKpcrExistXP = FALSE;
	
	// m_pXPEprocessResult二维数组申请内存
	ppEprocessResultXP = new char*[100]; // 设最多一百个
	for(int i=0;i<100;i++)
	{
		ppEprocessResultXP[i] = new char[EPROCESS_LENGTH_XP];
	}
	pProcessMemoryXP = NULL;

	mEprocessActiveAmountXP = 0;		// 进程的数量初始化为归零

	//////////////////////////////////////////////////////////////////////////
	// WIN7下

	

	pKpcrResultWin7 = new char[KPCR_LENGTH];
	// m_pWIN7EprocessResult二维数组申请内存

	ppEprocessResultWin7 = new char*[100]; // 设最多一百个
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
	// XP下
	delete[] pKpcrResultXP;
	pKpcrResultXP = NULL;
	//////////////////////////////////////////////////////////////////////////
	// m_pXPEprocessResult二维数组释放内存
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
	// WIN7下
	delete[] pKpcrResultWin7;
	pKpcrResultWin7 = NULL;
	//////////////////////////////////////////////////////////////////////////
	// m_pWIN7EprocessResult二维数组释放内存
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
// pMemBlock:输入的字符串的头指针
// mBlockSize:输入的字符串的长度
void MemInfoExtraction::ExtractKpcrXP(const char* pMemBlock,ULONGLONG  mBlockSize)
{
	if(!(pMemBlock&&mBlockSize))// 防止输入错误，两个参数任意一个为零则返回
		return;
	//pKpcrResultXP = new char[KPCRLENGTH];

	DWORD loops(0);
	if(mBlockSize>4096)		//防止出现小于零的情况
		loops = mBlockSize - 4096; //kpcr的线性地址为0xffdff000表明在一个页的第一项，
	                               //所以检索到最后一页开头即可，4k
	bKpcrExistXP = FALSE;  // 初始化为FALSE，位置是不是应该放在构造函数中呢？？
	
	//////////////////////////////////////////////////////////////////////////
	// 要匹配的字符
	char pattern[] = {0x00,0xf0,0xdf,0xff,0x20,0xf1,0xdf,0xff}; 

	int matchAmount = 0;// 条件匹配计数器

	//////////////////////////////////////////////////////////////////////////
	// 匹配算法，4k,4k(4k=4096)的匹配，检索的值在页内偏移0x1c（同时是KPCR内偏移0X1c处）即十进制28处
	for(DWORD i=0; i < loops; i+=4096)
	{
		int j = 0;
		for(; j < KPCR_SEARCH_SIZE;j++ )
		{
			if(pMemBlock[i+0x1c+j] == pattern[j])
				++matchAmount;    // 匹配一个字母j自加一
			else
				break;
		}
		if(matchAmount == KPCR_SEARCH_SIZE) // 发现一个匹配的对象			
		{
			for(int k=0;k<KPCR_LENGTH;++k)
			{
				pKpcrResultXP[k]=pMemBlock[i+k];  //00f0dfff20f1dfff的位置减去0x01c即十进制28,得到KPCR的位置
			}
			bKpcrExistXP = TRUE;
			return;
		}
		matchAmount = 0;//将matchAmount清零
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

// XP 进程结构提取

void MemInfoExtraction::ExtractEprocessXP(const char* MemBlock,DWORD MemBlockSize)
{
	if(!(MemBlock&&MemBlockSize)) // 
		return;

	DWORD loops = 0;
	if(MemBlockSize>EPROCESS_LENGTH_XP)
		loops = MemBlockSize - EPROCESS_LENGTH_XP;
	//mEprocessActiveAmountXP = 0;		// 新的调用，进程的数量归零
	//////////////////////////////////////////////////////////////////////////
	// 要匹配的字符
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime 运行的进程为0x00000000
	char pattern2[] = {0xfd,0x7f};							     // Peb地址的值，必为0x7ffdxxxx在实际内存中的存储方式是0xxxxxfd7f （intel小端模式）
	char pattern3[] = {0x03,0x01,0x00,0x00};					 // ExitStatus  值为0x00000103在内存中实际存储的值为0x03010000

	int matchAmount = 0;// 条件匹配计数器
	
	//////////////////////////////////////////////////////////////////////////
	// 匹配算法
	// ERPOCESS结构肯定在一个页内，只在一个页内寻找，效率更高
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(MemBlockSize>4096)  //防止memLen小于4096越界访问
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_XP;j++)  //不能是<而是<=， 防止漏掉适应情况的，可以这样理解：假如XPEPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(MemBlock[i+j+0x078+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(MemBlock[i+j+0x1b2+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(MemBlock[i+j+0x24c+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_XP;l++)
					{
						ppEprocessResultXP[mEprocessActiveAmountXP][l] = MemBlock[i+j+l];
					}
					mEprocessActiveAmountXP++;	//匹配一次自加一					
				}
				matchAmount = 0;		// 归零，防止累加
			}
		}
		else					// memLen小于4096时
		{
			for(DWORD j=0;j<=MemBlockSize-EPROCESS_LENGTH_XP;j++)		//不能是<而是<=， 防止漏掉适应情况的，如memLen==XPEPROCESSLENTH时
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(MemBlock[i+j+0x078+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(MemBlock[i+j+0x1b2+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(MemBlock[i+j+0x24c+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_XP;l++)
					{
						//ppEprocessResultXP[mEprocessActiveAmountXP][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountXP++;	//匹配一次自加一

				}
				matchAmount = 0;		// 归零，防止累加
			}
		}

		MemBlockSize = MemBlockSize - 4096; //匹配完一次自减4k
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
// 以下是WIN7中寻找EPROCESS的函数
/*
void MemInfoExtraction::ExtractEprocessWin7(const char* Memory,DWORD memLen)
{
	if(!(Memory&&memLen)) // 防止输入错误，两个参数任意一个为零则返回
		return;
	//ppEprocessResultWin7 = new char[WIN7EPROCESSLENGTH];

	
	DWORD loops = 0;
	if(memLen>EPROCESS_LENGTH_WIN7)
		loops = memLen - EPROCESS_LENGTH_WIN7;
	mEprocessActiveAmountWin7 = 0;		// 新的调用，进程的数量归零
	
	//////////////////////////////////////////////////////////////////////////
	// 要匹配的字符
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime 运行的进程为0x00000000
	char pattern2[] = {0xfd,0x7f};								// Peb地址的值，必为0x7ffdxxxx在实际内存中的存储方式是0xxxxxfd7f （intel小端模式）
	char pattern3[] = {0x03,0x01,0x00,0x00};					// ExitStatus  值为0x00000103在内存中实际存储的值为0x03010000


	int matchAmount = 0;		// 条件匹配计数器
	
	//////////////////////////////////////////////////////////////////////////
	// 匹配算法
	// ERPOCESS结构肯定在一个页内，只在一个页内寻找，效率更高
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //防止memLen小于4096越界访问
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_WIN7;j++)  //不能是<而是<=， 防止漏掉适应情况的，可以这样理解：假如WIN7EPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}

					mEprocessActiveAmountWin7++;	//匹配一次自加一
					
				}
				matchAmount = 0;		// 归零，防止累加
			}
		}
		else// memLen小于4096时
		{
			for(DWORD j=0;j<=memLen-EPROCESS_LENGTH_WIN7;j++)		//不能是<而是<=， 防止漏掉适应情况的，如memLen==WIN7EPROCESSLENTH时
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountWin7++;	//匹配一次自加一

				}
				matchAmount = 0;		// 归零，防止累加
			}
		}

		memLen = memLen - 4096; //匹配完一次自减4k
	}
	//////////////////////////////////////////////////////////////////////////

}
*/

void MemInfoExtraction::ExtractEprocessWin7(const char* Memory,DWORD memLen)
{
	if(!(Memory&&memLen)) // 防止输入错误，两个参数任意一个为零则返回
		return;
	//m_pWIN7EprocessResult = new char[WIN7EPROCESSLENGTH];

	
	DWORD loops = 0;
	if(memLen>EPROCESS_LENGTH_WIN7)
		loops = memLen - EPROCESS_LENGTH_WIN7;
	mEprocessActiveAmountWin7 = 0;		// 新的调用，进程的数量归零
	
	//////////////////////////////////////////////////////////////////////////
	// 要匹配的字符
	char pattern1[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // ExitTime 运行的进程为0x00000000
	char pattern2[] = {0xfd,0x7f};								// Peb地址的值，必为0x7ffdxxxx在实际内存中的存储方式是0xxxxxfd7f （intel小端模式）
	char pattern3[] = {0x03,0x01,0x00,0x00};					// ExitStatus  值为0x00000103在内存中实际存储的值为0x03010000


	int matchAmount = 0;		// 条件匹配计数器
	

	//////////////////////////////////////////////////////////////////////////
	// 匹配算法
	// ERPOCESS结构肯定在一个页内，只在一个页内寻找，效率更高
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //防止memLen小于4096越界访问
		{
			for(int j=0;j<=4096-EPROCESS_LENGTH_WIN7;j++)  //不能是<而是<=， 防止漏掉适应情况的，可以这样理解：假如WIN7EPROCESSLENGTH==4096
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l]=Memory[i+j+l];
					}										
					//////////////////////////////////////////////////////////////////////////


					mEprocessActiveAmountWin7++;	//匹配一次自加一
					
				}
				matchAmount = 0;		// 归零，防止累加
			}
		}
		else					// memLen小于4096时
		{
			for(int j=0;j<=memLen-EPROCESS_LENGTH_WIN7;j++)		//不能是<而是<=， 防止漏掉适应情况的，如memLen==WIN7EPROCESSLENTH时
			{
				int k;
				for(k=0;k<8;k++)					//pattern1的长度为8
				{
					if(Memory[i+j+0x0a8+k]==pattern1[k])  // ExitTime位置在EPROCESS内+0x078处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<2;k++)
				{
					if(Memory[i+j+0x1aa+k]==pattern2[k])	// Peb指针位置在EPROCESS内+0x1b0处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				for(k=0;k<4;k++)
				{
					if(Memory[i+j+0x274+k]==pattern3[k])	// ExitStatus位置在EPROCESS内+0x24c处
						++matchAmount;		// 匹配一个字母matchAmount自加一
					else
						break;
				}
				if (matchAmount==14)  //如果上述三个属性值都匹配（总共占14个字节）
				{
					for(int l=0;l<EPROCESS_LENGTH_WIN7;l++)
					{
						ppEprocessResultWin7[mEprocessActiveAmountWin7][l] = Memory[i+j+l];
					}
					mEprocessActiveAmountWin7++;	//匹配一次自加一

				}
				matchAmount = 0;		// 归零，防止累加
			}
		}

		memLen = memLen - 4096; //匹配完一次自减4k
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
	if(!(Memory&&memLen)) // 防止输入错误，两个参数任意一个为零则返回
		return;

	DWORD loops = 0;
	if(memLen>0x120)		//防止出现小于零的情况
		loops = memLen - 0x120; //


	mKpcrAmountWin7 = 0;		// 
	

	//////////////////////////////////////////////////////////////////////////
	// 要匹配的字符
	char* original1;	//  存储原始的字符串
	char* original2;
	char trueKpcrVAValue[4] = {0x00,0x00,0x00,0x00};	// 将物理内存中的值转换成系统的实际值  存储转换后的字符串
	char trueKprcbValue[4] = {0x00,0x00,0x00,0x00};

	long* pattern1(0);	// long占四个字节，存储四个自己的值，进行按位与运算
	long* pattern2(0);

	bool kpcrVAisRight(FALSE),kprcbVAisRight(FALSE);  //  代表四个字节的高位和地位		8字节与0x80000000和0x7fffffff若结果都为1证明 此数>0x80000000
									// 在内存中实际存储的值是
	int offset(0);					// KPRCB相对于KPCR的位移
	bool offsetRight = FALSE;		// KPRCB相对于KPCR的位移大小为0x120,结果正确 (32位Windows 7下是0x120)

	int matchAmount = 0;		// 条件匹配计数器
	

	//////////////////////////////////////////////////////////////////////////
	// 匹配算法
	// ERPOCESS结构肯定在一个页内，只在一个页内寻找，效率更高
	for (DWORD i=0;i<loops;i+=4096)
	{
		if(memLen>4096)  //防止memLen小于4096越界访问
		{
			for(int j=0;j<=4096;j++)  //
			{
				//pattern1 = (long*)Memory;
				//pattern2 = (long*)(Memory + 4);
				//////////////////////////////////////////////////////////////////////////
				// 获取原始字符串
				original1 = Memory+i+j;
				original2 = Memory+i+j+ 4;
				//////////////////////////////////////////////////////////////////////////
				// 转换成系统内的实际值
				trueKpcrVAValue[0] = original1[3];
				trueKpcrVAValue[1] = original1[2];
				trueKpcrVAValue[2] = original1[1];
				trueKpcrVAValue[3] = original1[0];

				trueKprcbValue[0] = original2[3];
				trueKprcbValue[1] = original2[2];
				trueKprcbValue[2] = original2[1];
				trueKprcbValue[3] = original2[0];

				//////////////////////////////////////////////////////////////////////////
				// 字符串按位与
				pattern1 = (long*)trueKpcrVAValue;
				pattern2 = (long*)trueKprcbValue;

				kpcrVAisRight = ((*pattern1)&0x80000000)&&((*pattern1)&0x7fffffff);
				kprcbVAisRight = ((*pattern2)&0x80000000)&&((*pattern2)&0x7fffffff);
				
				//////////////////////////////////////////////////////////////////////////
				// 查看PRCB相对于KPCR的位移大小为0x120
				offset = *pattern2 - *pattern1;
				if (offset == 0x120)
				{
					offsetRight = TRUE;
				}


				//////////////////////////////////////////////////////////////////////////
				// 若结果正确
				if(kpcrVAisRight&&kprcbVAisRight&&offsetRight)
				{
					mKpcrAmountWin7++;	//匹配一次自加一
					/*CString num;
					num.Format("%d",mKpcrAmountWin7);                                                            
					CString result = "find"+num+"times";
					MessageBox(0,result,0,0);*/					
				}				
			}
		}
		else					// memLen小于4096时
		{
			for(int j=0;j<=4096;j++)  //
			{
				//pattern1 = (long*)Memory;
				//pattern2 = (long*)(Memory + 4);
				//////////////////////////////////////////////////////////////////////////
				// 获取原始字符串
				original1 = Memory+i+j;
				original2 = Memory+i+j+ 4;
				//////////////////////////////////////////////////////////////////////////
				// 转换成系统内的实际值
				trueKpcrVAValue[0] = original1[3];
				trueKpcrVAValue[1] = original1[2];
				trueKpcrVAValue[2] = original1[1];
				trueKpcrVAValue[3] = original1[0];

				trueKprcbValue[0] = original2[3];
				trueKprcbValue[1] = original2[2];
				trueKprcbValue[2] = original2[1];
				trueKprcbValue[3] = original2[0];

				//////////////////////////////////////////////////////////////////////////
				// 字符串按位与
				pattern1 = (long*)trueKpcrVAValue;
				pattern2 = (long*)trueKprcbValue;

				kpcrVAisRight = ((*pattern1)&0x80000000)&&((*pattern1)&0x7fffffff);
				kprcbVAisRight = ((*pattern2)&0x80000000)&&((*pattern2)&0x7fffffff);

				//////////////////////////////////////////////////////////////////////////
				// 查看PRCB相对于KPCR的位移大小为0x120
				offset = *pattern2 - *pattern1;
				if (offset == 0x120)
				{
					offsetRight = TRUE;
				}


				//////////////////////////////////////////////////////////////////////////
				// 若结果正确
				if(kpcrVAisRight&&kprcbVAisRight&&offsetRight)
				{
					mKpcrAmountWin7++;	//匹配一次自加一
					/*CString num;
					num.Format("%d",mKpcrAmountWin7);
					CString result = "find"+num+"times";
					MessageBox(0,result,0,0);	*/				
				}				
			}
		}

		memLen = memLen - 4096; //匹配完一次自减4k
	}
	//////////////////////////////////////////////////////////////////////////
}

int MemInfoExtraction::GetKpcrAmountWin7()
{
	return mKpcrAmountWin7;
}

//////////////////////////////////////////////////////////////////////////
//  获取进程内存---Page Table

void MemInfoExtraction::ExtractProcessMemoryPageTableXP(PROCESS pro,CString imgFilePath)
{
	//判断是pae？no-pae

	/*
	if (isPae(pro.DirectoryTableBase,imgFilePath))
	{
	AfxMessageBox("pae mode!");
	}
	else
	AfxMessageBox("no-pae mode!");
	*/

	// 调用统一的接口
	DumpProcessMemory(pro,imgFilePath);
/*
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

	//创建进度条显示进度——使用非模态对话框
	CProgressDlg *pProgressDlg=new CProgressDlg;
	pProgressDlg->SetProgressRange(0,512);//多少呢？
	int curPosition=0;
	pProgressDlg->Create();
	pProgressDlg->CenterWindow();
	pProgressDlg->ShowWindow(SW_SHOW);

	//一次获取一个页面的页目录项
	GetPageDirTable(pro.DirectoryTableBase,PAGE_SIZE,ImgFile,pPageDirectory);

	for( int DirIndex=0; DirIndex < 512 ; DirIndex++)
	{
		pProgressDlg->UpdateProgressCtrl(DirIndex);
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
// 获取进程内存----VadTree

// 改进型

void MemInfoExtraction::ExtractProcessMemoryVadTreeXP(PROCESS pro,CString imgFilePath)
{
	// 得到EP[porcessNum]内的页目录基地址及其vadRoot的值，然后在进行中序遍历traverse

	DWORD dirTableBase(0);	// 后12位为标志位，将dirTableBaseOri后12位置零
	DWORD vadRootVA(0);

	// 1、得到第i个进程的页目录基地址
	dirTableBase=(pro.DirectoryTableBase>>12)<<12;// 作用：将后12位置零

	// 2、得到第i个进程的vadRoot
	vadRootVA=pro.VadRootVirAddr;

	// 3、准备输出文件

	// 3.1、为内存导出文件准备属性
	CString mDumpFilePath = ".\\Mem-"+pro.strName+".raw";	// mDumpFilePath:产生的内存文件目录

	// 3.2、创建输出文件
	CFile fDumpFile;
	fDumpFile.Open(mDumpFilePath,CFile::modeReadWrite|CFile::modeCreate);
	fDumpFile.Close();

	// 4、中序遍历Vad树——需要在改进
	MiddleOrderVadTree(pro,vadRootVA,imgFilePath,mDumpFilePath);
}

void MemInfoExtraction::ExtractProcessMemoryVadTreeXP(CString filePath)
{
	//////////////////////////////////////////////////////////////////////////
	// 得到EP[porcessNum]内的页目录基地址及其vadRoot的值

	char* tmpStr= new char[4];	// 字符串临时变量
	DWORD* tmpValue(0);	
	DWORD dirTableBaseOri(0);	// dirTableBase,页目录基地址原型，包括后12位的值
	DWORD dirTableBase(0);	// 后12位为标志位，将dirTableBaseOri后12位置零
	DWORD vadRootVA(0);
	DWORD vadRootPA(0);
	int processNum(0);

	//vadRootPA = VAtoPAXP(dirTableBase,vadRootVA,filePath);
	CString num;
	

	processNum = mEprocessActiveAmountXP;

	

	for(int i=0;i<processNum;i++)
	{
		//////////////////////////////////////////////////////////////////////////
		// 为提取做准备工作

		// 1、得到第i个进程的页目录基地址

		tmpStr = ppEprocessResultXP[i] + 0x18;	// 0x18为页目录基地址DirectoryTableBase所在位置 
		tmpValue = (DWORD*)tmpStr;  // char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
		dirTableBaseOri = *tmpValue;
		dirTableBase = (dirTableBaseOri>>12)<<12;	// 作用：将后12位置零
		//dirTableBase=

		// 2、得到第i个进程的vadRoot

		tmpStr = ppEprocessResultXP[i] + 0x11c;	// 0x11c为vadroot位置 
		tmpValue = (DWORD*)tmpStr;
		vadRootVA = *tmpValue;

		// 3、准备输出文件

		// 3.1、为内存导出文件准备属性
		
		char* procName = new char[16];		// ImageFileName域长为16  不能delete  影响pEprocessResult 程序崩溃
		procName = ppEprocessResultXP[i]+0X174;

		num.Format("%d",i);

		CString filePath1 = ".\\Mem"+num+procName+".raw";	// filePath1:产生的内存文件目录
		
		// 3.1、创建输出文件

		CFile file;
		file.Open(filePath1,CFile::modeReadWrite|CFile::modeCreate);
		file.Close();

		// 4、中序遍历Vad树
		MiddleOrderVadTree1(dirTableBase,vadRootVA,filePath,i,filePath1);
	}
}

//////////////////////////////////////////////////////////////////////////
// Middle-Order-Traverse the VadTree
void MemInfoExtraction::MiddleOrderVadTree1(DWORD dirTableBase,DWORD vadNodeVirtuallAdd,CString filePath,int processNum,CString fp)
{
	// 将VA->PA  并求出这个结点内的leftchild,rightchild指针的值  并将指针的值转换成物理地址
	DWORD vadNodeVirtuallAddL(0),vadNodeVirtuallAddR(0);
	DWORD vadNodePhysicalAdd(0);

	DWORD startingVpn(0),endingVpn(0);
	DWORD startingVA(0),endingVA(0);
	DWORD startingPA(0),endingPA(0);
	DWORD space(0);
	DWORD tmpVA(0);		// 临时存储虚拟地址
	DWORD tmpPA(0);		// 临时存储物理地址
	

	char* tmp = new char[4];  // 存储从镜像中提取出的字符
	DWORD* tmpValue(0);

	CString num;
	num.Format("%d",processNum);
	
	

	// 提取出vadRootVirtuallAddL(0),vadRootVirtuallAddR的值
	vadNodePhysicalAdd = VAtoPAXP(dirTableBase,vadNodeVirtuallAdd,filePath);
	CFile file(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	file.Seek(vadNodePhysicalAdd+12,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	vadNodeVirtuallAddL = *tmpValue;

	file.Seek(vadNodePhysicalAdd+16,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	vadNodeVirtuallAddR = *tmpValue;

	file.Close();

	//
	if ((vadNodeVirtuallAdd >> 31)==1)	// 若此节点存在
	{
		MiddleOrderVadTree1(dirTableBase,vadNodeVirtuallAddL,filePath,processNum,fp);

		// 处理过程
		

		CString filePath1 = ".\\Mem"+num+".raw";
		//ofstream outFile(mDumpFilePath,ios::app);	// 写入的数据将被追加在文件的末尾，此方式使用ios::out
		ofstream outFile;
		//outFile.open(mDumpFilePath,ios::app);
		outFile.open(fp,ios::app);
		outFile.seekp(0,ios::end);				// 在文件的末尾写入数据，先将文件的指针移到末尾

		CFile file1(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);
		file1.Seek(vadNodePhysicalAdd,CFile::begin);		// 怎么把不存在结点的内容弄出来了？？
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

			// 写入文件
			outFile.write(pPageDataBuf,PAGE_SIZE);
			outFile.seekp(0,ios::end);

			tmpVA += PAGE_SIZE;		// 加4096
			tmpPA = VAtoPAXP(dirTableBase,tmpVA,filePath);
		}
		int a = 0;
		outFile.close();

		// 提取内存
		MiddleOrderVadTree1(dirTableBase,vadNodeVirtuallAddR,filePath,processNum,fp);
	}
}


//改进版本
//MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath);

void MemInfoExtraction::MiddleOrderVadTree(PROCESS pro,DWORD vadNodeVirtuallAdd,CString imgFilePath,CString dmpFilePath)
{
	// 将VA->PA  并求出这个结点内的leftchild,rightchild指针的值  并将指针的值转换成物理地址
	DWORD vadNodeVirtuallAddL(0),vadNodeVirtuallAddR(0);
	DWORD vadNodePhysicalAdd(0);

	DWORD startingVpn(0),endingVpn(0);
	DWORD startingVA(0),endingVA(0);
	DWORD startingPA(0),endingPA(0);
	DWORD space(0);
	DWORD tmpVA(0);		// 临时存储虚拟地址
	DWORD tmpPA(0);		// 临时存储物理地址

	char* tmp = new char[4];  // 存储从镜像中提取出的字符
	DWORD* tmpValue(0);

	DWORD dirTableBase=(pro.DirectoryTableBase>>12)<<12;


	// 1、提取出vadRootVirtuallAddL、vadRootVirtuallAddR的值
	vadNodePhysicalAdd = VAtoPAXP(dirTableBase,vadNodeVirtuallAdd,imgFilePath);
	CFile file(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 1.1 vadRootVirtuallAddL
	file.Seek(vadNodePhysicalAdd+12,CFile::begin);
	file.Read(tmp,4);		
	tmpValue = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	vadNodeVirtuallAddL = *tmpValue;

	// 1.2、vadRootVirtuallAddR
	file.Seek(vadNodePhysicalAdd+16,CFile::begin);
	file.Read(tmp,4);
	tmpValue = (DWORD*)tmp;	// char数组转换DWORD*时也按小端方式，数组的顺序倒置，所以不用再将原始数组顺序颠倒
	vadNodeVirtuallAddR = *tmpValue;

	file.Close();

	// 2、开始提数据

	if ((vadNodeVirtuallAdd >> 31)==1)	// 若此节点存在
	{
		// 2.1、遍历左子树
		MiddleOrderVadTree(pro,vadNodeVirtuallAddL,imgFilePath,dmpFilePath);

		// 2.2、提取当前的节点数据

		
//		CString mDumpFilePath = ".\\Mem"+num+".raw";
		//ofstream dmpFile(mDumpFilePath,ios::app);	// 写入的数据将被追加在文件的末尾，此方式使用ios::out

		// 2.2.1、准备输出文件
		ofstream dmpFile;
		dmpFile.open(dmpFilePath,ios::app);
		dmpFile.seekp(0,ios::end);				// 在文件的末尾写入数据，先将文件的指针移到末尾

		// 2.2.2、 准备输出数据

		// 起始虚拟页号 startingVpn
		
		CFile file1(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);

		file1.Seek(vadNodePhysicalAdd,CFile::begin);		// 怎么把不存在结点的内容弄出来了？？
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		startingVpn = *tmpValue;
		startingVA = startingVpn<<12;

		// 终止虚拟页号			endingVpn
		file1.Seek(vadNodePhysicalAdd+4,CFile::begin);
		file1.Read(tmp,4);
		tmpValue = (DWORD*)tmp;
		endingVpn = *tmpValue;
		endingVA = endingVpn<<12;

		file1.Close();

		// 起始物理页地址 startingPA
		startingPA = VAtoPAXP(dirTableBase,startingVA,imgFilePath);
		tmpVA = startingVA;
		tmpPA = startingPA;
		space = endingVA - startingVA;	//一个Vad节点是一块连续的虚拟地址块

		// 2.2.3、开始将节点数据从file2 写入文件dmpFile

		char* pDataBuf=new char[PAGE_SIZE];
		while(tmpVA<=endingVA)
		{
			CFile file2(imgFilePath,CFile::modeReadWrite | CFile::shareDenyWrite);
			file2.Seek(tmpPA,CFile::begin);
			int readLen=file2.Read(pDataBuf,PAGE_SIZE);
			file2.Close();

			// 写入文件——问题出在这里啊
			dmpFile.write(pDataBuf,PAGE_SIZE);
			dmpFile.seekp(0,ios::end);

			tmpVA += PAGE_SIZE;	// 加4096
			tmpPA = VAtoPAXP(dirTableBase,tmpVA,imgFilePath);
		}
		delete[] pDataBuf;
		pDataBuf=NULL;

		dmpFile.close();
	

		// 2.3、遍历右子树
		MiddleOrderVadTree(pro,vadNodeVirtuallAddR,imgFilePath,dmpFilePath);
	}
//	tmpValue=NULL;
//  delete[] tmp;
}


//////////////////////////////////////////////////////////////////////////
// 虚拟地址转化为物理地址
// mPageDirectoryBase:页目录基地址
// virtualAdd:虚拟地址
// 返回物理地址


// 输入：页目录基地址，虚拟地址，镜像文件路径
// 输出：物理地址

DWORD MemInfoExtraction::VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CString filePath)  
{
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

		// 得到物理地址
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
	tmpAddress=NULL;
	delete[] tmp;
	return result;
}

// 改进型

// 传进的镜像文件参数是 CFile 类型

DWORD MemInfoExtraction::VAtoPAXP(DWORD mPageDirectoryBase,DWORD virtualAdd,CFile imgFile)  
{
	//////////////////////////////////////////////////////////////////////////
	// 先判断是否为大页模式——根据PDE的值来判断

	// 以下情况适合于页大小为4KB，非PAE模式，不能适用于大页模式，内核变量好像都是用大页模式


	// 1、准备工作
	DWORD OffsetNoLargePage = virtualAdd&0xfff;		// 不是大页模式时的页内偏移量  后十二位，0xfff==(00000000000000000000111111111111)2
	DWORD OffsetOfPageTab = ((virtualAdd >> 12)&0x3ff)<<2;	// 中间十位，0x3ff==(1111111111)2，页表内偏移量  左移两位，
	                                                // 相当于乘以4

	DWORD OffsetLargePage = virtualAdd&0x3fffff;	// 取后22位

	DWORD Dir = (virtualAdd >> 22)<<2;				// 前十位  页目录表内偏移量  左移两位，相当于乘以4	
	DWORD result(0);                            // 返回值
	BOOL isLargePage(FALSE);					// 是否为大页模式，缺省值为否

	DWORD PDE(0),PTE(0),PA(0);

	char* tmp = new char[4];  // 存储从镜像中提取出的字符
	char trueValue[4] = {0x00,0x00,0x00,0x00};
	DWORD* tmpAddress(0);

//	CFile imgFile(filePath,CFile::modeReadWrite | CFile::shareDenyWrite);

	// 2、读取PDE
	imgFile.Seek((mPageDirectoryBase+Dir),CFile::begin);  // PDE的位置为dirTableBase+Dir
	imgFile.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PDE   ？页目录项要不要位移？
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

		// 得到物理地址
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
		imgFile.Seek((((PDE>>12)<<12)+OffsetOfPageTab),CFile::begin);	// PDE后12位置零，PTE的位置为 (PDE后12位置零后)+Table
		imgFile.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PTE
/*
		trueValue[0] = tmp[3];
		trueValue[1] = tmp[2];
		trueValue[2] = tmp[1];
		trueValue[3] = tmp[0];
*/
		tmpAddress = (DWORD*)tmp;
		PTE = *tmpAddress;	 // 页表项，后12位置零
		/*imgFile.Seek((PTE+Offset),CFile::begin);	// PA的位置为PTE+Offset
		imgFile.Read(tmp,4);  // 读4个字节到tmp中  再将tmp中四个字节的值变为 DWORD型，值为PTE
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
//	imgFile.Close();
	tmpAddress=NULL;
	delete[] tmp;
	return result;
}


//////////////////////////////////////////////////////////////////////////
// 求出虚拟地址处的值

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
	//这个地方是个大问题啊！
//	delete pProgressDlg;
//pProgressDlg=new CProgressDlg;
	return *this;
}
