
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


// wstring ——> string
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
为了支持Unicode编码，需要多字节与宽字节之间的相互转换。这两个系统函数在使用时需要指定代码页，在实际应用过程中遇到乱码问题，然后重新阅读《Windows核心编程》，总结出正确的用法。
WideCharToMultiByte的代码页用来标记与新转换的字符串相关的代码页。
MultiByteToWideChar的代码页用来标记与一个多字节字符串相关的代码页。

常用的代码页由CP_ACP和CP_UTF8两个。
使用CP_ACP代码页就实现了ANSI与Unicode之间的转换。
使用CP_UTF8代码页就实现了UTF-8与Unicode之间的转换。
下面是代码实现：
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
它通过简单的码位截取与合成即可完成.
本函数提供这一实现.
dest_wchar:
保存转换后的宽字节字符目标地址.
src_str:
被转换的UTF-8编码源字符的多字节序列.
返回值:
返回被转换的字符的字节数. 若遇到错误或检测到非法字节序列, 则返回-1.

注意! 传递进来的宽字符应是能被合法转换为UTF-8编码的字符.
------------------------------------------------------------------------------*/

/*
size_t g_f_u8towc(wchar_t &dest_wchar, const unsigned char * src_str)
{
int count_bytes = 0;
unsigned char byte_one = 0, byte_other = 0x3f; // 用于位与运算以提取位值 0x3f-->00111111
wchar_t tmp_wchar = L'0';

if (!src_str)//tring
return (size_t)-1;

for (;;) // 检测字节序列长度,根据第一个字节头的1个个数
{
if (src_str[0] <= 0x7f){
count_bytes = 1; // ASCII字符: 0xxxxxxx( ~ 01111111)
byte_one = 0x7f; // 用于位与运算, 提取有效位值, 下同 01111111
break;
}
if ( (src_str[0] >= 0xc0) && (src_str[0] <= 0xdf) ){
count_bytes = 2; // 110xxxxx(110 00000 ~ 110 111111)
byte_one = 0x1f; //00011111 第一字节有效位的个数
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
return (size_t)-1; // 以上皆不满足则为非法序列
}
// 以下几行析取UTF-8编码字符各个字节的有效位值
//先得到第一个字节的有效位数据
tmp_wchar = src_str[0] & byte_one;
for (int i=1; ;)//有错
{
tmp_wchar <<= 6; // 左移6位后与后续字节的有效位值"位或"赋值
tmp_wchar = tmp_wchar | (src_str[i] & byte_other);//先与后或
}
// 位值析取__End!
dest_wchar = tmp_wchar;
return count_bytes;
}
*/

/*-------------------------------------------------------------------------
目的 :实现 UTF-8格式 的(char *) p->(UNICODE 下 CString )wstr 的转化
循环利用g_f_u8towc()这个函数来把char *p转换成宽字符，再存到CString 中！
----------------------------------------------------------------------------
*/

/*
CString g_f_pchar_to_wcs(const unsigned char * p )
{
CString wstr(_T(""));
wchar_t wc=L’1’;
size_t r=0; //size_t unsigned integer Result of sizeof operator
while (1)
{
if(*p==NULL||*p==’0’)//如果为空或者结束则为跳出循环
break;
r=g_f_u8towc(wc,p);//从UTF-8格式的地址中，读一个wchar_t出来
if(r==-1)//出现错误
AfxMessageBox(_T("g_f_u8towc error"));
p=p+r; //移位，准备下一次的转换
wstr+=wc;//给CString 附值
}

return wstr;

}

*/