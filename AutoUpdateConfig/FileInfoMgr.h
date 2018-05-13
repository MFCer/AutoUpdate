#pragma once

#include <winver.h>
#pragma comment(lib, "Version.lib")

class CFileInfoMgr
{
public:
	CFileInfoMgr(void);
	~CFileInfoMgr(void);

	static HRESULT LastError();
	static HRESULT GetFileVersion(LPCTSTR lpszFileName, VS_FIXEDFILEINFO *pvsf);
};
