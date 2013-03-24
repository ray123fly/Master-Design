// MemoryAnalysisView.h : CMemoryAnalysisView 类的接口
//


#pragma once


class CMemoryAnalysisView : public CView
{
protected: // 仅从序列化创建
	CMemoryAnalysisView();
	DECLARE_DYNCREATE(CMemoryAnalysisView)

// 属性
public:
	CMemoryAnalysisDoc* GetDocument() const;
	int m_CharCount;

// 操作
public:
	void SetText(char* input,ULONGLONG  len);

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMemoryAnalysisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bFirst;
	UINT m_PageRows;				//一页的行数
	UINT m_PageCols;

	UINT m_TotalRows;					//总的行数
	int m_TopLine;					//最上面的行
	int m_LeftChar;					//最左面的列


	void GetPageRowCols();

	void SetHorzScrollBar();		//设置水平滚动条
	void SetVertScrollBar();		//设置垂直滚动条

	UINT Point2Pos(CPoint point);

	BOOL m_StartDrag;
	BOOL m_bFocused;

	int m_TextStart;
	int m_HexStart;

	int m_LineHeight;				//行高
	int m_CharWidth;				//字符宽度


	int m_SelStart, m_SelEnd, m_oriStart;		//被选中的字符串


	char* m_Text;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnWebAnalysis();
//	afx_msg void OnWebWebanalysisView();
//	afx_msg void OnWebWebanalysis();
};

#ifndef _DEBUG  // MemoryAnalysisView.cpp 中的调试版本
inline CMemoryAnalysisDoc* CMemoryAnalysisView::GetDocument() const
   { return reinterpret_cast<CMemoryAnalysisDoc*>(m_pDocument); }
#endif

