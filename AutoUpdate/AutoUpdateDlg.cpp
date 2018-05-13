
// AutoUpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"

#include "..\Common\IniFileOperations.h"
#include "..\Common\HashMgr.h"

#include "UpdateHintFileReader.h"

#include <locale>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoUpdateDlg 对话框




CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpdateDlg::IDD, pParent)
	, m_dwCount(0)
	, m_dwTotalLen(0)
	, m_nMaxDownThread(5)
	, m_nCurThreadCount(0)
	, m_bShowTrayInfo(FALSE)
	, m_bSelfNeedUpdate(FALSE)
	, m_dwAlreadyDownLen(0)
	, m_bExecuteFiles(FALSE)
	, m_bHideWindow(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hStartDownloadFileThread = INVALID_HANDLE_VALUE;
}

void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAYICONNOTIFY, &CAutoUpdateDlg::OnTrayIconNotify)
	ON_MESSAGE(WM_DOWNLOADFILE, OnDownLoadFile)
	ON_MESSAGE(WM_DOWNLOADFILEEND, OnDownLoadFileEnd)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CAutoUpdateDlg::OnBnClickedOk)
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BTN_YES, &CAutoUpdateDlg::OnBnClickedBtnYes)
	ON_BN_CLICKED(IDC_BTN_NO, &CAutoUpdateDlg::OnBnClickedBtnNo)
END_MESSAGE_MAP()


// CAutoUpdateDlg 消息处理程序

BOOL CAutoUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CTime objCurrTime = CTime::GetCurrentTime();
	CTime objLastUpdateTime;
	if (g_objIniFileOperations.GetIniStruct(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_UPDATE_TIME, &objLastUpdateTime, sizeof(objLastUpdateTime)))
	{
		CTimeSpan objTimeSpan = objCurrTime - objLastUpdateTime;
		if (objTimeSpan.GetDays() > PROMPT_NETWORK_ERROR_DAYS)
		{
			CString strText;
			strText.Format(g_objLocalizeMgr.GetString(LID_PROMPT_NETWORK_ERROR_MESSAGEBOX_TEXT, LID_PNEMT_DEFAULT), PROMPT_NETWORK_ERROR_DAYS);
			MessageBox(strText, 
				g_objLocalizeMgr.GetString(LID_PROMPT_NETWORK_ERROR_MESSAGEBOX_CAPTION, LID_PNEMC_DEFAULT), 
				MB_OK | MB_ICONWARNING);
			g_objIniFileOperations.WriteIniStruct(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_UPDATE_TIME, &objCurrTime, sizeof(objCurrTime));
		}
		if (objTimeSpan.GetDays() < 0)
		{
			g_objIniFileOperations.WriteIniStruct(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_UPDATE_TIME, &objCurrTime, sizeof(objCurrTime));
		}
	}
	// 首次打开，或者被用户删除
	else
	{
		g_objIniFileOperations.WriteIniStruct(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_UPDATE_TIME, &objCurrTime, sizeof(objCurrTime));
	}
 	// ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_CAPTION, LID_UQDC_DEFAULT));
	GetDlgItem(IDC_STATIC_LATEST_FEATURES)->SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_LATEST_FEATURE, LID_UQDLF_DEFAULT));
	GetDlgItem(IDC_STATIC_ENSURE)->SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_ENSURE, LID_UQDE_DEFAULT));
	GetDlgItem(IDC_CHECK_NO_PROMPT)->SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_IGNORE_CHECK, LID_UQDIC_DEFAULT));
	GetDlgItem(IDC_BTN_YES)->SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_YES_BTN_TEXT, LID_UQDYBT_DEFAULT));
	GetDlgItem(IDC_BTN_NO)->SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_NO_BTN_TEXT, LID_UQDNBT_DEFAULT));

// 	// FALSE == ReadyToUpdate
// 	if (FALSE == g_objIniFileOperations.GetIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_READY_TO_UPDATE, IFO_SAU_RTU_DFT_VALUE))
// 	{
// 		// 检查更新并下载文件
// 		StartCheckUpdate();
// 	}
// 	// TRUE == ReadyToUpdate
// 	else
// 	{
		// E_US_UPDATING == UpdateStatus
		if (E_US_UPDATING == g_objIniFileOperations.GetIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_UPDATE_STATUS, IFO_SAU_US_DFT_VALUE))
		{
			CGlobal::OutputDebugString(_T("Version back."));
			// 版本回退
			RestoreFiles();
		}
		// 校验文件，即需要下载的文件数为0
		if (!ValidateFiles())
		{
			// 如果校验失败，开始检查更新
			StartCheckUpdate();
		}
		else
		{
			// 执行更新替换操作
			m_RemoteFiles.ClearAllList();
			// 获得需要更新的文件列表
			if (m_RemoteFiles.ReadFromXMLFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML))
			{
				m_RemoteFiles.GetExecuteFileList(m_lstFileToExecute);
				m_dwCount = m_RemoteFiles.CheckNeedUpdate();
				if (0 == m_dwCount)
				{
					// 不可能为0，所以，只可能出现了错误
					// 例如，上次运行时更新的是一个英文版的帮助文件
					// 而在下次更新替换前用户又切回了简体中文
					// 导致不需要下载文件，也没有要更新的文件了，此种情形下，清空即可。

					// 清空下载文件夹
					CGlobal::ForceDeleteFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_DOWNLOAD);
					CGlobal::ForceDeleteFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML);

					// 重置更新状态标识
					g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_UPDATE_STATUS, E_US_NOTSTART);
					// 开始检查更新
					StartCheckUpdate();
				}
				else
				{
					SetTimer(TIMERID_SHOW_CONFIRM_UPDATE_DLG, TIMER_SHOW_CU_DLG_ELAPSE, NULL);
				}
			}
			else
			{
				CGlobal::OutputDebugString(_T("Error occurred when m_RemoteFiles.ReadFromXMLFile."));
				PostQuitMessage(EXIT_CODE_ERROR_OCCURRED);
			}
		}
