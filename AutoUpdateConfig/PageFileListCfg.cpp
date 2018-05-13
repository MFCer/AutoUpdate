// PageFileListCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoUpdateConfig.h"
#include "PageFileListCfg.h"

#include "..\Common\HashMgr.h"
#include "DigitGrouping.h"
#include "FileInfoMgr.h"
#include "..\Common\IniFileOperations.h"

#define FL_COLUMN_CHECKBOX			0
#define FL_COLUMN_NO				(FL_COLUMN_CHECKBOX + 1)
#define FL_COLUMN_FILENAME			(FL_COLUMN_NO + 1)
#define FL_COLUMN_EXECUTE			(FL_COLUMN_FILENAME + 1)
#define FL_COLUMN_OVERWRITE			(FL_COLUMN_EXECUTE + 1)
#define FL_COLUMN_REGISTER			(FL_COLUMN_OVERWRITE + 1)
#define FL_COLUMN_DECOMPRESS		(FL_COLUMN_REGISTER + 1)
#define FL_COLUMN_UPDATENOTEXSIT	(FL_COLUMN_DECOMPRESS + 1)
#define FL_COLUMN_FILELEN			(FL_COLUMN_UPDATENOTEXSIT + 1)
#define FL_COLUMN_FILEVER			(FL_COLUMN_FILELEN + 1)
#define FL_COLUMN_FILEMD5			(FL_COLUMN_FILEVER + 1)
#define FL_COLUMN_LANGID			(FL_COLUMN_FILEMD5 + 1)
#define FL_COLUMN_DETFILENAME		(FL_COLUMN_LANGID + 1)

#define LANGID_ALL_SUPPORT			_T("(所有语言)")
#define LANGID_DEFAULT_SUPPORT		_T("(默认)")


// CPageFileListCfg 对话框

IMPLEMENT_DYNAMIC(CPageFileListCfg, CPropertyPage)

CPageFileListCfg::CPageFileListCfg()
	: CPropertyPage(CPageFileListCfg::IDD)
{

}

CPageFileListCfg::~CPageFileListCfg()
{
}

void CPageFileListCfg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WORK_DIR, m_ctrlWorkDir);
}


BEGIN_MESSAGE_MAP(CPageFileListCfg, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_WORK_DIR, &CPageFileListCfg::OnEnChangeEditWorkDir)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CPageFileListCfg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CPageFileListCfg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_ADD_FILE, &CPageFileListCfg::OnBnClickedBtnAddFile)
	ON_BN_CLICKED(IDC_BTN_ADD_DIR, &CPageFileListCfg::OnBnClickedBtnAddDir)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPageFileListCfg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_SAVETOFILE, &CPageFileListCfg::OnBnClickedBtnSavetofile)
	ON_BN_CLICKED(IDC_BTN_DEL_CHECK, &CPageFileListCfg::OnBnClickedBtnDelCheck)
	ON_BN_CLICKED(IDC_BTN_SEL_ALL, &CPageFileListCfg::OnBnClickedBtnSelAll)
	ON_BN_CLICKED(IDC_BTN_UNSEL_ALL, &CPageFileListCfg::OnBnClickedBtnUnselAll)
	ON_BN_CLICKED(IDC_BTN_REVERSE_SEL, &CPageFileListCfg::OnBnClickedBtnReverseSel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILES, &CPageFileListCfg::OnNMClickListFiles)
	ON_BN_CLICKED(IDC_CHECK_EXEC, &CPageFileListCfg::OnBnClickedCheckExec)
	ON_BN_CLICKED(IDC_CHECK_OVERWRITE, &CPageFileListCfg::OnBnClickedCheckOverwrite)
	ON_BN_CLICKED(IDC_CHECK_REG, &CPageFileListCfg::OnBnClickedCheckReg)
	ON_BN_CLICKED(IDC_CHECK_DECOMPRESS, &CPageFileListCfg::OnBnClickedCheckDecompress)
	ON_BN_CLICKED(IDC_CHECK_UPDATENOTEXIST, &CPageFileListCfg::OnBnClickedCheckUpdatenotexist)
	ON_CBN_SELCHANGE(IDC_COMBO_LANGLIST, &CPageFileListCfg::OnCbnSelchangeComboLanglist)
	ON_EN_CHANGE(IDC_EDIT_DETFILENAME, &CPageFileListCfg::OnEnChangeEditDetname)
