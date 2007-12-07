#pragma once

#include "ITuniacPlugin.h"
#include "shlwapi.h"

//#define WM_MCOMMAND WM_USER + 200

class CNowPlayingTxt :
	public ITuniacPlugin
{
protected:
	bool					m_bThreadActive;
	HANDLE					m_hThread;
	DWORD					m_dwThreadId;

	ATOM					m_aCopy;

	ITuniacPluginHelper *	m_pHelper;

	static unsigned long __stdcall	ThreadStub(void * in);
	unsigned long			ThreadProc(void);

public:
	CNowPlayingTxt();
	~CNowPlayingTxt();

	void				Destroy(void);

	LPTSTR				GetPluginName(void);
	unsigned long		GetFlags(void);

	bool				SetHelper(ITuniacPluginHelper *pHelper);
	
	HANDLE				CreateThread(LPDWORD lpThreadId);
	HWND				GetMainWindow(void);

	bool				About(HWND hWndParent);
	bool				Configure(HWND hWndParent);
};