//	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutoUpdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutoUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMERID_TRAY_HIDE:
		{
			POINT point;
			GetCursorPos(&point);
			if (!m_rtTrayShowPt.IsRectEmpty() && !m_rtTrayShowPt.PtInRect(point))
			{
#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szInfo, 255, _T(""));
#else
				_tcscpy(m_tNID.szInfo, _T(""));
#endif
				m_tNID.uFlags = NIF_INFO;
				Shell_NotifyIcon(NIM_MODIFY, &m_tNID);
				m_rtTrayShowPt.SetRectEmpty();
				KillTimer(TIMERID_TRAY_HIDE);
			}
		}
		break;
	case TIMERID_SHOW_UPDATE_HINT_DLG:
		{
			WINDOWPLACEMENT wp;
			GetWindowPlacement(&wp);
 			if (!g_objGlobalData.m_objFullScreenChecker.IsFullScreen(wp.rcNormalPosition))
			{
				KillTimer(TIMERID_SHOW_UPDATE_HINT_DLG);
				m_bHideWindow = FALSE;
				SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				ShowWindow(SW_SHOW);
 			}
		}
		break;
	case TIMERID_SHOW_DOWNLOAD_COMPLETE:
		{
			WINDOWPLACEMENT wp;
			GetWindowPlacement(&wp);
			if (!g_objGlobalData.m_objFullScreenChecker.IsFullScreen(wp.rcNormalPosition))
			{
				KillTimer(TIMERID_SHOW_DOWNLOAD_COMPLETE);

#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szInfoTitle, 63, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_TITLE, LID_TDCBT_DEFAULT));
#else
				_tcscpy(m_tNID.szInfoTitle, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_TITLE, LID_TDCBT_DEFAULT));
#endif

#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szInfo, 255, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_INFO, LID_TDCBI_DEFAULT));
#else
				_tcscpy(m_tNID.szInfo, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_INFO, LID_TDCBI_DEFAULT));
