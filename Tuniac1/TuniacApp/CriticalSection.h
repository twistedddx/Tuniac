#pragma once

class CCriticalSection
{
protected:
	HANDLE	m_hMutex;

public:
	CCriticalSection()
	{
		m_hMutex = CreateMutex(NULL, FALSE, NULL);
	}

	~CCriticalSection()
	{
		CloseHandle(m_hMutex);
	}

	void Lock(void)
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	void Unlock(void)
	{
		ReleaseMutex(m_hMutex);
	}
};

class CAutoLock
{
private:
	CCriticalSection *	m_CS;

public:

	CAutoLock( CCriticalSection *CS )
	{
		m_CS = CS;
		m_CS->Lock();
	}

	~CAutoLock(void)
	{
		m_CS->Unlock();
	}
};
