// RightUpView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "RightUpView.h"


// CRightUpView

IMPLEMENT_DYNCREATE(CRightUpView, CView)

CRightUpView::CRightUpView()
{

}

CRightUpView::~CRightUpView()
{
}

BEGIN_MESSAGE_MAP(CRightUpView, CView)
END_MESSAGE_MAP()


// CRightUpView ��ͼ

void CRightUpView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// CRightUpView ���

#ifdef _DEBUG
void CRightUpView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CRightUpView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRightUpView ��Ϣ�������
