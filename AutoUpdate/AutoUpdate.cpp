
// AutoUpdate.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"

#include "..\Common\IniFileOperations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoUpdateApp

BEGIN_MESSAGE_MAP(CAutoUpdateApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoUpdateApp 构造

CAutoUpdateApp::CAutoUpdateApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAutoUpdateApp 对象

CAutoUpdateApp theApp;

CGlobal g_objGlobalData;

CLocalizeMgr g_objLocalizeMgr;

CIniFileOperations g_objIniFileOperations(IFO_FILE_NAME, IFO_FILE_EXT);

// CAutoUpdateApp 初始化

BOOL CAutoUpdateApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	::CoInitialize(NULL);

	AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	// SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("Magon_Auto_Update"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 如果实例已经运行
		if (hMutex)
		{
			CloseHandle(hMutex);
		}
		return FALSE;
	}

	// 读取命令行参数：URL和语言ID
	CString strCommand = m_lpCmdLine;
	if (!strCommand.IsEmpty())
	{
		INT iPos = strCommand.Find(_T(" "));
		if (iPos > 0)
		{
			// 截取URL
			g_objGlobalData.g_strUpdateURL = strCommand.Left(iPos);
			if (g_objGlobalData.g_strUpdateURL.Right(1) != _T("/"))
			{
				g_objGlobalData.g_strUpdateURL += _T("/");
			}

			// 截取语言ID
			strCommand = strCommand.Right(strCommand.GetLength() - iPos -1);
			strCommand.Trim();
			if (!strCommand.IsEmpty())
			{
				// 获取语言ID
				g_objLocalizeMgr.m_iLangID = _ttoi(strCommand);
			}
			else
			{
				// 获取系统语言ID
				g_objLocalizeMgr.GetLocaleLangID();
			}
		}
		else
		{
			// 只有URL时，复制URL
			g_objGlobalData.g_strUpdateURL = strCommand;
			if (g_objGlobalData.g_strUpdateURL.Right(1) != _T("/"))
			{
				g_objGlobalData.g_strUpdateURL += _T("/");
			}
		}
	}
	else
	{
		// 获取系统语言ID
		g_objLocalizeMgr.GetLocaleLangID();
		::MessageBox(NULL, g_objLocalizeMgr.GetString(LID_DONT_NEED_MANUAL_CALL_TEXT, LID_DNMCT_DEFAULT), g_objLocalizeMgr.GetString(LID_DONT_NEED_MANUAL_CALL_CAPTION, LID_DNMCC_DEFAULT), MB_ICONERROR);
		return FALSE;
	}
#if  _MSC_VER >= 1400 // vs2005+
	_tcscpy_s(m_lpCmdLine, 1, _T(""));
#else
	_tcscpy(m_lpCmdLine, _T(""));
#endif

	CAutoUpdateDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	::CoUninitialize();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
