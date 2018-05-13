#include "StdAfx.h"
#include "FileInfoMgr.h"

CFileInfoMgr::CFileInfoMgr(void)
{
}

CFileInfoMgr::~CFileInfoMgr(void)
{
}

HRESULT  CFileInfoMgr::LastError()
{
	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
	if (SUCCEEDED(hr))
	{
		return E_FAIL;
	}
	return hr;
}

// This function gets the file version info structure
HRESULT CFileInfoMgr::GetFileVersion(LPCTSTR lpszFileName, VS_FIXEDFILEINFO *pvsf)
{
	DWORD dwHandle;
	DWORD cchver = GetFileVersionInfoSize(lpszFileName, &dwHandle);
	if (cchver == 0)
	{
		return LastError();
	}
	TCHAR* pver = new TCHAR[cchver];
	BOOL bret = GetFileVersionInfo(lpszFileName, dwHandle, cchver, pver);
	if (!bret)
	{
		return LastError();
	}
	UINT uLen;
	void *pbuf;
	bret = VerQueryValue(pver,_T("\\"), &pbuf, &uLen);
	if (!bret)
	{
		return LastError();
	}
	memcpy(pvsf, pbuf, sizeof(VS_FIXEDFILEINFO));
	delete[] pver;

	return S_OK;
}