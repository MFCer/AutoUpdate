#include "StdAfx.h"
#include "Global.h"

CGlobal::CGlobal(void)
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
	_tmakepath_s(szPath, _MAX_PATH, szDrive, szDir, _T(""), _T(""));
#else
	_tmakepath(szPath, szDrive, szDir, _T(""), _T(""));
#endif

	// 取得更新工作目录
	g_strUpdateDir = szPath;
	// 去除最后一个'\'
	g_strUpdateDir.Delete(g_strUpdateDir.GetLength() - 1, 1);
	// 去除更新目录，获得产品所在目录
	g_strProductDir = g_strUpdateDir.Left(g_strUpdateDir.ReverseFind(_T('\\')));
}

CGlobal::~CGlobal(void)
{
}

BOOL CGlobal::FileExist(CString strFileName)
{
	DWORD dwAttr = GetFileAttributes(strFileName);
	return (dwAttr != INVALID_FILE_ATTRIBUTES) 
		&& !(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CGlobal::ShellExecute(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd)
{
	BOOL bRet = FALSE;
	// vs2005以后Wow64DisableWow64FsRedirection、Wow64RevertWow64FsRedirection函数在库中直接有
	// 故低版本才需要从dll中获取函数地址
	PVOID oldValue;
	// 关闭调用线程的文件系统重定向。参数用于保存以恢复现场。文件系统重定向默认开启。
	// OldValue [out]
	//	The WOW64 file system redirection value. The system uses this parameter to store information necessary to revert (re-enable) file system redirection.
	typedef	BOOL (WINAPI *lpfnWow64DisableWow64FsRedirection) (PVOID *OldValue);
	// 恢复重定向函数
	typedef	BOOL (WINAPI *lpfnWow64RevertWow64FsRedirection) (PVOID OldValue);

	// 定义函数指针
	lpfnWow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = NULL;
	lpfnWow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection = NULL;

	// 加载系统动态链接库"Kernel32.dll"
	HINSTANCE hInstance = LoadLibrary(_T("Kernel32.dll"));
	// 如果加载成功
	if (hInstance)
	{
		// 得到函数地址
		fnWow64DisableWow64FsRedirection = (lpfnWow64DisableWow64FsRedirection)GetProcAddress(hInstance, "Wow64DisableWow64FsRedirection");
		fnWow64RevertWow64FsRedirection = (lpfnWow64RevertWow64FsRedirection)GetProcAddress(hInstance, "Wow64RevertWow64FsRedirection");
		// 如果关闭重定向函数获得成功
		if (fnWow64DisableWow64FsRedirection)
		{
			// 如果关闭重定向失败
			if (FALSE == fnWow64DisableWow64FsRedirection(&oldValue))
			{
				// 注意：此函数在32位系统下肯定会失败
				// Show Error Info.
			}
		}
		// 执行操作
		bRet = (INT)::ShellExecute(NULL, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd) > 32;
		// 如果恢复重定向函数获得成功
		if (fnWow64RevertWow64FsRedirection)
		{
			// 如果恢复重定向失败
			if (FALSE == fnWow64RevertWow64FsRedirection(oldValue))
			{
				// 注意：此函数在32位系统下肯定会失败
				// Show Error Info.
			}
		}
		// 释放系统动态链接库句柄
		FreeLibrary(hInstance);
	}
	return bRet;
}

// 判断目录是否存在    
BOOL CGlobal::FolderExists(CString strFolderPath)
{
	DWORD attr;
	attr = ::GetFileAttributes(strFolderPath);
	return (attr != (DWORD)(INVALID_FILE_ATTRIBUTES)) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

// 创建目录
BOOL CGlobal::CreateFolder(CString strFolderPath)
{
	if (strFolderPath.Mid(1, 1) != _T(":") || strFolderPath.GetLength() < 2)
	{
		return FALSE;
	}
	// 如果存在, 则不需要创建, 直接返回
	if (FolderExists(strFolderPath))
	{
		return TRUE;
	}
	// 如果不存在, 则创建上一级目录
	else
	{
		if (!CreateFolder(strFolderPath.Left(strFolderPath.ReverseFind(_T('\\')))))
		{
			return FALSE;
		}
	}
	return ::CreateDirectory(strFolderPath, 0);
}

//#ifdef _DEBUG
void CGlobal::OutputDebugString(LPCTSTR lpszFmt, ...)
{
	TCHAR msg[1024];
	memset(msg, 0, 1024);
	va_list args;
	va_start(args, lpszFmt);
#if _MSC_VER >= 1400 // vs2005+
	_vstprintf_s(msg, 1024, lpszFmt, args);
#else
	_vstprintf(msg, lpszFmt, args);
#endif
	va_end(args);
	::OutputDebugString(msg);
}

BOOL CGlobal::ForceDeleteFile( CString strPath)
{
	BOOL ret = TRUE;
	unlocker::File* file = unlocker::Path::Exists((LPCTSTR)strPath);
	if (file) {
		ret = file->ForceDelete();
		delete file;
	}
	return ret;
}

//#endif