#endif
				m_tNID.uFlags = NIF_INFO;
				Shell_NotifyIcon(NIM_MODIFY, &m_tNID);

				SetTimer(TIMERID_TRAY_BALLOON_TIMEOUT, TIMER_TRAY_BALLOON_TIMEOUT, NULL);
			}
		}
		break;
	case TIMERID_TRAY_BALLOON_TIMEOUT:
		{
			KillTimer(TIMERID_TRAY_BALLOON_TIMEOUT);
#if  _MSC_VER >= 1400 // vs2005+
			_tcscpy_s(m_tNID.szInfo, 255, _T(""));
#else
			_tcscpy(m_tNID.szInfo, _T(""));
#endif
			m_tNID.uFlags = NIF_INFO;
			Shell_NotifyIcon(NIM_MODIFY, &m_tNID);
			PostQuitMessage(EXIT_CODE_ERROR_SUCCESS);
		}
		break;
	case TIMERID_SHOW_CONFIRM_UPDATE_DLG:
		{
			WINDOWPLACEMENT wp;
			GetWindowPlacement(&wp);
			if (!g_objGlobalData.m_objFullScreenChecker.IsFullScreen(wp.rcNormalPosition))
			{
				KillTimer(TIMERID_SHOW_CONFIRM_UPDATE_DLG);
				if (MessageBox(g_objLocalizeMgr.GetString(LID_CONFIRM_UPDATE_MESSAEBOX_TEXT, LID_CUMT_DEFAULT), 
					g_objLocalizeMgr.GetString(LID_CONFIRM_UPDATE_MESSAEBOX_CAPTION, LID_CUMC_DEFAULT), MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					// 清空备份文件夹
					CGlobal::ForceDeleteFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_BACKUP);

					// 先备份文件
					BackupFiles();

					// 设置更新状态标志
					g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_UPDATE_STATUS, E_US_UPDATING);

					CUpdateFile *pUpdateFile = m_RemoteFiles.PopUpdateFile();
					list<CFileToUpdateInfo> lstFileToUpdateInfo;
					list<CString> lstFileToDeleteInfo;
					while (pUpdateFile)
					{
						CString strLocalFilePath = g_objGlobalData.g_strProductDir + _T("\\") + pUpdateFile->m_strFileName;
						// 用于客户端自己的更新
						TCHAR szAutoUpdatePath[_MAX_PATH];
						GetModuleFileName(NULL, szAutoUpdatePath, _MAX_PATH);
						// 如果存在
						if (CGlobal::FileExist(strLocalFilePath))
						{
							if (strLocalFilePath.CompareNoCase(szAutoUpdatePath) == 0)
							{
								CopyFile(pUpdateFile->m_strFilePath + UPDATE_FILE_POSTFIX, strLocalFilePath + UPDATE_FILE_POSTFIX, FALSE);
								m_bSelfNeedUpdate = TRUE;
							}
							else
							{
								// 删除文件
								if (!CGlobal::ForceDeleteFile(strLocalFilePath))
								{
									// 删除不了的暂时放置到临时队列
									lstFileToDeleteInfo.push_back(strLocalFilePath);
								}
								// 存放更新文件路径和目的路径到列表
								lstFileToUpdateInfo.push_back(CFileToUpdateInfo(pUpdateFile));
							}
						}
						else
						{
							// 存放更新文件路径和目的路径到列表
							lstFileToUpdateInfo.push_back(CFileToUpdateInfo(pUpdateFile));
						}

						MG_SAFE_DELETE(pUpdateFile);
						pUpdateFile = m_RemoteFiles.PopUpdateFile();
					}

					for (list<CString>::iterator iter=lstFileToDeleteInfo.begin(); 
						iter!=lstFileToDeleteInfo.end(); 
						)
					{
						// 删除文件
						if (!CGlobal::ForceDeleteFile(*iter))
						{
							lstFileToDeleteInfo.push_back(*iter);
							// 从列表清空，后移
							iter = lstFileToDeleteInfo.erase(iter);
							iter++;
						}
						else
						{
							// 删除了的，从列表清空
							iter = lstFileToDeleteInfo.erase(iter);
						}
					}

					// 全部删除后，将文件复制到指定位置
					for (list<CFileToUpdateInfo>::iterator iter=lstFileToUpdateInfo.begin(); 
						iter!=lstFileToUpdateInfo.end(); 
						iter++)
					{
						CString strParentDir = iter->m_strDetPath.Left(iter->m_strDetPath.ReverseFind(_T('\\')));
						if (!CGlobal::FolderExists(strParentDir))
						{
							CGlobal::ForceDeleteFile(strParentDir);
							CGlobal::CreateFolder(strParentDir);
						}
						if (!CopyFile(iter->m_strSrcPath, iter->m_strDetPath, FALSE))
						{
							CGlobal::OutputDebugString(_T("Error while CopyFile: %s -> %s, errcode %d."), iter->m_strSrcPath, iter->m_strDetPath, GetLastError());
							// 如果失败，回退版本
							RestoreFiles();
							g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_UPDATE_STATUS, E_US_NOTSTART);
							PostQuitMessage(EXIT_CODE_ERROR_OCCURRED);
						}
					}

					// 清空下载文件夹
					CGlobal::ForceDeleteFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_DOWNLOAD);

					// 删除列表文件
					CGlobal::ForceDeleteFile(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML);

					// 重置更新状态标识
					g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_UPDATE_STATUS, E_US_NOTSTART);

					// 设置更新时间
					CTime objCurrTime = CTime::GetCurrentTime();
					g_objIniFileOperations.WriteIniStruct(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_UPDATE_TIME, &objCurrTime, sizeof(objCurrTime));

					// 提示用户更新成功
					SetTimer(TIMERID_SHOW_UPDATE_COMPLETE, TIMERID_SHOW_UPDATE_COMPLETE, NULL);

					m_bExecuteFiles = TRUE;
				}
				else
				{
					PostQuitMessage(EXIT_CODE_ERROR_SUCCESS);
				}
			}
		}
		break;
	case TIMERID_SHOW_UPDATE_COMPLETE:
		{
			WINDOWPLACEMENT wp;
			GetWindowPlacement(&wp);
			if (!g_objGlobalData.m_objFullScreenChecker.IsFullScreen(wp.rcNormalPosition))
			{
				KillTimer(TIMERID_SHOW_UPDATE_COMPLETE);

				m_tNID.uFlags = NIF_INFO;
				m_tNID.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
				m_tNID.hWnd = GetSafeHwnd();
				m_tNID.uID = IDR_MAINFRAME;
				m_tNID.uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
				m_tNID.hIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szTip, 127, g_objLocalizeMgr.GetString(LID_TRAY_TIP, LID_TT_DEFAULT));
#else
				_tcscpy(m_tNID.szTip, g_objLocalizeMgr.GetString(LID_TRAY_TIP, LID_TT_DEFAULT));
#endif

#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szInfoTitle, 63, g_objLocalizeMgr.GetString(LID_TRAY_UPDATE_COMPLETE_BALLOON_TITLE, LID_TUCBT_DEFAULT));
#else
				_tcscpy(m_tNID.szInfoTitle, g_objLocalizeMgr.GetString(LID_TRAY_UPDATE_COMPLETE_BALLOON_TITLE, LID_TUCBT_DEFAULT));
#endif

#if  _MSC_VER >= 1400 // vs2005+
				_tcscpy_s(m_tNID.szInfo, 255, g_objLocalizeMgr.GetString(LID_TRAY_UPDATE_COMPLETE_BALLOON_INFO, LID_TUCBI_DEFAULT));
#else
				_tcscpy(m_tNID.szInfo, g_objLocalizeMgr.GetString(LID_TRAY_UPDATE_COMPLETE_BALLOON_INFO, LID_TUCBI_DEFAULT));
#endif

				m_tNID.dwInfoFlags = NIIF_INFO;
				Shell_NotifyIcon(NIM_ADD, &m_tNID);

				SetTimer(TIMERID_TRAY_BALLOON_TIMEOUT, TIMER_TRAY_BALLOON_TIMEOUT, NULL);
			}
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

