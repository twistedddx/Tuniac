/*
 *  tuniacthread.h
 *  tuniac
 *
 *  Created by Tony Million on 08/12/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

typedef int (*threadfunction)(void * pStartData);

#ifdef _MSC_VER

#include <windows.h>

class CTuniacThread
{
protected:
	HANDLE			m_hThread;
	DWORD			m_dwThreadID;
	
	threadfunction	m_ourfunction;
	void *			m_pData;
	int				m_retValue;
	
	static DWORD __stdcall threadStub(void * pData)
	{
		CTuniacThread * pTThread = (CTuniacThread *)pData;
		pTThread->m_retValue = pTThread->m_ourfunction(pTThread->m_pData);
		return 0;
	}
	
public:
	bool StartThread(threadfunction func, void * pData)
	{
		// TODO call CreateThread here innit
		m_ourfunction	= func;
		m_pData			= pData;
		
		m_hThread = CreateThread(NULL, 0, threadStub, this, 0,  &m_dwThreadID);
		
		if(!m_hThread)
			return false;
		
		return true;
	}
	
	void Join(void)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
	}
	
	// only valid after Join() has returned
	int getReturnValue(void)
	{
		return m_retValue;
	}	
};


#else
#include <pthread.h>
					
// lets do POSIX THREADS (w00t)
class CTuniacThread
{
protected:
	pthread_t		theThread;
	
	threadfunction	m_ourfunction;
	void *			m_pData;
	int				m_retValue;
	
	static void * threadStub(void * pData)
	{
		CTuniacThread * pTThread = (CTuniacThread *)pData;
		pTThread->m_retValue = pTThread->m_ourfunction(pTThread->m_pData);
		return 0;
	}
	
public:
	bool StartThread(threadfunction func, void * pData)
	{
		m_ourfunction	= func;
		m_pData			= pData;
		if(pthread_create(&theThread, NULL, threadStub, this) != 0)
			return false;
		
		return true;
	}
	
	void Join(void)
	{
		pthread_join(theThread, NULL);
	}
	
	// only valid after Join() has returned
	int getReturnValue(void)
	{
		return m_retValue;
	}
};
#endif
