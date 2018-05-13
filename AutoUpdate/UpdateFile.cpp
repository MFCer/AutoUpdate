#include "StdAfx.h"
#include "UpdateFile.h"
#include "..\Common\HashMgr.h"

CUpdateFile::CUpdateFile(LPCTSTR lpszFilePath)
	: m_ullSize(0)
	, m_bRegister(false)
	, m_bZipFile(false)
	, m_bForceOverwrite(false)
	, m_bUpdateNotExist(false)
	, m_bExecute(false)
	, m_bLocalNeedUpdate(false)
	, m_strURL(_T(""))
// 	, m_strUserName(_T(""))
// 	, m_strPassword(_T(""))
{
	m_iLangID = ALL_SUPPORT;	// 支持所有语言
	m_strFilePath = lpszFilePath;
	m_strInstallDir = _T("CURRENTDIR");
	m_strLocalPath = _T("");
//	ZeroMemory(&m_fileTime, sizeof(FILETIME));
	ZeroMemory(&m_vfixedFileInfo, sizeof(m_vfixedFileInfo));
}

CUpdateFile::~CUpdateFile(void)
{
}

BOOL CUpdateFile::InitUpdateFile()
{
	if (m_strFilePath.IsEmpty())
	{
		return FALSE;
	}

	bool bRet = FALSE;
	CFile f;
	if (f.Open(m_strFilePath, CFile::modeRead | CFile::shareDenyNone))
	{
		m_ullSize = f.GetLength();
		m_strFileName = f.GetFileName();
		f.Close();
		bRet = TRUE;
		GetFileVersion((TCHAR*)(LPCTSTR)m_strFilePath, &m_vfixedFileInfo);
	}

// 	if (SUCCEEDED(GetFileDate((TCHAR*)(LPCTSTR)m_strFilePath, &m_fileTime)))
// 	{
// 		bRet = TRUE;
// 	}

	return bRet;
}

//void printtime (FILETIME *t) {
//
//   FILETIME lft;
//   FILETIME *ft = &lft;
//   FileTimeToLocalFileTime(t,ft);
//   printf("%08x %08x",ft->dwHighDateTime,ft->dwLowDateTime); {
//      SYSTEMTIME stCreate;
//      BOOL bret = FileTimeToSystemTime(ft,&stCreate);
//      printf("    %02d/%02d/%d  %02d:%02d:%02d\n",
//      stCreate.wMonth, stCreate.wDay, stCreate.wYear,
//      stCreate.wHour, stCreate.wMinute, stCreate.wSecond);
//   }
//}
//
//int main(int argc, TCHAR* argv[]) {
//    printf("usage: checkversion file1 file2\n"
//      "\tReports which file is newer, first by checking the file version in "
//      "\tthe version resource, then by checking the date\n\n" );
//
//   if (argc != 3) 
//      return 1;
//
//   int newer = WhichIsNewer(argv[1],argv[2]);
//   switch(newer) {
//      case 1:
//      case 2: printf("%s is newer\n",argv[newer]); break;
//      case 3: printf("they are the same version\n"); break;
//      case 0:
//      default: printf("there was an error\n"); break;
//   }
//
//   return !newer;
//}
// 
// INT CUpdateFile::WhichIsNewer(TCHAR *fname1, TCHAR *fname2)
// {
// 	// 1 if argv[1] is newer
// 	// 2 if argv[2] is newer
// 	// 3 if they are the same version
// 	// 0 if there is an error
// 
// 	INT ndxNewerFile;
// 	HRESULT ret;
// 	VS_FIXEDFILEINFO vsf1, vsf2;
// 
// 	if ( SUCCEEDED((ret = GetFileVersion(fname1, &vsf1))) && SUCCEEDED((ret = GetFileVersion(fname2, &vsf2))))
// 	{
// 		// both files have a file version resource
// 		// compare by file version resource
// 		if (vsf1.dwFileVersionMS > vsf2.dwFileVersionMS)
// 		{
// 			ndxNewerFile = 1;
// 		}
// 		else if (vsf1.dwFileVersionMS < vsf2.dwFileVersionMS)
// 		{
// 			ndxNewerFile = 2;
// 		}
// 		else
// 		{   // if (vsf1.dwFileVersionMS == vsf2.dwFileVersionMS)
// 			if (vsf1.dwFileVersionLS > vsf2.dwFileVersionLS)
// 			{
// 				ndxNewerFile = 1;
// 			}
// 			else if (vsf1.dwFileVersionLS < vsf2.dwFileVersionLS)
// 			{
// 				ndxNewerFile = 2;
// 			}
// 			// if (vsf1.dwFileVersionLS == vsf2.dwFileVersionLS)
// 			else
// 			{
// 				ndxNewerFile = 3;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// compare by date
// 		FILETIME ft1, ft2;
// 		if (SUCCEEDED((ret = GetFileDate(fname1, &ft1))) && SUCCEEDED((ret = GetFileDate(fname2, &ft2))))
// 		{
// 			LONG x = CompareFileTime(&ft1, &ft2);
// 			if (x == -1)
// 			{
// 				ndxNewerFile = 2;
// 			}
// 			else if (x == 0)
// 			{
// 				ndxNewerFile = 3;
// 			}
// 			else if (x == 1)
// 			{
// 				ndxNewerFile = 1;
// 			}
// 			else
// 			{
// 				EmitErrorMsg(E_FAIL);
// 				return 0;
// 			}
// 		}
// 		else
// 		{
// 			EmitErrorMsg(ret);
// 			return 0;
// 		}
// 	}
// 	return ndxNewerFile;
// }

