#pragma once

#include <afxinet.h>
#include "Thread.h"
#include "updatefile.h"

#define WM_DOWNLOADFILE			(WM_USER + 100)
#define WM_DOWNLOADFILEEND		(WM_USER + 101)
#define WM_TRAYICONNOTIFY		(WM_USER + 102)

typedef enum DOWNLOAD_FILE_TYPE
{
	FILELIST_XML = 0, 
	UPDATEHINT_XML, 
	GENERAL_FILE
};

// 每个实例仅开启一个 GET 线程
class CInetFile : public CThread
{
public:
	class CInetFileSession : public CInternetSession
	{
	public:
		CInetFileSession(LPCTSTR pszAppName, INT nMethod): CInternetSession(pszAppName, 1, nMethod){};

		virtual void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus,
			LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen) 
		{
			if (dwInternetStatus == INTERNET_STATUS_CONNECTED_TO_SERVER)
				return;
		};
	};

	class CInetFileException : public CException
	{
	public:
		CInetFileException(INT nCode = 0) : m_nErrorCode(nCode){};
		~CInetFileException() {}

		INT m_nErrorCode;
	};

	CInetFile(void);
	virtual ~CInetFile(void);
	CString GetLastError(void);
	bool GetInetFile();

private:
	CString m_strLastErr;
	void ThrowInetFileException(INT nCode);
	virtual void ThreadProcMain(void);

public:
	bool m_bProgressMode;
	DOWNLOAD_FILE_TYPE m_dlFileType;
	CString m_strPath;
	CString m_strFileName;
	CString m_strURL;
	HWND m_hWnd;
	bool m_bWantStop;
	INT m_nDownloadTime;	// 下载次数
	UINT m_nAlreadyDown;
	DWORD m_dwTotalLen;

	INT		m_iLangID;
	bool	m_bRegister;		// 是否注册，如果是组件，要注册
	bool	m_bZipFile;			// 是否为ZIP文件，如果是，要解压 (目前保留)
	bool	m_bForceOverwrite;	// 是否覆盖 如果是新版本，无条件覆盖；如果此值为真，无条件覆盖 (目前保留)
	bool	m_bExecute;			// 是否执行
	bool	m_bUpdateNotExist;	// 不存在时才更新
	CString m_strExecParam;		// 如果执行，执行参数
	CString m_strMD5;			// 文件MD5，用于校验
};
