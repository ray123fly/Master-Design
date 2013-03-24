// MemoryAnalysisView.cpp : CMemoryAnalysisView ���ʵ��
//

#include "stdafx.h"
#include "MemoryAnalysis.h"

#include "MemoryAnalysisDoc.h"
#include "MemoryAnalysisView.h"

#include "DialogWebAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMemoryAnalysisView

IMPLEMENT_DYNCREATE(CMemoryAnalysisView, CView)

BEGIN_MESSAGE_MAP(CMemoryAnalysisView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_HSCROLL()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETFOCUS()
	ON_WM_VSCROLL()
//	ON_COMMAND(IDM_WEB_ANALYSIS, &CMemoryAnalysisView::OnWebAnalysis)
//	ON_COMMAND(IDM_WEB_WEBANALYSIS_VIEW, &CMemoryAnalysisView::OnWebWebanalysisView)
//ON_COMMAND(IDM_WEB_WEBANALYSIS, &CMemoryAnalysisView::OnWebWebanalysis)
END_MESSAGE_MAP()

// CMemoryAnalysisView ����/����

CMemoryAnalysisView::CMemoryAnalysisView()
{
	// TODO: �ڴ˴���ӹ������

}

CMemoryAnalysisView::~CMemoryAnalysisView()
{
}

BOOL CMemoryAnalysisView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CMemoryAnalysisView ����

void CMemoryAnalysisView::OnDraw(CDC* pDC)
{
	CMemoryAnalysisDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	CRect rc;
	GetClientRect(rc);

	if(m_Text == NULL)
	{
		pDC->FillRect(rc, &CBrush(RGB(255,255,255)));
		return;
	}


	CDC	dc;
	dc.CreateCompatibleDC(pDC);

	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	dc.SelectObject(bm);
	dc.SetBoundsRect(&rc, DCB_DISABLE);

	CFont font;
	font.CreateFont ( m_LineHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, 0 );
	CFont* oldFont = dc.SelectObject ( &font );

	CBrush bkBrush(RGB(255,255,255));
	dc.FillRect(rc, &bkBrush);

	UINT lineCY, lineCX;
	UINT dwPos;
	char currentchar[20];
	//UCHAR text[17];
	char text[17];

	int lineBeginPos;

	m_HexStart = m_CharWidth * (14 - m_LeftChar);	//�����������ʾ��ʼλ��
	m_TextStart = m_HexStart + m_CharWidth * 50;	//����ASCII����ʾ��ʼλ��

	for(UINT i = 0; i < m_PageRows; i++)			//������ʾ����
	{
		lineBeginPos = (m_TopLine+i-1)*16;			//���㵱ǰ�п�ʼ�ַ�������ܻ�������λ��
		lineCY = i * m_LineHeight;					//��ǰ�е�Y����
		sprintf(currentchar, "%08Xh:", lineBeginPos);

		dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		dc.SetBkColor(RGB(255,255,255));

		dc.TextOut (m_CharWidth*(2-m_LeftChar), lineCY, currentchar);	//��ʾ��ַ��ֵ

		memcpy(text,&m_Text[lineBeginPos],16);	//ȡ��ǰ������
		text[16] = '\0';

		for(int l = 0; l < 16; l++)
		{
			if(text[l] < 32)
				text[l] = 46;	//���ɼ�ASCII�ַ���.��ʾ
		}
		if(lineBeginPos+16 > m_CharCount)
		{
			text[m_CharCount%16] = '\0';	//�������ݲ���һ��ʱ
		}

		dc.TextOut (m_TextStart, lineCY, text);//��ʾ�����ASCII�ַ�
		//	dc.TextOut()


		//��ѡ��ʱ��ɫ��ʾ�����ASCII�ַ�
		int selStart = (m_SelStart - lineBeginPos)<0? 0:(m_SelStart - lineBeginPos);
		int selEnd = (m_SelEnd - lineBeginPos)>15? 15:(m_SelEnd - lineBeginPos);
		if(selStart <= 15 && selEnd >= 0)
		{
			dc.SetBkColor(::GetSysColor ( COLOR_HIGHLIGHT ));
			dc.SetTextColor(::GetSysColor ( COLOR_HIGHLIGHTTEXT ));
			text[selEnd+1]='\0';
			dc.TextOut (m_TextStart+selStart*m_CharWidth, lineCY, &text[selStart]);
		}
		else
		{
			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			dc.SetBkColor(RGB(255,255,255));
		}

		for(UINT j = 0; j < 16; j++)
		{
			dwPos = j + lineBeginPos;

			if(dwPos > (m_CharCount-1))
				goto bitblt;			//��ʾ�����һ���˳�

			if(dwPos >= m_SelStart && dwPos <= m_SelEnd)
			{
				dc.SetBkColor(::GetSysColor ( COLOR_HIGHLIGHT ));
				dc.SetTextColor(::GetSysColor ( COLOR_HIGHLIGHTTEXT ));
			}
			else
			{
				dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(RGB(255,255,255));
			}

			lineCX = m_HexStart + j * m_CharWidth *3 ;				//���㵱ǰҪ��ʾ�Ķ������ַ���λ��
			sprintf(currentchar, "%02X", ((UCHAR *)m_Text)[dwPos]);	
			dc.TextOut (lineCX, lineCY, currentchar);				//��ʾ�������ַ�

			if(dwPos == m_SelEnd)									//ѡ�е����ַ�����ʾ
			{
				dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
				dc.SetBkColor(RGB(255,0,0));
				UCHAR cHex = m_Text[dwPos];
				if(m_bFirst)//�������ַ��ĸ���λѡ��
				{
					cHex = cHex & 0xf0;
					cHex = cHex >> 4;
					sprintf(currentchar, "%X", cHex);
					dc.TextOut (lineCX, lineCY, currentchar);
				}
				else
				{
					lineCX = m_HexStart + j * m_CharWidth *3 + m_CharWidth;
					cHex = cHex & 0x0f;
					sprintf(currentchar, "%X", cHex);
					dc.TextOut (lineCX, lineCY, currentchar);
				}
			}
		}
	}

bitblt:
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
	dc.DeleteDC ();
}


// CMemoryAnalysisView ��ӡ

BOOL CMemoryAnalysisView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CMemoryAnalysisView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CMemoryAnalysisView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CMemoryAnalysisView ���

#ifdef _DEBUG
void CMemoryAnalysisView::AssertValid() const
{
	CView::AssertValid();
}

void CMemoryAnalysisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMemoryAnalysisDoc* CMemoryAnalysisView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMemoryAnalysisDoc)));
	return (CMemoryAnalysisDoc*)m_pDocument;
}
#endif //_DEBUG