END_MESSAGE_MAP()


// CPageFileListCfg 消息处理程序

BOOL CPageFileListCfg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlWorkDir.EnableBrowseButton();
	m_ctrlWorkDir.EnableFolderBrowseButton();

	m_ctrlListFiles.InsertColumn(FL_COLUMN_CHECKBOX, _T(""), LVCFMT_LEFT, 20);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_NO, _T("No"), LVCFMT_RIGHT, 30);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_FILENAME, _T("文件名"), LVCFMT_LEFT, 280);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_EXECUTE, _T("执行"), LVCFMT_CENTER, 36);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_OVERWRITE, _T("覆盖"), LVCFMT_CENTER, 36);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_REGISTER, _T("注册"), LVCFMT_CENTER, 36);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_DECOMPRESS, _T("解压"), LVCFMT_CENTER, 36);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_UPDATENOTEXSIT, _T("不存在更新"), LVCFMT_CENTER, 72);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_FILELEN, _T("文件长度"), LVCFMT_RIGHT, 100);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_FILEVER, _T("文件版本"), LVCFMT_RIGHT, 140);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_FILEMD5, _T("文件MD5值"), LVCFMT_LEFT, 220);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_LANGID, _T("适用语言"), LVCFMT_LEFT, 100);
	m_ctrlListFiles.InsertColumn(FL_COLUMN_DETFILENAME, _T("目标文件名"), LVCFMT_LEFT, 280);

	m_ctrlListFiles.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	GetDlgItem(IDC_EDIT_UPDATEURL)->SetWindowText(_T("http://"));

	m_ctrlComboBoxLangList.AddString(LANGID_ALL_SUPPORT);
	INT iLangCnt = g_objIniFileOperations.GetIniInt(IFO_SECTION_GENERAL, IFO_SG_LANG_CNT, 0);
	CString str, strItem;
	for (INT i=0; i<iLangCnt; i++)
	{
		str.Format(_T("%d"), i);
		if ((strItem = g_objIniFileOperations.GetIniString(IFO_SECTION_GENERAL, str, _T(""))) != _T(""))
		{
			strItem.Insert(0, str);
			m_ctrlComboBoxLangList.AddString(strItem);
		}
	}
	if (iLangCnt)
	{
		m_ctrlComboBoxLangList.AddString(LANGID_DEFAULT_SUPPORT);
		m_ctrlComboBoxLangList.SetCurSel(0);
	}
