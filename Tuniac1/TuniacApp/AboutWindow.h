#pragma once

class CAboutWindow
{
protected:

	HWND		m_hAboutDlg;


public:
	CAboutWindow(void);
	~CAboutWindow(void);
public:

	bool Show(void);
};
