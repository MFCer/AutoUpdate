#pragma once

#include <vector>
using namespace std;

#define FCS_GWBS_XBORDER			0
#define FCS_GWBS_YBORDER			1

class CFullScreenChecker
{
public:
	CFullScreenChecker(void);
	virtual ~CFullScreenChecker(void);

	// 检测是否处于全屏模式（单屏）
	BOOL IsFullScreen();

	// 检测是否处于全屏模式（多屏）
	BOOL IsFullScreen(CRect rtSrcWnd);

protected:
	// 检测是否处于全屏模式_递归子函数（单屏）
	BOOL _SubIsFullScreen(HWND hWnd);

	// 检测是否处于全屏模式_递归子函数（多屏）
	BOOL _SubIsFullScreen(HWND hWnd, CRect rtSrcWnd);

	// 判断矩形是否在矩形内部
	BOOL IsRectInRect(LPRECT lprcInner, LPRECT lprcOuter);

	// 显示器枚举回调
	static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

	// 得到窗口实际矩形大小
	void GetWindowRealtimeRect(HWND hWnd, CRect &rect);

	// 得到窗口边框（支持Vista以上操作系统的边框填充）
	INT GetWindowBorderSize(HWND hWnd, INT iIndex);

	// 多屏显示器矩形容器
	vector<CRect> m_vecCRect;
};