// Test
// 	TCHAR szMD5[_MAX_PATH];
// 	CHashMgr::GetFileHash(_T("C:\\AutoUpdate.pdb"), CALG_MD5, szMD5);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPageFileListCfg::OnEnChangeEditWorkDir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	static BOOL s_bSelfChanging = FALSE;
	if (!m_strWorkingDir.IsEmpty() && !s_bSelfChanging)
	{
		if (m_ctrlListFiles.GetItemCount() == 0 || MessageBox(_T("是否要更改工作目录？更改工作目录将清空文件列表。"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDYES) 
		{
			GetDlgItem(IDC_EDIT_WORK_DIR)->GetWindowText(m_strWorkingDir);
			m_ctrlListFiles.DeleteAllItems();
			m_mapUniqueFile.clear();
		}
		else
		{
			s_bSelfChanging = TRUE;
			GetDlgItem(IDC_EDIT_WORK_DIR)->SetWindowText(m_strWorkingDir);
		}
	}
	else
	{
		s_bSelfChanging = FALSE;
		GetDlgItem(IDC_EDIT_WORK_DIR)->GetWindowText(m_strWorkingDir);
	}
}

void CPageFileListCfg::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlgOpen(TRUE, _T("cfg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("自动更新配置文件(*.cfg)|*.cfg|所有文件(*.*)|*.*||"));

	INT structsize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindowsMinorVersion;

	dwVersion = GetVersion(); 
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion))); 
	dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion))); 

	if (dwVersion < 0x80000000)
	{
		structsize = 88;
		// 显示新的文件对话框
	}
	else 
	{
		structsize = 76;
		// 显示老的文件对话框
	}

	dlgOpen.m_ofn.lStructSize = structsize; 
	dlgOpen.m_ofn.lpstrTitle = _T("选择配置文件");

	if (dlgOpen.DoModal() == IDOK)
	{
		CIniFileOperations objIniFile(dlgOpen.GetPathName());
		GetDlgItem(IDC_EDIT_PRODUCTNAME)->SetWindowText(objIniFile.GetIniString(CFG_SECTION_CONFIG, CFG_SC_PRODUCT_NAME, _T("")));

		GetDlgItem(IDC_EDIT_WORK_DIR)->SetWindowText(objIniFile.GetIniString(CFG_SECTION_CONFIG, CFG_SC_WORKING_DIR, _T("")));

		GetDlgItem(IDC_EDIT_UPDATEURL)->SetWindowText(objIniFile.GetIniString(CFG_SECTION_CONFIG, CFG_SC_UPDATE_URL, _T("http://")));

		MessageBox(_T("载入成功！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
}

void CPageFileListCfg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strProductName;
	GetDlgItem(IDC_EDIT_PRODUCTNAME)->GetWindowText(strProductName);
	CFileDialog dlgSave(FALSE, _T("cfg"), strProductName + _T(".cfg"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("自动更新配置文件(*.cfg)|*.cfg|所有文件(*.*)|*.*||"));

	INT structsize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindowsMinorVersion;

	dwVersion = GetVersion(); 
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion))); 
	dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion))); 

	if (dwVersion < 0x80000000)
	{
		structsize = 88;
		// 显示新的文件对话框
	}
	else 
	{
		structsize = 76;
		// 显示老的文件对话框
	}

	dlgSave.m_ofn.lStructSize = structsize; 
	dlgSave.m_ofn.lpstrTitle = _T("选择保存路径");

	if (dlgSave.DoModal() == IDOK)
	{
		WritePrivateProfileString(CFG_SECTION_CONFIG, CFG_SC_PRODUCT_NAME, strProductName, dlgSave.GetPathName());

		WritePrivateProfileString(CFG_SECTION_CONFIG, CFG_SC_WORKING_DIR, m_strWorkingDir, dlgSave.GetPathName());

		CString str;
		GetDlgItem(IDC_EDIT_UPDATEURL)->GetWindowText(str);
		WritePrivateProfileString(CFG_SECTION_CONFIG, CFG_SC_UPDATE_URL, str, dlgSave.GetPathName());

		MessageBox(_T("保存成功！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
}

void CPageFileListCfg::OnBnClickedBtnAddFile()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_strWorkingDir.IsEmpty())
	{
		MessageBox(_T("请先设置工作目录！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CFileDialog dlgOpen(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_NODEREFERENCELINKS, _T("所有文件(*.*)|*.*||"));

	INT structsize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindowsMinorVersion;

	dwVersion = GetVersion(); 
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion))); 
	dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion))); 

	if (dwVersion < 0x80000000)
	{
		structsize = 88;
		// 显示新的文件对话框
	}
	else 
	{
		structsize = 76;
		// 显示老的文件对话框
	}

	TCHAR szFileBuffer[8192];
	memset(szFileBuffer, 0, 8192); 

	dlgOpen.m_ofn.lStructSize = structsize; 
	dlgOpen.m_ofn.lpstrTitle = _T("选择文件");
	dlgOpen.m_ofn.lpstrInitialDir = m_strWorkingDir;
	dlgOpen.m_ofn.nMaxFile = 8192;
	dlgOpen.m_ofn.lpstrFile = szFileBuffer;

	BOOL bIgnored = FALSE;
	CString strIgnoredFiles(_T("以下文件不在工作目录中，已经被忽略：\n"));
	if (dlgOpen.DoModal() == IDOK)
	{
		CString strFileName;
		POSITION pos = dlgOpen.GetStartPosition();
		while (pos != NULL)
		{
			strFileName = dlgOpen.GetNextPathName(pos);
			if (m_strWorkingDir.CompareNoCase(strFileName.Left(m_strWorkingDir.GetLength())) == 0)
			{
				AddFile(strFileName);
			}
			else
			{
				bIgnored++;
				if (bIgnored < 10)
				{
					strIgnoredFiles += strFileName + _T("\n");
				}
				else if (bIgnored == 10)
				{
					strIgnoredFiles += _T("......\n");
				}
			}
		}
		if (bIgnored > 10)
		{
			strIgnoredFiles += strFileName + _T("\n");
		}
		if (bIgnored)
		{
			MessageBox(strIgnoredFiles, _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
	}
}

static CButton *s_pButton;
static BOOL s_bCheck;
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		s_pButton = new CButton();
		ASSERT_VALID(s_pButton);
		CWnd *pWnd = CWnd::FromHandle(hwnd);
		s_pButton->Create( _T("包含子文件夹"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(15, 35, 105, 49), pWnd, -1);
		CFont *pFont = pWnd->GetFont();
		s_pButton->SetFont(pFont, TRUE);
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		SendMessage(hwnd, BFFM_SETEXPANDED, TRUE, lpData);
	}
	else if (uMsg == WM_DESTROY)
	{
		s_bCheck = (UINT)::SendMessage(s_pButton->GetSafeHwnd(), BM_GETCHECK, 0, 0);
	}
	return 0;
}

