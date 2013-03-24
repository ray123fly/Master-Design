// MemoryAnalysisView.h : CMemoryAnalysisView ��Ľӿ�
//


#pragma once


class CMemoryAnalysisView : public CView
{
protected: // �������л�����
	CMemoryAnalysisView();
	DECLARE_DYNCREATE(CMemoryAnalysisView)

// ����
public:
	CMemoryAnalysisDoc* GetDocument() const;
	int m_CharCount;

// ����
public:
	void SetText(char* input,ULONGLONG  len);

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMemoryAnalysisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bFirst;
	UINT m_PageRows;				//һҳ������
	UINT m_PageCols;

	UINT m_TotalRows;					//�ܵ�����
	int m_TopLine;					//���������
	int m_LeftChar;					//���������


	void GetPageRowCols();

	void SetHorzScrollBar();		//����ˮƽ������
	void SetVertScrollBar();		//���ô�ֱ������

	UINT Point2Pos(CPoint point);

	BOOL m_StartDrag;
	BOOL m_bFocused;

	int m_TextStart;
	int m_HexStart;

	int m_LineHeight;				//�и�
	int m_CharWidth;				//�ַ����


	int m_SelStart, m_SelEnd, m_oriStart;		//��ѡ�е��ַ���


	char* m_Text;

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // MemoryAnalysisView.cpp �еĵ��԰汾
inline CMemoryAnalysisDoc* CMemoryAnalysisView::GetDocument() const
   { return reinterpret_cast<CMemoryAnalysisDoc*>(m_pDocument); }
#endif

