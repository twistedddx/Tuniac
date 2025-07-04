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

#include "StdAfx.h"
#include "visualwindow.h"

#include "resource.h"

#include "TuniacVisual.h"

#define MOUSE_TIMEOUT			3000

HANDLE							m_hRenderEvent;

VOID CALLBACK TimerRoutine(PVOID lpParam, BOOL TimerOrWaitFired)
{
	SetEvent(m_hRenderEvent);
}

CVisualWindow::CVisualWindow(void) :
	m_hWnd (NULL),
	m_hThread (NULL),
	m_WindowDC(NULL),
	m_iActivePlugin(-1),
	m_bFullScreen(false)
{
}

CVisualWindow::~CVisualWindow(void)
{
}

bool 			CVisualWindow::SetVisPlugin(int iPlugin)
{
	int postcount = 100;

	while(!PostThreadMessage(m_dwThreadID, WM_APP, 0, iPlugin) && postcount--)
		Sleep(5);

	tuniacApp.m_Preferences.SetCurrentVisual(iPlugin);

	return true;
}

void			CVisualWindow::Destroy(void)
{
	while(m_VisualArray.GetCount())
	{
		if (m_VisualArray[0].pPlugin)
		{
			m_VisualArray[0].pPlugin->Destroy();
		}
		if(m_VisualArray[0].hInst)
			FreeLibrary(m_VisualArray[0].hInst);

		m_VisualArray.RemoveAt(0);
	}

	delete this;
}

LPTSTR			CVisualWindow::GetName(void)
{
	return TEXT("Visuals");
}

GUID			CVisualWindow::GetPluginID(void)
{
	static GUID VisWindowGUID = {0x675ab1f6, 0xba41, 0x4930, {0x8b, 0x8d, 0x7d, 0x7f, 0x08, 0x56, 0xe5, 0xf8}};

	return (VisWindowGUID);
}

unsigned long	CVisualWindow::GetFlags(void)
{
	return 0;
}