// CMemoryAnalysisView ��Ϣ�������

void CMemoryAnalysisView::SetText(char* input,ULONGLONG  len)
{
	m_Text = input;

	m_CharCount = len;
	m_TotalRows = m_CharCount/16;

	m_TopLine = 1;
	m_LeftChar = 0;
	m_SelStart = 0;
	m_SelEnd = 0;
	m_StartDrag = FALSE;
	m_bFirst = TRUE;

	/*CFont font;
	m_LineHeight = 16;
	font.CreateFont(m_LineHeight,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH | FF_MODERN,0);
	CFont* oldFont = GetDC()->SelectObject(&font);*/

	CFont font;
	m_LineHeight = 16;
	font.CreateFont ( m_LineHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, 0 );
	CFont* oldFont = GetDC()->SelectObject ( &font );

	CSize size = GetDC()->GetTextExtent("A");
	m_LineHeight = size.cy;
	m_CharWidth = size.cx;
	
	GetPageRowCols();
	SetVertScrollBar();
	SetHorzScrollBar();
}

//����һҳ�ܹ���ʾ���ַ�������������
void CMemoryAnalysisView::GetPageRowCols()
{
	CRect rc;
	GetClientRect(rc);
	m_PageRows =  rc.Height ()/m_LineHeight;
	m_PageCols = rc.Width ()/m_CharWidth;
}

// ����ˮƽ������
void CMemoryAnalysisView::SetHorzScrollBar()
{
	if((80 - m_LeftChar + 2) < m_PageCols) m_LeftChar = 0;

	SCROLLINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = 80;
	si.nPage = m_PageCols;
	si.nPos = m_LeftChar;
	VERIFY(SetScrollInfo(SB_HORZ, &si, TRUE));
	Invalidate(FALSE);
}

// ���ô�ֱ������
void CMemoryAnalysisView::SetVertScrollBar()
{
	/*if ((m_TotalRows - m_TopLine + 2) < m_PageRows) 
	{
		m_TopLine = 1;
	}

	SCROLLINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = m_TotalRows;
	si.nPage = m_PageRows;
	si.nPos = m_TopLine - 1;
	VERIFY(SetScrollInfo(SB_VERT,&si,TRUE));
	Invalidate();*/

	if((m_TotalRows - m_TopLine +  2) < m_PageRows) m_TopLine = 1;

	SCROLLINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = m_TotalRows;
	si.nPage = m_PageRows;
	si.nPos = m_TopLine-1;
	VERIFY(SetScrollInfo(SB_VERT, &si, TRUE));
	Invalidate(FALSE);


}

