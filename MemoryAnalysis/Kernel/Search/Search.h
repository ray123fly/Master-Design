// search.h: 搜索程序的接口
/************************************************************************/
/* @Brief 
/* @Author  张雷  
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



//仅用来搜索关键字，结果保存在char* resultFile中
void SearchKeysInFile(const char * file,const char * keys,const char * resultFile);