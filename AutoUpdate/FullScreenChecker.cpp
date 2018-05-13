#include "StdAfx.h"
#include "FullScreenChecker.h"

CFullScreenChecker::CFullScreenChecker(void)
{
	// 如果只有一个屏幕，直接添加
	if (GetSystemMetrics(SM_CMONITORS) == 1)
	{
		m_vecCRect.push_back(
			CRect(0, 
			0, 
			GetSystemMetrics(SM_CXSCREEN), 
			GetSystemMetrics(SM_CYSCREEN)
			));
	}
	// 否则遍历添加屏幕区域
	else
	{
		EnumDisplayMonitors(NULL, NULL, &CFullScreenChecker::MonitorEnumProc, (LPARAM)(vector<CRect> *)&m_vecCRect);
	}
}

CFullScreenChecker::~CFullScreenChecker(void)
{
}

BOOL CALLBACK CFullScreenChecker::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)   
{
	vector<CRect> *pvecCRect = (vector<CRect> *)dwData;
	pvecCRect->push_back(*lprcMonitor);
	return TRUE;
}

BOOL CFullScreenChecker::IsRectInRect(LPRECT lprcInner, LPRECT lprcOuter)
{
	// 判断矩形是否在矩形内部
	return lprcOuter->top <= lprcInner->top 
		&& lprcOuter->bottom >= lprcInner->bottom 
		&& lprcOuter->left <= lprcInner->left 
		&& lprcOuter->right >= lprcInner->right;
}

void CFullScreenChecker::GetWindowRealtimeRect(HWND hWnd, CRect &rect)
{
	WINDOWPLACEMENT wp;
	::GetWindowPlacement(hWnd, &wp);

	// 如果不是最大化
	if (wp.flags != WPF_RESTORETOMAXIMIZED)
	{
		rect = wp.rcNormalPosition;
		// 坍塌掉边缘区域
		rect.DeflateRect(GetWindowBorderSize(hWnd, FCS_GWBS_XBORDER), GetWindowBorderSize(hWnd, FCS_GWBS_YBORDER));
	}
	// 如果是
	else
	{
		// 得到工作区大小，即屏幕大小去掉任务栏
		CRect rtWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rtWorkArea, 0);
		rect = rtWorkArea;
	}
}

INT CFullScreenChecker::GetWindowBorderSize(HWND hWnd, INT iIndex)
{
#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER		92
#endif
	OSVERSIONINFO ovi = {sizeof(OSVERSIONINFO)};
	GetVersionEx(&ovi);
	// 是否是vista以上版本
	BOOL bIsAboveVista = (ovi.dwMajorVersion >= 6);
	// 得到窗口风格
	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	// 如果是可变大小
	if (dwStyle & WS_THICKFRAME)
	{
		switch (iIndex)
		{
		case FCS_GWBS_XBORDER:
			return ::GetSystemMetrics(SM_CXSIZEFRAME) + ::GetSystemMetrics(SM_CXBORDER) + (bIsAboveVista ? GetSystemMetrics(SM_CXPADDEDBORDER) : 0) - ((dwStyle & WS_CAPTION) ? 1 : 2);
		case FCS_GWBS_YBORDER:
			return ::GetSystemMetrics(SM_CYSIZEFRAME) + ::GetSystemMetrics(SM_CYBORDER) + (bIsAboveVista ? GetSystemMetrics(SM_CXPADDEDBORDER) : 0) - ((dwStyle & WS_CAPTION) ? 1 : 2);
		}
	}
	// 如果是细小有标题栏或者是对话框架边框有标题栏
	if (dwStyle & WS_DLGFRAME)
	{
		switch (iIndex)
		{
		case FCS_GWBS_XBORDER:
			return ::GetSystemMetrics(SM_CXDLGFRAME) + ::GetSystemMetrics(SM_CXBORDER)+ (bIsAboveVista ? GetSystemMetrics(SM_CXPADDEDBORDER) : 0) - 1;
		case FCS_GWBS_YBORDER:
			return ::GetSystemMetrics(SM_CYDLGFRAME) + ::GetSystemMetrics(SM_CYBORDER)+ (bIsAboveVista ? GetSystemMetrics(SM_CXPADDEDBORDER) : 0) - 1;
		}
	}
	// 如果是细小无标题栏
	if (dwStyle & WS_BORDER)
	{
		return 1;
	}
	// 对话框架边框无标题栏
	if (dwStyle & DS_MODALFRAME)
	{
		switch (iIndex)
		{
		case FCS_GWBS_XBORDER:
			return ::GetSystemMetrics(SM_CXDLGFRAME);
		case FCS_GWBS_YBORDER:
			return ::GetSystemMetrics(SM_CYDLGFRAME);
		}
	}
	// 否则，为无边框
	return 0;
}

// 测试用户是否在使用全屏软件
// 检测是否处于全屏模式（单屏）
BOOL CFullScreenChecker::IsFullScreen()
{
	HWND hWndTop = ::GetForegroundWindow();

	CGlobal::OutputDebugString(_T("CFullScreenChecker::IsFullScreen"));

	return _SubIsFullScreen(hWndTop);
}

// 检测是否处于全屏模式_递归子函数（单屏）
BOOL CFullScreenChecker::_SubIsFullScreen(HWND hWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	if (!::IsWindowVisible(hWnd))
	{
		return _SubIsFullScreen(::GetParent(hWnd));
	}

	CRect rtWnd;
	GetWindowRealtimeRect(hWnd, rtWnd);

	return IsRectInRect(rtWnd, 
		CRect(0, 
		0, 
		GetSystemMetrics(SM_CXSCREEN), 
		GetSystemMetrics(SM_CYSCREEN)
		)) || _SubIsFullScreen(::GetParent(hWnd));
}

// 检测是否处于全屏模式（多屏）
// 传入参数为要检测的屏幕中的子矩形
BOOL CFullScreenChecker::IsFullScreen(CRect rtSrcWnd)
{
	HWND hWndTop = ::GetForegroundWindow();

	CGlobal::OutputDebugString(_T("CFullScreenChecker::IsFullScreen"));

	return _SubIsFullScreen(hWndTop, rtSrcWnd);
}

// 检测是否处于全屏模式_递归子函数（多屏）
BOOL CFullScreenChecker::_SubIsFullScreen(HWND hWnd, CRect rtSrcWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	if (!::IsWindowVisible(hWnd))
	{
		return _SubIsFullScreen(::GetParent(hWnd), rtSrcWnd);
	}

	CRect rtWnd;
	GetWindowRealtimeRect(hWnd, rtWnd);

	CRect rtScreen(0, 
		0, 
		GetSystemMetrics(SM_CXSCREEN), 
		GetSystemMetrics(SM_CYSCREEN)
		);

	// 找到对话框所在的屏幕矩形
	UINT i;
	for (i=0; i<m_vecCRect.size(); i++)
	{
		if (IsRectInRect(rtSrcWnd, m_vecCRect[i]))
		{
			rtScreen = m_vecCRect[i];
			break;
		}
	}

	ASSERT(i == 0 || i >= m_vecCRect.size());

	return IsRectInRect(rtScreen, rtWnd) || _SubIsFullScreen(::GetParent(hWnd), rtSrcWnd);
}