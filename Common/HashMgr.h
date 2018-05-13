#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <wincrypt.h>

class CHashMgr
{
public:
	CHashMgr(void);
	~CHashMgr(void);

	static DWORD GetBinaryDataHash(CONST BYTE *pbData, DWORD dwDataLen, ALG_ID algId, LPTSTR pszHash);
	static DWORD GetFileHash(LPCTSTR lpszFilePath, ALG_ID algId, LPTSTR pszHash);
};
