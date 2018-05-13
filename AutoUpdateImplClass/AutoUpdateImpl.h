#pragma once

#define UPDATE_WORKING_DIRECTORY		_T("AutoUpdate")
#define UPDATE_FILE_NAME				_T("AutoUpdate.exe")

#define UPDATE_DEFAULT_URL				_T("http://localhost/")

typedef enum E_AUI_LANGID
{
	AUI_CHINESE_SIMPLE = 0, 
	AUI_CHINESE_TRADITIONAL, 
	AUI_ENGLISH, 
	AUI_TURKISH, 
	AUI_GERMAN, 
	AUI_FRENCH, 
	AUI_ITALIAN, 
	AUI_SPANISH, 
	AUI_JAPANESE
} E_AUI_LANGID;

class CAutoUpdateImpl
{
public:
	CAutoUpdateImpl(void);
	virtual ~CAutoUpdateImpl(void);

	// 执行文件（64位兼容）
	BOOL CreateProcess(LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

	// 获取系统语言ID
	INT GetLocaleLangID();

	void RefreshTrayIcons();

	HANDLE m_hProcess;
};
