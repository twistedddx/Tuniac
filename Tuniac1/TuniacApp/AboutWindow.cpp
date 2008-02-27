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
	DialogBox(	NULL, 
				MAKEINTRESOURCE(IDD_ABOUTBOX), 
				tuniacApp.getMainWindow(),
				(DLGPROC)AboutProc);

	return true;
}
