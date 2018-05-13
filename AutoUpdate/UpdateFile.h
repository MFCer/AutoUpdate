#pragma once

#pragma comment(lib,"version.lib")

class CUpdateFile
{
public:
	CUpdateFile(LPCTSTR lpszFilePath);
	virtual ~CUpdateFile(void);
	BOOL InitUpdateFile();
// 	//   int newer = WhichIsNewer(argv[1],argv[2]);
// 	//   switch(newer) {
// 	//      case 1:
// 	//      case 2: printf("%s is newer\n",argv[newer]); break;
// 	//      case 3: printf("they are the same version\n"); break;
// 	//      case 0:
// 	//      default: printf("there was an error\n"); break;
// 	//   }
// 	INT WhichIsNewer(TCHAR *fname1, TCHAR *fname2);

	static INT CheckVersionNewer(CString &strVersion1, CString &strVersion2);

private:
//	void EmitErrorMsg (HRESULT hr);
	HRESULT GetFileVersion(LPCTSTR szFileName, VS_FIXEDFILEINFO *vsf);
	HRESULT GetFileDate(LPCTSTR szFileName, FILETIME *pft);
	HRESULT LastError();

//	bool GetVersion(TCHAR* pszFileName);
	VS_FIXEDFILEINFO m_vfixedFileInfo;

public:
	ULONGLONG m_ullSize;
	CString m_strFileName;
	CString m_strFileVersion;
	CString m_strProductVersion;

	INT		m_iLangID;
	bool	m_bRegister;		// 是否注册，如果是组件，要注册
	CString m_strInstallDir;	// WINDOWSDIR SYSTEMDIR CURRENTDIR 安装目录
	bool	m_bZipFile;			// 是否为ZIP文件，如果是，要解压
	bool	m_bForceOverwrite;	// 是否覆盖 如果是新版本，无条件覆盖；如果此值为真，无条件覆盖
	bool	m_bUpdateNotExist;	// 不存在才更新、存在不更新
	bool	m_bExecute;			// 是否执行
	CString m_strExecParam;		// 如果执行，执行参数
	CString m_strLocalPath;		// 安装的相对路径
//	FILETIME m_fileTime;		// 文件日期
	CString m_strFilePath;		// 本地文件绝对路径
	CString m_strURL;			// 供下载URL地址，HTTP，文件名可以与 m_strFileName 不同
	CString m_strMD5;			// 文件MD5值，用于比对和校验
// 	CString m_strUserName;		// 如果提供登录，登录用户名
// 	CString m_strPassword;		// 如果提供登录，登录密码

	// 检查当前文件是否需要更新
	INT CheckNeedUpdate();
	// 检查当前文件是否需要下载
	INT CheckNeedDownload(BOOL &bHasDownloadedFile);
	// 查看是否已经下载，返回是否需要更新
	INT CheckDownloadedAndIfNeedUpdate(BOOL &bHasDownloadFile);
	// 查看更新客户端是否已经下载，返回是否需要更新
	INT CheckSelfDownloadedAndIfNeedUpdate(BOOL &bHasDownloadFile);
	BOOL m_bLocalNeedUpdate;
};

class CFileToExecute
{
public:
	CFileToExecute(CUpdateFile *pUpdateFile)
	{
		m_strPath = g_objGlobalData.g_strProductDir + _T("\\") + pUpdateFile->m_strFileName;

		m_bRegister = pUpdateFile->m_bRegister;
		m_bZipFile = pUpdateFile->m_bZipFile;
		m_bExecute = pUpdateFile->m_bExecute;
		m_strExecParam = pUpdateFile->m_strExecParam;
	}
	virtual ~CFileToExecute(void) {}

	CString m_strPath;
	bool	m_bRegister;		// 是否注册，如果是组件，要注册
	bool	m_bZipFile;			// 是否为ZIP文件，如果是，要解压
	bool	m_bExecute;			// 是否执行
	CString m_strExecParam;		// 如果执行，执行参数
};

class CFileToUpdateInfo
{
public:
	CFileToUpdateInfo(CUpdateFile *pUpdateFile)
	{
		m_strSrcPath = pUpdateFile->m_strFilePath + UPDATE_FILE_POSTFIX;
		m_strDetPath = g_objGlobalData.g_strProductDir + _T("\\") + pUpdateFile->m_strFileName;
	}
	virtual ~CFileToUpdateInfo(void) {}

	// 在更新目录的下载目录下
	CString m_strSrcPath;
	// 在产品目录下
	CString m_strDetPath;
};