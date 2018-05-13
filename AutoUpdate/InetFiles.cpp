#include "StdAfx.h"
#include "InetFiles.h"

CInetFiles::CInetFiles(void)
{
}

CInetFiles::~CInetFiles(void)
{
	Clear();
}

CInetFile* CInetFiles::Pop(void)
{
	CAutoLock  lock(&m_lock);

	CInetFile* pInetFile = NULL;
	if (m_listInetFile.size() > 0)
	{
		pInetFile = m_listInetFile.front();
		m_listInetFile.pop_front();
	}

	return pInetFile;
}

void CInetFiles::Push(CInetFile* pInetFile)
{
	CAutoLock  lock(&m_lock);
	m_listInetFile.push_back(pInetFile);
}

DWORD CInetFiles::Count(void)
{
	CAutoLock lock(&m_lock);

	return (DWORD)m_listInetFile.size();
}

void CInetFiles::Clear(void)
{
	CAutoLock lock(&m_lock);

	while (m_listInetFile.size() > 0)
	{
		list<CInetFile *>::iterator it = m_listInetFile.begin();
		CInetFile* pInetFile = *it;
		m_listInetFile.erase(it);
		MG_SAFE_DELETE(pInetFile);
	}
}

CInetFile* CInetFiles::Front(void)
{
	CAutoLock lock(&m_lock);

	CInetFile* pInetFile = NULL;
	if (m_listInetFile.size() > 0)
	{
		pInetFile = m_listInetFile.front();
	}

	return pInetFile;
}

CInetFile* CInetFiles::Back(void)
{
	CAutoLock lock(&m_lock);

	CInetFile* pInetFile = NULL;
	if (m_listInetFile.size() > 0)
	{
		pInetFile = m_listInetFile.back();
	}

	return pInetFile;
}

void CInetFiles::DeleteItem(CInetFile* pInetFile)
{
	CAutoLock lock(&m_lock);
	m_listInetFile.remove(pInetFile);
	MG_SAFE_DELETE(pInetFile);
};