INT CUpdateFile::CheckVersionNewer(CString &strVersion1, CString &strVersion2)
{
	WORD wVer1_1, wVer1_2, wVer1_3, wVer1_4;
	WORD wVer2_1, wVer2_2, wVer2_3, wVer2_4;
#if _MSC_VER >= 1400 // vs2005+
	_stscanf_s(strVersion1.GetBuffer(0), _T("%hd.%hd.%hd.%hd"), &wVer1_1, &wVer1_2, &wVer1_3, &wVer1_4);
#else
	_stscanf(strVersion1.GetBuffer(0), _T("%hd.%hd.%hd.%hd"), &wVer1_1, &wVer1_2, &wVer1_3, &wVer1_4);
#endif
	strVersion1.ReleaseBuffer();

#if _MSC_VER >= 1400 // vs2005+
	_stscanf_s(strVersion2.GetBuffer(0), _T("%hd.%hd.%hd.%hd"), &wVer2_1, &wVer2_2, &wVer2_3, &wVer2_4);
#else
	_stscanf(strVersion2.GetBuffer(0), _T("%hd.%hd.%hd.%hd"), &wVer2_1, &wVer2_2, &wVer2_3, &wVer2_4);
#endif
	strVersion2.ReleaseBuffer();

	return 
		wVer1_1 > wVer2_1 ? 1 : (wVer1_1 < wVer2_1 ? 2 : (
			wVer1_2 > wVer2_2 ? 1 : (wVer1_2 < wVer2_2 ? 2 : (
				wVer1_3 > wVer2_3 ? 1 : (wVer1_3 < wVer2_3 ? 2 : (
					wVer1_4 > wVer2_4 ? 1 : (wVer1_4 < wVer2_4 ? 2 : 0)
				))
			))
		));
}


HRESULT  CUpdateFile::GetFileDate(LPCTSTR szFileName, FILETIME *pft)
{
	// we are interested only in the create time
	// this is the equiv of "modified time" in the 
	// Windows Explorer properties dialog
	FILETIME ct, lat;
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return LastError();
	}
	BOOL bret = ::GetFileTime(hFile, &ct, &lat, pft);
	if (bret == 0)
	{
		::CloseHandle(hFile);
		return LastError();
	}

	::CloseHandle(hFile);
	return S_OK;
}

