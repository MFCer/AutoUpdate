#pragma once

#include <MsXml.h>
#include "UpdateFile.h"
#include "AutoLock.h"

#include <list>
using namespace std;

class CUpdateFiles
{
public:
	CUpdateFiles(void);
	virtual ~CUpdateFiles(void);

private:
	list<CUpdateFile *> m_listFile;
	CCriticalSection m_lock;

	TCHAR m_szWindowsDir[_MAX_PATH]; 
	TCHAR m_szSystemDir[_MAX_PATH];

	bool GetFileList(IXMLDOMNodeList* pDOMNodeList);

public:
	void ClearAllList(void);
	bool ReadFromXMLFile(LPCTSTR lpszFileName);
	bool AddUpdateFile(LPCTSTR lpszFileName);
	void DeleteItem(CUpdateFile * pUpdateFile);
	CUpdateFile* PopUpdateFile(void);
	void PushUpdateFile(CUpdateFile* pUpdateFile);

	// 检查需要下载的文件个数
	DWORD CheckNeedDownload(BOOL &bHasDownloadedFile);
	// 检查需要更新的文件个数
	DWORD CheckNeedUpdate(void);
	// 得到需要下载文件的总长度
	DWORD GetNeedDownloadTotalLength(void);
	DWORD Count(void);

	// 更新文件存在的根目录
	CString m_strBaseURL;

	DWORD GetExecuteFileList(list<CFileToExecute> &lstFileToExecute);
};
