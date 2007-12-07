#pragma once

#include "Preferences.h"
#include "TuniacApp.h"

class CTaskbar
{

protected:

	NOTIFYICONDATA	m_TrayIconData;

	bool			bWasMaximized;

public:
	CTaskbar(void);
	~CTaskbar(void);

	bool			Initialize(HWND hTargetWindow, unsigned long AppMessage);
	bool			Shutdown(void);

	void			SetTitle(LPCWSTR	szTitle);
	void			SetIcon(HICON		hIcon);

	void			Show();
	void			Hide();
};