LRESULT CAutoUpdateDlg::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDR_MAINFRAME)
	{
		return 1;
	}
	static BOOL s_bIsShown = FALSE;
	switch (lParam)
	{
	case WM_MOUSEMOVE:
		if (!s_bIsShown && m_bShowTrayInfo)
		{
#if  _MSC_VER >= 1400 // vs2005+
			_tcscpy_s(m_tNID.szInfo, 255, m_strTrayInfo);
#else
			_tcscpy(m_tNID.szInfo, m_strTrayInfo);
#endif
			m_tNID.uFlags |= NIF_INFO;
			Shell_NotifyIcon(NIM_MODIFY, &m_tNID);

			POINT point;
			GetCursorPos(&point);

			m_rtTrayShowPt.SetRect(point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			SetTimer(TIMERID_TRAY_HIDE, TIMER_TRAY_HIDE_ELAPSE, NULL);
		}
		break;
	case WM_MOUSELEAVE:
		break;
	case NIN_BALLOONTIMEOUT:
		s_bIsShown = FALSE;
		if (!m_bShowTrayInfo)
		{
			PostQuitMessage(EXIT_CODE_ERROR_SUCCESS);
		}
		break;
	case NIN_BALLOONSHOW:
		s_bIsShown = TRUE;
		break;
	case NIN_BALLOONHIDE:
		s_bIsShown = FALSE;
		break;
	case NIN_BALLOONUSERCLICK:
		if (!m_bShowTrayInfo)
		{
			PostQuitMessage(EXIT_CODE_ERROR_SUCCESS);
		}
		break;
	default:
		break;
	}
	return 0;
}

void CAutoUpdateDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Shell_NotifyIcon(NIM_DELETE, &m_tNID);

	m_RemoteFiles.ClearAllList();
	m_InetFiles.Clear();

	if (m_hStartDownloadFileThread != INVALID_HANDLE_VALUE)
	{
		if (WaitForSingleObject(m_hStartDownloadFileThread, 100) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hStartDownloadFileThread, 0);
		}
		CloseHandle(m_hStartDownloadFileThread);
		m_hStartDownloadFileThread = INVALID_HANDLE_VALUE;
	}

	if (m_bSelfNeedUpdate)
	{
		CString strTempFile;
		{
			TCHAR szTempPath[_MAX_PATH];
			::GetTempPath(_MAX_PATH, szTempPath);
			strTempFile = szTempPath;
		}
		strTempFile += _T("\\MGAutoUpdate.bat");
		CGlobal::ForceDeleteFile(strTempFile);
		CString strSelfPath;
		{
			TCHAR szSelfPath[_MAX_PATH];
			GetModuleFileName(NULL, szSelfPath, _MAX_PATH);
			strSelfPath = szSelfPath;
		}
		if (g_objGlobalData.FileExist(strSelfPath + UPDATE_FILE_POSTFIX))
		{
			CHAR *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
			// 设定<ctpye.h>中字符处理方式
			setlocale(LC_CTYPE, "chs");

			FILE *fp;
#if  _MSC_VER >= 1400 // vs2005+
			_tfopen_s(&fp, strTempFile, _T("w"));
#else
			fp = _tfopen(strTempFile, _T("w"));
#endif
			_ftprintf(fp, _T(":Repeat\n"));
			_ftprintf(fp, _T("del /f /s /q \"%s\"\n"), strSelfPath);
			_ftprintf(fp, _T("if exist \"%s\" goto Repeat\n"), strSelfPath);
			_ftprintf(fp, _T("ren \"%s.update\" %s\n"), strSelfPath, strSelfPath.Mid(strSelfPath.ReverseFind('\\') + 1));
			_ftprintf(fp, _T("del \"%s\"\n"), strTempFile);

			if (m_bExecuteFiles && !m_bSelfNeedUpdate)
			{
				// 执行需要执行的文件
				for (list<CFileToExecute>::iterator iter=m_lstFileToExecute.begin(); 
					iter!=m_lstFileToExecute.end(); 
					iter++)
				{
					if (iter->m_bExecute)
					{
						_ftprintf(fp, _T("\"%s\" %s\n"), iter->m_strPath, iter->m_strExecParam);
					}
				}
			}

			fclose(fp);

			// 还原区域设定
			setlocale(LC_CTYPE, old_locale);
			free(old_locale);
		}
		CGlobal::ShellExecute(_T("open"), _T("\"") + strTempFile + _T("\""), NULL, NULL, SW_HIDE);
	}

	if (m_bExecuteFiles && !m_bSelfNeedUpdate)
	{
		// 执行需要执行的文件
		for (list<CFileToExecute>::iterator iter=m_lstFileToExecute.begin(); 
			iter!=m_lstFileToExecute.end(); 
			iter++)
		{
			if (iter->m_bExecute)
			{
				CGlobal::ShellExecute(_T("open"), iter->m_strPath, iter->m_strExecParam, NULL, SW_SHOW);
			}
		}
	}
}

