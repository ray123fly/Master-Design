// DialogUtf8Test.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryAnalysis.h"
#include "DialogUtf8Test.h"
#include "afxdialogex.h"

#include "CodeTranster.h"
#include "CharacterSetCode.h"

// CDialogUtf8Test 对话框

IMPLEMENT_DYNAMIC(CDialogUtf8Test, CDialog)

CDialogUtf8Test::CDialogUtf8Test(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUtf8Test::IDD, pParent)
{

	m_strUtf8 = _T("");
	m_strShow = _T("");
}

CDialogUtf8Test::~CDialogUtf8Test()
{
}

void CDialogUtf8Test::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UTF8, m_strUtf8);
	//  DDX_Control(pDX, IDC_SHOW, m_strShow);
	DDX_Text(pDX, IDC_SHOW, m_strShow);
}


BEGIN_MESSAGE_MAP(CDialogUtf8Test, CDialog)
	ON_BN_CLICKED(IDC_TRANSFER, &CDialogUtf8Test::OnBnClickedTransfer)
END_MESSAGE_MAP()


// CDialogUtf8Test 消息处理程序


char * ascii_to_hex(char * des, int len, const char * src)
{
	int n = 0, i = 0;
	unsigned char c;
	while( n < len-3 && src[i] != '\0')
	{
		c = src[i];
		int h = c >> 4;
		int l = c & 0x0F;
		if ( h > 9) h+=7;
		if (l > 9) l+=7;
		des[n++] = h | 0x30;
		des[n++] = l | 0x30;
		des[n++] = ' ';
	}
	return des;
}


static void PreShow(char* input,wchar_t * output)
{
	char* in=input;
	wchar_t* out=output;

	char tmp[5];
	tmp[4]='\0';

	wchar_t wout;
	for (;;)
	{
		// 出现死循环
		if (*in =='\0')
			break;

		if (*in =='\\' && *(in+1)== 'u')  //unicode
		{
			//// 去除\u，去除四字节的编码
			in+=2;
			strncpy(tmp,in,4);

			//转换wchar_t
			wout=strtol(tmp,NULL,16);
			*out++=wout;
			in+=4;
		}
		else  //ansi
		{
			// 读取当前字符
			strncpy(tmp,in,1);
			//tmp[1]='\0';

			//转换wchar_t
			//wout=strtol(tmp,NULL,16);
			*out++= 0x00ff&tmp[0];
			in++;
		}
	}
	*out=_T('\0');
}

/*
void transfer(CString before,wstring& after)
{
	int len=before.GetLength();
	wchar_t* pWchar=new wchar_t[len];
	//string tmp=before.GetString();
	//char* pTempUtf=tmp.c_str();
	char* pTempUtf=before.GetBuffer();
	wchar_t* pTempUnicode=pWchar;
	for(int index=0;index<len;index+=3)
	{

		CCharacterSetCode::UTF_8ToUnicode(pTempUnicode,pTempUtf);
		//判断当前位置的UTF-8编码是三字节？二字节？一字节
		if ((*pTempUtf>>7)==0)
		{
			pTempUtf+=1;
		} 
		else
		{
			pTempUtf+=3;
		}
		pTempUnicode++;
	}
	wstring s(pWchar);
	after=s;
	delete[] pWchar;

}

string UTF_8toString(CString utf8)
{
	wstring wstrTmp;
	transfer(utf8,wstrTmp);
	return WstrToStr(wstrTmp);
}
*/
void CDialogUtf8Test::OnBnClickedTransfer()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox("OK!");

	//谷歌浏览器服务条款
	//char pUtf8[]={0xE8,0xB0,0xB7,0xE6,0xAD,0x8C,0xE6,0xB5,0x8F,0xE8,0xA7,0x88,0xE5,0x99,0xA8,0xE6,0x9C,0x8D,0xE5,0x8A,0xA1,0xE6,0x9D,0xA1,0xE6,0xAC,0xBE};


	// 字符串操作需要在数组的最后添加结尾标志
	UpdateData(TRUE);

	//////////////////////////////////////////////////////////////////////////
	// 方法一：失败
	// 转成十六进制字符串
/*	char pTmp[500];
	ascii_to_hex(pTmp,m_strUtf8.GetLength(),m_strUtf8.GetBuffer());

	// utf-8    unicode     多字节
	string mUtf8(pTmp);
	wstring mUnicode=UTF8ToWide(mUtf8);
	string mString=WstrToStr(mUnicode);

	// 显示
	m_strShow.Append(mString.c_str());
*/
	//////////////////////////////////////////////////////////////////////////
	// 方法二：成功（含有\u的串）

	int size=m_strUtf8.GetLength();
	//wchar_t *out=new wchar_t[size];
	
	// \u序列：成功
	//PreShow(m_strUtf8.GetBuffer(),out);
	//string show=WstrToStr(out);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	// utf-8输出
	wstring out2;
	// 测试一：失败
//	string strUtf8(m_strUtf8.GetString());
//	out2=UTF8ToWide(strUtf8);
	//string show=WstrToStr(out2);

	// 测试二;成功
	//transfer(m_strUtf8,out2);
	//string show=WstrToStr(out2);

	// 测试三;成功
	string show=CCharacterSetCode::UTF_8toString(m_strUtf8);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 输出
	
	m_strShow.Append(show.c_str());
	UpdateData(FALSE);
}
