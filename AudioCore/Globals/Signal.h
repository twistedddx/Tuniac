#pragma once

// wrapper for HEVENT or platform specific
class CSignal
{
protected:
	HANDLE		m_hEvent;
	bool		m_bManualReset;
public:
	CSignal(bool bManualReset = false) : m_hEvent(NULL)
	{
		m_bManualReset = bManualReset;
		m_hEvent = CreateEvent(NULL, (BOOL)bManualReset, FALSE, NULL);
		if(!m_hEvent)
			throw(0);
	}

	~CSignal()
	{
		CloseHandle(m_hEvent);
	}

	void Set()
	{
		SetEvent(m_hEvent);
	}

	void Reset()
	{
		ResetEvent(m_hEvent);
	}

	bool IsSet(void)
	{
		DWORD ret; 
		if((ret = WaitForSingleObject(m_hEvent, 0)) == WAIT_OBJECT_0)
		{
			return true;
		}

		return false;
	}

	bool Wait()
	{
		DWORD ret; 
		if((ret = WaitForSingleObject(m_hEvent, INFINITE)) == WAIT_OBJECT_0)
		{
			return true;
		}

		return false;
	}

	bool WaitWithTimeout(unsigned long ulTimeout)
	{
		if(ulTimeout == -1)
			return Wait();

		DWORD ret;
		if((ret = WaitForSingleObject(m_hEvent, ulTimeout)) == WAIT_OBJECT_0)
		{
			return true;
		}

		return false;
	}
};