LPITEMIDLIST GetIDListFromPath(LPCTSTR lpszPath)
{ 
	LPITEMIDLIST    pidl = NULL;
	LPSHELLFOLDER   pDesktopFolder;
	OLECHAR         szOleChar[_MAX_PATH];
	ULONG           chEaten;
	ULONG           dwAttributes;
	HRESULT         hr;

	// Get a pointer to the Desktop 's IShellFolder interface.
	if (SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder)))   
	{
		// IShellFolder::ParseDisplayName requires the file name be in
		// Unicode.   

#if !defined(_UNICODE)
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszPath, -1, 
			szOleChar, MAX_PATH); 
#else
#if _MSC_VER >= 1400 // vs2005+
		_tcscpy_s(szOleChar, _MAX_PATH, lpszPath);
#else
		_tcscpy(szOleChar, lpszPath);
#endif
#endif

		// Convert the path to an ITEMIDLIST.
		hr = pDesktopFolder->ParseDisplayName(NULL, NULL, szOleChar, 
			&chEaten, &pidl, &dwAttributes);

		pDesktopFolder->Release();
		if (SUCCEEDED(hr))
			return pidl;
	}

	return NULL;
}

void CPageFileListCfg::OnBnClickedBtnAddDir()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_strWorkingDir.IsEmpty())
	{
		MessageBox(_T("请先设置工作目录！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	TCHAR szDir[MAX_PATH];

	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.iImage = 0;
	bi.pszDisplayName = NULL;

	bi.lParam = (long)(m_strWorkingDir.GetBuffer(0));
	// 初始化路径，形如(_T("c:\\Symbian"));
	m_strWorkingDir.ReleaseBuffer();
	bi.lpfn = BrowseCallbackProc;

	bi.lpszTitle = _T("请选择需要添加的目录:");

	bi.pidlRoot = GetIDListFromPath(m_strWorkingDir.GetBuffer(0));
	m_strWorkingDir.ReleaseBuffer();

	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);
	s_pButton->DestroyWindow();
	MG_SAFE_DELETE(s_pButton);
	if (SHGetPathFromIDList(pIDList, szDir))
	{
		AddDirectory(szDir, s_bCheck);
	}
	IMalloc *imalloc = NULL;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{
		imalloc->Free(pIDList);
		imalloc->Release();
	}
}

void CPageFileListCfg::OnBnClickedBtnDel()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		MessageBox(_T("请选择需要删除的条目！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CString strHint;
	strHint.Format(_T("确定要删除 %s 这项么？"), m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_FILENAME));
	if (MessageBox(strHint, _T("确认"), MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		return;
	}
	m_mapUniqueFile.erase(m_mapUniqueFile.find(m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_FILENAME)));
	m_ctrlListFiles.DeleteItem(iItem);
	CString str;
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		str.Format(_T("%d"), i + 1);
		m_ctrlListFiles.SetItemText(i, FL_COLUMN_NO, str);
	}
	if (m_ctrlListFiles.GetItemCount() == 0)
	{
		m_ctrlCheckExecute.EnableWindow(FALSE);
		m_ctrlCheckOverwrite.EnableWindow(FALSE);
		m_ctrlCheckRegister.EnableWindow(FALSE);
		m_ctrlCheckDecompress.EnableWindow(FALSE);
		m_ctrlCheckUpdateNotExist.EnableWindow(FALSE);
		m_ctrlCheckExecute.SetCheck(FALSE);
		m_ctrlCheckOverwrite.SetCheck(FALSE);
		m_ctrlCheckRegister.SetCheck(FALSE);
		m_ctrlCheckDecompress.SetCheck(FALSE);
		m_ctrlCheckUpdateNotExist.SetCheck(FALSE);
	}
}