LRESULT CAutoUpdateDlg::OnDownLoadFile(WPARAM wParam, LPARAM lParam) 
{ 
	CAutoLock  lock(&m_lock);

	static DWORD s_dwLastTime;

	INT nLen = LOWORD(wParam);
	DOWNLOAD_FILE_TYPE llFileType = (DOWNLOAD_FILE_TYPE)HIWORD(wParam);
	CInetFile *pInetFile = (CInetFile *)lParam;
	ASSERT(pInetFile);

	if (pInetFile)
	{
		if (GENERAL_FILE == llFileType)
		{
			m_dwAlreadyDownLen += nLen;
			if (m_dwTotalLen > 0 && GetTickCount() - s_dwLastTime > INTERVAL_TIME_TRAY_INFO)
			{
				if (m_bShowTrayInfo && !m_rtTrayShowPt.IsRectEmpty())
				{
					m_strTrayInfo.Format(g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOADING_BALLOON_INFO, LID_TDBI_DEFAULT), 
						pInetFile->m_strFileName, 
						pInetFile->m_nAlreadyDown * 100. / pInetFile->m_dwTotalLen, 
						m_dwAlreadyDownLen * 100. / m_dwTotalLen);
#if  _MSC_VER >= 1400 // vs2005+
					_tcscpy_s(m_tNID.szInfo, 255, m_strTrayInfo);
#else
					_tcscpy(m_tNID.szInfo, m_strTrayInfo);
#endif
					m_tNID.uFlags = NIF_INFO;
					Shell_NotifyIcon(NIM_MODIFY, &m_tNID);
					s_dwLastTime = GetTickCount();
				}
			}
		}
	}

	return 0;
}

