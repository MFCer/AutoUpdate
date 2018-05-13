#pragma once

// 本地化（多语言）
#include "LocalizeMgr.h"
#include "LocalizeItemDefinition.h"

// 强制删除、解锁文件
#include "unlocker.hpp"

// 全屏检测
#include "FullScreenChecker.h"

#define MG_SAFE_DELETE(ptr)				{if (ptr) { delete (ptr); (ptr) = NULL; }}
#define MG_SAFE_DELETEA(pBase)			{if (pBase) { delete [] (pBase); (pBase) = NULL; }}
#define MG_SAFE_RELEASE(ptr)			{if (ptr) { (ptr)->Release(); (ptr) = NULL; }}

#define WORKING_DIRECTORY_DOWNLOAD		_T("\\Download\\")
#define WORKING_DIRECTORY_CONFIG		_T("\\Config\\")
#define WORKING_DIRECTORY_BACKUP		_T("\\Backup\\")

#define FILENAME_FILELIST_XML			_T("FileList.xml")
#define FILENAME_UPDATEHINT_XML			_T("UpdateHint.xml")

#define IFO_FILE_NAME					_T("\\Config\\Config")
#define IFO_FILE_EXT					_T("ini")

#define IFO_SECION_AUTO_UPDATE			_T("AutoUpdate")

// #define IFO_SAU_READY_TO_UPDATE			_T("ReadyToUpdate")
// #define IFO_SAU_RTU_DFT_VALUE			FALSE

typedef enum E_UPDATE_STATUS
{
	// 未开始
	E_US_NOTSTART = 0, 
	// 正在更新
	E_US_UPDATING
} E_UPDATE_STATUS;

#define IFO_SAU_UPDATE_STATUS			_T("UpdateStatus")
#define IFO_SAU_US_DFT_VALUE			E_US_NOTSTART

#define IFO_SAU_USER_IGNORED			_T("UserIgnored")
#define IFO_SAU_UI_DFT_VALUE			FALSE

#define IFO_SAU_LAST_PRODUCT_VERSION	_T("LastProductVersion")
#define IFO_SAU_LPV_DFT_VALUE			_T("")

#define IFO_SAU_LAST_UPDATE_TIME		_T("LastUpdateTime")

// 多少天后显示警告
#define PROMPT_NETWORK_ERROR_DAYS		30

// 下载的待更新文件临时后缀
#define UPDATE_FILE_POSTFIX				_T(".update")

// 用户主动退出
#define EXIT_CODE_USER_CANCEL			2
// 没有可用更新
#define EXIT_CODE_NO_UPDATE_AVAILABLE	1
// 更新过程中，发生错误
#define EXIT_CODE_ERROR_OCCURRED		-1
// 没有错误
#define EXIT_CODE_ERROR_SUCCESS			0

// 定时器

// 进度显示托盘隐藏定时器
#define TIMERID_TRAY_HIDE				0
#define TIMER_TRAY_HIDE_ELAPSE			100

// 显示更新信息对话框定时器
#define TIMERID_SHOW_UPDATE_HINT_DLG	1
#define TIMER_SHOW_UH_DLG_ELAPSE		1000

// 显示下载完成气泡定时器
#define TIMERID_SHOW_DOWNLOAD_COMPLETE	2
#define TIMER_SHOW_DC_ELAPSE			1000

// 显示确认替换对话框
#define TIMERID_SHOW_CONFIRM_UPDATE_DLG	3
#define TIMER_SHOW_CU_DLG_ELAPSE		1000

// 显示更新完成气泡定时器
#define TIMERID_SHOW_UPDATE_COMPLETE	4
#define TIMER_SHOW_UC_ELAPSE			1000

// 显示气泡延时
#define TIMERID_TRAY_BALLOON_TIMEOUT	255
#define TIMER_TRAY_BALLOON_TIMEOUT		3000

// 托盘信息刷新时间间隔
#define INTERVAL_TIME_TRAY_INFO			500

// 显示更新提示对话框消息
#define WM_USER_SHOW_UPDATE_HINT		(WM_USER + 1000)
#define WM_USER_ADD_FILES_TO_LIST		(WM_USER_SHOW_UPDATE_HINT + 1)

class CGlobal
{
public:
	CGlobal(void);
	~CGlobal(void);

	// 命令行参数，更新文件列表所在目录URL
	CString g_strUpdateURL;

	// 产品所在目录
	CString g_strProductDir;
	// 更新工作目录（更新工作目录在产品目录下一级，即此程序和产品不在同一级目录中）
	CString g_strUpdateDir;

	// 查看文件是否存在
	static BOOL FileExist(CString strFileName);

	// 文件夹是否存在
	static BOOL FolderExists(CString strFolderPath);

	// 创建文件夹
	static BOOL CreateFolder(CString strFolderPath);

	// 强制删除文件
	static BOOL ForceDeleteFile(CString strFolderPath);

	// 执行文件（64位兼容）
	static BOOL ShellExecute(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

//#ifdef _DEBUG
	static void OutputDebugString(LPCTSTR lpszFmt, ...);
//#else
//	static void OutputDebugString(...){}
//#endif

	// 全屏检测
	CFullScreenChecker m_objFullScreenChecker;
};

extern CGlobal g_objGlobalData;
