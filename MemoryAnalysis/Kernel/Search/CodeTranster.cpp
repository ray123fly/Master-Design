
#include "stdafx.h"
#include <string>
#include "CodeTranster.h"

wstring UTF8ToWide(string & multibyte)
{
	size_t length = multibyte.length();
	if (length == 0)
		return wstring();

	wchar_t * wide = new wchar_t[length*2+2];
	if (wide == NULL)
		return wstring();
	int ret = (int)MultiByteToWideChar(CP_UTF8, 0, multibyte.c_str(), (int)multibyte.size(), wide, (int)length*2 - 1);
	wide[ret] = 0;
	wstring str = wide;
	delete[] wide;
	return str;
}


// wstring ����> string
string WstrToStr(const wstring & w)
{
	int len= WideCharToMultiByte(GetACP(), 0, w.c_str(), -1, NULL, 0, NULL,        NULL);
	char *buf= new char[len];
	WideCharToMultiByte(GetACP(), 0, w.c_str(), -1, buf, len, NULL, NULL);
	string s(buf);
	delete[] buf;
	return s;
}



wstring StrToWstr(const string& str)
{/*
	int len= MultiByteToWideChar(GetACP(), 0, str.c_str(), -1, NULL, 0, NULL,        NULL);
	wchar_t *buf= new char[len];
	MultiByteToWideChar(GetACP(), 0, str.c_str(), -1, buf, len, NULL, NULL);

	wstring ws(buf);
	delete[] buf;
	return ws;
	*/
	wstring ws;
	return ws;
}
//////////////////////////////////////////////////////////////////////////
/*
Ϊ��֧��Unicode���룬��Ҫ���ֽ�����ֽ�֮����໥ת����������ϵͳ������ʹ��ʱ��Ҫָ������ҳ����ʵ��Ӧ�ù����������������⣬Ȼ�������Ķ���Windows���ı�̡����ܽ����ȷ���÷���
WideCharToMultiByte�Ĵ���ҳ�����������ת�����ַ�����صĴ���ҳ��
MultiByteToWideChar�Ĵ���ҳ���������һ�����ֽ��ַ�����صĴ���ҳ��

���õĴ���ҳ��CP_ACP��CP_UTF8������
ʹ��CP_ACP����ҳ��ʵ����ANSI��Unicode֮���ת����
ʹ��CP_UTF8����ҳ��ʵ����UTF-8��Unicode֮���ת����
�����Ǵ���ʵ�֣�
*/

/*
//1.  ANSI to Unicode
wstring ANSIToUnicode( const string& str )
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0 );  
	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen );  
	wstring  rt;  
	rt = ( wchar_t* )pUnicode;
	delete  pUnicode; 

	return  rt;  
}
//2.  Unicode to ANSI
string UnicodeToANSI( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

string UnicodeToANSI( const wstring& str )
{
char*     pElementText;
int    iTextLen;
// wide char to multi char
iTextLen = WideCharToMultiByte( CP_ACP,0,str.c_str(),-1,NULL,0,NULL,NULL );
pElementText = new char[iTextLen + 1];
memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
WideCharToMultiByte( CP_ACP,0,str.c_str(),-1,pElementText,iTextLen,NULL,NULL );
string strText;
strText = pElementText;
delete[] pElementText;
return strText;
}

//3.  UTF-8 to Unicode
wstring UTF8ToUnicode( const string& str )
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0 );  
	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar( CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen );  
	wstring  rt;  
	rt = ( wchar_t* )pUnicode;
	delete  pUnicode; 

	return  rt;  
}

//4.  Unicode to UTF-8    
string UnicodeToUTF8( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_UTF8,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

*/


