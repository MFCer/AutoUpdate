#include "StdAfx.h"
#include "UpdateFiles.h"

CUpdateFiles::CUpdateFiles(void)
{
	::GetWindowsDirectory(m_szWindowsDir, sizeof(m_szWindowsDir));
	::GetSystemDirectory(m_szSystemDir, sizeof(m_szSystemDir));
}

CUpdateFiles::~CUpdateFiles(void)
{
	ClearAllList();
}

void CUpdateFiles::ClearAllList(void)
{
	CAutoLock  lock(&m_lock);

	while (m_listFile.size() > 0)
	{
		list<CUpdateFile*>::iterator it = m_listFile.begin();
		CUpdateFile* pUpdateFile = *it;
		m_listFile.erase(it);
		MG_SAFE_DELETE(pUpdateFile);
	}
}

bool CUpdateFiles::ReadFromXMLFile(LPCTSTR lpszFileName)
{
	ClearAllList();

	CAutoLock  lock(&m_lock);

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
							return false;
						}
						IXMLDOMNode* pNode = NULL;
						if (SUCCEEDED(pDOMNodeList->get_item(0, &pNode)))
						{
							BSTR bstrNodeName = ::SysAllocString(_T("url"));
							BSTR bstrText;

							IXMLDOMNamedNodeMap *pAttrMap = NULL;
							pNode->get_attributes(&pAttrMap);

							IXMLDOMNode *pAttrNode;
							pAttrMap->getNamedItem(bstrNodeName, &pAttrNode);
							pAttrNode->get_text(&bstrText);
							m_strBaseURL = bstrText;

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

	// 获取文件列表
	bstrBaseName = ::SysAllocString(_T("F"));
	bool bRet = false;

	if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDOMDocument)))
	{	
		if (SUCCEEDED(pDOMDocument->put_async(VARIANT_FALSE)) && SUCCEEDED(pDOMDocument->load(vFile, &vBool)))
		{
			if (VARIANT_TRUE == vBool && SUCCEEDED(pDOMDocument->get_documentElement(&pDOMElement)))
			{
				if (SUCCEEDED(pDOMElement->getElementsByTagName(bstrBaseName, &pDOMNodeList)))
				{
					bRet = GetFileList(pDOMNodeList);
				}
			}
		}
	}

	::SysFreeString(bstrBaseName);
	MG_SAFE_RELEASE(pDOMDocument);
	MG_SAFE_RELEASE(pDOMElement);
	MG_SAFE_RELEASE(pDOMNodeList);

	return bRet;
}

bool CUpdateFiles::GetFileList(IXMLDOMNodeList* pDOMNodeList)
{
	long lLen = 0;
	if (SUCCEEDED(pDOMNodeList->get_length(&lLen)))
	{
		for (int i=0; i<lLen; i++)
		{
			IXMLDOMNode* pNode = NULL;
			if (SUCCEEDED(pDOMNodeList->get_item(i, &pNode)))
			{
				CUpdateFile* pUpdateFile = new CUpdateFile(_T(""));
				if (!pUpdateFile)
				{
					return false;
				}

				IXMLDOMNode* pNext = NULL;
				IXMLDOMNode* pChild;
				pNode->get_firstChild(&pChild);

				CString strOriginalName;

				USES_CONVERSION;
				while (pChild)
				{					
					TCHAR szPathName[_MAX_PATH];
					memset(szPathName, 0, sizeof(szPathName));
					BSTR bstrNodeName, bstrText;
					pChild->get_nodeName(&bstrNodeName);
					pChild->get_text(&bstrText);

					if (0 == _tcsicmp(bstrNodeName, _T("N")))
					{
						pUpdateFile->m_strFileName = bstrText;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("V")))
					{
						pUpdateFile->m_strFileVersion = bstrText;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("V")))
					{
						pUpdateFile->m_strProductVersion = bstrText;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("L")))
					{
						pUpdateFile->m_ullSize = _ttoi(bstrText);
					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("InstallDir")))
// 					{
// 						pUpdateFile->m_strInstallDir.SetString(bstrText);
// 					}
					else if (0 == _tcsicmp(bstrNodeName, _T("R")))
					{
						pUpdateFile->m_bRegister = (0 == _tcsicmp(bstrText, _T("Y"))) ? TRUE : FALSE;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("D")))
					{
						pUpdateFile->m_bZipFile = (0 == _tcsicmp(bstrText, _T("Y"))) ? TRUE : FALSE;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("O")))
					{
						pUpdateFile->m_bForceOverwrite = (0 == _tcsicmp(bstrText, _T("Y"))) ? TRUE : FALSE;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("E")))
					{
						pUpdateFile->m_bExecute = (0 == _tcsicmp(bstrText, _T("Y"))) ? TRUE : FALSE;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("U")))
					{
						pUpdateFile->m_bUpdateNotExist = (0 == _tcsicmp(bstrText, _T("Y"))) ? TRUE : FALSE;
					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("ExecutePar")))
