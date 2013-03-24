/************************************************************************/
/* @Brief 
/* @Author  ���� �� 
/* @Company  CQUPT
/* @E-mail   ray123fly@sohu.com
/* @Date     2012.9.24                                   
/************************************************************************/

// Search.cpp:
#include "stdafx.h"
#include "Search.h"
#include <fstream>
#include <string> 
#include "CodeTranster.h"
#include "CharacterSetCode.h"
#include <boost/regex.hpp>

#include "deelx.h"

using namespace boost;
using namespace std;

// ��ʼ��
string mTmpFile;
//char mCount='a';
int mIndex(0);
long mPageNum(0);
//int mFileNum(0);

long countNum(0);

std::string::const_iterator base; 

char *pFlogfile;// ��־�ļ���
char** pLogReturn=NULL;      // ��־�ļ�������
char *pUtf8;// �����ʱ���ƥ���������������utf8ת����string����

// ���������ļ�
string logFile;
ofstream outFile;
string strOut;
bool bFind(false);

void SearchFileO(const std::string& file,boost::regex expression)
{
	std::ifstream fs;
	fs.open(file,ifstream::binary);
	fs.seekg(0,fstream::beg);

	char * pBuffer=new char[BLOCK_SIZE];
	std::string text;
	strcpy(pFlogfile,".\\log----.txt");

	while(!fs.eof())
	{
		fs.read(pBuffer,BLOCK_SIZE);
		text.assign(pBuffer);
		IndexClasses(text,expression);
		mPageNum++;
	}
	//	cout<<"�����ɹ�������";
	fs.close();
	outFile.close();
	delete[] pBuffer;
}
//////////////////////////////////////////////////////////////////////////
// �Ľ��ͣ���������־(���\u���͵�)
void SearchFile(const std::string& file,boost::regex expression, char** resultFile)
{
	pLogReturn=resultFile;
	std::ifstream fs;
	fs.open(file,ifstream::binary);
	fs.seekg(0,fstream::beg);

	//////////////////////////////////////////////////////////////////////////
	// 1����ʼ��

	//mCount='a';
	mTmpFile=file;
	mPageNum=0;
	countNum=0;

	pFlogfile=new char[20];// ��־�ļ���
	pUtf8=new char[4096*24];// �����ʱ���ƥ���������������utf8ת����string����

	char * pBuffer=new char[BLOCK_SIZE];
	std::string text;

	// ��־�ļ�
	//strcpy(pFlogfile,".\\log----.txt");
//	strcpy(pFlogfile,file.c_str);
	

	CString strFileName(file.c_str());
	strFileName=strFileName.Mid(strFileName.ReverseFind('\\')+1); 
	strFileName=strFileName.Left(strFileName.GetLength()-4);
	string tmp=strFileName.GetString();
	logFile=tmp+"_log.txt";
	
	//////////////////////////////////////////////////////////////////////////
	//2��start

	while(!fs.eof())
	{
		fs.read(pBuffer,BLOCK_SIZE);
		text.assign(pBuffer);
		IndexClasses(text,expression);
		mPageNum++;
	}

	// �����ɹ�����
	fs.close();

	if (bFind)// ������
	{
		bFind=false;// ��λ
		outFile.close();
		mIndex++;//�ҵ�һ���ļ�
	}

	delete[] pBuffer;
	delete[] pFlogfile;
	delete[] pUtf8;
	//mFileNum++;
}

void IndexClasses(const std::string& file,regex expression) 
{ 
	std::string::const_iterator start, end; 
	start = file.begin();
	end = file.end(); 
	base = start;
//	boost::match_flag_type flag;
//	flag=match_default|match_prev_avail|match_not_bob|match_not_dot_null;
	boost::regex_grep(grep_callback, start, end, expression, match_default); 
}


static void PreShow(const char* input,wchar_t * output)
{
	const char* in=input;
	wchar_t* out=output;

	char tmp[5];
	tmp[4]='\0';

	wchar_t wout;
	for (;;)
	{
		// ������ѭ��
		if (*in =='\0')
			break;

		if (*in =='\\' && *(in+1)== 'u')  //unicode
		{
			//// ȥ��\u��ȥ�����ֽڵı���
			in+=2;
			strncpy(tmp,in,4);

			//ת��wchar_t
			wout=strtol(tmp,NULL,16);
			*out++=wout;
			in+=4;
		}
		else  //ansi
		{
			// ��ȡ��ǰ�ַ�
			strncpy(tmp,in,1);
			//tmp[1]='\0';

			//ת��wchar_t
			//wout=strtol(tmp,NULL,16);
			*out++= 0x00ff&tmp[0];
			in++;
		}
	}
	*out=_T('\0');
}

bool grep_callback(const  boost::smatch& what)
{
	// 1���Ƚ�what[0]�е����ݿ����� pUtf8[] �У���ת���� wstring�͵����

	// 1.1���������ļ��е�λ��
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

	// 1.4�����-wstringת����string
	strOut=WstrToStr(mUnicode);

	// ÿ��ƥ�䣬���޸���־�ļ���
//	pFlogfile[6]=mCount;
//	int nIndex=mCount-'a';
//	strcpy(pLogReturn[nIndex],pFlogfile);
//	mCount++;

	//outFile.seekp(0,ios::end);

	if (bFind==false)
	{
		outFile.open(logFile,ofstream::out);
		outFile<<"������£�"<<endl;
		outFile<<"��������������������������������������������������������"<<endl;
		strcpy(pLogReturn[mIndex],logFile.c_str());
		bFind=true;
	}
	// ת��һ����д����


	int size=strOut.size();
	wchar_t *out=new wchar_t[size];
	PreShow(strOut.c_str(),out);

	string show=WstrToStr(out);
	
	outFile<<show<<endl;

	//outFile<<strOut<<endl;
	outFile<<endl;
	outFile<<"��������������������������������������������������������"<<endl;
	outFile<<endl;
	//outFile.open(pFlogfile,ofstream::out);
	//outFile<<strOut;
	//outFile.close();
	delete[] out;
	return TRUE;
} 

