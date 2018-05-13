#include "StdAfx.h"
#include "LocalizeMgr.h"

CLocalizeMgr::CLocalizeMgr(void)
{
	m_iLangID = 0;

	TCHAR szPath	[_MAX_PATH];
	TCHAR szDrive	[_MAX_DRIVE];
	TCHAR szDir		[_MAX_DIR];
	TCHAR szFName	[_MAX_FNAME];
	TCHAR szExt		[_MAX_EXT];
	GetModuleFileName(NULL, szPath, _MAX_PATH);

#if  _MSC_VER >= 1400 // vs2005+
	_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
#else
	_tsplitpath(szPath, szDrive, szDir, szFName, szExt);
#endif

#if  _MSC_VER >= 1400 // vs2005+
	_tmakepath_s(szPath, _MAX_PATH, szDrive, szDir, LOCALIZE_MGR_FILE_NAME, _T(""));
#else
	_tmakepath(szPath, szDrive, szDir, LOCALIZE_MGR_FILE_NAME, _T(""));
#endif

	m_strFileName = szPath;
}

CLocalizeMgr::~CLocalizeMgr(void)
{
}

CString CLocalizeMgr::GetString(INT iLocalizeItemNo, CString strDefault/* = _T("")*/)
{
	IXMLDOMDocument* pDOMDocument = NULL;
	IXMLDOMElement*  pDOMElement = NULL;
	IXMLDOMNodeList* pDOMNodeList = NULL;
	CString strBaseName;
	strBaseName.Format(LOCALIZE_MGR_LANG_FMT, m_iLangID);
	BSTR bstrBaseName = ::SysAllocString(strBaseName);
	CComVariant vFile(m_strFileName);
	VARIANT_BOOL vBool;
	CString strGet;

	if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDOMDocument)))
	{	
		if (SUCCEEDED(pDOMDocument->put_async(VARIANT_FALSE)) && SUCCEEDED(pDOMDocument->load(vFile, &vBool)))
		{
			if (VARIANT_TRUE == vBool && SUCCEEDED(pDOMDocument->get_documentElement(&pDOMElement)))
			{
				if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
				{
					 if (!GetString(strGet, pDOMNodeList, iLocalizeItemNo))
					 {
						 strBaseName.Format(LOCALIZE_MGR_LANG_FMT, LOCALIZE_MGR_DEFAULT_LANGID);
						 BSTR bstrBaseName = ::SysAllocString(strBaseName);
						 if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
						 {
							 GetString(strGet, pDOMNodeList, iLocalizeItemNo);
						 }
					 }
				}
			}
		}
	}

	::SysFreeString(bstrBaseName);
	MG_SAFE_RELEASE(pDOMDocument);
	MG_SAFE_RELEASE(pDOMElement);
	MG_SAFE_RELEASE(pDOMNodeList);

	strGet.Replace(_T("\\r\\n"), _T("\r"));
	return strGet.IsEmpty() ? strDefault : strGet;
}

BOOL CLocalizeMgr::GetString(CString &strGet, IXMLDOMNodeList* pDOMNodeList, INT iLocalizeItemNo)
{
	LONG lLen = 0;
	CString strItemName;
	strItemName.Format(LOCALIZE_MGR_ITEM_FMT, iLocalizeItemNo);
	if (SUCCEEDED(pDOMNodeList->get_length(&lLen)))
	{
		if (lLen == 0)
		{
			return FALSE;
		}
		for (INT i=0; i<lLen; i++)
		{
			IXMLDOMNode* pNode = NULL;
			if (SUCCEEDED(pDOMNodeList->get_item(i, &pNode)))
			{
				IXMLDOMNode* pNext = NULL;
				IXMLDOMNode* pChild;
				pNode->get_firstChild(&pChild);

				USES_CONVERSION;
				while(pChild)
				{					
					TCHAR szPathName[_MAX_PATH];
					memset(szPathName, 0, sizeof(szPathName));
					BSTR bstrNodeName, bstrText;
					pChild->get_nodeName(&bstrNodeName);
					pChild->get_text(&bstrText);

					if (0 == _tcsicmp(bstrNodeName, strItemName))
					{
						strGet = bstrText;
					}

					::SysFreeString(bstrNodeName);
					::SysFreeString(bstrText);

					pChild->get_nextSibling(&pNext);
					pChild->Release();
					pChild = pNext;
				}

				MG_SAFE_RELEASE(pNode);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CLocalizeMgr::GetLocaleLangID()
{
	// ÷˜”Ô—‘ID
	LANGID wLangPID = PRIMARYLANGID(::GetSystemDefaultLangID());
	// ◊””Ô—‘ID
	WORD wLangSID = SUBLANGID(::GetSystemDefaultLangID());
	// …Ë÷√”Ô—‘ID
	switch (wLangPID) 
	{
	case LANG_CHINESE: 
		switch (wLangSID)
		{
		case SUBLANG_CHINESE_HONGKONG:
		case SUBLANG_CHINESE_MACAU:
		case SUBLANG_CHINESE_TRADITIONAL:
			m_iLangID = CHINESE_TRADITIONAL;
			break;

		case SUBLANG_CHINESE_SINGAPORE:
		case SUBLANG_CHINESE_SIMPLIFIED:
			m_iLangID = CHINESE_SIMPLE;
			break;

		default:
			m_iLangID = ENGLISH;
			break;
		}
		break;

	case LANG_ENGLISH: 
		m_iLangID = ENGLISH;
		break;

	case LANG_TURKISH:
		m_iLangID = TURKISH;
		break;

	case LANG_GERMAN:
		m_iLangID = GERMAN;
		break;

	case LANG_FRENCH:
		m_iLangID = FRENCH;
		break;

	case LANG_ITALIAN:
		m_iLangID = ITALIAN;
		break;

	case LANG_SPANISH:
		m_iLangID = SPANISH;
		break;

	case LANG_JAPANESE:
		m_iLangID = JAPANESE;
		break;

	default:
		m_iLangID = ENGLISH;
		break;
	}
}
