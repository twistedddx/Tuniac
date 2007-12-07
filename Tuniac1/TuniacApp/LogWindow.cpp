#include "stdafx.h"

#include "resource.h"
#include "LogWindow.h"

CLogWindow::CLogWindow(void)
{
	m_hLogWnd = NULL;
}

CLogWindow::~CLogWindow(void)
{
}

void			CLogWindow::Destroy(void)
{
	delete this;
}


LPTSTR			CLogWindow::GetName(void)
{
	return TEXT("Log Window");
}

GUID			CLogWindow::GetPluginID(void)
{
// {AA433E7A-4035-46d5-9694-DD1C45471668}
static const GUID LOGWINDOWGUID = { 0xaa433e7a, 0x4035, 0x46d5, { 0x96, 0x94, 0xdd, 0x1c, 0x45, 0x47, 0x16, 0x68 } };

	return LOGWINDOWGUID;

}


unsigned long	CLogWindow::GetFlags(void)
{
	return 0;
}


bool			CLogWindow::CreatePluginWindow(HWND hParent, HINSTANCE hInst)
{
	m_hLogWnd = CreateDialogParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_LOGVIEW), hParent, (DLGPROC)WndProcStub, (DWORD_PTR)this);

	if(m_hLogWnd == NULL)
		return false;

	return true;
}

bool			CLogWindow::DestroyPluginWindow(void)
{
	if(m_hLogWnd)
	{
		DestroyWindow(m_hLogWnd);
		return true;
	}

	return false;
}


bool			CLogWindow::Show(void)
{
	if(m_hLogWnd)
	{
		ShowWindow(m_hLogWnd, SW_SHOW);
		return true;
	}

	return false;
}

bool			CLogWindow::Hide(void)
{
	if(m_hLogWnd)
	{
		ShowWindow(m_hLogWnd, SW_HIDE);
		return true;
	}

	return false;
}


bool			CLogWindow::SetPos(int x, int y, int w, int h)
{
	if(m_hLogWnd)
	{
		SetWindowPos(m_hLogWnd, NULL, x, y, w, h, SWP_NOZORDER);
		return true;
	}

	return false;
}

LRESULT CALLBACK			CLogWindow::WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CLogWindow * pSSW = (CLogWindow *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pSSW->WndProc(hDlg, message, wParam, lParam));

}

LRESULT CALLBACK			CLogWindow::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
//				m_EditLog = new CEditLog(GetDlgItem(hDlg, IDC_LOGVIEW_EDIT));
			}
			break;

		case WM_SIZE:
			{
				WORD Width	= LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(GetDlgItem(hDlg, IDC_LOGVIEW_EDIT), 0, 0, Width, Height - 24, TRUE);

				MoveWindow(GetDlgItem(hDlg, IDC_LOGVIEW_CLEAR),		Width - 48,	Height - 20, 48, 20, TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_LOGVIEW_ENABLE),	0,			Height - 20, 128, 20, TRUE);
			}
			break;

		case WM_COMMAND:
			{
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

void			CLogWindow::LogMessage(LPTSTR szModuleName, LPTSTR szMessage)
{
//	m_EditLog->AddText(szModuleName);
//	m_EditLog->AddText(TEXT(": "));
//	m_EditLog->AddText(szMessage, true);
//	m_EditLog->AddText(TEXT("\r\n"));
}