void CPageFileListCfg::OnBnClickedBtnSavetofile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strProductName;
	GetDlgItem(IDC_EDIT_PRODUCTNAME)->GetWindowText(strProductName);
	if (strProductName.IsEmpty())
	{
		MessageBox(_T("请填写产品名称！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_PRODUCTNAME));
		return;
	}
	if (m_ctrlListFiles.GetItemCount() == 0)
	{
		MessageBox(_T("请添加至少一个文件！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CFileDialog dlgSaveFile(FALSE, 
		_T(".xml"), 
		_T("FileList.xml"), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("XML文件(*.xml)|*.xml|所有文件(*.*)|*.*||"));

	INT structsize = 0; 
	DWORD dwVersion, dwWindowsMajorVersion, dwWindowsMinorVersion; 

	dwVersion = GetVersion(); 
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion))); 
	dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion))); 

	if (dwVersion < 0x80000000)
	{
		structsize = 88;
		// 显示新的文件对话框 
	}
	else
	{
		structsize = 76;
		// 显示老的文件对话框 
	}

	dlgSaveFile.m_ofn.lStructSize = structsize; 
	dlgSaveFile.m_ofn.lpstrTitle = _T("选择文件保存路径"); 

	if (dlgSaveFile.DoModal() == IDOK)
	{
		CFile fileUpdateHint;
		if (fileUpdateHint.Open(dlgSaveFile.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			USES_CONVERSION;
			fileUpdateHint.Write(XML_FL_HEADER, (UINT)strlen(XML_FL_HEADER));

			CHAR szText[512];
			CString strUrl;
			GetDlgItem(IDC_EDIT_UPDATEURL)->GetWindowText(strUrl);
			CTime timeNow = CTime::GetCurrentTime();
#if  _MSC_VER >= 1400 // vs2005+
			sprintf_s(szText, 512, XML_FL_PRODUCT_HEADER, T2A(strProductName), T2A(timeNow.Format(_T("%Y-%m-%d  %H:%M:%S"))), m_ctrlListFiles.GetItemCount(), T2A(strUrl));
#else
			sprintf(szText, XML_FL_PRODUCT_HEADER, T2A(strProductName), T2A(timeNow.Format(_T("%Y-%m-%d  %H:%M:%S"))), m_ctrlListFiles.GetItemCount(), T2A(strUrl));
#endif
			fileUpdateHint.Write(szText, (UINT)strlen(szText));

			CHAR szXMLStr[1024];
			for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
			{
				fileUpdateHint.Write(XML_FL_FILE_HEADER, (UINT)strlen(XML_FL_FILE_HEADER));

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_DETFILENAME).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_FILENAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_DETFILENAME)));
#else
					sprintf(szXMLStr, XML_FL_NODE_FILENAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_DETFILENAME)));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));

					if (m_ctrlListFiles.GetItemText(i, FL_COLUMN_LANGID).CompareNoCase(LANGID_ALL_SUPPORT) != 0)
					{
						CString strLangID = m_ctrlListFiles.GetItemText(i, FL_COLUMN_LANGID);
#if  _MSC_VER >= 1400 // vs2005+
						sprintf_s(szXMLStr, 1024, XML_FL_NODE_LANGID, strLangID.CompareNoCase(LANGID_DEFAULT_SUPPORT) == 0 ? -2 : _ttoi(strLangID));
#else
						sprintf(szXMLStr, XML_FL_NODE_LANGID, strLangID.CompareNoCase(LANGID_DEFAULT_SUPPORT) == 0 ? -2 : _ttoi(strLangID));
#endif
						fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
					}

#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_ORIGINALNAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILENAME)));
#else
					sprintf(szXMLStr, XML_FL_NODE_ORIGINALNAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILENAME)));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}
				else
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_FILENAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILENAME)));
#else
					sprintf(szXMLStr, XML_FL_NODE_FILENAME, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILENAME)));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				CString str(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILELEN));
				str.Remove(_T(','));
#if  _MSC_VER >= 1400 // vs2005+
				sprintf_s(szXMLStr, 1024, XML_FL_NODE_FILELEN, T2A(str));
