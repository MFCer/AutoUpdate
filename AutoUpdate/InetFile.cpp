#include "StdAfx.h"
#include "InetFile.h"

CInetFile::CInetFile(void)
	: m_strLastErr(_T(""))
	, m_bProgressMode(false)
	, m_strURL(_T(""))
	, m_strPath(_T(""))
	, m_strFileName(_T(""))
	, m_hWnd(NULL)
	, m_bWantStop(false)
	, m_dlFileType(GENERAL_FILE)
	, m_nDownloadTime(0)
	, m_nAlreadyDown(0)
	, m_dwTotalLen(0)
{
	m_iLangID = ALL_SUPPORT;	// 支持所有语言
	m_bRegister = false;		// 是否注册，如果是组件，要注册
	m_bZipFile = false;			// 是否为ZIP文件，如果是，要解压
	m_bForceOverwrite = false;	// 是否覆盖 如果是新版本，无条件覆盖；如果此值为真，无条件覆盖
	m_bExecute = false;			// 是否执行
	m_bUpdateNotExist = false;	// 不存在才更新
	m_strExecParam = _T("");	// 如果执行，执行参数
}

CInetFile::~CInetFile(void)
{
	m_bWantStop = true;
	WaitForStop();
}

bool CInetFile::GetInetFile()
{
	m_nDownloadTime ++;
	WaitForStop();
	m_bWantStop = false;
	return StartThread();
}

CString CInetFile::GetLastError(void)
{
	return m_strLastErr;
}

void CInetFile::ThrowInetFileException(INT nCode)
{
	switch(nCode)
	{
		case 1:
			m_strLastErr = _T("URL解析错误");
			break;
		case 2:
			m_strLastErr = _T("服务器拒绝连接");
			break;
		case 3:
			m_strLastErr = _T("重定向失败");
			break;
		case 4:
			m_strLastErr = _T("重定向URL解析错误");
			break;
		case 5:
			m_strLastErr = _T("不是HTTP连接");
			break;
		case 6:
			m_strLastErr = _T("取得服务器文件失败");
			break;
		case 7:
			m_strLastErr = _T("创建本地文件失败");
			break;			
		case 8:
			m_strLastErr = _T("主动取消连接");
			break;	
		case 9:
			m_strLastErr = _T("URL不正确");
			break;
		default:
			m_strLastErr = _T("连接服务器失败,");
			break;
	}
	
	CInetFileException* pEx = new CInetFileException(nCode);
	throw pEx;
}

void CInetFile::ThreadProcMain(void)
{
	//ASSERT(!m_strURL.IsEmpty());
	ASSERT(!m_strPath.IsEmpty());

	int nRetCode = 0;

	CInetFileSession session(_T("MAGON AUTO UPDATE ClENT"), PRE_CONFIG_INTERNET_ACCESS);
	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	try
	{
		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;
		const TCHAR szHeaders[] = _T("Accept: */*\r\nUser-Agent: MGAutoUpdate\r\n");
		DWORD dwHttpRequestFlags = INTERNET_FLAG_RELOAD;

		if (m_strURL.IsEmpty())
		{
			ThrowInetFileException(9);
		}

		if (!AfxParseURL(m_strURL, dwServiceType, strServerName, strObject, nPort) ||
			dwServiceType != INTERNET_SERVICE_HTTP)
		{
			ThrowInetFileException(1);
		}

		if (m_bProgressMode)
		{
			VERIFY(session.EnableStatusCallback(true));
		}

		pServer = session.GetHttpConnection(strServerName, nPort);

		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
			strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
		pFile->AddRequestHeaders(szHeaders);
		pFile->SendRequest();

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);

		if (dwRet != HTTP_STATUS_OK)
		{
			ThrowInetFileException(6);
		}

		CString strNewLocation;
		pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);

		CString strTotalLen;
		pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH , strTotalLen);
		USES_CONVERSION;
		DWORD dwTotalLen = (DWORD)atoi(T2A(strTotalLen));

		CFile fLocal;
		CString strUpdateFileName = m_strPath;
		if (GENERAL_FILE == m_dlFileType)
		{
			strUpdateFileName += UPDATE_FILE_POSTFIX;
		}

		// 若目标文件夹不存在，先创建目录
		CString strParentDir = strUpdateFileName.Left(strUpdateFileName.ReverseFind(_T('\\')));
		if (!CGlobal::FolderExists(strParentDir))
		{
			CGlobal::ForceDeleteFile(strParentDir);
			CGlobal::CreateFolder(strParentDir);
		}

		if (!fLocal.Open(strUpdateFileName, CFile::modeCreate | CFile::modeWrite))
		{
			ThrowInetFileException(7);
		}

		BYTE szBuf[1024];
		UINT nLen = 0;
		m_nAlreadyDown = 0;

		while (nLen = pFile->Read(szBuf, sizeof(szBuf)))
		{
			fLocal.Write(szBuf, nLen);
			m_nAlreadyDown += nLen;
			if (m_hWnd)
			{
				PostMessage(m_hWnd, WM_DOWNLOADFILE, MAKEWPARAM(nLen, m_dlFileType), (LPARAM)this);
			}

			if (m_bWantStop)
			{
				nRetCode = 8;
				break;
			}
		}

		fLocal.Close();
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		nRetCode = 2;
		pEx->Delete();
	}
	catch (CInetFileException* pEx)
	{
		nRetCode = pEx->m_nErrorCode;
		TRACE("Error: Exiting with CInetFileException(%d)\n", nRetCode);
		pEx->Delete();
	}

	if (pFile != NULL)
	{
		pFile->Close();
		delete pFile;
	}

	if (pServer != NULL)
	{
		pServer->Close();
		delete pServer;
	}

	session.Close();

	if (m_hWnd)
	{
		PostMessage(m_hWnd, WM_DOWNLOADFILEEND, MAKEWPARAM(nRetCode, m_dlFileType), (LPARAM)this);
	}
}

