#pragma once

#include "InetFile.h"
#include "AutoLock.h"

#include <list>
using namespace std;

class CInetFiles
{
public:
	CInetFiles(void);
	virtual ~CInetFiles(void);
	
private:
	list<CInetFile *> m_listInetFile;
	CCriticalSection m_lock;

public:
	CInetFile* Pop(void);
	CInetFile* Front(void);
	CInetFile* Back(void);
	void Push(CInetFile* pInetFile);
	DWORD Count(void);
	void Clear(void);
	void DeleteItem(CInetFile* pInetFile);
};
