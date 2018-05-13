// AutoLock.h
//
#ifndef __AUTO_LOCK_H__
#define __AUTO_LOCK_H__

class CAutoLock
{
public:
    CAutoLock(CCriticalSection * pLock)
    {
        m_pLock = pLock;
        m_pLock->Lock();
    };

    ~CAutoLock()
	{
        m_pLock->Unlock();
    };

protected:
    CCriticalSection *m_pLock;
};

#endif