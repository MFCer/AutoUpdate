#include "StdAfx.h"
#include "HashMgr.h"

CHashMgr::CHashMgr(void)
{
}

CHashMgr::~CHashMgr(void)
{
}

// 计算Hash，成功返回0，失败返回GetLastError()
//   CONST BYTE *pbData, // 输入数据 
//   DWORD dwDataLen,     // 输入数据字节长度 
//   ALG_ID algId              // Hash 算法：CALG_MD5,CALG_SHA
//   LPTSTR pszHash,       // 输出16进制Hash字符串，MD5长度为32+1, SHA长度为40+1
// 
DWORD CHashMgr::GetBinaryDataHash(CONST BYTE *pbData, DWORD dwDataLen, ALG_ID algId, LPTSTR pszHash)
{
	DWORD dwReturn = 0;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		return (dwReturn = GetLastError());
	}

	HCRYPTHASH hHash;
	// Alg Id:CALG_MD5,CALG_SHA
	if (!CryptCreateHash(hProv, algId, 0, 0, &hHash)) 
	{
		dwReturn = GetLastError();
		CryptReleaseContext(hProv, 0);
		return dwReturn;
	}

	if (!CryptHashData(hHash, pbData, dwDataLen, 0))
	{
		dwReturn = GetLastError();
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return dwReturn;
	}

	DWORD dwSize;
	DWORD dwLen = sizeof(dwSize);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

	BYTE* pHash = new BYTE[dwSize];
	dwLen = dwSize;
	CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

	lstrcpy(pszHash, _T(""));
	TCHAR szTemp[3];
	for (DWORD i=0; i<dwLen; ++i)
	{
#if _MSC_VER >= 1400 // vs2005+
		_stprintf_s(szTemp, 3, _T("%02X"), pHash[i]);
#else
		_stprintf(szTemp, _T("%02X"), pHash[i]);
#endif
		lstrcat(pszHash, szTemp);
	}
	MG_SAFE_DELETEA(pHash);

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return dwReturn;
}

DWORD CHashMgr::GetFileHash(LPCTSTR lpszFilePath, ALG_ID algId, LPTSTR pszHash)
{
	DWORD dwReturn = 0;
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		return (dwReturn = GetLastError());
	}

	HCRYPTHASH hHash;
	// Alg Id:CALG_MD5,CALG_SHA
	if (!CryptCreateHash(hProv, algId, 0, 0, &hHash)) 
	{
		dwReturn = GetLastError();
		CryptReleaseContext(hProv, 0);
		return dwReturn;
	}

	CFile fileOpen(lpszFilePath, CFile::modeRead);
	TCHAR szBuffer[_MAX_PATH];
	ULONGLONG ullFileLen = fileOpen.GetLength();
	// 以每_MAX_PATH字节读入，传入CryptProvider容器
	while (ullFileLen / _MAX_PATH)
	{
		fileOpen.Read(szBuffer, _MAX_PATH);
		if (!CryptHashData(hHash, (CONST BYTE *)szBuffer, _MAX_PATH, 0))
		{
			fileOpen.Close();
			dwReturn = GetLastError();
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			return dwReturn;
		}
		ullFileLen -= _MAX_PATH;
	}
	// 尾部字节读入，传入CryptProvider容器
	if (ullFileLen)
	{
		fileOpen.Read(szBuffer, (UINT)ullFileLen);
		if (!CryptHashData(hHash, (CONST BYTE *)szBuffer, (UINT)ullFileLen, 0))
		{
			fileOpen.Close();
			dwReturn = GetLastError();
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			return dwReturn;
		}
	}
	fileOpen.Close();

	DWORD dwSize;
	DWORD dwLen = sizeof(dwSize);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

	BYTE* pHash = new BYTE[dwSize];
	dwLen = dwSize;
	CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

	lstrcpy(pszHash, _T(""));
	TCHAR szTemp[3];
	for (DWORD i=0; i<dwLen; ++i)
	{
#if _MSC_VER >= 1400 // vs2005+
		_stprintf_s(szTemp, 3, _T("%02X"), pHash[i]);
#else
		_stprintf(szTemp, _T("%02X"), pHash[i]);
#endif
		lstrcat(pszHash, szTemp);
	}
	MG_SAFE_DELETEA(pHash);

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return dwReturn;
}