LRESULT CAutoUpdateDlg::OnDownLoadFileEnd(WPARAM wParam, LPARAM lParam)
{
	// wParam 0 成功, 2, SESSION 异常 3, 主动停止
	CAutoLock  lock(&m_lock);

	INT nRetCode = LOWORD(wParam);
	DOWNLOAD_FILE_TYPE llFileType = (DOWNLOAD_FILE_TYPE)HIWORD(wParam);
	CInetFile *pInetFile = (CInetFile *)lParam;
	ASSERT(pInetFile);

	BOOL bSucceed = TRUE;

	switch (llFileType)
	{
	case FILELIST_XML:		
		{
			if (0 == nRetCode)
			{
				// 读取用户上次是否忽略了
				BOOL bUserIgnored = g_objIniFileOperations.GetIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_USER_IGNORED, IFO_SAU_UI_DFT_VALUE);
				if (bUserIgnored)
				{
					// 得到此次的版本，（FileList.Xml文件的HASH）
					TCHAR szProductVersion[_MAX_PATH];
					CHashMgr::GetFileHash(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML, CALG_MD5, szProductVersion);

					// 读取上次忽略的版本
					CString strLastIgnoredVersion = g_objIniFileOperations.GetIniString(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_PRODUCT_VERSION, IFO_SAU_LPV_DFT_VALUE);

					// 如果是忽略的那个版本
					if (strLastIgnoredVersion.CompareNoCase(szProductVersion) == 0)
					{
						CGlobal::OutputDebugString(_T("Ignored version skipped."));
						PostQuitMessage(EXIT_CODE_USER_CANCEL);
					}
					else
					{
						// 取消忽略
						g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_USER_IGNORED, FALSE);
						g_objIniFileOperations.WriteIniString(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_PRODUCT_VERSION, IFO_SAU_LPV_DFT_VALUE);
					}
				}
				else
				{
					CGlobal::OutputDebugString(_T("FileList download success: %s."), pInetFile->m_strPath);
					if (m_RemoteFiles.ReadFromXMLFile((LPTSTR)(LPCTSTR)pInetFile->m_strPath))
					{
						BOOL bHasDownloadedFile;
						m_dwCount = m_RemoteFiles.CheckNeedDownload(bHasDownloadedFile);
						m_dwTotalLen = m_RemoteFiles.GetNeedDownloadTotalLength();
						if (0 == m_dwCount)
						{
							CGlobal::OutputDebugString(_T("No update available."));
							// MessageBox(_T("版本为最新！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
							PostQuitMessage(EXIT_CODE_NO_UPDATE_AVAILABLE);
						}
						else
						{
							// 在托盘区添加图标
							Shell_NotifyIcon(NIM_ADD, &m_tNID);
							CGlobal::OutputDebugString(_T("%d file(s) need to be updated, total len: %d KB."), m_dwCount, m_dwTotalLen / 1024);
						}
						// 如果已经有下载过的文件，继续下载
						if (bHasDownloadedFile)
						{
							// 可以显示托盘气泡（下载信息）
							m_bShowTrayInfo = TRUE;
							PostMessage(WM_USER_ADD_FILES_TO_LIST, 0, 0);
						}
						// 否则，显示更新说明，让用户选择是否下载更新
						else
						{
							// 暂停下载线程
							SuspendThread(m_hStartDownloadFileThread);
							PostMessage(WM_USER_SHOW_UPDATE_HINT, 0, 0);
						}
					}
				}
			}
			else
			{
				CGlobal::OutputDebugString(_T("Error occurred while downloading file: %s, errcode %d."), pInetFile->m_strPath, pInetFile->GetLastError());
				PostQuitMessage(EXIT_CODE_ERROR_OCCURRED);
			}
		}
		break;

	case UPDATEHINT_XML:
		{
			CUpdateHintFileReader objUpdateHintFileReader(pInetFile->m_strPath);
			// 下载成功且有新特性说明
			if (0 == nRetCode && !objUpdateHintFileReader.m_strLatestFeatures.IsEmpty())
			{
				// 设置到对话框
				((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_FEATURES))->SetSel(-1, -1);
				((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_FEATURES))->ReplaceSel(objUpdateHintFileReader.m_strLatestFeatures);

				SetWindowText(g_objLocalizeMgr.GetString(LID_UPDATE_QUESTION_DIALOG_CAPTION, LID_UQDC_DEFAULT) + _T(": ") + objUpdateHintFileReader.m_strProductName);
			}
			// 如果下载失败，把对话框缩小
			else
			{
				CGlobal::OutputDebugString(_T("Error occurred while downloading UpdateHint file, errcode %d."), nRetCode);
				CRect rtTmp;

				GetDlgItem(IDC_STATIC_LATEST_FEATURES)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_RICHEDIT2_FEATURES)->ShowWindow(SW_HIDE);

				WINDOWPLACEMENT wp;
				INT iY, iHeight;
				GetDlgItem(IDC_STATIC_LATEST_FEATURES)->GetWindowPlacement(&wp);
				iY = wp.rcNormalPosition.top;
				GetDlgItem(IDC_STATIC_ENSURE)->GetWindowPlacement(&wp);
				rtTmp = wp.rcNormalPosition;
				rtTmp.OffsetRect(0, iY - rtTmp.top);
				GetDlgItem(IDC_STATIC_ENSURE)->SetWindowPos(NULL, rtTmp.left, rtTmp.top, rtTmp.Width(), rtTmp.Height(), SWP_NOSIZE | SWP_NOZORDER);

				GetDlgItem(IDC_RICHEDIT2_FEATURES)->GetWindowPlacement(&wp);
				iY = wp.rcNormalPosition.top;
				GetDlgItem(IDC_CHECK_NO_PROMPT)->GetWindowPlacement(&wp);
				rtTmp = wp.rcNormalPosition;
				iHeight = wp.rcNormalPosition.top - iY;
				rtTmp.OffsetRect(0, iY - rtTmp.top);
				GetDlgItem(IDC_CHECK_NO_PROMPT)->SetWindowPos(NULL, rtTmp.left, rtTmp.top, rtTmp.Width(), rtTmp.Height(), SWP_NOSIZE | SWP_NOZORDER);

				GetDlgItem(IDC_BTN_YES)->GetWindowPlacement(&wp);
				rtTmp = wp.rcNormalPosition;
				rtTmp.OffsetRect(0, -iHeight);
				GetDlgItem(IDC_BTN_YES)->SetWindowPos(NULL, rtTmp.left, rtTmp.top, rtTmp.Width(), rtTmp.Height(), SWP_NOSIZE | SWP_NOZORDER);

				GetDlgItem(IDC_BTN_NO)->GetWindowPlacement(&wp);
				rtTmp = wp.rcNormalPosition;
				rtTmp.OffsetRect(0, -iHeight);
				GetDlgItem(IDC_BTN_NO)->SetWindowPos(NULL, rtTmp.left, rtTmp.top, rtTmp.Width(), rtTmp.Height(), SWP_NOSIZE | SWP_NOZORDER);

				GetWindowRect(&rtTmp);
				rtTmp.bottom -= iHeight;
				SetWindowPos(NULL, rtTmp.left, rtTmp.top, rtTmp.Width(), rtTmp.Height(), SWP_NOZORDER);

				CenterWindow();
			}
			// 开启定时器，检测是否在全屏模式
			SetTimer(TIMERID_SHOW_UPDATE_HINT_DLG, TIMER_SHOW_UH_DLG_ELAPSE, NULL);
		}
		break;

	case GENERAL_FILE:
		{
			if (0 == nRetCode)			// 下载成功
			{
				;
			}
			else
			{
				// 下载失败
				CGlobal::OutputDebugString(_T("Error occurred while downloading file: %s, errcode %u."), pInetFile->m_strFileName, nRetCode);
				bSucceed = FALSE;
			}
		}
		break;
	}

	m_InetFiles.DeleteItem(pInetFile);
	if (m_InetFiles.Count() == 0)
	{
		if (GENERAL_FILE == llFileType)
		{
			m_bShowTrayInfo = FALSE;
			m_bNeedThreadRun = FALSE;
			KillTimer(TIMERID_TRAY_HIDE);
#if  _MSC_VER >= 1400 // vs2005+
			_tcscpy_s(m_tNID.szInfo, 255, _T(""));
#else
			_tcscpy(m_tNID.szInfo, _T(""));
#endif
			m_tNID.uFlags = NIF_INFO;
			Shell_NotifyIcon(NIM_MODIFY, &m_tNID);
			if (bSucceed)
			{
				CGlobal::OutputDebugString(_T("Download complete."));
				SetTimer(TIMERID_SHOW_DOWNLOAD_COMPLETE, TIMER_SHOW_DC_ELAPSE, NULL);
			}
			else
			{
			}
		}
	}

	m_nCurThreadCount--;
	return 0;
}