#else
				sprintf(szXMLStr, XML_FL_NODE_FILELEN, T2A(str));
#endif
				fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEVER).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_FILEVER, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEVER)));
#else
					sprintf(szXMLStr, XML_FL_NODE_FILEVER, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEVER)));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_EXECUTE).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_EXECUTE, T2A(_T("Y")));
#else
					sprintf(szXMLStr, XML_FL_NODE_EXECUTE, T2A(_T("Y")));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_OVERWRITE).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_OVERWRITE, T2A(_T("Y")));
#else
					sprintf(szXMLStr, XML_FL_NODE_OVERWRITE, T2A(_T("Y")));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_REGISTER).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_REGISTER, T2A(_T("Y")));
#else
					sprintf(szXMLStr, XML_FL_NODE_REGISTER, T2A(_T("Y")));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_DECOMPRESS).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_DECOMPRESS, T2A(_T("Y")));
#else
					sprintf(szXMLStr, XML_FL_NODE_DECOMPRESS, T2A(_T("Y")));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_UPDATENOTEXSIT).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_UPDATENOTEXIST, T2A(_T("Y")));
#else)
					sprintf(szXMLStr, XML_FL_NODE_UPDATENOTEXIST, T2A(_T("Y")));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				if (!m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEMD5).IsEmpty())
				{
#if  _MSC_VER >= 1400 // vs2005+
					sprintf_s(szXMLStr, 1024, XML_FL_NODE_FILEMD5, T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEMD5)));
#else
					sprintf(szXMLStr, XML_FL_NODE_FILEMD5,T2A(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILEMD5)));
#endif
					fileUpdateHint.Write(szXMLStr, (UINT)strlen(szXMLStr));
				}

				fileUpdateHint.Write(XML_FL_FILE_TAIL, (UINT)strlen(XML_FL_FILE_TAIL));
			}

			fileUpdateHint.Write(XML_FL_PRODUCT_TAIL, (UINT)strlen(XML_FL_PRODUCT_TAIL));
			fileUpdateHint.Write(XML_FL_TAIL, (UINT)strlen(XML_FL_TAIL));
			fileUpdateHint.Close();
			MessageBox(_T("保存成功！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
	}
}

void CPageFileListCfg::AddFile(LPCTSTR lpszFilePath)
{
	INT iItemCnt = m_ctrlListFiles.GetItemCount();
	CString strFileName(lpszFilePath);
	strFileName.Replace(m_strWorkingDir, _T(""));
	if (strFileName.Left(1) == _T("\\"))
	{
		strFileName.Delete(0, 1);
	}
	if (m_mapUniqueFile.find(strFileName) != m_mapUniqueFile.end())
	{
		return;
	}
	m_mapUniqueFile.insert(pair<CString, UINT>(strFileName, 0));
	CString str;
	str.Format(_T("%d"), iItemCnt + 1);
	m_ctrlListFiles.InsertItem(iItemCnt, _T(""));
	m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_NO, str);
	m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_FILENAME, strFileName);
	CFile file;
	if (file.Open(lpszFilePath, CFile::modeRead | CFile::shareDenyNone))
	{
		str.Format(_T("%llu"), file.GetLength());
		m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_FILELEN, CDigitGrouping::DigitGrouping(str));
		file.Close();
	}
	VS_FIXEDFILEINFO vsf;
	if (CFileInfoMgr::GetFileVersion(lpszFilePath, &vsf) == S_OK)
	{
		str.Format(_T("%u.%u.%u.%u"), 
		HIWORD(vsf.dwFileVersionMS), 
		LOWORD(vsf.dwFileVersionMS), 
		HIWORD(vsf.dwFileVersionLS), 
		LOWORD(vsf.dwFileVersionLS));
		m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_FILEVER, str);
	}
	TCHAR szFileMD5[_MAX_PATH];
	CHashMgr::GetFileHash(lpszFilePath, CALG_MD5, szFileMD5);
	m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_FILEMD5, szFileMD5);
	m_ctrlListFiles.SetItemText(iItemCnt, FL_COLUMN_LANGID, LANGID_ALL_SUPPORT);
}

