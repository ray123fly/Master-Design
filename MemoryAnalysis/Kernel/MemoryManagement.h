// PDE PTE��ص����ݺͲ���
//#ifndef __MEMORYMANAGEMENT_H__
//#define __MEMORYMANAGEMENT_H__
#pragma once

#include <stdio.h>
#include "AfxTempl.h"
#include "Process.h"

// no-pae

#define PDE_COUNT       1024
#define PTE_COUNT       1024
#define PDE_COUNT_LARGE 1024
#define PTE_COUNT_LARGE 1024


#define PAGE_SIZE       4096
#define PAGE_SIZE_LARGE 4*1024*1024

//pae
#define PAE_PDE_COUNT       512
#define PAE_PTE_COUNT       512
#define PAE_PDE_COUNT_LARGE 512
#define PAE_PTE_COUNT_LARGE 512


#define PAE_PAGE_SIZE       4096
#define PAE_PAGE_SIZE_LARGE 2*1024*1024

typedef LONGLONG INT64;




//////////////////////////////////////////////////////////////////////////
// 1��ͨ���������� 

//PDE �� PTE ������
enum PTE_TYPE{ valide,transition,prototype,other};

struct VirtualAddrFlags
{
	DWORD validbit:1;
	DWORD transitionbit:1;
	DWORD prototypebit:1;
};

// �ж�pae��no-pae
BOOL isPae(DWORD cr3,CString imagefile);

//////////////////////////////////////////////////////////////////////////
// 2��no-pae

void GetAddrFlags(DWORD addr,VirtualAddrFlags &state);

//ֻ���� PDE ���� PTE ������
PTE_TYPE GetPdeOrPteType(DWORD addr);


// �ֱ��ж� PDE �� PTE �ĸ�����־λ����Ϣ
//pte   pde
BOOL isLargePageMode(DWORD pde);

BOOL IsValid(DWORD addr);

BOOL IsTransition(DWORD addr);

BOOL IsPrototype(DWORD addr);

void GetPageDirTable(DWORD baseAddr,int pageDirSize,CFile &ImgFile,CArray<DWORD,DWORD> *pPageDir);

void GetPageTable(DWORD pdeOri,int pageTableSize,CFile &ImgFile,CArray<DWORD,DWORD>* pPageTab);

void GetPhysicalPageData(DWORD PdeOrPteOri,BOOL isLarge,CFile &ImgFile,char* pTmpData);


DWORD PdeToData(DWORD pde);

DWORD PdeToPageTable(DWORD pde);

DWORD PteToData(DWORD PdeOrPte);

void NoPaeDumpProcessMemory(PROCESS pro,CString imgFilePath);

//////////////////////////////////////////////////////////////////////////
// 3��pae

// �ֱ��ж� PDE �� PTE �ĸ�����־λ����Ϣ
//pte   pde

void PaeGetAddrFlags(DWORD addr,VirtualAddrFlags &state);

// ����pde ��pte������
PTE_TYPE PaeGetPdeOrPteType(DWORD addr);

BOOL PaeIsLargePageMode(DWORD pde);

BOOL PaeIsValid(DWORD addr);

BOOL PaeIsTransition(DWORD addr);

BOOL PaeIsPrototype(DWORD addr);

BOOL isPae(DWORD pPTB,CString imagefile);



void PaeGetPageDirPointerTable(DWORD cr3,int tableSize,CFile &imgFile,CArray<DWORD,DWORD> *pPageDirPointerTab);

void PaeGetPageDirTable(DWORD baseAddr,int pageDirSize,CFile &ImgFile,CArray<DWORD,DWORD> *pPageDir);

void PaeGetPageTable(DWORD pdeOri,int pageTableSize,CFile &ImgFile,CArray<DWORD,DWORD>* pPageTab);

void PaeGetPhysicalPageData(DWORD PdeOrPteOri,BOOL isLarge,CFile &ImgFile,char* pTmpData);

DWORD PaeCR3ToPDPTBase(DWORD cr3);

INT64 PaePDPTEToPageDirBase(INT64 pdpte);

INT64 PaePdeToPageTableBase(INT64 pde);

INT64 PaePteToDataBase(INT64 pte);

INT64 PaePdeToDataBase(INT64 pde,BOOL isLarge);

void PaeDumpProcessMemory(PROCESS pro,CString imgFilePath);

//////////////////////////////////////////////////////////////////////////
// �����ڴ浼��

// ͳһ�ӿ�
void DumpProcessMemory(PROCESS pro,CString imgFilePath);





//////////////////////////////////////////////////////////////////////////
// �������

// Address translation
DWORD VirtualToPhysicalAddr(DWORD mPageDirectoryBase,DWORD virtualAdd,CString filePath);
