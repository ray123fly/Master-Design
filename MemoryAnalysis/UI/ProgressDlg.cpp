// ProgressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


// CProgressDlg �Ի���

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


// CProgressDlg ��Ϣ�������


BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_ProgressCtrl.SetRange(lower,upper);
	m_ProgressCtrl.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CProgressDlg::Create()
{
	return CDialog::Create(CProgressDlg::IDD,NULL);//���⣿������
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
	// TODO: �ڴ����ר�ô����/����û���

	return CDialog::DestroyWindow();
}



