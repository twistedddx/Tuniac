#include "stdafx.h"
#include "PopupNotify_plugin.h"
#include <stdio.h>

#define POPUPWINDOWCLASS		TEXT("TuniacPlugin_PopupNotify")

#define ID_TIMER_HIDE			(1)
#define ID_TIMER_FADE			(2)
#define ID_TIMER_PAINT			(3)


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ulReason, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) ITuniacPlugin * CreateTuniacPlugin(void)
{
	ITuniacPlugin * pPlugin = new CPopupNotify;
	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACPLUGIN_VERSION;
}


CPopupNotify::CPopupNotify(void)
{
	m_ShowTimeMS = 4000;
	m_FadeTimeMS = 1000;
}

CPopupNotify::~CPopupNotify(void)
{
}

void			CPopupNotify::Destroy(void)
{
	delete this;
}

LPTSTR			CPopupNotify::GetPluginName(void)
{
	return TEXT("Popup Notify");
}

unsigned long	CPopupNotify::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool			CPopupNotify::SetHelper(ITuniacPluginHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

HANDLE			CPopupNotify::CreateThread(LPDWORD lpThreadId)
{
	m_hThread = ::CreateThread(	NULL,
								16384,
								this->ThreadStub,
								this,
								0,
								lpThreadId);

	if(m_hThread == NULL)
		return NULL;
	return m_hThread;
}

HWND			CPopupNotify::GetMainWindow(void)
{
	return m_hWnd;
}

unsigned long	CPopupNotify::ThreadStub(void * in)
{
	CPopupNotify * pPlugin = (CPopupNotify *)in;
	return(pPlugin->ThreadProc());
}

unsigned long	CPopupNotify::ThreadProc(void)
{
	WNDCLASSEX			wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProcStub;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_pHelper->GetMainInstance();
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) COLOR_WINDOW;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= POPUPWINDOWCLASS;
	wcex.hIconSm		= NULL;

	if(RegisterClassEx(&wcex) == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
		return(false);

    RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

	m_hWnd = CreateWindowEx(	WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_STATICEDGE,
								POPUPWINDOWCLASS,
								TEXT("Tuniac"),
								WS_POPUP,
								rcWorkArea.right - 260,
								rcWorkArea.bottom - 60,
								260,
								60,
								NULL,
								NULL,
								m_pHelper->GetMainInstance(),
								this);

	if(m_hWnd == NULL)
		return false;

	HDC hDC = GetDC(m_hWnd);

	m_SmallFont = CreateFont(	-MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
								0,
								0,
								0,
								FW_NORMAL,
								0,
								0,
								0,
								DEFAULT_CHARSET,
								OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								PROOF_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE, 
								TEXT("Trebuchet MS"));

	m_SmallFontB = CreateFont(	-MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
								0,
								0,
								0,
								FW_EXTRABOLD,
								0,
								0,
								0,
								DEFAULT_CHARSET,
								OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								PROOF_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE, 
								TEXT("Trebuchet MS"));

	m_SmallFontU = CreateFont(	-MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
								0,
								0,
								0,
								FW_NORMAL,
								0,
								TRUE,
								0,
								DEFAULT_CHARSET,
								OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								PROOF_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE, 
								TEXT("Trebuchet MS"));

	ReleaseDC(m_hWnd, hDC);

	m_aHotkeyShow = GlobalAddAtom(TEXT("TUNIACPOPUPNOTIFY_HOTKEYSHOW"));
	RegisterHotKey(m_hWnd, m_aHotkeyShow, MOD_WIN, VK_SUBTRACT);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	UnregisterHotKey(m_hWnd, m_aHotkeyShow);
	GlobalDeleteAtom(m_aHotkeyShow);
	
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	UnregisterClass(POPUPWINDOWCLASS, m_pHelper->GetMainInstance());
	return 0;
}

LRESULT CALLBACK	CPopupNotify::WndProcStub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPopupNotify * pPlugin;

	if(uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pPlugin = (CPopupNotify *)lpCS->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pPlugin);
	}
	else
	{
		pPlugin = (CPopupNotify *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pPlugin->WndProc(hWnd, uMsg, wParam, lParam));

}

