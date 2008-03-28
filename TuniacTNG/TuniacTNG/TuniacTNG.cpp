// TuniacTNG.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TuniacTNG.h"
#include "MediaManager.h"

#define MAX_LOADSTRING 100

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND		hListView;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	INITCOMMONCONTROLSEX		iccex = {0};

	iccex.dwSize	= sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC		= ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS | ICC_NATIVEFNTCTL_CLASS | ICC_STANDARD_CLASSES | ICC_LINK_CLASS;

	InitCommonControlsEx(&iccex);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TUNIACTNG, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	CMediaManager::Instance()->Initialize();

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TUNIACTNG));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CMediaManager::Instance()->Shutdown();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TUNIACTNG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TUNIACTNG);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowEx(	WS_EX_ACCEPTFILES,
							szWindowClass, 
							szTitle, 
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							850, 
							650, 
							NULL, 
							NULL, 
							hInstance, 
							NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_CREATE:
			{
				RECT r;
				GetClientRect(hWnd, &r);
				hListView = CreateWindowEx(	0, 
											WC_LISTVIEW, 
											TEXT("TUNIACLIST"), 
											WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_OWNERDATA, 
											0,
											0,
											100,
											100, 
											hWnd, 
											(HMENU)100, 
											NULL, 
											NULL);

				ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_AUTOSIZECOLUMNS );


				MoveWindow(hListView, 0,0,r.right, r.bottom, FALSE);

				LV_COLUMN lvC;

				// Create columns.
				lvC.mask	= LVCF_WIDTH | LVCF_TEXT;
				lvC.cx		= 200;
				lvC.pszText	= TEXT("Title");
				ListView_InsertColumn(hListView, 0, &lvC);

				lvC.pszText	= TEXT("Artist");
				ListView_InsertColumn(hListView, 1, &lvC);

				lvC.pszText	= TEXT("Album");
				ListView_InsertColumn(hListView, 2, &lvC);

				lvC.pszText	= TEXT("Comment");
				ListView_InsertColumn(hListView, 3, &lvC);

				ListView_SetItemCountEx(hListView, CMediaManager::Instance()->GetNumEntries(), 0);
			}
			break;

		case WM_COMMAND:
			{
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
				// Parse the menu selections:
				switch (wmId)
				{
					case IDM_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
						break;
					case IDM_EXIT:
						DestroyWindow(hWnd);
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;

		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

		case WM_SIZE:
			{
				MoveWindow(hListView, 0,0,LOWORD(lParam), HIWORD(lParam), FALSE);
			}
			break;


		case WM_NOTIFY:
			{
				UINT idCtrl = wParam;
				LPNMHDR lpNotify = (LPNMHDR)lParam;

				switch(lpNotify->code)
				{
					case LVN_ODCACHEHINT:
						{
							NMLVCACHEHINT * pCacheHint = (NMLVCACHEHINT*)lParam;

							TCHAR		szBuffer[1024];

							wsprintf(szBuffer, TEXT("From: %d, To: %d\n"), pCacheHint->iFrom, pCacheHint->iTo);

							OutputDebugString(szBuffer);
						}
						break;

					case LVN_GETDISPINFO:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *) lParam;
							if(pDispInfo->item.iItem != -1)
							{
								static LPTSTR	szThing = TEXT("Test");
								pDispInfo->item.pszText = szThing;
	/*
								TCHAR		szBuffer[1024];
								wsprintf(szBuffer, TEXT("Item: %d, "), pDispInfo->item.iItem);
								OutputDebugString(szBuffer);*/
							}
						}
						break;
				}
			}
			break;

		case WM_DROPFILES:
			{
				HDROP hDrop = (HDROP)wParam;

				TCHAR szURL[MAX_PATH];

				UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);

//				if(m_MediaLibrary.BeginAdd(uNumFiles))
				{
					for(UINT file = 0; file<uNumFiles; file++)
					{
						DragQueryFile(hDrop, file, szURL, MAX_PATH);
						CMediaManager::Instance()->AddFile(szURL);
					}

//					m_MediaLibrary.EndAdd();
				}

				DragFinish(hDrop);

				ListView_SetItemCountEx(hListView, CMediaManager::Instance()->GetNumEntries(), 0);

			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
