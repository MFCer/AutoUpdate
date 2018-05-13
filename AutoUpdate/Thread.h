// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MG_THREAD_H__
#define __MG_THREAD_H__

class CThread
{
public:
	CThread();
	virtual ~CThread();

public:
	bool StartThread(void);
	void WaitForStop(void);

	static DWORD WINAPI InitThreadProc(PVOID pObj)
	{
		return	((CThread *)pObj)->ThreadProc();
	}
	
	unsigned long ThreadProc(void);

protected:
	virtual void ThreadProcMain(void) = 0;

protected:
	DWORD	m_dwThreadID;		// 线程标识
	HANDLE	m_hThread;			// 线程句柄
	HANDLE	m_evStop;
};

#endif