LRESULT CALLBACK	CPopupNotify::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_PAINT:
			{
				RePaint(hWnd);
			}
            break;

		case WM_TIMER:
			{
      			if(wParam == ID_TIMER_HIDE)
				{
        			KillTimer(m_hWnd, ID_TIMER_HIDE);
					if(m_FadeTimeMS > 50)
						SetTimer(m_hWnd, ID_TIMER_FADE, m_FadeTimeMS / 10, NULL);
					else
						ShowWindow(m_hWnd, SW_HIDE);
				}
				else if(wParam == ID_TIMER_FADE)
				{
					if(m_Alpha <= 30)
					{
						KillTimer(m_hWnd, ID_TIMER_FADE);
						ShowWindow(m_hWnd, SW_HIDE);
						m_Alpha = 255;
						SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha, LWA_ALPHA);
					}
					else
					{
						m_Alpha -= 25;
						SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha, LWA_ALPHA);
					}
				}
				else if(wParam == ID_TIMER_PAINT)
				{
					RedrawWindow(m_hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_INTERNALPAINT);
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				KillTimer(m_hWnd, ID_TIMER_HIDE);
				KillTimer(m_hWnd, ID_TIMER_FADE);
				ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
				SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha = 254, LWA_ALPHA);
				SetTimer(m_hWnd, ID_TIMER_HIDE, m_ShowTimeMS / 2, NULL);

				POINTS pt = MAKEPOINTS(lParam);
				if(pt.x >= m_rcHit.left) {
					SetCursor(LoadCursor(NULL, IDC_HAND));
					RedrawWindow(m_hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_INTERNALPAINT);
				}

				SetTimer(m_hWnd, ID_TIMER_PAINT, 100, NULL);
			}
			break;

		case WM_LBUTTONDOWN:
			{
				POINTS pt = MAKEPOINTS(lParam);
				if(pt.x >= m_rcHit.left) {
					int iJump;
					if(pt.y > 40)
						iJump = 2;
					else if(pt.y > 20)
						iJump = 1;
					else
						iJump = 0;
					m_pHelper->Navigate(iJump);
				}

				KillTimer(m_hWnd, ID_TIMER_HIDE);
				KillTimer(m_hWnd, ID_TIMER_FADE);
				ShowWindow(m_hWnd, SW_HIDE);
				m_Alpha = 255;
				SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha, LWA_ALPHA);
			}
			break;

		case WM_RBUTTONDOWN:
			{
				KillTimer(m_hWnd, ID_TIMER_HIDE);
				KillTimer(m_hWnd, ID_TIMER_FADE);
				ShowWindow(m_hWnd, SW_HIDE);
				m_Alpha = 255;
				SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha, LWA_ALPHA);
			}
			break;

		case WM_MBUTTONDOWN:
			{
				m_pHelper->Navigate(-1);
			}
			break;

		case WM_HOTKEY:
		case PLUGINNOTIFY_SONGCHANGE:
		case PLUGINNOTIFY_SONGCHANGE_MANUAL:
			{
				KillTimer(m_hWnd, ID_TIMER_HIDE);
				KillTimer(m_hWnd, ID_TIMER_FADE);
				ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
				SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha = 254, LWA_ALPHA);
				RedrawWindow(m_hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_INTERNALPAINT);
				SetTimer(m_hWnd, ID_TIMER_HIDE, m_ShowTimeMS, NULL);
			}
			break;

		default:
			return(DefWindowProc(hWnd, uMsg, wParam, lParam));
			break;
	}

	return 0;
}

