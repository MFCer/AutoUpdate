#pragma once

/*!
	\class CIniFileOperations
	\brief 配置文件操作类
	\details 用于配置文件操作
*/
class CIniFileOperations
{
public:
	//! 构造函数
	CIniFileOperations(void);
	//! 析构函数
	~CIniFileOperations(void);

	/*!	\fn INT GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iDefault)
		\brief 获取整型值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] iDefault			若获取失败使用的默认值
		\retval INT					获取到的整型值
	*/
	INT GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iDefault);
	/*!	\fn void WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iIn)
		\brief 写入整型值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] iIn				待写入的值
	*/
	void WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, INT iIn);

	/*!	\fn CString GetIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpDefault)
		\brief 获取字符串值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] lpDefault		若获取失败使用的默认值
		\retval CString				获取到的字符串值
	*/
	CString GetIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpDefault);
	/*!	\fn void WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpString)
		\brief 写入字符串值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] lpString			待写入的字符串值
	*/
	void WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpString);

	/*!	\fn BOOL GetIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct)
		\brief 获取结构值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] lpStruct			用于存储的空间的指针
		\param[in] uSizeStruct		结构大小
		\retval BOOL				是否获取成功(成功返回\b TRUE, 失败返回\b FALSE)
	*/
	BOOL GetIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct);
	/*!	\fn void WriteIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct)
		\brief 写入结构值
		\param[in] lpszSection		段名
		\param[in] lpszKey			键名
		\param[in] lpStruct			存放待写入的结构空间的指针
		\param[in] uSizeStruct		结构大小
	*/
	void WriteIniStruct(LPCTSTR lpszSection, LPCTSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct);

protected:
	//! Ini配置文件路径
	CString m_strIniFilePath;
};

//! 全局配置文件操作对象
extern CIniFileOperations g_objIniFileOperations;
