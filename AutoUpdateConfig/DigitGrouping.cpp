#include "StdAfx.h"
#include "DigitGrouping.h"

CDigitGrouping::CDigitGrouping(void)
{
}

CDigitGrouping::~CDigitGrouping(void)
{
}

CString CDigitGrouping::DigitGrouping(LPCTSTR lpszString)
{
	CString strRet(lpszString);
	INT i = 4;
	while (strRet.GetLength() >= i)
	{
		strRet.Insert(strRet.GetLength() - i + 1, ',');
		i += 4;
	}
	return strRet;
}