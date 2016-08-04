#include "stdafx.h"
#include "PopupNotify_plugin.h"
#include "resource.h"

#define POPUPWINDOWCLASS		TEXT("TuniacPlugin_PopupNotify")

#define ID_TIMER_HIDE			(1)
#define ID_TIMER_FADE			(2)
#define ID_TIMER_PAINT			(3)

#define FORMATSTRING_HELP	TEXT("\
@U\tURL\r\n\
@F\tFilename\r\n\
@X\tFile Type\r\n\
@K\tKind\r\n\
@S\tSize\r\n\
@A\tArtist\r\n\
@L\tAlbum\r\n\
@T\tTitle\r\n\
@#\tTrack\r\n\
@V\tDisc Number\r\n\
@G\tGenre\r\n\
@Y\tYear\r\n\
@I\tPlay Time\r\n\
@i\tPlay Time (short form)\r\n\
@D\tDate Added\r\n\
@E\tFile Creation Date\r\n\
@P\tLast Played Date\r\n\
@B\tBitrate\r\n\
@M\tSample Rate\r\n\
@N\tChannels\r\n\
@C\tComment\r\n\
@Z\tPlay Count\r\n\
@R\tRating\r\n\
@!\tPlay State\r\n\
@W\tBPM\r\n\
@Q\tAlbum Artist\r\n\
@O\tComposer\n\
")

static UINT WM_SW_APPBAR = WM_APP + 111;

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
	m_bInhibit   = false;
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
	return PLUGINFLAGS_ABOUT | PLUGINFLAGS_CONFIG;
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

HWND			CPopupNotify::GetPluginWindow(void)
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


	DWORD	lpRegType = REG_DWORD;
	DWORD	iRegSize = sizeof(int);

	m_bAllowInhibit = false;
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("AllowInhibit"), &lpRegType, (LPBYTE)&m_bAllowInhibit, &iRegSize);

	m_bManualTrigger = true;
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("Manual"), &lpRegType, (LPBYTE)&m_bManualTrigger, &iRegSize);
	m_bManualBlindTrigger = true;
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("ManualBlind"), &lpRegType, (LPBYTE)&m_bManualBlindTrigger, &iRegSize);
	m_bAutoTrigger = true;
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("Auto"), &lpRegType, (LPBYTE)&m_bAutoTrigger, &iRegSize);
	m_bAutoBlindTrigger = true;
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("AutoBlind"), &lpRegType, (LPBYTE)&m_bAutoBlindTrigger, &iRegSize);


	iRegSize = 128 * sizeof(WCHAR);

	StringCchCopy(m_szNormalFormatString, 128, TEXT("@T - @A"));
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("NormalFormatString"), &lpRegType, (LPBYTE)&m_szNormalFormatString, &iRegSize);

	StringCchCopy(m_szStreamFormatString, 128, TEXT("@T - @A"));
	m_pHelper->PreferencesGet(TEXT("PopupNotify"), TEXT("StreamFormatString"), &lpRegType, (LPBYTE)&m_szStreamFormatString, &iRegSize);


	m_abd.cbSize = sizeof(APPBARDATA);
	m_abd.hWnd = m_hWnd;
	m_abd.uCallbackMessage = WM_SW_APPBAR;
	SHAppBarMessage(ABM_NEW, &m_abd);

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

