#pragma once

class CMutex
{
protected:
	HANDLE	m_hMutex;

public:
	CMutex()
	{
		m_hMutex = CreateMutex(NULL, FALSE, NULL);
	}

	~CMutex()
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

	bool AttemptLock(void)
	{
		if(WaitForSingleObject(m_hMutex, 0) == WAIT_OBJECT_0)
			return true;

		return false;
	}
};

class CAutoLock
{
private:
	CMutex *	m_Mutex;

public:

	CAutoLock( CMutex *CS )
	{
		m_Mutex = CS;
		m_Mutex->Lock();
	}

	~CAutoLock(void)
	{
		m_Mutex->Unlock();
	}
};
