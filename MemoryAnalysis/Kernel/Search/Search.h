// search.h: ��������Ľӿ�
/************************************************************************/
/* @Brief 
/* @Author  ���� �� 
/* @Company  CQUPT
/* @E-mail   ray123fly@sohu.com
/* @Date     2012.9.24                                   
/************************************************************************/

#pragma once
#include "MemoryAnalysis.h"
#include <string>

#include <boost/regex.hpp>
//using namespace std;
//using namespace boost;


#define  BLOCK_SIZE 8192
// sina
void IndexClasses(const std::string& file,boost::regex expression);
bool grep_callback(const boost::smatch& what);

//tianya
void IndexClasses_tianya(const std::string& file,boost::regex expression);
bool grep_callback_tianya(const  boost::smatch& what);


void SearchFileO(const std::string& file,boost::regex expression);

// test_ok
void SearchFile(const std::string& file,boost::regex expression,char** resultFile);
void SearchFile_tianya(const std::string& file,boost::regex expression, char** resultFile);



//�����������ؼ��֣����������char* resultFile��
void SearchKeysInFile(const char * file,const char * keys,const char * resultFile);