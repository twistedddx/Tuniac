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
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

#include "stdafx.h"

#include "resource.h"
#include "LogWindow.h"

CLogWindow::CLogWindow(void)
{
	bLogEnabled = false;
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
				m_hLogEditWnd = GetDlgItem(hDlg, IDC_LOGVIEW_EDIT);
			}
			break;

		case WM_SIZE:
			{
				WORD Width	= LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(m_hLogEditWnd, 0, 0, Width, Height - 24, TRUE);

				MoveWindow(GetDlgItem(hDlg, IDC_LOGVIEW_CLEAR),		Width - 48,	Height - 20, 48, 20, TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_LOGVIEW_ENABLE),	0,			Height - 20, 128, 20, TRUE);
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch (wCmdID)
				{
					case IDC_LOGVIEW_ENABLE:
						{
								int State = SendDlgItemMessage(hDlg, IDC_LOGVIEW_ENABLE, BM_GETCHECK, 0, 0);
								bLogEnabled = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_LOGVIEW_CLEAR:
						{
								BOOL bReadOnly = ::GetWindowLong(m_hLogEditWnd, GWL_STYLE) & ES_READONLY;
								if (bReadOnly)
									::SendMessage(m_hLogEditWnd, EM_SETREADONLY, FALSE, 0);
								::SendMessage(m_hLogEditWnd, EM_SETSEL, 0, -1);
								::SendMessage(m_hLogEditWnd, WM_CLEAR, 0, 0);
								if (bReadOnly)
									::SendMessage(m_hLogEditWnd, EM_SETREADONLY, TRUE, 0);
						}
						break;
				}
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
	if (bLogEnabled)
	{
		TCHAR szFormattedMessage[512];
		StringCchPrintf(szFormattedMessage, 512, TEXT("%s:	%s\r\n"), szModuleName, szMessage);
		int iCurLength = GetWindowTextLength(m_hLogEditWnd);
		::SendMessage(m_hLogEditWnd, EM_SETSEL, (WPARAM)iCurLength, (LPARAM)iCurLength);
		::SendMessage(m_hLogEditWnd, EM_REPLACESEL, FALSE, (LPARAM)szFormattedMessage);
		::SendMessage(m_hLogEditWnd, EM_SCROLLCARET, 0, 0);
	}
}
