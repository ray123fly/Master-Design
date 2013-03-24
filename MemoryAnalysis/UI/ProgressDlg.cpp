// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


// CProgressDlg 对话框

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	pos = 0;
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
END_MESSAGE_MAP()


// CProgressDlg 消息处理程序


BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ProgressCtrl.SetRange(lower,upper);
	m_ProgressCtrl.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CProgressDlg::Create()
{
	return CDialog::Create(CProgressDlg::IDD,NULL);//问题？？？？
}

void CProgressDlg::SetProgressRange(int begin,int end)
{
	lower=begin;
	upper=end;

}
void CProgressDlg::UpdateProgressCtrl(int curPos)
{
	m_ProgressCtrl.SetPos(curPos);
	UpdateData(TRUE);
}



VOID CProgressDlg::Close()
{
	DestroyWindow();

}


BOOL CProgressDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::DestroyWindow();
}