bool			CVisualWindow::CreatePluginWindow(HWND hParent, HINSTANCE hInstance)
{
	WNDCLASSEX	wcex;

	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szVisualsPath[MAX_PATH];
	TCHAR				szFilename[MAX_PATH];

	m_hParentWnd = hParent;
	

	{
		VisualPlugin	builtin;
		builtin.hInst			= NULL;
		builtin.pPlugin			= new CTuniacVisual;
		builtin.pPlugin->SetHelper(this);
		m_VisualArray.AddTail(builtin);
	}

	GetModuleFileName(NULL, szVisualsPath, MAX_PATH);
	PathRemoveFileSpec(szVisualsPath);
	PathAddBackslash(szVisualsPath);
	StringCchCat(szVisualsPath, MAX_PATH, TEXT("visuals"));
	PathAddBackslash(szVisualsPath);
	StringCchCopy(szFilename, MAX_PATH, szVisualsPath);
	StringCchCat(szFilename, MAX_PATH, TEXT("*.dll"));

	hFind = FindFirstFile( szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR temp[MAX_PATH];

			StringCchCopy(temp, MAX_PATH, szVisualsPath);
			StringCchCat(temp, MAX_PATH, w32fd.cFileName);

			HINSTANCE hDLL = LoadLibrary(temp);
			if(hDLL)
			{
				GetTuniacVisPluginVersionFunc pGTVPVF = (GetTuniacVisPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacVisPluginVersion");
				/* One day we can disable old plugins but right now rabbithole doesn't have version info  :(
				if(pGTVPVF == NULL)
				{
					FreeLibrary(hDLL);
					continue;
				}
				*/

				if(pGTVPVF != NULL)
				{
					if(pGTVPVF() != ITUNIACVISPLUGIN_VERSION)
					{
						TCHAR szError[512];
						StringCchPrintf(szError, 512, TEXT("Incompatable visual found: \\visuals\\%s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
						MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONWARNING);
						FreeLibrary(hDLL);
						continue;
					}
				}

				CreateTuniacVisPluginFunc pCTVPF;
				pCTVPF = (CreateTuniacVisPluginFunc)GetProcAddress(hDLL, "CreateTuniacVisPlugin");
				if(pCTVPF)
				{
					ITuniacVisPlugin * pPlugin = pCTVPF();

					if(pPlugin)
					{
						VisualPlugin t;
						t.hInst			= hDLL;
						t.pPlugin		= pPlugin;

						m_VisualArray.AddTail(t);

						pPlugin->SetHelper(this);
					}
					else
					{
						FreeLibrary(hDLL);
					}
				}
				else
				{
					FreeLibrary(hDLL);
				}
			}

		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}

	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_OWNDC | CS_BYTEALIGNCLIENT | CS_DBLCLKS ;
	wcex.lpfnWndProc	= (WNDPROC)WndProcStub;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= TEXT("TUNIACVISUALWINDOW");
	wcex.hIconSm		= NULL;

	if(!RegisterClassEx(&wcex))
		return(false);

	m_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
							TEXT("TUNIACVISUALWINDOW"), 
							TEXT("TUNIACVISUALWINDOW"), 
							WS_CHILD,
							0, 
							0, 
							100, 
							100, 
							hParent, 
							NULL, 
							hInstance, 
							this);


	if(!m_hWnd)
	{
		return(false);
	}

	m_hRenderEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hTimer = NULL;
	hTimerQueue = NULL;

	hTimerQueue = CreateTimerQueue();
	CreateTimerQueueTimer( &hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, NULL, 0, ((1000 / tuniacApp.m_Preferences.GetVisualFPS()) + 1), 0);


	m_hThread = CreateThread(	NULL,
								16384,
								ThreadStub,
								this,
								0,
								&m_dwThreadID);


	if(!m_hThread)
	{
		return(false);
	}

	if(m_VisualArray.GetCount())
	{
		if(tuniacApp.m_Preferences.GetCurrentVisual() >= m_VisualArray.GetCount())
			tuniacApp.m_Preferences.SetCurrentVisual(0);

		SetVisPlugin(tuniacApp.m_Preferences.GetCurrentVisual());
	}
	else
	{
		SetVisPlugin(-1);
	}

	return(true);
}

bool			CVisualWindow::DestroyPluginWindow(void)
{
	CAutoLock m(&m_RenderLock);

	if(m_hThread)
	{
		int Count = 10;
		while(!PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0) && Count--)
			Sleep(10);

		if(WaitForSingleObject(m_hThread, 10000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	CloseHandle(m_hRenderEvent);

	// Delete all timers in the timer queue.
	DeleteTimerQueue(hTimerQueue);

	if (m_iActivePlugin != -1)
	{
		if (m_VisualArray[m_iActivePlugin].pPlugin)
			m_VisualArray[m_iActivePlugin].pPlugin->Detach();
	}

	if (m_WindowDC)
	{
		ReleaseDC(m_hWnd, m_WindowDC);
		m_WindowDC = NULL;
	}

	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	return true;
}

bool CVisualWindow::Show(void)
{
	if(m_hWnd)
	{
		ShowWindow(m_hWnd, SW_SHOW);
		return(true);
	}

	return(false);
}

bool CVisualWindow::Hide(void)
{
	if(m_hWnd)
	{
		if(m_bFullScreen)
		{
			SetFullscreen(false);
		}		
		ShowWindow(m_hWnd, SW_HIDE);
		return(true);
	}

	return(false);
}

bool CVisualWindow::SetPos(int x, int y, int w, int h)
{
	if(m_hWnd)
	{
		SetWindowPos(m_hWnd, HWND_TOP, x+1, y+1, w-2, h-2, NULL);
		//MoveWindow(m_hWnd, x + 1, y + 1, w - 2, h - 2, FALSE);
		SetRect(&m_OldSize, x + 1, y + 1, w - 2, h - 2);

		return(true);
	}

	return(false);
}

void		CVisualWindow::SetVisualFPS(int iFPS)
{
	DeleteTimerQueue(hTimerQueue);
	hTimerQueue = CreateTimerQueue();
	CreateTimerQueueTimer( &hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, NULL , 0, ((1000 / iFPS) + 1), 0);
}

bool CVisualWindow::GetFullscreen(void)
{
	return m_bFullScreen;
}

void CVisualWindow::SetFullscreen(bool bFull)
{
	if(bFull == m_bFullScreen)
		return;

	LONG_PTR StyleEx = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	LONG_PTR Style = GetWindowLongPtr(m_hWnd, GWL_STYLE);
	if(bFull)
	{
		//StyleEx &= ~WS_EX_CLIENTEDGE;
		Style &= ~WS_CHILD;

		SetParent(m_hWnd, NULL);
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOSENDCHANGING);



		SetTimer(m_hWnd, 0, MOUSE_TIMEOUT, NULL);
	}
	else
	{
		//StyleEx |= WS_EX_CLIENTEDGE;
		Style |= WS_CHILD;

		SetParent(m_hWnd, m_hParentWnd);
		SetWindowPos(m_hWnd, 0, m_OldSize.left, m_OldSize.top, m_OldSize.right, m_OldSize.bottom, SWP_NOZORDER);

		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	//SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, StyleEx);
	SetWindowLongPtr(m_hWnd, GWL_STYLE, Style);
	m_bFullScreen = bFull;
}

#define VISMENU_BASE			(60000)
#define GetWindowStyle(hwnd)	((DWORD)GetWindowLongPtr(hwnd, GWLP_STYLE))

LRESULT CALLBACK CVisualWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			{
				m_ContextMenu = LoadMenu(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDR_VISMENU));

				HMENU hVisList = GetSubMenu(GetSubMenu(m_ContextMenu, 0), 0);
				while(GetMenuItemCount(hVisList))
				{
					DeleteMenu(hVisList, 0, MF_BYPOSITION);
				}

				for(unsigned long item = 0; item < m_VisualArray.GetCount(); item++)
				{
					AppendMenu(hVisList, MF_ENABLED, VISMENU_BASE+item, m_VisualArray[item].pPlugin->GetPluginName());
				}
			}
			break;

		case WM_DESTROY:
			{
				DestroyMenu(m_ContextMenu);
			}
			break;

		case WM_CONTEXTMENU:
			{
				if(m_VisualArray.GetCount() < 1) 
					break;

				POINT		pt;
				POINTSTOPOINT(pt,MAKEPOINTS(lParam));

				CheckMenuRadioItem(GetSubMenu(GetSubMenu(m_ContextMenu, 0), 0), 0, m_VisualArray.GetCount(), m_iActivePlugin, MF_BYPOSITION);

				CheckMenuItem(GetSubMenu(m_ContextMenu, 0), 1, m_bFullScreen == true ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION | MF_UNCHECKED);
				if (m_VisualArray[m_iActivePlugin].pPlugin)
				{
					if (m_VisualArray[m_iActivePlugin].pPlugin->GetFlags() & PLUGINFLAGS_CONFIG)
					{
						EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 3, MF_BYPOSITION | MF_ENABLED);
					}
					else
					{
						EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
					}

					if (m_VisualArray[m_iActivePlugin].pPlugin->GetFlags() & PLUGINFLAGS_ABOUT)
					{
						EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 4, MF_BYPOSITION | MF_ENABLED);
					}
					else
					{
						EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
					}
				}

				TrackPopupMenu(GetSubMenu(m_ContextMenu, 0), 0, pt.x, pt.y, 0, hWnd, NULL);

			}
			break;

		case WM_SIZE:
			{

			}
			break;
		case WM_ERASEBKGND:
			{
				return TRUE;
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				if(wCmdID >= VISMENU_BASE)
				{
					unsigned long iPlugin = wCmdID - VISMENU_BASE;

					SetVisPlugin(iPlugin);

					return(0);
				}

				switch(wCmdID)
				{
					case ID_VIS_CONFIGURE:
						{
							if(m_iActivePlugin != -1)
							{
								m_VisualArray[m_iActivePlugin].pPlugin->Configure(hWnd);
							}
						}
						break;

					case ID_VIS_ABOUT:
						{
							if(m_iActivePlugin != -1)
							{
								m_VisualArray[m_iActivePlugin].pPlugin->About(hWnd);
							}
						}
						break;
					
					case ID_VIS_FULLSCREEN:
						{
							if(m_bFullScreen)
								SetFullscreen(false);
							else
								SetFullscreen(true);
						}
						break;
				}
			}
			break;

		case WM_TIMER:
			{
				SetCursor(NULL);
				KillTimer(hWnd, 0);
			}
			break;

		case WM_MOUSEMOVE:
			{
				if(m_bFullScreen)
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					SetTimer(hWnd, 0, MOUSE_TIMEOUT, NULL);
				}

				if(m_iActivePlugin != -1)
				{
					POINTS pts = POINTS MAKEPOINTS(lParam);

					m_VisualArray[m_iActivePlugin].pPlugin->MouseFunction(VISUAL_MOUSEFUNCTION_MOVE, pts.x, pts.y);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			{
				if(m_iActivePlugin != -1)
				{
					POINTS pts = POINTS MAKEPOINTS(lParam);

					m_VisualArray[m_iActivePlugin].pPlugin->MouseFunction(VISUAL_MOUSEFUNCTION_DOWN, pts.x, pts.y);
				}

				return(DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;

		case WM_LBUTTONUP:
			{
				if(m_iActivePlugin != -1)
				{
					POINTS pts = POINTS MAKEPOINTS(lParam);

					m_VisualArray[m_iActivePlugin].pPlugin->MouseFunction(VISUAL_MOUSEFUNCTION_UP, pts.x, pts.y);
				}

				return(DefWindowProc(hWnd, message, wParam, lParam));

			}
			break;

			case WM_KEYDOWN:
				{
					if(wParam == VK_ESCAPE)
					{
						if (tuniacApp.m_VisualWindow)
						{
							if (tuniacApp.m_VisualWindow->GetFullscreen())
							{
								tuniacApp.m_VisualWindow->SetFullscreen(false);
							}
						}
					}
				}
				break;


		default:
			return(DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}

	return(0);
}

LRESULT CALLBACK CVisualWindow::WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CVisualWindow * pVW;

	if(message == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pVW = (CVisualWindow *)lpCS->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pVW);
	}
	else
	{
		pVW = (CVisualWindow *)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pVW->WndProc(hWnd, message, wParam, lParam));
}