void CPageFileListCfg::AddDirectory(LPCTSTR lpszDirPath, BOOL bRecursion /*= FALSE*/)
{
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	BOOL bFinished = FALSE;

	CString strDirPath(lpszDirPath);
	if (strDirPath.Right(1) != _T("\\"))
	{
		strDirPath += _T("\\");
	}

	hSearch = FindFirstFile(strDirPath + _T("*.*"), &fd);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			&& _tcscmp(fd.cFileName, _T("."))
			&& _tcscmp(fd.cFileName, _T("..")))
		{
			if (bRecursion)
			{
				AddDirectory(strDirPath + fd.cFileName, bRecursion);
			}
		}
		else if (_tcscmp(fd.cFileName, _T("."))
			&& _tcscmp(fd.cFileName, _T("..")))
		{
			AddFile(strDirPath + fd.cFileName);
		}
		while (!bFinished)
		{
			if (FindNextFile(hSearch, &fd))
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
					&& _tcscmp(fd.cFileName, _T("."))
					&& _tcscmp(fd.cFileName, _T("..")))
				{
					if (bRecursion)
					{
						AddDirectory(strDirPath + fd.cFileName, bRecursion);
					}
				}
				else if (_tcscmp(fd.cFileName, _T("."))
					&& _tcscmp(fd.cFileName, _T("..")))
				{
					AddFile(strDirPath + fd.cFileName);
				}
			}
			else
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)          //Normal Finished
				{
					bFinished = TRUE;
				}
				else
				{
					bFinished = TRUE;     //Terminate Search
				}
			}
		}
		FindClose(hSearch);
	}
}

void CPageFileListCfg::OnBnClickedBtnDelCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iCheckCnt = 0;
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		if (m_ctrlListFiles.GetCheck(i))
		{
			iCheckCnt++;
		}
	}
	if (iCheckCnt == 0)
	{
		MessageBox(_T("没有勾选任何项！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CString strHint;
	strHint.Format(_T("确认删除勾选的这 %d 项么？"), iCheckCnt);
	if (MessageBox(strHint, _T("确认"), MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		return;
	}
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); )
	{
		if (m_ctrlListFiles.GetCheck(i))
		{
			m_mapUniqueFile.erase(m_mapUniqueFile.find(m_ctrlListFiles.GetItemText(i, FL_COLUMN_FILENAME)));
			m_ctrlListFiles.DeleteItem(i);
		}
		else
		{
			i++;
		}
	}
	CString str;
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		str.Format(_T("%d"), i + 1);
		m_ctrlListFiles.SetItemText(i, FL_COLUMN_NO, str);
	}
	if (m_ctrlListFiles.GetItemCount() == 0)
	{
		m_ctrlCheckExecute.EnableWindow(FALSE);
		m_ctrlCheckOverwrite.EnableWindow(FALSE);
		m_ctrlCheckRegister.EnableWindow(FALSE);
		m_ctrlCheckDecompress.EnableWindow(FALSE);
		m_ctrlCheckUpdateNotExist.EnableWindow(FALSE);
		m_ctrlCheckExecute.SetCheck(FALSE);
		m_ctrlCheckOverwrite.SetCheck(FALSE);
		m_ctrlCheckRegister.SetCheck(FALSE);
		m_ctrlCheckDecompress.SetCheck(FALSE);
		m_ctrlCheckUpdateNotExist.SetCheck(FALSE);
		m_ctrlComboBoxLangList.EnableWindow(FALSE);
		m_ctrlComboBoxLangList.SetCurSel(0);
	}
}

void CPageFileListCfg::OnBnClickedBtnSelAll()
{
	// TODO: 在此添加控件通知处理程序代码
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		m_ctrlListFiles.SetCheck(i, TRUE);
	}
}

void CPageFileListCfg::OnBnClickedBtnUnselAll()
{
	// TODO: 在此添加控件通知处理程序代码
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		m_ctrlListFiles.SetCheck(i, FALSE);
	}
}

void CPageFileListCfg::OnBnClickedBtnReverseSel()
{
	// TODO: 在此添加控件通知处理程序代码
	for (INT i=0; i<m_ctrlListFiles.GetItemCount(); i++)
	{
		m_ctrlListFiles.SetCheck(i, !m_ctrlListFiles.GetCheck(i));
	}
}