//////////////////////////////////////////////////////////////////////////
// �Ľ��ͣ���������־�����ģ�
void SearchFile_tianya(const std::string& file,boost::regex expression, char** resultFile)
{
	pLogReturn=resultFile;
	std::ifstream fs;
	fs.open(file,ifstream::binary);
	fs.seekg(0,fstream::beg);

	//////////////////////////////////////////////////////////////////////////
	// 1����ʼ��

	//mCount='a';
	mTmpFile=file;
	mPageNum=0;
	countNum=0;

	pFlogfile=new char[20];// ��־�ļ���
	pUtf8=new char[4096*24];// �����ʱ���ƥ���������������utf8ת����string����

	char * pBuffer=new char[BLOCK_SIZE];
	std::string text;

	// ��־�ļ�
	//strcpy(pFlogfile,".\\log----.txt");
	//	strcpy(pFlogfile,file.c_str);


	CString strFileName(file.c_str());
	strFileName=strFileName.Mid(strFileName.ReverseFind('\\')+1); 
	strFileName=strFileName.Left(strFileName.GetLength()-4);
	string tmp=strFileName.GetString();
	logFile=tmp+"_log.txt";

	//////////////////////////////////////////////////////////////////////////
	//2��start

	while(!fs.eof())
	{
		fs.read(pBuffer,BLOCK_SIZE);
		text.assign(pBuffer);
		IndexClasses_tianya(text,expression);
		mPageNum++;
	}

	// �����ɹ�����
	fs.close();

	if (bFind)// ������
	{
		bFind=false;// ��λ
		outFile.close();
		mIndex++;//�ҵ�һ���ļ�
	}

	delete[] pBuffer;
	delete[] pFlogfile;
	delete[] pUtf8;
	//mFileNum++;
}

void IndexClasses_tianya(const std::string& file,boost::regex expression) 
{ 
	std::string::const_iterator start, end; 
	start = file.begin();
	end = file.end(); 
	base = start;
	//	boost::match_flag_type flag;
	//	flag=match_default|match_prev_avail|match_not_bob|match_not_dot_null;
	boost::regex_grep(grep_callback_tianya, start, end, expression, match_default); 
}

bool grep_callback_tianya(const  boost::smatch& what)
{
	// 1���Ƚ�what[0]�е����ݿ����� pUtf8[] �У���ת���� wstring�͵����

	// 1.1���������ļ��е�λ��
	long mLocation=mPageNum*2*4096+what.position();

	// 1.2��ȡ��ƥ����
	//fs.seekg(mLocation,fstream::beg);
	//fs.read(pUtf8,(what[0]->second - what[0]->first));

	// 1.3��ת���� wstring ��
	string mUtf8(pUtf8);
	mUtf8.assign(what[0]);

	if (bFind==false)
	{
		outFile.open(logFile,ofstream::out);
		outFile<<"������£�"<<endl;
		outFile<<"��������������������������������������������������������"<<endl;
		strcpy(pLogReturn[mIndex],logFile.c_str());
		bFind=true;
	}
	// ת��һ����д����
	CString strUtf8(mUtf8.c_str());
	
	string show=CCharacterSetCode::UTF_8toString(strUtf8);

	outFile<<show<<endl;

	//outFile<<strOut<<endl;
	outFile<<endl;
	outFile<<"��������������������������������������������������������"<<endl;
	outFile<<endl;
	//outFile.open(pFlogfile,ofstream::out);
	//outFile<<strOut;
	//outFile.close();
	//delete[] out;
	return TRUE;
} 


//////////////////////////////////////////////////////////////////////////


// �����������ؼ��֣����������char* resultFile��
void SearchKeysInFile(const char* file,const char * keys,const char * resultFile)
{
	// text
	//char * text = "12.5, a1.1, 0.123, 178";

	// declare
	//static CRegexpT <char> regexp("\\b\\d+\\.\\d+");
	//char * key=m_Keyword.GetBuffer();
	static CRegexpT <char> regexp(keys);
	CContext * pContext;

	//���ļ�,�ֿ����
	std::ifstream ifs;
	ifs.open(file,ifstream::binary);
	ifs.seekg(0,ifstream::beg);

	std::ofstream ofs;
	ofs.open(resultFile,ofstream::binary);
	ofs.clear();
	//ofs.seekg(0,ofstream::beg);

	char * pBuffer=new char[BLOCK_SIZE];
	while(!ifs.eof())
	{
		ifs.read(pBuffer,BLOCK_SIZE);

		//////////////////////////////////////////////////////////////////////////
		//�ֿ����

		// prepare
		pContext= regexp.PrepareMatch(pBuffer);

		// loop
		MatchResult result = regexp.Match(pContext);

		while( result.IsMatched() )
		{
			// д�����ļ�
			//	printf("%.*s\n", result.GetEnd() - result.GetStart(), pBuffer + result.GetStart());
			
			// ��ת������д��
			ofs.write(pBuffer + result.GetStart(),result.GetEnd() - result.GetStart());
			ofs<<endl;
			ofs<<"��������������������������������������������"<<endl;
			// get next
			result = regexp.Match(pContext);
		}
	}

	// �����ɹ�����
	ifs.close();
	ofs.close();
	

	// release
	regexp.ReleaseContext(pContext);

	delete[] pBuffer;

}