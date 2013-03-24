#pragma once

#include <Windows.h>
#include <string>
using namespace std;

wstring UTF8ToWide(string& multibyte);
string WstrToStr(const wstring&w);

wstring StrToWstr(const string& str);

//size_t g_f_u8towc(wchar_t &dest_wchar, const unsigned char * src_str);