// This function gets the file version info structure
HRESULT  CUpdateFile::GetFileVersion(LPCTSTR szFileName, VS_FIXEDFILEINFO *pvsf)
{
	DWORD dwHandle;
	DWORD cchver = GetFileVersionInfoSize(szFileName, &dwHandle);
	if (cchver == 0)
	{
		return LastError();
	}
	TCHAR* pver = new TCHAR[cchver];
	BOOL bret = GetFileVersionInfo(szFileName, dwHandle, cchver, pver);
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

	m_strFileVersion.Format(_T("%u.%u.%u.%u"), 
		HIWORD(m_vfixedFileInfo.dwFileVersionMS), 
		LOWORD(m_vfixedFileInfo.dwFileVersionMS), 
		HIWORD(m_vfixedFileInfo.dwFileVersionLS), 
		LOWORD(m_vfixedFileInfo.dwFileVersionLS)
		);

	m_strProductVersion.Format(_T("%u.%u.%u.%u"), 
		HIWORD(m_vfixedFileInfo.dwProductVersionMS), 
		LOWORD(m_vfixedFileInfo.dwProductVersionMS), 
		HIWORD(m_vfixedFileInfo.dwProductVersionLS), 
		LOWORD(m_vfixedFileInfo.dwProductVersionLS)
		);

	return S_OK;
}

HRESULT  CUpdateFile::LastError()
{
	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
	if (SUCCEEDED(hr))
	{
		return E_FAIL;
	}
	return hr;
}
// 
// // This little function emits an error message based on WIN32 error messages
// void  CUpdateFile::EmitErrorMsg(HRESULT hr)
// {
// 	TCHAR szMsg[1024];
// 	FormatMessage( 
// 		FORMAT_MESSAGE_FROM_SYSTEM, 
// 		NULL,
// 		hr,
// 		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 		szMsg,
// 		1024,
// 		NULL 
// 		);
// 
// #ifdef _DEBUG
// 	OutputDebugString(szMsg);
// #endif
// }
//
//bool CUpdateFile::GetVersion(TCHAR* pszFileName)
//{
//	DWORD dwHandle,InfoSize;
//	//首先获得版本信息资源的长度
//	InfoSize = GetFileVersionInfoSize(pszFileName,&dwHandle);
//	//将版本信息资源读入缓冲区
//	if(InfoSize==0) return false;
//	char *InfoBuf = new char[InfoSize];
//	GetFileVersionInfo(pszFileName,0,InfoSize,InfoBuf);
//	//获得生成文件使用的代码页及文件版本
//	unsigned int  cbTranslate = 0;
//	struct LANGANDCODEPAGE {
//		WORD wLanguage;
//		WORD wCodePage;
//	} *lpTranslate;
//	VerQueryValue(InfoBuf, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,&cbTranslate);
//	// Read the file description for each language and code page.
//	for( UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
//	{
//		TCHAR  SubBlock[200];
//		wsprintf( SubBlock, 
//			TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
//			lpTranslate[i].wLanguage,
//			lpTranslate[i].wCodePage);
//		void *lpBuffer=NULL;
//		unsigned int dwBytes=0;
//		VerQueryValue(InfoBuf, 
//			SubBlock, 
//			&lpBuffer, 
//			&dwBytes); 
//		m_strFileVersion = (TCHAR *)lpBuffer;
//		
//		wsprintf( SubBlock, 
//			TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"),
//			lpTranslate[i].wLanguage,
//			lpTranslate[i].wCodePage);
//		lpBuffer=NULL;
//		dwBytes=0;
//		VerQueryValue(InfoBuf, 
//			SubBlock, 
//			&lpBuffer, 
//			&dwBytes); 
//		m_strProductVersion = (TCHAR *)lpBuffer;
//	}
//
//	if (InfoBuf) delete[] InfoBuf; InfoBuf = NULL;
//
//	return true;
//}

