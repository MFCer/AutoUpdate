#include "StdAfx.h"
#include "UpdateHintFileReader.h"

CUpdateHintFileReader::CUpdateHintFileReader(void)
{
}

CUpdateHintFileReader::CUpdateHintFileReader(LPCTSTR lpszFileName)
{
	if (g_objGlobalData.FileExist(lpszFileName))
	{
		IXMLDOMDocument* pDOMDocument = NULL;
		IXMLDOMElement*  pDOMElement = NULL;
		IXMLDOMNodeList* pDOMNodeList = NULL;
		BSTR bstrBaseName = ::SysAllocString(_T("Product"));
		CComVariant vFile(lpszFileName);
		VARIANT_BOOL vBool;

		// 获取URL属性
		if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDOMDocument)))
		{	
			if (SUCCEEDED(pDOMDocument->put_async(VARIANT_FALSE)) && SUCCEEDED(pDOMDocument->load(vFile, &vBool)))
			{
				if (VARIANT_TRUE == vBool && SUCCEEDED(pDOMDocument->get_documentElement(&pDOMElement)))
				{
					if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
					{
						long lLen = 0;
						if (SUCCEEDED(pDOMNodeList->get_length(&lLen)))
						{
							if (lLen != 1)
							{
								::SysFreeString(bstrBaseName);
								MG_SAFE_RELEASE(pDOMDocument);
								MG_SAFE_RELEASE(pDOMElement);
								MG_SAFE_RELEASE(pDOMNodeList);
								return;
							}
							IXMLDOMNode* pNode = NULL;
							if (SUCCEEDED(pDOMNodeList->get_item(0, &pNode)))
							{
								BSTR bstrNodeName = ::SysAllocString(_T("productname"));
								BSTR bstrText;

								IXMLDOMNamedNodeMap *pAttrMap = NULL;
								pNode->get_attributes(&pAttrMap);

								IXMLDOMNode *pAttrNode;
								pAttrMap->getNamedItem(bstrNodeName, &pAttrNode);
								pAttrNode->get_text(&bstrText);
								m_strProductName = bstrText;

								::SysFreeString(bstrNodeName);
								::SysFreeString(bstrText);

								MG_SAFE_RELEASE(pAttrMap);
								MG_SAFE_RELEASE(pAttrNode);
								MG_SAFE_RELEASE(pNode);
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

		CString strBaseName;
		strBaseName.Format(LOCALIZE_MGR_LANG_FMT, g_objLocalizeMgr.m_iLangID);
		// 获取描述
		bstrBaseName = ::SysAllocString(strBaseName);
		bool bRet = false;

		if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDOMDocument)))
		{	
			if (SUCCEEDED(pDOMDocument->put_async(VARIANT_FALSE)) && SUCCEEDED(pDOMDocument->load(vFile, &vBool)))
			{
				if (VARIANT_TRUE == vBool && SUCCEEDED(pDOMDocument->get_documentElement(&pDOMElement)))
				{
					if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
					{
						if (!GetString(m_strLatestFeatures, pDOMNodeList))
						{
							strBaseName.Format(LOCALIZE_MGR_LANG_FMT, LOCALIZE_MGR_DEFAULT_LANGID);
							BSTR bstrBaseName = ::SysAllocString(strBaseName);
							if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
							{
								GetString(m_strLatestFeatures, pDOMNodeList);
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

		m_strLatestFeatures.Replace(_T("\\r\\n"), _T("\r"));

		CGlobal::ForceDeleteFile(lpszFileName);
	}
}

CUpdateHintFileReader::~CUpdateHintFileReader(void)
{
}

BOOL CUpdateHintFileReader::GetString(CString &strGet, IXMLDOMNodeList* pDOMNodeList)
{
	LONG lLen = 0;
	if (SUCCEEDED(pDOMNodeList->get_length(&lLen)))
	{
		if (lLen != 1)
		{
			return FALSE;
		}
		IXMLDOMNode* pNode = NULL;
		if (SUCCEEDED(pDOMNodeList->get_item(0, &pNode)))
		{
			BSTR bstrText;
			pNode->get_text(&bstrText);

			strGet = bstrText;
			::SysFreeString(bstrText);

			MG_SAFE_RELEASE(pNode);
		}
		return TRUE;
	}
	return FALSE;
}
