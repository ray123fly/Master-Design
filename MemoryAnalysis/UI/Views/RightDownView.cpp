// RightDownView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "RightDownView.h"


// CRightDownView

IMPLEMENT_DYNCREATE(CRightDownView, CView)

CRightDownView::CRightDownView()
{

}

CRightDownView::~CRightDownView()
{
}

BEGIN_MESSAGE_MAP(CRightDownView, CView)
END_MESSAGE_MAP()


// CRightDownView ��ͼ

void CRightDownView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// CRightDownView ���

#ifdef _DEBUG
void CRightDownView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CRightDownView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRightDownView ��Ϣ�������
