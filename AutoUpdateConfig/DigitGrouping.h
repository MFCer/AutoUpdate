#pragma once

/*!
	\class CDigitGrouping
	\brief 数字分组类
	\details 用于将数字字符串分组
*/
class CDigitGrouping
{
public:
	//! 构造函数
	CDigitGrouping(void);
	//! 析构函数
	~CDigitGrouping(void);

	/*!	\fn CString DigitGrouping(LPCTSTR lpszString)
		\brief 数字分组函数
		\details 用于数字分组 例: \a lpszString = _T("1234567890"), 分组后\a lpszString = _T("1,234,567,890").
		\param[in] lpszString	传入字符串
		\retval	CString	返回分组后字符串
	*/
	static CString DigitGrouping(LPCTSTR lpszString);
};