//pref dialog
LRESULT CALLBACK	CPopupNotify::DlgProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CPopupNotify * pPopupNotify = (CPopupNotify *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pPopupNotify->DlgProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK	CPopupNotify::DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPopupNotify * pPopupNotify = (CPopupNotify *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPopupNotify = (CPopupNotify *)lParam;

				SendDlgItemMessage(hDlg, IDC_FULLSCREENINHIBIT_CHECK, BM_SETCHECK, pPopupNotify->m_bAllowInhibit ? BST_CHECKED : BST_UNCHECKED, 0);
				
				SendDlgItemMessage(hDlg, IDC_TRIGGER_MANUAL, BM_SETCHECK, pPopupNotify->m_bManualTrigger ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_TRIGGER_MANUALBLIND, BM_SETCHECK, pPopupNotify->m_bManualBlindTrigger ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_TRIGGER_AUTO, BM_SETCHECK, pPopupNotify->m_bAutoTrigger ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_TRIGGER_AUTOBLIND, BM_SETCHECK, pPopupNotify->m_bAutoBlindTrigger ? BST_CHECKED : BST_UNCHECKED, 0);


				SendDlgItemMessage(hDlg, IDC_FORMATTING_NORMALFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_NORMALFORMAT, WM_SETTEXT, 0, (LPARAM)pPopupNotify->m_szNormalFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_NORMALFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@A - @T"));

				SendDlgItemMessage(hDlg, IDC_FORMATTING_STREAMFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_STREAMFORMAT, WM_SETTEXT, 0, (LPARAM)pPopupNotify->m_szStreamFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_STREAMFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@A - @T"));


			}
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
				case IDC_FULLSCREENINHIBIT_CHECK:
					{
						int State = SendDlgItemMessage(hDlg, IDC_FULLSCREENINHIBIT_CHECK, BM_GETCHECK, 0, 0);
						pPopupNotify->m_bAllowInhibit = State == BST_UNCHECKED ? FALSE : TRUE;
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("AllowInhibit"), REG_DWORD, (LPBYTE)&m_bAllowInhibit, sizeof(int));
					}
					break;

				case IDC_TRIGGER_MANUAL:
					{
						int State = SendDlgItemMessage(hDlg, IDC_TRIGGER_MANUAL, BM_GETCHECK, 0, 0);
						pPopupNotify->m_bManualTrigger = State == BST_UNCHECKED ? FALSE : TRUE;
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("Manual"), REG_DWORD, (LPBYTE)&m_bManualTrigger, sizeof(int));
					}
					break;

				case IDC_TRIGGER_MANUALBLIND:
					{
						int State = SendDlgItemMessage(hDlg, IDC_TRIGGER_MANUALBLIND, BM_GETCHECK, 0, 0);
						pPopupNotify->m_bManualBlindTrigger = State == BST_UNCHECKED ? FALSE : TRUE;
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("ManualBlind"), REG_DWORD, (LPBYTE)&m_bManualBlindTrigger, sizeof(int));
					}
					break;

				case IDC_TRIGGER_AUTO:
					{
						int State = SendDlgItemMessage(hDlg, IDC_TRIGGER_AUTO, BM_GETCHECK, 0, 0);
						pPopupNotify->m_bAutoTrigger = State == BST_UNCHECKED ? FALSE : TRUE;
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("Auto"), REG_DWORD, (LPBYTE)&m_bAutoTrigger, sizeof(int));
					}
					break;

				case IDC_TRIGGER_AUTOBLIND:
					{
						int State = SendDlgItemMessage(hDlg, IDC_TRIGGER_AUTOBLIND, BM_GETCHECK, 0, 0);
						pPopupNotify->m_bAutoBlindTrigger = State == BST_UNCHECKED ? FALSE : TRUE;
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("AutoBlind"), REG_DWORD, (LPBYTE)&m_bAutoBlindTrigger, sizeof(int));
					}
					break;

				case IDC_FORMATTING_NORMALFORMAT:
					{
						SendDlgItemMessage(hDlg, IDC_FORMATTING_NORMALFORMAT, WM_GETTEXT, 256, (LPARAM)m_szNormalFormatString);
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("NormalFormatString"), REG_DWORD, (LPBYTE)&m_szNormalFormatString, (128 * sizeof(WCHAR)));
					}
					break;

				case IDC_FORMATTING_STREAMFORMAT:
					{
						SendDlgItemMessage(hDlg, IDC_FORMATTING_STREAMFORMAT, WM_GETTEXT, 256, (LPARAM)m_szStreamFormatString);
						m_pHelper->PreferencesSet(TEXT("PopupNotify"), TEXT("StreamFormatString"), REG_DWORD, (LPBYTE)&m_szStreamFormatString, (128 * sizeof(WCHAR)));
					}
					break;

				case IDC_FORMATTING_FORMATSTRING_HELP:
					{
						MessageBox(hDlg, FORMATSTRING_HELP, TEXT("Help"), MB_OK | MB_ICONINFORMATION);
					}
					break;

                case IDOK:
				case IDCANCEL:
					{
						EndDialog(hDlg, wParam); 
						return TRUE;
					}
					break;
			}
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

