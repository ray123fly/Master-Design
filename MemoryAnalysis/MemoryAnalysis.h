// MemoryAnalysis.h : MemoryAnalysis Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMemoryAnalysisApp:
// �йش����ʵ�֣������ MemoryAnalysis.cpp
//

class CMemoryAnalysisApp : public CWinApp
{
public:
	CMemoryAnalysisApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMemoryAnalysisApp theApp;