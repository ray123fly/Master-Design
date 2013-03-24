#pragma once
#include "afxcmn.h"
#include "AfxTempl.h"



#include "MemoryManagement.h"

#include "MemInfoExtraction.h"


// CResultProcess �Ի���

class CResultProcess : public CDialog
{
	DECLARE_DYNAMIC(CResultProcess)

public:
	CResultProcess(CWnd* pParent = NULL);   // ��׼���캯��
	//CResultProcess(CArray* pProcessArray);
	virtual ~CResultProcess();

// �Ի�������
	enum { IDD = IDD_DIALOG_RESULT_PROCESS };

private:
	CArray <PROCESS,PROCESS> mProInfoArray;//���̵�EProcess�ṹ��Ϣ
	CArray <PROCESS,PROCESS>* pProInfoArray;//���̵�EProcess�ṹ��Ϣ
	CString imageFilePath;//����·��
	MemInfoExtraction mMemInfo;//



private:
	//�ؼ�
	//CProgressCtrl *pProCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	//CListCtrl m_ProcessList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedShow();

	void SetProcessInfo(PROCESS process);//��Ҫ�޸�
	void SetProcessInfo(CArray<PROCESS,PROCESS>* pProcessArray);
	void TransferPara(CString imgFilePath,MemInfoExtraction memInfo);
	void ExtractProcMemXP(PROCESS pro,CString imgFilePath);



	CListBox m_ListProcesses;
	afx_msg void OnLbnDblclkListProcesses();
//	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedSave();
	


	CListCtrl m_ListCtrProcesses;
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
//	afx_msg void OnBnClickedChromeMemoryExtract();


	afx_msg void OnBnClickedButton4();
	CString m_strProName;
	afx_msg void OnClickedNotepad();
};

UINT ThreadPageTableFunc(LPVOID pParam);