DWORD WINAPI CAutoUpdateDlg::_StartDownloadFileThread(LPVOID lParam)
{
	CAutoUpdateDlg *pDlg = (CAutoUpdateDlg *)lParam;
	while (pDlg->m_bNeedThreadRun)
	{
		CAutoLock  lock(&pDlg->m_lock);
		if (pDlg->m_nCurThreadCount < pDlg->m_nMaxDownThread)
		{
			CInetFile* pInetFile = pDlg->m_InetFiles.Front();
			if (pInetFile && 0 == pInetFile->m_nDownloadTime)
			{
				// 只下载已下载次数为  0 的列表
// 				if (GENERAL_FILE == pInetFile->m_dlFileType)
// 				{
// 				}
				if (pInetFile->GetInetFile())
				{
					pDlg->m_nCurThreadCount++;
				}
			}
		}
		else
		{
			Sleep(10);
		}
	}
	return 0L;
}

void CAutoUpdateDlg::StartCheckUpdate()
{
	m_bNeedThreadRun = TRUE;
	m_hStartDownloadFileThread = CreateThread(NULL, 0, _StartDownloadFileThread, this, 0, NULL);

	m_tNID.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_tNID.hWnd = GetSafeHwnd();
	m_tNID.uID = IDR_MAINFRAME;
	m_tNID.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	// 自定义的消息名称
	m_tNID.uCallbackMessage = WM_TRAYICONNOTIFY;
	m_tNID.hIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

#if  _MSC_VER >= 1400 // vs2005+
	_tcscpy_s(m_tNID.szTip, 127, g_objLocalizeMgr.GetString(LID_TRAY_TIP, LID_TT_DEFAULT));
#else
	_tcscpy(m_tNID.szTip, g_objLocalizeMgr.GetString(LID_TRAY_TIP, LID_TT_DEFAULT));
#endif

#if  _MSC_VER >= 1400 // vs2005+
	_tcscpy_s(m_tNID.szInfoTitle, 63, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOADING_BALLOON_TITLE, LID_TDBT_DEFAULT));
#else
	_tcscpy(m_tNID.szInfoTitle, g_objLocalizeMgr.GetString(LID_TRAY_DOWNLOADING_BALLOON_TITLE, LID_TDBT_DEFAULT));
#endif

#if  _MSC_VER >= 1400 // vs2005+
	_tcscpy_s(m_tNID.szInfo, 255, _T(""));
#else
	_tcscpy(m_tNID.szInfo, _T(""));
#endif

	m_tNID.dwInfoFlags = NIIF_INFO;

	m_RemoteFiles.ClearAllList();
	m_dwAlreadyDownLen = 0;
	CInetFile *pInetFileNew = new CInetFile();
	if (pInetFileNew)
	{
		pInetFileNew->m_bProgressMode = true;
		pInetFileNew->m_dlFileType = FILELIST_XML;
		pInetFileNew->m_hWnd = m_hWnd;
		pInetFileNew->m_strURL = g_objGlobalData.g_strUpdateURL + FILENAME_FILELIST_XML;
		pInetFileNew->m_strPath = g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML;
		pInetFileNew->m_dwTotalLen = 0;
		pInetFileNew->m_strFileName = FILENAME_FILELIST_XML;
		m_InetFiles.Push(pInetFileNew);
	}
}

