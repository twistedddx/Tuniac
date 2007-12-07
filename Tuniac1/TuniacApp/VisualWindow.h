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

};
