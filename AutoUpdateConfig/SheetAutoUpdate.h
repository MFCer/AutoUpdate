#pragma once

#include "PageFileListCfg.h"
#include "PageUpdateHintCfg.h"

// CSheetAutoUpdate

class CSheetAutoUpdate : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetAutoUpdate)

public:
	CSheetAutoUpdate(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetAutoUpdate(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CSheetAutoUpdate();

protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()

	CPageFileListCfg	m_pageFileListCfg;
	CPageUpdateHintCfg	m_pageUpdateHintCfg;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};


