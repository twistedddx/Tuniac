#pragma once
#include "iwindow.h"
//#include "EditLog.h"

class CLogWindow :
	public IWindow
{
protected:
			HWND						m_hLogWnd;

			//CEditLog				*	m_EditLog;

			LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static	LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CLogWindow(void);
public:
	~CLogWindow(void);

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
	void			LogMessage(LPTSTR szModuleName, LPTSTR szMessage);
};
