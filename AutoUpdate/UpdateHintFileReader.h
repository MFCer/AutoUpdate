#pragma once

class CUpdateHintFileReader
{
public:
	CUpdateHintFileReader(void);
	CUpdateHintFileReader(LPCTSTR lpszFileName);
	virtual ~CUpdateHintFileReader(void);

	CString m_strProductName;
	CString m_strLatestFeatures;

protected:
	BOOL GetString(CString &strGet, IXMLDOMNodeList* pDOMNodeList);
};