BOOL CAutoUpdateDlg::ValidateFiles()
{
	CString strFileListPath = g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML;
	if (!CGlobal::FileExist(strFileListPath))
	{
		return FALSE;
	}
	else
	{
		m_RemoteFiles.ClearAllList();
		if (m_RemoteFiles.ReadFromXMLFile((LPTSTR)(LPCTSTR)strFileListPath))
		{
			BOOL bHasDownloadedFile;
			m_dwCount = m_RemoteFiles.CheckNeedDownload(bHasDownloadedFile);
			if (0 == m_dwCount)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

void CAutoUpdateDlg::BackupFiles()
{
}

void CAutoUpdateDlg::RestoreFiles()
{
}

LRESULT CAutoUpdateDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (message)
	{
	case WM_USER_SHOW_UPDATE_HINT:
		{
			// 读取/拉取更新说明
			m_dwAlreadyDownLen = 0;
			CInetFile *pInetFileNew = new CInetFile();
			if (pInetFileNew)
			{
				pInetFileNew->m_bProgressMode = true;
				pInetFileNew->m_dlFileType = UPDATEHINT_XML;
				pInetFileNew->m_hWnd = m_hWnd;
				pInetFileNew->m_strURL = g_objGlobalData.g_strUpdateURL + FILENAME_UPDATEHINT_XML;
				pInetFileNew->m_strPath = g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_UPDATEHINT_XML;
				pInetFileNew->m_dwTotalLen = 0;
				pInetFileNew->m_strFileName = FILENAME_UPDATEHINT_XML;
				m_InetFiles.Push(pInetFileNew);
			}

			CAutoLock  lock(&m_lock);
			CInetFile* pInetFile = m_InetFiles.Front();
			if (pInetFile && 0 == pInetFile->m_nDownloadTime)
			{
				if (pInetFile->GetInetFile())
				{
					m_nCurThreadCount++;
				}
			}
		}

	case WM_USER_ADD_FILES_TO_LIST:
		{
			// 将要下载文件添加到 m_InetFiles 列表中
			CUpdateFile *pUpdateFile = m_RemoteFiles.PopUpdateFile();
			while (pUpdateFile)
			{
				CInetFile *pInetFileNew = new CInetFile();
				if (pInetFileNew)
				{
					pInetFileNew->m_bProgressMode = true;
					pInetFileNew->m_dlFileType = GENERAL_FILE;
					pInetFileNew->m_hWnd = m_hWnd;
					pInetFileNew->m_strURL = pUpdateFile->m_strURL;
					pInetFileNew->m_strPath = pUpdateFile->m_strFilePath;
					pInetFileNew->m_dwTotalLen = (DWORD)pUpdateFile->m_ullSize;
					pInetFileNew->m_strFileName = pUpdateFile->m_strFileName;

					pInetFileNew->m_bUpdateNotExist = pUpdateFile->m_bUpdateNotExist;
					pInetFileNew->m_bRegister = pUpdateFile->m_bRegister;
					pInetFileNew->m_bZipFile = pUpdateFile->m_bZipFile;
					pInetFileNew->m_bForceOverwrite = pUpdateFile->m_bForceOverwrite;
					pInetFileNew->m_bExecute = pUpdateFile->m_bExecute;
					pInetFileNew->m_strExecParam = pUpdateFile->m_strExecParam;

					pInetFileNew->m_strMD5 = pUpdateFile->m_strMD5;

					m_InetFiles.Push(pInetFileNew);

					MG_SAFE_DELETE(pUpdateFile);
					pUpdateFile = m_RemoteFiles.PopUpdateFile();
				}
			}
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CAutoUpdateDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// OnOK();
}

void CAutoUpdateDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
	if (m_bHideWindow)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
}

void CAutoUpdateDlg::OnBnClickedBtnYes()
{
	// TODO: 在此添加控件通知处理程序代码
	// 隐藏界面
	ShowWindow(SW_HIDE);
	// 恢复开始下载线程
	ResumeThread(m_hStartDownloadFileThread);
	// 可以显示托盘
	m_bShowTrayInfo = TRUE;
}

void CAutoUpdateDlg::OnBnClickedBtnNo()
{
	// TODO: 在此添加控件通知处理程序代码
	// 记录用户喜好，如是否忽略
	if (((CButton *)GetDlgItem(IDC_CHECK_NO_PROMPT))->GetCheck())
	{
		g_objIniFileOperations.WriteIniInt(IFO_SECION_AUTO_UPDATE, IFO_SAU_USER_IGNORED, TRUE);

		// 如果忽略，记录FileList.Xml文件的HASH为版本
		TCHAR szProductVersion[_MAX_PATH];
		CHashMgr::GetFileHash(g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_CONFIG + FILENAME_FILELIST_XML, CALG_MD5, szProductVersion);
		g_objIniFileOperations.WriteIniString(IFO_SECION_AUTO_UPDATE, IFO_SAU_LAST_PRODUCT_VERSION, szProductVersion);
	}
	// 退出程序
	PostQuitMessage(EXIT_CODE_USER_CANCEL);
}