void CPageFileListCfg::OnNMClickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR)
	{
		return;
	}
	m_ctrlCheckExecute.EnableWindow(TRUE);
	m_ctrlCheckOverwrite.EnableWindow(TRUE);
	m_ctrlCheckRegister.EnableWindow(TRUE);
	m_ctrlCheckDecompress.EnableWindow(TRUE);
	m_ctrlCheckUpdateNotExist.EnableWindow(TRUE);
	m_ctrlComboBoxLangList.EnableWindow(TRUE);
	m_ctrlCheckExecute.SetCheck(!m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_EXECUTE).IsEmpty());
	m_ctrlCheckOverwrite.SetCheck(!m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_OVERWRITE).IsEmpty());
	m_ctrlCheckRegister.SetCheck(!m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_REGISTER).IsEmpty());
	m_ctrlCheckDecompress.SetCheck(!m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_DECOMPRESS).IsEmpty());
	m_ctrlCheckUpdateNotExist.SetCheck(!m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_UPDATENOTEXSIT).IsEmpty());

	CString strLangSupport = m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_LANGID);
	GetDlgItem(IDC_EDIT_DETFILENAME)->EnableWindow(TRUE);
	m_ctrlComboBoxLangList.SetCurSel(m_ctrlComboBoxLangList.FindString(-1, strLangSupport));
	GetDlgItem(IDC_EDIT_DETFILENAME)->SetWindowText(m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_DETFILENAME));

	*pResult = 0;
}

void CPageFileListCfg::OnBnClickedCheckExec()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_EXECUTE, m_ctrlCheckExecute.GetCheck() ? _T("√") : _T(""));
}

void CPageFileListCfg::OnBnClickedCheckOverwrite()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_OVERWRITE, m_ctrlCheckOverwrite.GetCheck() ? _T("√") : _T(""));
	if (m_ctrlCheckOverwrite.GetCheck())
	{
		m_ctrlCheckUpdateNotExist.SetCheck(FALSE);
		m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_UPDATENOTEXSIT, _T(""));
	}
}

void CPageFileListCfg::OnBnClickedCheckReg()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_REGISTER, m_ctrlCheckRegister.GetCheck() ? _T("√") : _T(""));
}

void CPageFileListCfg::OnBnClickedCheckDecompress()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_DECOMPRESS, m_ctrlCheckDecompress.GetCheck() ? _T("√") : _T(""));
}

void CPageFileListCfg::OnBnClickedCheckUpdatenotexist()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_UPDATENOTEXSIT, m_ctrlCheckUpdateNotExist.GetCheck() ? _T("√") : _T(""));
	if (m_ctrlCheckUpdateNotExist.GetCheck())
	{
		m_ctrlCheckOverwrite.SetCheck(FALSE);
		m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_OVERWRITE, _T(""));
	}
}

void CPageFileListCfg::OnCbnSelchangeComboLanglist()
{
	// TODO: 在此添加控件通知处理程序代码
	INT iSel = m_ctrlComboBoxLangList.GetCurSel();
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	switch (iSel)
	{
	case LB_ERR:
		{
			GetDlgItem(IDC_EDIT_DETFILENAME)->SetWindowText(_T(""));
			GetDlgItem(IDC_EDIT_DETFILENAME)->EnableWindow(FALSE);
			m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_DETFILENAME, _T(""));
			break;
		}
	default:
		{
			GetDlgItem(IDC_EDIT_DETFILENAME)->EnableWindow(TRUE);
			CString strDetFileName = m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_DETFILENAME);
			if (strDetFileName.IsEmpty())
			{
				GetDlgItem(IDC_EDIT_DETFILENAME)->SetWindowText(m_ctrlListFiles.GetItemText(iItem, FL_COLUMN_FILENAME));
			}
			else
			{
				GetDlgItem(IDC_EDIT_DETFILENAME)->SetWindowText(strDetFileName);
			}
			break;
		}
	}
	CString strLangID;
	m_ctrlComboBoxLangList.GetLBText(iSel, strLangID);
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_LANGID, strLangID);
}

void CPageFileListCfg::OnEnChangeEditDetname()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	INT iItem = (INT)m_ctrlListFiles.GetFirstSelectedItemPosition() - 1;
	if (iItem == LB_ERR || iItem >= m_ctrlListFiles.GetItemCount())
	{
		return;
	}
	CString strDetFileName;
	GetDlgItem(IDC_EDIT_DETFILENAME)->GetWindowText(strDetFileName);
	m_ctrlListFiles.SetItemText(iItem, FL_COLUMN_DETFILENAME, strDetFileName);
}
