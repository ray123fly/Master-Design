// Regex-ASCII.cpp : �������̨Ӧ�ó������ڵ㡣
//
/************************************************************************/
/* @Brief 
/* @Author  ���� 
/* @Company  CQUPT
/* @E-mail   13272623580@wo.com.cn
/* @Date     2012.5                             
/************************************************************************/

#include "stdafx.h"
#include <fstream>
#include <string> 
#include <boost/regex.hpp>
using namespace boost;
using namespace std;
#define  BLOCK_SIZE 8192

#include "CodeTranster.h"
#include "CharacterSetCode.h"

#include "locale" 
#include <cassert> 

std::string::const_iterator base; 
char mCount='a';
long mPageNum=0;
long countNum=0;

std::ifstream fs;

string strOut;
//ofstream outFile(".\\log.html", fstream::out);
ofstream outFile;

char *pfName=new char[20];
char *pUtf8=new char[4096*2];

bool grep_callback(const  boost::smatch& what) 
{ 
	// 1���Ƚ�what[0]�е����ݿ����� pUtf8[] �У���ת���� wstring�͵����
	

// 1.1���������ļ��е�λ��
	countNum++;
	long mLocation=mPageNum*2*4096+what.position();

	// 1.2��ȡ��ƥ����
	//fs.seekg(mLocation,fstream::beg);
	//fs.read(pUtf8,(what[0]->second - what[0]->first));
	
	
	// 1.3��ת���� wstring ��
	string mUtf8(pUtf8);
	mUtf8.assign(what[0]);
	wstring mUnicode=UTF8ToWide(mUtf8);

	// ����
	//string strUtf8(what[0].first+7,what[0].second-8);
	//wstring wstrTitle=UTF8ToWide(strUtf8);

	// 1.4�����
	//wcout<<mUnicode<<endl;
	
	wcout<<L">>����������������������������������������������������������������������������"<<endl;
	wcout <<countNum<<L":";
	wcout <<L"\""<< mUnicode << L"\" found at index: " <<hex<<mLocation<< endl;

	//wcout<<L"��ҳ�����ǣ�"<<wstrTitle<<endl<<endl;
	
	
	strOut=WstrToStr(mUnicode);
	//outFile<<">>����������������������������������������������������������������������������"<<endl;
	//outFile<<countNum<< ":";
	//outFile<<"\""<< strOut << "\" found at index: " <<hex<<mLocation<< endl<<endl;

	
	pfName[6]=mCount;
	mCount++;
	outFile.open(pfName,ofstream::out);
	outFile<<strOut;
	outFile.close();

	/*
	cout<<">>����������������������������������������������������������������������������"<<endl;
	cout <<countNum<<":";
	cout <<"\""<< what[0] << "\" found at index: " <<hex<<mLocation<< endl<<endl;
	outFile<<">>����������������������������������������������������������������������������"<<endl;
	outFile<<countNum<< ":";
	outFile<<"\""<< what[0] << "\" found at index: " <<hex<<mLocation<< endl<<endl;
	*/
	return true; 
} 
void IndexClasses(const std::string& file,regex expression) 
{ 
	std::string::const_iterator start, end; 
	start = file.begin();
	end = file.end(); 
	base = start; 
	boost::regex_grep(grep_callback, start, end, expression, match_default|match_prev_avail|match_not_bob|match_not_dot_null); 
}

int main()
{
	locale loc( "Chinese-simplified" ); 
	wcout.imbue(loc); 

	std::string text;
	std::string mFilePath;
	std::string mPattern;
	char * pBuffer=new char[BLOCK_SIZE];
	std::cout<<"Ŀ���ļ�:"<<endl;
	std::getline(cin,mFilePath);
	cout<<endl;
	std::cout<<"ģʽ��"<<endl;
	std::getline(cin,mPattern);
	cout<<endl;


	boost::regex exp(mPattern.c_str());
	cout << "Processing file " << mFilePath<< endl;
	//std::ifstream fs(mFilePath,ifstream::binary);
	fs.open(mFilePath,ifstream::binary);
	fs.seekg(0,fstream::beg);

	strcpy(pfName,".\\log----.html");

	while(!fs.eof())
	{
		// �ض�λ�ļ���ȡλ��
		//fs.seekg(mPageNum*4096,fstream::beg);
	
		fs.read(pBuffer,BLOCK_SIZE);
		text.assign(pBuffer);
		IndexClasses(text,exp);
		mPageNum++;
	}
	cout<<"�����ɹ�������";
	fs.close();
	outFile.close();
	delete[] pBuffer;
	return 1;
}






