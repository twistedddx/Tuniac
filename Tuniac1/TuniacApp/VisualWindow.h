/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once
#include "iwindow.h"

#include "ITuniacVisPlugin.h"

typedef struct 
{
	HINSTANCE				hInst;
	ITuniacVisPlugin	*	pPlugin;
}VisualPlugin;

class CVisualWindow :
	public IWindow,
	public ITuniacVisHelper
{
protected:
	HWND							m_hWnd;
	HWND							m_hParentWnd;
	HDC								m_WindowDC;

	CCriticalSection				m_RenderLock;

	bool							m_bFullScreen;
	RECT							m_OldSize;

	int								m_iActivePlugin;
	Array<VisualPlugin, 3>			m_VisualArray;

	HMENU							m_ContextMenu;

	HANDLE							m_hRenderEvent;

	HANDLE							m_hThread;
	unsigned long					m_dwThreadID;

	static unsigned long __stdcall ThreadStub(void * in);
	unsigned long ThreadProc(void);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CVisualWindow(void);
	~CVisualWindow(void);

	bool SetVisPlugin(int iPlugin);

public:
	void			Destroy(void);

	LPTSTR			GetName(void);
	GUID			GetPluginID(void);

	unsigned long	GetFlags(void);

	bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst);
	bool			DestroyPluginWindow(void);

	bool			Show(void);
	bool			Hide(void);

	bool			SetPos(int x, int y, int w, int h);

public:
	bool	GetVisData(float * pWaveformData, unsigned long ulNumSamples);
	void *	GetVariable(Variable eVar);

	bool	GetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool	SetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);

	void	GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iIndex);
};