//popup window
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
	CPopupNotify * pPopupNotify = (CPopupNotify *)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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
						SetTimer(m_hWnd, ID_TIMER_FADE, m_FadeTimeMS / 50, NULL);
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
						m_Alpha -= 5;
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

		case PLUGINNOTIFY_SONGCHANGE_INIT:
			{
				//fullscreen inhibit
				if(m_bInhibit)
					ResetWindow();
			}
			break;

		case PLUGINNOTIFY_SONGCHANGE:
			{
				//auto with UI
				if(!m_bAutoTrigger || m_bInhibit)
					break;

				ResetWindow();
			}
			break;

		case PLUGINNOTIFY_SONGCHANGE_BLIND:
			{
				//auto without UI
				if(!m_bAutoBlindTrigger || m_bInhibit)
					break;
				
				ResetWindow();
			}
			break;

		case PLUGINNOTIFY_SONGCHANGE_MANUAL:
			{
				//user with UI
				if(!m_bManualTrigger || m_bInhibit)
					break;

				ResetWindow();
			}
			break;

		case PLUGINNOTIFY_SONGCHANGE_MANUALBLIND:
			{
				//user without UI
				if(!m_bManualBlindTrigger || m_bInhibit)
					break;

				ResetWindow();
			}
			break;

		case WM_HOTKEY:
			{
				ResetWindow();
			}
			break;

		default:
			if(uMsg == WM_SW_APPBAR)
			{
				if(wParam == ABN_FULLSCREENAPP)
				{
					if(m_bAllowInhibit)
						m_bInhibit = (BOOL) lParam; 
				}
				break;
       		}
			return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}

	return FALSE;
}

void			CPopupNotify::ResetWindow(void)
{
	KillTimer(m_hWnd, ID_TIMER_HIDE);
	KillTimer(m_hWnd, ID_TIMER_FADE);

	ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);

	ShowWindowAsync(m_hWnd, SW_SHOWDEFAULT);
	ShowWindowAsync(m_hWnd, SW_SHOW);

	SetLayeredWindowAttributes(m_hWnd, 0, m_Alpha = 254, LWA_ALPHA);
	RedrawWindow(m_hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_INTERNALPAINT);
	SetTimer(m_hWnd, ID_TIMER_HIDE, m_ShowTimeMS, NULL);
	return;
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
	TCHAR szURL[MAX_PATH];

	m_pHelper->GetTrackInfo(szURL, 128, TEXT("@U"), 0);

	if(PathIsURL(szURL))
		m_pHelper->GetTrackInfo(szTrack, 128, m_szStreamFormatString, 0);
	else
		m_pHelper->GetTrackInfo(szTrack, 128, m_szNormalFormatString, 0);

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

	m_pHelper->GetTrackInfo(szURL, 128, TEXT("@U"), 1);

	if(PathIsURL(szURL))
		m_pHelper->GetTrackInfo(szTrack, 128, m_szStreamFormatString, 1);
	else
		m_pHelper->GetTrackInfo(szTrack, 128, m_szNormalFormatString, 1);

	if(wcsnlen_s(szTrack, 128) > 0)
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

	m_pHelper->GetTrackInfo(szURL, 128, TEXT("@U"), 2);

	if(PathIsURL(szURL))
		m_pHelper->GetTrackInfo(szTrack, 128, m_szStreamFormatString, 2);
	else
		m_pHelper->GetTrackInfo(szTrack, 128, m_szNormalFormatString, 2);

	if(wcsnlen_s(szTrack, 128) > 0)
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
	MessageBox(hWndParent, TEXT("Popup Nofity plugin for Tuniac.\r\nBy Blur and Brett H, 2005-2012.\r\n\r\nWill popup a small window to display the current track when the song changes."), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CPopupNotify::Configure(HWND hWndParent)
{
	DialogBoxParam(GetModuleHandle(L"PopupNotify_Plugin.dll"), MAKEINTRESOURCE(IDD_NOTIFYPREFWINDOW), hWndParent, (DLGPROC)DlgProcStub, (DWORD_PTR)this);

	return true;
}