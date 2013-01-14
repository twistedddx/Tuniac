#pragma once

#include "ITuniacPlugin.h"

#include <windows.h>
#include <Shellapi.h>
#include <shlwapi.h>

#define DllExport __declspec(dllexport)

extern "C"
{

	DllExport BOOL SetMMShellHook(HWND hWnd	);
	DllExport BOOL UnSetMMShellHook(HWND hWnd);
}

class CMMShellHook :
	public ITuniacPlugin
{
protected:
	bool					m_bThreadActive;
	HANDLE					m_hThread;

	ITuniacPluginHelper *	m_pHelper;

	static unsigned long __stdcall	ThreadStub(void * in);
	unsigned long			ThreadProc(void);

public:
	CMMShellHook();
	~CMMShellHook();

	void				Destroy(void);

	LPTSTR				GetPluginName(void);
	unsigned long		GetFlags(void);

	bool				SetHelper(ITuniacPluginHelper *pHelper);
	
	HANDLE				CreateThread(LPDWORD lpThreadId);
	HWND				GetPluginWindow(void);

	bool				About(HWND hWndParent);
	bool				Configure(HWND hWndParent);
};