void			CPopupNotify::RePaint(HWND hWnd)
{
	KillTimer(m_hWnd, ID_TIMER_PAINT);

	if(hWnd != m_hWnd)
		return;

	if(GetUpdateRect(hWnd, NULL, FALSE) == 0)
		return;

	POINT pt;
	GetCursorPos(&pt);
	int iHover = -1;
	if(ScreenToClient(m_hWnd, &pt) != 0 && ChildWindowFromPoint(m_hWnd, pt) != NULL)
	{
		if(pt.x >= m_rcHit.left) {
			if(pt.y > 40)
				iHover = 2;
			else if(pt.y > 20)
				iHover = 1;
			else
				iHover = 0;
		}
	}

    PAINTSTRUCT ps;
    HDC hDC;

    hDC = BeginPaint(hWnd, &ps);
    SetBkMode(hDC, TRANSPARENT);

    RECT rcStatus, rcDetails;
    GetClientRect(hWnd, &rcStatus);
    GetClientRect(hWnd, &rcDetails);

	SIZE s;
	GetTextExtentPoint32(hDC, TEXT("Now:"), 4, &s);

	rcStatus.left		+= 2;
	rcStatus.right		 = rcStatus.left + s.cx + 2;
	rcStatus.bottom		 = 20;

	rcDetails.left		+= rcStatus.left + rcStatus.right;
	rcDetails.right		-= 2;
	rcDetails.bottom	 = 20;

	m_rcHit = rcDetails;
	m_rcHit.bottom = 60;

	TCHAR szTrack[128];

	m_pHelper->GetTrackInfo(szTrack, 128, TEXT("@T - @A"), 0);
	SelectObject(hDC, (HGDIOBJ) m_SmallFontB);
    SetTextColor(hDC, GetSysColor(COLOR_INFOTEXT));
	DrawText(hDC, TEXT("Now:"), -1, &rcStatus, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
	
	if(iHover == 0)
	{
		SelectObject(hDC, (HGDIOBJ) m_SmallFontU);
		SetTextColor(hDC, GetSysColor(COLOR_HOTLIGHT));
	}
	else
	{
		SelectObject(hDC, (HGDIOBJ) m_SmallFont);
	}
	DrawText(hDC, szTrack, -1, &rcDetails, DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_WORD_ELLIPSIS);

	rcStatus.top		+= 20;
	rcStatus.bottom		+= 20;
	rcDetails.top		+= 20;
	rcDetails.bottom	+= 20;

	m_pHelper->GetTrackInfo(szTrack, 128, TEXT("@T - @A"), 1);
	if(wcslen(szTrack) > 0)
	{
		SelectObject(hDC, (HGDIOBJ) m_SmallFontB);
		SetTextColor(hDC, GetSysColor(COLOR_INFOTEXT));
		DrawText(hDC, TEXT("Next:"), -1, &rcStatus, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
	
		if(iHover == 1)
		{
			SelectObject(hDC, (HGDIOBJ) m_SmallFontU);
			SetTextColor(hDC, GetSysColor(COLOR_HOTLIGHT));
		}
		else
		{
			SelectObject(hDC, (HGDIOBJ) m_SmallFont);
		}
		DrawText(hDC, szTrack, -1, &rcDetails, DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_WORD_ELLIPSIS);

		rcStatus.top		+= 20;
		rcStatus.bottom		+= 20;
		rcDetails.top		+= 20;
		rcDetails.bottom	+= 20;
	}



	m_pHelper->GetTrackInfo(szTrack, 128, TEXT("@T - @A"), 2);
	if(wcslen(szTrack) > 0)
	{
		SelectObject(hDC, (HGDIOBJ) m_SmallFontB);
		SetTextColor(hDC, GetSysColor(COLOR_INFOTEXT));
		DrawText(hDC, TEXT("Next:"), -1, &rcStatus, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
		
		if(iHover == 2)
		{
			SelectObject(hDC, (HGDIOBJ) m_SmallFontU);
			SetTextColor(hDC, GetSysColor(COLOR_HOTLIGHT));
		}
		else
		{
			SelectObject(hDC, (HGDIOBJ) m_SmallFont);
		}
		DrawText(hDC, szTrack, -1, &rcDetails, DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_WORD_ELLIPSIS);
	}

	EndPaint(hWnd, &ps);
}

bool			CPopupNotify::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Popup Nofity plugin for Tuniac.\r\nBy Blur, 2005-2008.\r\n\r\nWill popup a small window to display the current track when something changes."), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CPopupNotify::Configure(HWND hWndParent)
{
	return false;
}
