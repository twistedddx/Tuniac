#pragma once

#include <stdio.h>
#include <shellapi.h>
#include "ITuniacPlugin.h"

class CPopupNotify :
	public ITuniacPlugin
{

protected:
	HANDLE					m_hThread;
	HWND					m_hWnd;

	ATOM					m_aHotkeyShow;

	APPBARDATA				m_abd;

	unsigned long			m_ShowTimeMS;
	unsigned long			m_FadeTimeMS;
	BYTE					m_Alpha;

	RECT					m_rcHit;

	HFONT					m_SmallFont;
	HFONT					m_SmallFontB;
	HFONT					m_SmallFontU;

	ITuniacPluginHelper *	m_pHelper;

	BOOL					m_bAllowInhibit;
	bool					m_bInhibit;

	static LRESULT CALLBACK	WndProcStub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK		WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static unsigned long __stdcall	ThreadStub(void * in);
	unsigned long			ThreadProc(void);

	void					RePaint(HWND hWnd);

public:
	CPopupNotify();
	~CPopupNotify();

	void				Destroy(void);

	LPTSTR				GetPluginName(void);
	unsigned long		GetFlags(void);

	bool				SetHelper(ITuniacPluginHelper *pHelper);

	HANDLE				CreateThread(LPDWORD lpThreadId);
	HWND				GetMainWindow(void);

	bool				About(HWND hWndParent);
	bool				Configure(HWND hWndParent);
};
