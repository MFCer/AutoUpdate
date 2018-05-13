// SheetAutoUpdate.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoUpdateConfig.h"
#include "SheetAutoUpdate.h"

#include "AboutDlg.h"


// CSheetAutoUpdate

IMPLEMENT_DYNAMIC(CSheetAutoUpdate, CPropertySheet)

CSheetAutoUpdate::CSheetAutoUpdate(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_pageFileListCfg);
	AddPage(&m_pageUpdateHintCfg);

	SetActivePage(iSelectPage);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSheetAutoUpdate::CSheetAutoUpdate(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_pageFileListCfg);
	AddPage(&m_pageUpdateHintCfg);

	SetActivePage(iSelectPage);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSheetAutoUpdate::~CSheetAutoUpdate()
{
}


BEGIN_MESSAGE_MAP(CSheetAutoUpdate, CPropertySheet)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CSheetAutoUpdate 消息处理程序

BOOL CSheetAutoUpdate::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO:  在此添加您的专用代码

	INT ids[] = {IDOK, IDCANCEL};//, ID_APPLY_NOW, IDHELP };

	// Hide Apply and Help buttons
	CWnd *pWnd = GetDlgItem(IDHELP);
	pWnd->ShowWindow(SW_HIDE);

	CRect rectBtn;
	INT nSpacing[] = {-15, 5}; // space between two buttons...

	for (INT i=0; i<sizeof(ids)/sizeof(int); i++)
	{
		GetDlgItem(ids[i])->GetWindowRect(&rectBtn);

		ScreenToClient(&rectBtn);
		int btnWidth = rectBtn.Width();
		rectBtn.left = rectBtn.left + (btnWidth + nSpacing[i]) - 10;
		rectBtn.right = rectBtn.right + (btnWidth + nSpacing[i]);
		rectBtn.bottom += 4;

		GetDlgItem(ids[i])->MoveWindow(rectBtn);
	}

	CRect rect;
	GetWindowRect(&rect);
	rect.bottom += 5;
	MoveWindow(&rect);

	return bResult;
}

void CSheetAutoUpdate::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CPropertySheet::OnSysCommand(nID, lParam);
	}
}
