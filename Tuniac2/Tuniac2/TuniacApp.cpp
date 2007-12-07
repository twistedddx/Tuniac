#include "StdAfx.h"

#include ".\tuniacapp.h"

const String szWindowName	= TEXT("Tuniac 2 - Alpha");
const String szClassName	= TEXT("TUNIAC2WINDOWCLASS");

CTuniacApp::CTuniacApp(void)
{
}

CTuniacApp::~CTuniacApp(void)
{
}

bool CTuniacApp::Initialise(HINSTANCE hInst)
{
	m_hInstance = hInst;

	CoInitialize(NULL);
	InitCommonControls();

	if(!m_PluginManager.Initialize())
		return false;

	if(!m_MediaManager.Initialize())
		return false;

	m_wc.cbSize			= sizeof(WNDCLASSEX); 
	m_wc.style			= 0;
	m_wc.lpfnWndProc	= (WNDPROC)WndProcStub;
	m_wc.cbClsExtra		= 0;
	m_wc.cbWndExtra		= 0;
	m_wc.hInstance		= m_hInstance;
	m_wc.hIcon			= LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TUNIAC2));
	m_wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	m_wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	m_wc.hIconSm		= LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TUNIAC2));
	m_wc.lpszMenuName	= MAKEINTRESOURCE(IDR_TUNIAC_MENU);
	m_wc.lpszClassName	= szClassName;
	if(!RegisterClassEx(&m_wc))
		return(false);

	return true;
}

bool CTuniacApp::Shutdown(void)
{
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		UnregisterClass(szClassName, m_hInstance);
	}

	m_MediaManager.Shutdown();

	m_PluginManager.Shutdown();

	CoUninitialize();

	return true;
}

bool CTuniacApp::Run(void)
{
	MSG	msg;

	m_hWnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_APPWINDOW,
							szClassName, 
							szWindowName, 
							WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							NULL,
							NULL, 
							m_hInstance,
							this);

	if(!m_hWnd)
	{
		return false;
	}

	while(GetMessage(&msg, NULL, 0 , 0))
	{
		//if(!TranslateAccelerator(m_hWnd, m_hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return true;
}

LRESULT CALLBACK CTuniacApp::WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CTuniacApp * pTA;

	if(message == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pTA = (CTuniacApp *)lpCS->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pTA);
	}
	else
	{
		pTA = (CTuniacApp *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pTA->WndProc(hWnd, message, wParam, lParam));
}

LRESULT CALLBACK CTuniacApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			{
				m_hWndStatus =  CreateWindowEx(	0,
												STATUSCLASSNAME,
												TEXT(""),
												WS_CHILD | WS_VISIBLE,
												0, 
												0,
												0,
												0,
												hWnd,
												NULL,
												m_hInstance,
												NULL);

				setStatusBarText(TEXT("Welcome To Tuniac"));

				ShowWindow(hWnd, SW_SHOW);
			}
			break;

		case WM_DESTROY:
			{
				DestroyWindow(m_hWndStatus);
				PostQuitMessage(0);
			}
			break;

		case WM_CLOSE:
			{
				DestroyWindow(hWnd);
			}
			break;

		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMinMaxInfo = (LPMINMAXINFO)lParam;

				lpMinMaxInfo->ptMinTrackSize.x = 500;
				lpMinMaxInfo->ptMinTrackSize.y = 400;
			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);
				RECT playcontrolsrect = {0,0, Width, 60};
				RECT statusRect;

				//m_PlayControls.Move(playcontrolsrect.left, playcontrolsrect.top, playcontrolsrect.right, playcontrolsrect.bottom);

				SendMessage(m_hWndStatus, message, wParam, lParam);
				SendMessage(m_hWndStatus, SB_GETRECT, 0, (LPARAM)&statusRect);
/*
				for(unsigned long x=0; x < m_WindowArray.GetCount(); x++)
				{
					m_WindowArray[x]->SetPos(	0,
												playcontrolsrect.bottom,
												Width,
												Height - statusRect.bottom - playcontrolsrect.bottom);
				}
				*/
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);


				switch(wCmdID)
				{
					case ID_FILE_IMPORTFILE:
						{
							m_MediaManager.ShowAddFiles(hWnd);
						}
						break;

					case ID_FILE_IMPORTDIRECTORY:
						{
							m_MediaManager.ShowAddFolderSelector(hWnd);
						}
						break;

					case ID_FILE_EXIT:
						{
							DestroyWindow(hWnd);
						}
						break;
				}
			}
			break;


		default:
			return(DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}

	return 0;
}

void CTuniacApp::setStatusBarText(String NewStatus)
{
	SendMessage(m_hWndStatus, SB_SETTEXT, 0, (LPARAM)NewStatus.c_str());
}

bool CTuniacApp::SetWindowTitle(String NewTitle)
{
	String TitleStr = TEXT("Tuniac - ");
	TitleStr += NewTitle;

	::SetWindowText(m_hWnd, TitleStr);

	return true;
}

