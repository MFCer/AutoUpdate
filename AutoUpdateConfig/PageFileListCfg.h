#pragma once
#include "afxeditbrowsectrl.h"

#define m_ctrlListFiles				(*(CMFCListCtrl *)GetDlgItem(IDC_LIST_FILES))
#define m_ctrlComboBoxLangList		(*((CComboBox *)GetDlgItem(IDC_COMBO_LANGLIST)))
#define m_ctrlCheckExecute			(*(CButton *)GetDlgItem(IDC_CHECK_EXEC))
#define m_ctrlCheckOverwrite		(*(CButton *)GetDlgItem(IDC_CHECK_OVERWRITE))
#define m_ctrlCheckRegister			(*(CButton *)GetDlgItem(IDC_CHECK_REG))
#define m_ctrlCheckDecompress		(*(CButton *)GetDlgItem(IDC_CHECK_DECOMPRESS))
#define m_ctrlCheckUpdateNotExist	(*(CButton *)GetDlgItem(IDC_CHECK_UPDATENOTEXIST))

// CPageFileListCfg 对话框

class CPageFileListCfg : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageFileListCfg)

public:
	CPageFileListCfg();
	virtual ~CPageFileListCfg();

// 对话框数据
	enum { IDD = IDD_PAGE_FILELISTCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CString m_strWorkingDir;

	map<CString, UINT> m_mapUniqueFile;

	void AddFile(LPCTSTR lpszFilePath);
	void AddDirectory(LPCTSTR lpszDirPath, BOOL bRecursion = FALSE);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

	CMFCEditBrowseCtrl m_ctrlWorkDir;

	afx_msg void OnEnChangeEditWorkDir();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnAddFile();
	afx_msg void OnBnClickedBtnAddDir();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnSavetofile();
	afx_msg void OnBnClickedBtnDelCheck();
	afx_msg void OnBnClickedBtnSelAll();
	afx_msg void OnBnClickedBtnUnselAll();
	afx_msg void OnBnClickedBtnReverseSel();
	afx_msg void OnBnClickedCheckExec();
	afx_msg void OnBnClickedCheckOverwrite();
	afx_msg void OnBnClickedCheckReg();
	afx_msg void OnBnClickedCheckDecompress();
	afx_msg void OnBnClickedCheckUpdatenotexist();
	afx_msg void OnNMClickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboLanglist();
	afx_msg void OnEnChangeEditDetname();
};