unsigned long CVisualWindow::ThreadStub(void * in)
{
	CVisualWindow * pVis = (CVisualWindow *)in;

	return(pVis->ThreadProc());
}

unsigned long CVisualWindow::ThreadProc(void)
{
	MSG					msg;
	bool				Done = false;
	RECT				r;
	while(!Done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{
				case WM_APP:
					{
						LPARAM iPlugin = msg.lParam;

						if(m_WindowDC)
						{
							if(m_iActivePlugin != -1)
							{
								if (m_VisualArray[m_iActivePlugin].pPlugin)
								{
									m_VisualArray[m_iActivePlugin].pPlugin->Detach();
								}
								m_iActivePlugin = -1;
							}

							ReleaseDC(m_hWnd, m_WindowDC);
							m_WindowDC = NULL;
						}

						if(iPlugin != -1)
						{
							m_WindowDC = GetDC(m_hWnd);
							if(m_WindowDC)
							{
								if(m_VisualArray.GetCount() > 0)
								{
									if (m_VisualArray[iPlugin].pPlugin)
									{
										if (m_VisualArray[iPlugin].pPlugin->Attach(m_WindowDC))
										{
											m_iActivePlugin = iPlugin;
										}
										//visual wont attach, revert to internal visual
										else if (m_VisualArray[0].pPlugin->Attach(m_WindowDC))
										{
											m_iActivePlugin = 0;
											tuniacApp.m_Preferences.SetCurrentVisual(0);
										}
									}
								}
							}
						}
					}
					break;

				case WM_QUIT:
					{
						Done = true;
					}
					break;
			}
		}
		else
		{
			if(::IsWindowVisible(m_hWnd) && !IsIconic(GetParent(m_hWnd)))
			{
				CAutoLock t(&m_RenderLock);
				GetClientRect(m_hWnd, &r);
				if(m_iActivePlugin != -1)
				{
					if (m_VisualArray[m_iActivePlugin].pPlugin)
					{
						m_VisualArray[m_iActivePlugin].pPlugin->Render((int)r.right, (int)r.bottom);
					}
				}
				else
				{
					FillRect(m_WindowDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
				}
			}

			WaitForSingleObject(m_hRenderEvent, INFINITE);

			//not accurate enough 10-16ms accuracy!!
			//WaitForSingleObject(m_hRenderEvent, m_offsettimer.MSToSleepSinceLastCall(1000 / tuniacApp.m_Preferences.GetVisualFPS()));
		}
	}

	return(0);
}

