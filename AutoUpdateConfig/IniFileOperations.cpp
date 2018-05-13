#include "StdAfx.h"
#include "IniFileOperations.h"

CIniFileOperations::CIniFileOperations(void)
{
	TCHAR szPath	[_MAX_PATH];
	TCHAR szDrive	[_MAX_DRIVE];
	TCHAR szDir		[_MAX_DIR];
	TCHAR szFName	[_MAX_FNAME];
	TCHAR szExt		[_MAX_EXT];
	GetModuleFileName(NULL, szPath, _MAX_PATH);

#if  _MSC_VER >= 1400 // vs2005+
	_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
#else
	_tsplitpath(szPath, szDrive, szDir, szFName, szExt);
#endif

#if  _MSC_VER >= 1400 // vs2005+
	_tmakepath_s(szPath, _MAX_PATH, szDrive, szDir, IFO_FILE_NAME, IFO_FILE_EXT);
#else
	_tmakepath(szPath, szDrive, szDir, IFO_FILE_NAME, IFO_FILE_EXT);
#endif

	m_strIniFilePath = szPath;
}

CIniFileOperations::~CIniFileOperations(void)
{
}

INT CIniFileOperations::GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iDefault)
{
	return GetPrivateProfileInt(lpszSection, lpszKey, iDefault, m_strIniFilePath);
}

void CIniFileOperations::WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iIn)
{
	CString str;
	str.Format(_T("%d"), iIn);
	WriteIniString(lpszSection, lpszKey, str);
}

CString CIniFileOperations::GetIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpDefault)
{
	CString strReturnedString;
	TCHAR szRetStr[_MAX_PATH];
	GetPrivateProfileString(lpszSection, lpszKey, lpDefault, szRetStr, _MAX_PATH, m_strIniFilePath);
	strReturnedString = szRetStr;
	return strReturnedString;
}

void CIniFileOperations::WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpString)
{
	WritePrivateProfileString(lpszSection, lpszKey, lpString, m_strIniFilePath);
}

BOOL CIniFileOperations::GetIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct)
{
	return GetPrivateProfileStruct(lpszSection, lpszKey, lpStruct, uSizeStruct, m_strIniFilePath);
}

void CIniFileOperations::WriteIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct)
{
	WritePrivateProfileStruct(lpszSection, lpszKey, lpStruct, uSizeStruct, m_strIniFilePath);
}