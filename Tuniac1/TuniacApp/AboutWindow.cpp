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
	Copyright (C) 2003-2014 Brett Hoyle
*/

#include "stdafx.h"

#include ".\aboutwindow.h"
#include "resource.h"


BOOL CALLBACK AboutProc(HWND hwndDlg, 
                             UINT message, 
                             WPARAM wParam, 
                             LPARAM lParam) 
{ 
    switch (message) 
    { 
		case WM_INITDIALOG:
		{
			HWND		hParent = GetParent(hwndDlg);
			RECT		rcDlg, rcParent;

			GetWindowRect(hwndDlg, &rcDlg);
			GetWindowRect(hParent, &rcParent);

			int iWidth = rcDlg.right - rcDlg.left;
			int iHeight = rcDlg.bottom - rcDlg.top;

			int x = ((rcParent.right - rcParent.left) - iWidth) / 2 + rcParent.left;
			int y = ((rcParent.bottom - rcParent.top) - iHeight) / 2 + rcParent.top;

			int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x + iWidth  > iScreenWidth)  x = iScreenWidth - iWidth;
			if (y + iHeight > iScreenHeight) y = iScreenHeight - iHeight;

			MoveWindow(hwndDlg, x, y, iWidth, iHeight, FALSE);
		}
		break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 

CAboutWindow::CAboutWindow(void)
{
}

CAboutWindow::~CAboutWindow(void)
{
}

bool CAboutWindow::Show(void)
{
	m_hAboutDlg = CreateDialog(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), tuniacApp.getMainWindow(), (DLGPROC)AboutProc);
	if(m_hAboutDlg)
	{
		TCHAR szBuild[128];
		StringCchPrintf(szBuild, 128, TEXT("Build: %u\r\n %s"), tuniacApp.GetTuniacBuildNumber(), tuniacApp.GetTuniacBuild());
		ShowWindow(m_hAboutDlg, SW_SHOW);
		SetDlgItemText(m_hAboutDlg, IDC_ABOUT_BUILD_TEXT, szBuild);
	}

	return true;
}