unsigned long CVisualWindow::GetVisData(float * pWaveformData, unsigned long ulNumSamples)
{
	if (CCoreAudio::Instance())
	{
		return CCoreAudio::Instance()->GetVisData(pWaveformData, ulNumSamples);
	}
	return 0;
}

void *	CVisualWindow::GetVariable(Variable eVar)
{
	return tuniacApp.m_PluginManager.GetVariable(eVar);
}

bool	CVisualWindow::GetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	TCHAR szVisualSubKey[128];
	StringCchPrintf(szVisualSubKey, 128, TEXT("visuals\\%s"), szSubKey);
	return tuniacApp.m_Preferences.PluginGetValue(szVisualSubKey, lpValueName, lpType, lpData, lpcbData);
}

bool	CVisualWindow::SetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
	TCHAR szVisualSubKey[128];
	StringCchPrintf(szVisualSubKey, 128, TEXT("visuals\\%s"), szSubKey);
	return tuniacApp.m_Preferences.PluginSetValue(szVisualSubKey, lpValueName, dwType, lpData, cbData);
}

void	CVisualWindow::GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iFromCurrent)
{ // szFormat=NULL to use full format from preferences

	IPlaylistEntry * pIPE = NULL;

	memset(szDest, L'\0', iDestSize);
	if(iFromCurrent == 0)
	{
		IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();
		if(pPlaylist)
			pIPE = pPlaylist->GetActiveEntry();
	}
	else
		pIPE = tuniacApp.GetFuturePlaylistEntry(iFromCurrent - 1);

	if(pIPE == NULL)
		return;

	tuniacApp.FormatSongInfo(szDest, iDestSize, pIPE, szFormat == NULL ? tuniacApp.m_Preferences.GetPluginFormatString() : szFormat, true);
}