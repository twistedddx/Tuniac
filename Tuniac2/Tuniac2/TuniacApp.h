#pragma once
#include "mediamanager.h"
#include "tuniacpluginmanager.h"
#include "tuniachelper.h"

class CTuniacApp
{
protected:

	HINSTANCE							m_hInstance;
	WNDCLASSEX							m_wc;

	HWND								m_hWnd;
	HWND								m_hWndStatus;

	CMediaManager						m_MediaManager;
	CTuniacPluginManager				m_PluginManager;
	CTuniacHelper						m_Helper;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK		WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


public:
	CTuniacApp(void);
	~CTuniacApp(void);

	CMediaManager			& getMediaManager()		{ return m_MediaManager; };
	CTuniacPluginManager	& getPluginManager()	{ return m_PluginManager; };
	CTuniacHelper			& getHelper(void)		{ return m_Helper; };

	const HINSTANCE getHINSTANCE() { return m_hInstance; };

	bool Initialise(HINSTANCE hInst);
	bool Shutdown(void);
	bool Run(void);

	void setStatusBarText(String NewStatus);
	bool SetWindowTitle(String NewTitle);
};