// 					{
// 						pUpdateFile->m_strExecParam = bstrText;
// 					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("LocalPath")))
// 					{
// 						pUpdateFile->m_strLocalPath = bstrText;
// 					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("URL")))
// 					{
// 						pUpdateFile->m_strURL = bstrText;
// 					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("UserName")))
// 					{
// 						pUpdateFile->m_strUserName = bstrText;;
// 					}
// 					else if (0 == _tcsicmp(bstrNodeName, _T("Password")))
// 					{
// 						pUpdateFile->m_strPassword = bstrText;
// 					}
					else if (0 == _tcsicmp(bstrNodeName, _T("M")))
					{
						pUpdateFile->m_strMD5 = bstrText;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("ON")))
					{
						strOriginalName = bstrText;
					}
					else if (0 == _tcsicmp(bstrNodeName, _T("I")))
					{
						pUpdateFile->m_iLangID = _ttoi(bstrText);
					}

					::SysFreeString(bstrNodeName);
					::SysFreeString(bstrText);

					pChild->get_nextSibling(&pNext);
					pChild->Release();
					pChild = pNext;
				}

// 				if (0 == pUpdateFile->m_strInstallDir.CollateNoCase(_T("WINDOWSDIR")))
// 				{
// 					pUpdateFile->m_strFilePath.Format(_T("%s\\%s"),m_szWindowsDir, pUpdateFile->m_strFileName);
// 				}
// 				else if (0 == pUpdateFile->m_strInstallDir.CollateNoCase(_T("SYSTEMDIR")))
// 				{
// 					pUpdateFile->m_strFilePath.Format(_T("%s\\%s"),m_szSystemDir, pUpdateFile->m_strFileName);
// 				}
// 				else
// 				{
					pUpdateFile->m_strFilePath = g_objGlobalData.g_strUpdateDir + WORKING_DIRECTORY_DOWNLOAD + pUpdateFile->m_strFileName;
//				}

				// 本地文件和服务器文件文件名
				// 相同的
				if (strOriginalName.IsEmpty())
				{
					pUpdateFile->m_strURL = m_strBaseURL + _T("/") + pUpdateFile->m_strFileName;
					pUpdateFile->m_strURL.Replace(_T('\\'), _T('/'));

					m_listFile.push_back(pUpdateFile);
//					TRACE("新文件对象添加：%s, 对象总数：%u\n", T2A(pUpdateFile->m_strFileName), m_listFile.size());
				}
				// 不同的
				else
				{
					// 默认语言处理
					CAutoLock  lock(&m_lock);

					// 先查找，若存在，修改，不存在，添加。
					list<CUpdateFile*>::iterator it = m_listFile.begin();
					while (it != m_listFile.end())
					{
						if ((*it)->m_strFilePath.CompareNoCase(pUpdateFile->m_strFilePath) == 0)
						{
							switch ((*it)->m_iLangID)
							{
							case DEFAULT_SUPPORT:
								{
									switch (pUpdateFile->m_iLangID)
									{
									case ALL_SUPPORT:
										// 不变
										break;
									case DEFAULT_SUPPORT:
										// 重复，不变
										break;
									default:
										{
											if (g_objLocalizeMgr.m_iLangID == pUpdateFile->m_iLangID)
											{
												(*it)->m_iLangID = pUpdateFile->m_iLangID;
												(*it)->m_strURL = m_strBaseURL + _T("/") + strOriginalName;
												(*it)->m_strURL.Replace(_T('\\'), _T('/'));
											}
										}
									}
								}
								break;
							case ALL_SUPPORT:
								{
									switch (pUpdateFile->m_iLangID)
									{
									case ALL_SUPPORT:
										// 重复，不变
										break;
									case DEFAULT_SUPPORT:
										{
											(*it)->m_iLangID = pUpdateFile->m_iLangID;
											(*it)->m_strURL = m_strBaseURL + _T("/") + strOriginalName;
											(*it)->m_strURL.Replace(_T('\\'), _T('/'));
										}
										break;
									default:
										{
											if (g_objLocalizeMgr.m_iLangID == pUpdateFile->m_iLangID)
											{
												(*it)->m_iLangID = pUpdateFile->m_iLangID;
												(*it)->m_strURL = m_strBaseURL + _T("/") + strOriginalName;
												(*it)->m_strURL.Replace(_T('\\'), _T('/'));
											}
										}
									}
								}
								break;
							default:
								switch (pUpdateFile->m_iLangID)
								{
								case ALL_SUPPORT:
									// 不变
									break;
								case DEFAULT_SUPPORT:
									// 不变
									break;
								default:
									// 不变
									break;
								}
								break;
							}
							MG_SAFE_DELETE(pUpdateFile);
							break;
						}
						it++;
					}
					if (it == m_listFile.end())
					{
						if (g_objLocalizeMgr.m_iLangID == pUpdateFile->m_iLangID || pUpdateFile->m_iLangID == ALL_SUPPORT || pUpdateFile->m_iLangID == DEFAULT_SUPPORT)
						{
							pUpdateFile->m_strURL = m_strBaseURL + _T("/") + strOriginalName;
							pUpdateFile->m_strURL.Replace(_T('\\'), _T('/'));

							m_listFile.push_back(pUpdateFile);
//							TRACE("新文件对象添加：%s, 对象总数：%u\n", T2A(pUpdateFile->m_strFileName), m_listFile.size());
						}
						else
						{
							MG_SAFE_DELETE(pUpdateFile);
						}
					}
				}
				MG_SAFE_RELEASE(pNode);
			}
		}
		return true;
	}
	return false;
}

