#pragma once


// CRightDownView ��ͼ

class CRightDownView : public CView
{
	DECLARE_DYNCREATE(CRightDownView)

protected:
	CRightDownView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CRightDownView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


