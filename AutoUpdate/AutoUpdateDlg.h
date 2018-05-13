
// AutoUpdateDlg.h : 头文件
//

#pragma once

#include "AutoLock.h"
#include "UpdateFiles.h"
#include "InetFiles.h"

// CAutoUpdateDlg 对话框
class CAutoUpdateDlg : public CDialog
{
// 构造
public:
	CAutoUpdateDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOUPDATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	DWORD m_dwCount;
	DWORD m_dwTotalLen;
	DWORD m_dwAlreadyDownLen;

	CCriticalSection m_lock;

	CUpdateFiles m_RemoteFiles;
	CInetFiles m_InetFiles;

	UINT m_nMaxDownThread;
	UINT m_nCurThreadCount;
	BOOL m_bShowTrayInfo;

	CString m_strTrayInfo;

	NOTIFYICONDATA m_tNID;

	BOOL m_bSelfNeedUpdate;

	CRect m_rtTrayShowPt;

	HANDLE m_hStartDownloadFileThread;

	BOOL m_bNeedThreadRun;

	// 用于隐藏界面
	BOOL m_bHideWindow;

	// 关闭程序时，是否要运行执行文件列表中的文件
	BOOL m_bExecuteFiles;

	// 用于保存要执行的文件列表
	list<CFileToExecute> m_lstFileToExecute;

	static DWORD WINAPI _StartDownloadFileThread(LPVOID lParam);

	LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam); 

	LRESULT OnDownLoadFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnDownLoadFileEnd(WPARAM wParam, LPARAM lParam);

	// 校验文件，查看是否可更新
	BOOL ValidateFiles();

	// 开始检查更新
	void StartCheckUpdate();

	// 备份文件
	void BackupFiles();

	// 还原文件
	void RestoreFiles();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnYes();
	afx_msg void OnBnClickedBtnNo();
};