bool CUpdateFiles::AddUpdateFile(LPCTSTR lpszFileName)
{
	CAutoLock  lock(&m_lock);

	// 删除重复
	list<CUpdateFile*>::iterator it = m_listFile.begin();
	while (it != m_listFile.end())
	{
		CUpdateFile* pUpdateFile = *it;
		if (pUpdateFile)
		{
			if (pUpdateFile->m_strFilePath.CompareNoCase(lpszFileName) == 0)
			{
				it = m_listFile.erase(it);
				MG_SAFE_DELETE(pUpdateFile)
				continue;
			}
		}
		it++;
	}

	CUpdateFile* pUpdateFile = new CUpdateFile(lpszFileName);
	if (pUpdateFile)
	{
		if (pUpdateFile->InitUpdateFile())
		{
			m_listFile.push_back(pUpdateFile);
			return true;
		}
		else
		{
			MG_SAFE_DELETE(pUpdateFile);
		}
	}

	return false;
}

void CUpdateFiles::DeleteItem(CUpdateFile * pUpdateFile)
{
	CAutoLock  lock(&m_lock);
	m_listFile.remove(pUpdateFile);
	MG_SAFE_DELETE(pUpdateFile);
}

CUpdateFile* CUpdateFiles::PopUpdateFile(void)
{
	CAutoLock  lock(&m_lock);
	CUpdateFile* pUpdateFile = NULL;
	if (m_listFile.size() > 0)
	{
		pUpdateFile = m_listFile.front();
		m_listFile.pop_front();
	}
	return pUpdateFile;
}

void CUpdateFiles::PushUpdateFile(CUpdateFile* pUpdateFile)
{
	CAutoLock  lock(&m_lock);
	m_listFile.push_back(pUpdateFile);
}

DWORD CUpdateFiles::CheckNeedDownload(BOOL &bHasDownloadedFile)
{
	bHasDownloadedFile = FALSE;
	CAutoLock  lock(&m_lock);
	DWORD dwUpdateCount = 0;
	list<CUpdateFile*>::iterator it = m_listFile.begin();
	while (it != m_listFile.end())
	{
		CUpdateFile* pUpdateFile = *it;
		if (pUpdateFile)
		{
			BOOL bFlag;
			if (pUpdateFile->CheckNeedDownload(bFlag))
			{
				CGlobal::OutputDebugString(_T("%s:%.2fKB"), pUpdateFile->m_strFileName, pUpdateFile->m_ullSize / 1024.);
				dwUpdateCount++;
			}
			else
			{
				bHasDownloadedFile += bFlag;
				// delete it
				it = m_listFile.erase(it);
				MG_SAFE_DELETE(pUpdateFile);
				continue;
			}
		}
		it++;
	}

	return dwUpdateCount;
}

DWORD CUpdateFiles::CheckNeedUpdate(void)
{
	CAutoLock  lock(&m_lock);
	DWORD dwUpdateCount = 0;
	list<CUpdateFile*>::iterator it = m_listFile.begin();
	while (it != m_listFile.end())
	{
		CUpdateFile* pUpdateFile = *it;
		if (pUpdateFile)
		{
			if (pUpdateFile->CheckNeedUpdate())
			{
				dwUpdateCount++;
			}
			else
			{
				// delete it
				it = m_listFile.erase(it);
				MG_SAFE_DELETE(pUpdateFile);
				continue;
			}
		}
		it++;
	}

	return dwUpdateCount;
}

// 调用此函数前请先调用 CheckNeedDownload() 以初始化 pUpdateFile->m_bLocalNeedUpdate
DWORD CUpdateFiles::GetNeedDownloadTotalLength(void)
{
	CAutoLock  lock(&m_lock);
	DWORD dwSize = 0;
	list<CUpdateFile*>::iterator it = m_listFile.begin();
	while (it != m_listFile.end())
	{
		CUpdateFile* pUpdateFile = *it;
		if (pUpdateFile && pUpdateFile->m_bLocalNeedUpdate)
		{
			dwSize += (DWORD)pUpdateFile->m_ullSize;
		}
		it++;
	}

	return dwSize;
}

DWORD CUpdateFiles::Count(void)
{
	CAutoLock  lock(&m_lock);

	return (DWORD)m_listFile.size();
}

DWORD CUpdateFiles::GetExecuteFileList(list<CFileToExecute> &lstFileToExecute)
{
	CAutoLock  lock(&m_lock);
	DWORD dwSize = 0;
	list<CUpdateFile*>::iterator it = m_listFile.begin();
	while (it != m_listFile.end())
	{
		CUpdateFile* pUpdateFile = *it;
		if (pUpdateFile && pUpdateFile->m_bExecute)
		{
			lstFileToExecute.push_back(CFileToExecute(pUpdateFile));
			dwSize ++;
		}
		it++;
	}

	return dwSize;
}
