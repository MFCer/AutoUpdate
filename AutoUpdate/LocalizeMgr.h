#pragma once

#include <MsXml.h>

typedef enum E_LOCALIZEMGR_LANGID
{
	DEFAULT_SUPPORT = -2, 
	ALL_SUPPORT = -1, 
	CHINESE_SIMPLE, 
	CHINESE_TRADITIONAL, 
	ENGLISH, 
	TURKISH, 
	GERMAN, 
	FRENCH, 
	ITALIAN, 
	SPANISH, 
	JAPANESE
} E_LOCALIZEMGR_LANGID;

#define LOCALIZE_MGR_FILE_NAME			_T("\\Config\\Localize.xml")

#define LOCALIZE_MGR_DEFAULT_LANGID		ENGLISH

#define LOCALIZE_MGR_LANG_FMT			_T("Lang%d")
#define LOCALIZE_MGR_ITEM_FMT			_T("Item%d")

class CLocalizeMgr
{
public:
	CLocalizeMgr(void);
	~CLocalizeMgr(void);

	INT m_iLangID;

	// 从语言文件中，读取本地化字符串
	CString GetString(INT iLocalizeItemNo, CString strDefault = _T(""));

	// 获取系统语言ID
	void GetLocaleLangID();

	CString m_strFileName;

protected:
	BOOL GetString(CString &strGet, IXMLDOMNodeList* pDOMNodeList, INT iLocalizeItemNo);
};

extern CLocalizeMgr g_objLocalizeMgr;
