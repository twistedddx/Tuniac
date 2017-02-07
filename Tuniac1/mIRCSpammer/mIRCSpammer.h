#pragma once

#include "ITuniacPlugin.h"


//#define WM_MCOMMAND WM_USER + 200

class CmIRCSpammer :
	public ITuniacPlugin
{
protected:
	bool					m_bThreadActive;
	HANDLE					m_hThread;
	DWORD					m_dwThreadId;

	ITuniacPluginHelper *	m_pHelper;

	static unsigned long __stdcall	ThreadStub(void * in);
	unsigned long			ThreadProc(void);

public:
	CmIRCSpammer();
	~CmIRCSpammer();

	void				Destroy(void);

	LPTSTR				GetPluginName(void);
	unsigned long		GetFlags(void);

	bool				SetHelper(ITuniacPluginHelper *pHelper);
	
	HANDLE				CreateThread(LPDWORD lpThreadId);
	HWND				GetPluginWindow(void);

	bool				About(HWND hWndParent);
	bool				Configure(HWND hWndParent);
};
