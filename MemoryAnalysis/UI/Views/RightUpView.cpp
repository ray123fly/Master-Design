// RightUpView.cpp : 实现文件
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


// CRightUpView 绘图

void CRightUpView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CRightUpView 诊断

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


// CRightUpView 消息处理程序