INT CUpdateFile::CheckNeedDownload(BOOL &bHasDownloadedFile)
{
	bHasDownloadedFile = FALSE;
	ASSERT(!m_strFilePath.IsEmpty());
	CString strLocalPath = g_objGlobalData.g_strProductDir + _T("\\") + m_strFileName;
	// 用于客户端自己的更新
	TCHAR szAutoUpdatePath[_MAX_PATH];
	GetModuleFileName(NULL, szAutoUpdatePath, _MAX_PATH);
	// 若文件不存在
	if (g_objGlobalData.FileExist(strLocalPath))
	{
		if (m_bUpdateNotExist)
		{
			m_bLocalNeedUpdate = FALSE;
			return m_bLocalNeedUpdate;
		}
		if (m_bForceOverwrite)
		{
			m_bLocalNeedUpdate = strLocalPath.CompareNoCase(szAutoUpdatePath) == 0 ? 
				CheckSelfDownloadedAndIfNeedUpdate(bHasDownloadedFile) : CheckDownloadedAndIfNeedUpdate(bHasDownloadedFile);
			return m_bLocalNeedUpdate;
		}
		CUpdateFile* plocalUpdateFile = new CUpdateFile((LPTSTR)(LPCTSTR)strLocalPath);
		if (plocalUpdateFile)
		{
			if (plocalUpdateFile->InitUpdateFile())
			{
				INT nNewer = CheckVersionNewer(plocalUpdateFile->m_strFileVersion, m_strFileVersion);
				MG_SAFE_DELETE(plocalUpdateFile);

				// 一样新
				if (nNewer == 0)
				{
					TCHAR szFileMD5[_MAX_PATH];
					CHashMgr::GetFileHash(strLocalPath, CALG_MD5, szFileMD5);
					m_bLocalNeedUpdate = 
						(
							m_strMD5.CompareNoCase(szFileMD5) == 0 ? FALSE : 
							(
								strLocalPath.CompareNoCase(szAutoUpdatePath) == 0 ? CheckSelfDownloadedAndIfNeedUpdate(bHasDownloadedFile) : CheckDownloadedAndIfNeedUpdate(bHasDownloadedFile)
							)
						);
					return m_bLocalNeedUpdate;
				}
				else
				{
					// 1:本地较新，不需要更新。
					m_bLocalNeedUpdate = nNewer == 1 ? FALSE : 
						(
						strLocalPath.CompareNoCase(szAutoUpdatePath) == 0 ? CheckSelfDownloadedAndIfNeedUpdate(bHasDownloadedFile) : CheckDownloadedAndIfNeedUpdate(bHasDownloadedFile)
						);
					return m_bLocalNeedUpdate;
				}
			}
			else
			{
				MG_SAFE_DELETE(plocalUpdateFile);

				TCHAR szFileMD5[_MAX_PATH];
				CHashMgr::GetFileHash(strLocalPath, CALG_MD5, szFileMD5);
				m_bLocalNeedUpdate = 
					m_strMD5.CompareNoCase(szFileMD5) == 0 ? FALSE : 
					(
					strLocalPath.CompareNoCase(szAutoUpdatePath) == 0 ? CheckSelfDownloadedAndIfNeedUpdate(bHasDownloadedFile) : CheckDownloadedAndIfNeedUpdate(bHasDownloadedFile)
					);
				return m_bLocalNeedUpdate;
			}
		}
	}
	else
	{
		m_bLocalNeedUpdate = strLocalPath.CompareNoCase(szAutoUpdatePath) == 0 ? 
			CheckSelfDownloadedAndIfNeedUpdate(bHasDownloadedFile) : CheckDownloadedAndIfNeedUpdate(bHasDownloadedFile);
	}

	return m_bLocalNeedUpdate;
}