/*-----------------------------------------------------------------------------
char *-->wchar_t
��ͨ���򵥵���λ��ȡ��ϳɼ������.
�������ṩ��һʵ��.
dest_wchar:
����ת����Ŀ��ֽ��ַ�Ŀ���ַ.
src_str:
��ת����UTF-8����Դ�ַ��Ķ��ֽ�����.
����ֵ:
���ر�ת�����ַ����ֽ���. ������������⵽�Ƿ��ֽ�����, �򷵻�-1.

ע��! ���ݽ����Ŀ��ַ�Ӧ���ܱ��Ϸ�ת��ΪUTF-8������ַ�.
------------------------------------------------------------------------------*/

/*
size_t g_f_u8towc(wchar_t &dest_wchar, const unsigned char * src_str)
{
int count_bytes = 0;
unsigned char byte_one = 0, byte_other = 0x3f; // ����λ����������ȡλֵ 0x3f-->00111111
wchar_t tmp_wchar = L'0';

if (!src_str)//tring
return (size_t)-1;

for (;;) // ����ֽ����г���,���ݵ�һ���ֽ�ͷ��1������
{
if (src_str[0] <= 0x7f){
count_bytes = 1; // ASCII�ַ�: 0xxxxxxx( ~ 01111111)
byte_one = 0x7f; // ����λ������, ��ȡ��Чλֵ, ��ͬ 01111111
break;
}
if ( (src_str[0] >= 0xc0) && (src_str[0] <= 0xdf) ){
count_bytes = 2; // 110xxxxx(110 00000 ~ 110 111111)
byte_one = 0x1f; //00011111 ��һ�ֽ���Чλ�ĸ���
break;
}
if ( (src_str[0] >= 0xe0) && (src_str[0] <= 0xef) ){
count_bytes = 3; // 1110xxxx(1110 0000 ~ 1110 1111)
byte_one = 0xf; //00001111
break;
}
if ( (src_str[0] >= 0xf0) && (src_str[0] <= 0xf7) ){
count_bytes = 4; // 11110xxx(11110 000 ~ 11110 111)
byte_one = 0x7;
break;
}
if ( (src_str[0] >= 0xf8) && (src_str[0] <= 0xfb) ){
count_bytes = 5; // 111110xx(111110 00 ~ 111110 11)
byte_one = 0x3;
break;
}
if ( (src_str[0] >= 0xfc) && (src_str[0] <= 0xfd) ){
count_bytes = 6; // 1111110x(1111110 0 ~ 1111110 1)
byte_one = 0x1;
break;
}
return (size_t)-1; // ���ϽԲ�������Ϊ�Ƿ�����
}
// ���¼�����ȡUTF-8�����ַ������ֽڵ���Чλֵ
//�ȵõ���һ���ֽڵ���Чλ����
tmp_wchar = src_str[0] & byte_one;
for (int i=1; ;)//�д�
{
tmp_wchar <<= 6; // ����6λ��������ֽڵ���Чλֵ"λ��"��ֵ
tmp_wchar = tmp_wchar | (src_str[i] & byte_other);//������
}
// λֵ��ȡ__End!
dest_wchar = tmp_wchar;
return count_bytes;
}
*/

/*-------------------------------------------------------------------------
Ŀ�� :ʵ�� UTF-8��ʽ ��(char *) p->(UNICODE �� CString )wstr ��ת��
ѭ������g_f_u8towc()�����������char *pת���ɿ��ַ����ٴ浽CString �У�
----------------------------------------------------------------------------
*/

/*
CString g_f_pchar_to_wcs(const unsigned char * p )
{
CString wstr(_T(""));
wchar_t wc=L��1��;
size_t r=0; //size_t unsigned integer Result of sizeof operator
while (1)
{
if(*p==NULL||*p==��0��)//���Ϊ�ջ��߽�����Ϊ����ѭ��
break;
r=g_f_u8towc(wc,p);//��UTF-8��ʽ�ĵ�ַ�У���һ��wchar_t����
if(r==-1)//���ִ���
AfxMessageBox(_T("g_f_u8towc error"));
p=p+r; //��λ��׼����һ�ε�ת��
wstr+=wc;//��CString ��ֵ
}

return wstr;

}

*/