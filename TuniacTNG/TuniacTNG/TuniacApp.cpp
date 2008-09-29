#include "StdAfx.h"
#include "TuniacApp.h"

static TCHAR szTitle[]			= TEXT("TuniacTNG");				// The title bar text
static TCHAR szWindowClass[]	= TEXT("TuniacTNGClass");			// the main window class name


CTuniacApp::CTuniacApp(void)
{
}

CTuniacApp::~CTuniacApp(void)
{
}

bool CTuniacApp::Initialize(HINSTANCE hInst, LPTSTR lpCmdLine)
{
	INITCOMMONCONTROLSEX		iccex = {0};

	iccex.dwSize	= sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC		= ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS | ICC_NATIVEFNTCTL_CLASS | ICC_STANDARD_CLASSES | ICC_LINK_CLASS;
	InitCommonControlsEx(&iccex);


	m_WCEX.cbSize			= sizeof(WNDCLASSEX);

	m_WCEX.style			= CS_OWNDC;
	m_WCEX.lpfnWndProc		= WndProcStub;
	m_WCEX.cbClsExtra		= 0;
	m_WCEX.cbWndExtra		= 0;
	m_WCEX.hInstance		= hInst;
	m_WCEX.hIcon			= NULL;
	m_WCEX.hCursor			= LoadCursor(NULL, IDC_ARROW);
	m_WCEX.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	m_WCEX.lpszMenuName		= NULL;
	m_WCEX.lpszClassName	= szWindowClass;
	m_WCEX.hIconSm			= NULL;

	if(! RegisterClassEx(&m_WCEX) )
		return false;

   m_hWnd = CreateWindowEx(	WS_EX_ACCEPTFILES,
							szWindowClass, 
							szTitle, 
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							800, 
							600, 
							NULL, 
							NULL, 
							hInst, 
							NULL);

   if (!m_hWnd)
   {
      return false;
   }


	return true;
}

bool CTuniacApp::Shutdown(void)
{
	return true;
}

bool CTuniacApp::Run(void)
{
	MSG		msg;
	BOOL	bRet = TRUE;

   while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
            // handle the error and possibly exit
			return false;
        }
        else
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
		pTA = (CTuniacApp *)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pTA->WndProc(hWnd, message, wParam, lParam));
}


LRESULT CALLBACK CTuniacApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CREATE:
			{
				ShowWindow(hWnd, SW_SHOW);
			}
			break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
