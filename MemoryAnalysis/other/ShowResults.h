#pragma once


//#include "templ.h"

// CShowResults ÃüÁîÄ¿±ê
struct PROCESS_STRUCT
{
	CString strName;
};

class CShowResults : public CObject
{
public:
	CShowResults();
	virtual ~CShowResults();
public:
	//CArray <PROCESS_STRUCT,PROCESS_STRUCT> mProcessArray;//process information array

	//void ShowProcessInfo(CString fProcessInfoPath);//Processs
	void ShowKeyWordSeacchResult(void);
	//void ShowProcessInfo(CArray processArray);
};