INT CUpdateFile::CheckNeedUpdate()
{
	ASSERT(!m_strFilePath.IsEmpty());
	CString strLocalPath = g_objGlobalData.g_strProductDir + _T("\\") + m_strFileName;
	// 若文件不存在
	if (g_objGlobalData.FileExist(strLocalPath))
	{
		if (m_bUpdateNotExist)
		{
			m_bLocalNeedUpdate = FALSE;
			return m_bLocalNeedUpdate;
		}
		if (m_bForceOverwrite)
		{
			m_bLocalNeedUpdate = TRUE;
			return m_bLocalNeedUpdate;
		}
		CUpdateFile* plocalUpdateFile = new CUpdateFile((LPTSTR)(LPCTSTR)strLocalPath);
		if (plocalUpdateFile)
		{
			if (plocalUpdateFile->InitUpdateFile())
			{
				INT nNewer = CheckVersionNewer(plocalUpdateFile->m_strFileVersion, m_strFileVersion);
				MG_SAFE_DELETE(plocalUpdateFile);

				// 一样新
				if (nNewer == 0)
				{
					TCHAR szFileMD5[_MAX_PATH];
					CHashMgr::GetFileHash(strLocalPath, CALG_MD5, szFileMD5);
					m_bLocalNeedUpdate = m_strMD5.CompareNoCase(szFileMD5) != 0;
					return m_bLocalNeedUpdate;
				}
				else
				{
					// 1:本地较新，不需要更新。
					m_bLocalNeedUpdate = nNewer == 1 ? FALSE : TRUE;
					return m_bLocalNeedUpdate;
				}
			}
			else
			{
				TCHAR szFileMD5[_MAX_PATH];
				CHashMgr::GetFileHash(strLocalPath, CALG_MD5, szFileMD5);
				m_bLocalNeedUpdate = m_strMD5.CompareNoCase(szFileMD5) != 0;
				return m_bLocalNeedUpdate;
			}
			MG_SAFE_DELETE(plocalUpdateFile);
		}
	}
	else
	{
		m_bLocalNeedUpdate = TRUE;
	}

	return m_bLocalNeedUpdate;
}

INT CUpdateFile::CheckDownloadedAndIfNeedUpdate(BOOL &bHasDownloadFile)
{
	CString strFilePath(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_DOWNLOAD + m_strFileName + UPDATE_FILE_POSTFIX);
	if (!CGlobal::FileExist(strFilePath))
	{
		return TRUE;
	}
	TCHAR szFileMD5[_MAX_PATH];
	CHashMgr::GetFileHash(strFilePath, CALG_MD5, szFileMD5);
	if (m_strMD5.CompareNoCase(szFileMD5) == 0)
	{
		bHasDownloadFile = TRUE;
		return FALSE;
	}
	else
	{
		bHasDownloadFile = FALSE;
		return TRUE;
	}
	return FALSE;
}

INT CUpdateFile::CheckSelfDownloadedAndIfNeedUpdate(BOOL &bHasDownloadFile)
{
	// 在更新客户端目录下查找已经移动过来的文件
	CString strFilePath(g_objGlobalData.g_strProductDir + _T("\\") + m_strFileName + UPDATE_FILE_POSTFIX);
	if (!CGlobal::FileExist(strFilePath))
	{
		// 如果更新客户端目录下不存在，到Download目录下检查
		return CheckDownloadedAndIfNeedUpdate(bHasDownloadFile);
	}
	CUpdateFile* plocalUpdateFile = new CUpdateFile((LPTSTR)(LPCTSTR)strFilePath);
	INT nNewer = CheckVersionNewer(plocalUpdateFile->m_strFileVersion, m_strFileVersion);
	MG_SAFE_DELETE(plocalUpdateFile);

	// 一样新
	if (nNewer == 0)
	{
		TCHAR szFileMD5[_MAX_PATH];
		CHashMgr::GetFileHash(strFilePath, CALG_MD5, szFileMD5);
		if (m_strMD5.CompareNoCase(szFileMD5) == 0)
		{
			bHasDownloadFile = TRUE;
			return FALSE;
		}
		else
		{
			CGlobal::ForceDeleteFile((LPCTSTR)strFilePath);
			bHasDownloadFile = FALSE;
			return TRUE;
		}
	}
	else
	{
		// 1:本地较新，不需要更新。
		return FALSE;
	}
	return FALSE;
}
