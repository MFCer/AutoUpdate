// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
{
	m_dwThreadID = 0;
	m_hThread = NULL;
	m_evStop = CreateEvent(NULL, true, true, NULL);
	SetEvent(m_evStop);
}

CThread::~CThread()
{
	CloseHandle(m_hThread);
	CloseHandle(m_evStop);
}

bool CThread::StartThread()
{
	// 如果线程已经创建，则不需要再创建
	if (!m_hThread)
	{ 
		// 创建并启动线程函数
		m_hThread = CreateThread(
					NULL,			// the handle cannot be inherited. 
                    0,				// the default Thread Stack Size is set
                    InitThreadProc,	// 线程函数
                    this,			// 线程函数的参数
                    0,				// 使线程函数创建完后立即启动
                    &m_dwThreadID	// receives the thread identifier
					);
	}// end if (!m_hThread...

	if (m_hThread)
	{
		ResetEvent(m_evStop);
	}

	return m_hThread != NULL;
}

void CThread::WaitForStop()
{
	WaitForSingleObject(m_evStop, INFINITE);

	// 返回线程句柄
	HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
	if (hThread)
	{
		// 等待线程终止
		WaitForSingleObject(hThread, INFINITE);
		// 关闭线程句柄
		CloseHandle(hThread);
	}// end if (hThread...
}

unsigned long  CThread::ThreadProc()
{
	ThreadProcMain();
//	TRACE("m_evStop m_evStop m_evStop\n");
	SetEvent(m_evStop);

	return 0;
}