//�������λ������굱ǰָ����ַ�
UINT CMemoryAnalysisView::Point2Pos(CPoint point)
{
	if(m_Text == NULL)
		return 0;
	UINT line = m_TopLine + point.y / m_LineHeight - 1;//������ڵ���
	UINT charset;

	if(point.x < m_HexStart)
		charset = 0;
	else if(point.x < m_HexStart + m_CharWidth * 48)
		charset = (point.x - m_HexStart)/(m_CharWidth*3);
	else if(point.x < m_TextStart + 16 * m_CharWidth)
		charset = (point.x - m_TextStart)/m_CharWidth;
	else
		charset = 15;
	return ((line * 16 + charset)>m_CharCount?m_CharCount-1:(line * 16 + charset));
}

void CMemoryAnalysisView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnHScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	VERIFY(GetScrollInfo(SB_HORZ, &si));

	switch (nSBCode)
	{
	case SB_LEFT:
		m_LeftChar = 0;
		break;
	case SB_BOTTOM:
		m_LeftChar = 80 - m_PageCols + 1;
		break;
	case SB_LINEUP:
		m_LeftChar = m_LeftChar - 1;
		break;
	case SB_LINEDOWN:
		m_LeftChar = m_LeftChar + 1;
		break;
	case SB_PAGEUP:
		m_LeftChar = m_LeftChar - si.nPage + 1;
		break;
	case SB_PAGEDOWN:
		m_LeftChar = m_LeftChar + si.nPage - 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_LeftChar = si.nTrackPos;
		break;
	default:
		return;
	}

	if((m_LeftChar + m_PageCols - 2) > 80) m_LeftChar = 80 - m_PageCols +2;
	if(m_LeftChar < 0) m_LeftChar = 0;

	SetHorzScrollBar();
}

void CMemoryAnalysisView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: �ڴ˴������Ϣ����������
	m_bFocused = FALSE;
}

void CMemoryAnalysisView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnLButtonDown(nFlags, point);

	m_StartDrag = TRUE;
	m_bFirst = TRUE;

	m_oriStart = m_SelStart = m_SelEnd = Point2Pos(point);
	Invalidate(FALSE);
}

void CMemoryAnalysisView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnLButtonUp(nFlags, point);

	if(m_StartDrag)
	{
		m_SelEnd = Point2Pos(point);
		if(m_SelEnd < m_oriStart)
		{
			m_SelStart = m_SelEnd;
			m_SelEnd = m_oriStart;
		}
		else
		{
			m_SelStart = m_oriStart;
		}
		Invalidate(FALSE);
		m_StartDrag = FALSE;
	}
}

void CMemoryAnalysisView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnMouseMove(nFlags, point);

	if(m_StartDrag)
	{
		m_SelEnd = Point2Pos(point);
		if(m_SelEnd < m_oriStart)	//����ѡ��
		{
			m_SelStart = m_SelEnd;
			m_SelEnd = m_oriStart;
		}
		else
		{
			m_SelStart = m_oriStart;
		}
		Invalidate(FALSE);
	}
}

BOOL CMemoryAnalysisView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if(zDelta > 0)
	{
		m_TopLine = --m_TopLine < 1 ? 1 : m_TopLine;
	}
	else
	{
		if((m_TopLine + m_PageRows -2) < m_TotalRows)
			m_TopLine+=1;
	}
	SetVertScrollBar();
	SetHorzScrollBar();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMemoryAnalysisView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: �ڴ˴������Ϣ����������
	m_bFocused = TRUE;
}

void CMemoryAnalysisView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnVScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	VERIFY(GetScrollInfo(SB_VERT, &si));

	switch (nSBCode)
	{
	case SB_TOP:
		m_TopLine = 0;
		break;
	case SB_BOTTOM:
		m_TopLine = m_TotalRows  - m_PageRows + 1;
		break;
	case SB_LINEUP:
		m_TopLine = m_TopLine - 1;
		break;
	case SB_LINEDOWN:
		m_TopLine = m_TopLine + 1;
		break;
	case SB_PAGEUP:
		m_TopLine = m_TopLine - si.nPage + 1;
		break;
	case SB_PAGEDOWN:
		m_TopLine = m_TopLine + si.nPage - 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_TopLine = si.nTrackPos+1;
		break;
	default:
		return;
	}

	if((m_TopLine + m_PageRows -2) > m_TotalRows) m_TopLine = m_TotalRows - m_PageRows + 2;
	if(m_TopLine < 1) m_TopLine = 1;

	SetVertScrollBar();
}