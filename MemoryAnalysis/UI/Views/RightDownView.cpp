// RightDownView.cpp : 实现文件
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


// CRightDownView 绘图

void CRightDownView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CRightDownView 诊断

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


// CRightDownView 消息处理程序
