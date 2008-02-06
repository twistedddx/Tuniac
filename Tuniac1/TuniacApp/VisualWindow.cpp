#include "StdAfx.h"
#include ".\visualwindow.h"

#include "resource.h"

#include "TuniacVisual.h"

#define MOUSE_TIMEOUT			3000

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

	return true;
}

void			CVisualWindow::Destroy(void)
{
	while(m_VisualArray.GetCount())
	{
		m_VisualArray[0].pPlugin->Destroy();
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
	TCHAR				szVisualsPath[512];
	TCHAR				szFilename[512];

	m_hParentWnd = hParent;

	{
		VisualPlugin	builtin;
		builtin.hInst			= NULL;
		builtin.pPlugin			= new CTuniacVisual;
		builtin.pPlugin->SetHelper(this);
		m_VisualArray.AddTail(builtin);
	}

	GetModuleFileName(NULL, szVisualsPath, 512);
	PathRemoveFileSpec(szVisualsPath);
	PathAddBackslash(szVisualsPath);
	StrCat(szVisualsPath, TEXT("visuals"));
	PathAddBackslash(szVisualsPath);
	StrCpy(szFilename, szVisualsPath);
	StrCat(szFilename, TEXT("*.dll"));

	hFind = FindFirstFile( szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR temp[512];

			StrCpy(temp, szVisualsPath);
			StrCat(temp, w32fd.cFileName);

			HINSTANCE hDLL = LoadLibrary(temp);
			if(hDLL)
			{
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
	wcex.style			= CS_OWNDC | CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
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

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW,
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
		SetVisPlugin(0);
	}
	else
	{
		SetVisPlugin(-1);
	}

	return(true);
}

bool			CVisualWindow::DestroyPluginWindow(void)
{
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
		ShowWindow(m_hWnd, SW_HIDE);
		return(true);
	}

	return(false);
}

bool CVisualWindow::SetPos(int x, int y, int w, int h)
{
	if(m_hWnd)
	{
		SetRect(&m_OldSize, x, y, w, h);
		SetWindowPos(m_hWnd, NULL, x, y, w, h, SWP_NOZORDER);
		return(true);
	}

	return(false);
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

				if(m_VisualArray[m_iActivePlugin].pPlugin->GetFlags() & PLUGINFLAGS_CONFIG)
				{
					EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 3, MF_BYPOSITION | MF_ENABLED);
				}
				else
				{
					EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
				}

				if(m_VisualArray[m_iActivePlugin].pPlugin->GetFlags() & PLUGINFLAGS_ABOUT)
				{
					EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 4, MF_BYPOSITION | MF_ENABLED);
				}
				else
				{
					EnableMenuItem(GetSubMenu(m_ContextMenu, 0), 4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
				}

				TrackPopupMenu(GetSubMenu(m_ContextMenu, 0), 0, pt.x, pt.y, 0, hWnd, NULL);
			}
			break;

		case WM_SIZE:
		case WM_ERASEBKGND:
			{
				SetEvent(m_hRenderEvent);
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
							{
								unsigned long StyleEx = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
								StyleEx |= WS_EX_CLIENTEDGE;
								SetWindowLongPtr(hWnd, GWL_EXSTYLE, StyleEx);

								unsigned long Style = GetWindowLongPtr(hWnd, GWL_STYLE);
								Style |= WS_CHILD;
								SetWindowLongPtr(hWnd, GWL_STYLE, Style);

								SetParent(hWnd, m_hParentWnd);
								SetWindowPos(hWnd, 0, m_OldSize.left, m_OldSize.top, m_OldSize.right, m_OldSize.bottom, SWP_NOZORDER);

								SetCursor(LoadCursor(NULL, IDC_ARROW));

								m_bFullScreen = false;
							}
							else
							{
								unsigned long StyleEx = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
								StyleEx &= ~WS_EX_CLIENTEDGE;
								SetWindowLongPtr(hWnd, GWL_EXSTYLE, StyleEx);

								unsigned long Style = GetWindowLongPtr(hWnd, GWL_STYLE);
								Style &= ~WS_CHILD;
								SetWindowLongPtr(hWnd, GWL_STYLE, Style);

								SetParent(hWnd, NULL);
								SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
								
								m_bFullScreen = true;

								SetTimer(hWnd, 0, MOUSE_TIMEOUT, NULL);
							}
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
			}
			break;

		case WM_LBUTTONUP:
			{
				if(m_iActivePlugin != -1)
				{
					POINTS pts = POINTS MAKEPOINTS(lParam);

					m_VisualArray[m_iActivePlugin].pPlugin->MouseFunction(VISUAL_MOUSEFUNCTION_UP, pts.x, pts.y);
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

	while(!Done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{
				case WM_APP:
					{
						int iPlugin = msg.lParam;

						if(m_WindowDC)
						{
							if(m_iActivePlugin != -1)
							{
								m_VisualArray[m_iActivePlugin].pPlugin->Detach();
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
									if(m_VisualArray[iPlugin].pPlugin->Attach(m_WindowDC))
									{
										m_iActivePlugin = iPlugin;
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
				RECT r;

				CAutoLock t(&m_RenderLock);
				if(m_iActivePlugin != -1)
				{
					GetClientRect(m_hWnd, &r);
					m_VisualArray[m_iActivePlugin].pPlugin->Render(r.right, r.bottom);
				}
				else
				{
					FillRect(m_WindowDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
				}
			}

			WaitForSingleObject(m_hRenderEvent, (unsigned long)(1000.0f / tuniacApp.m_Preferences.GetVisualFPS()) );
		}
	}

	return(0);
}

bool	CVisualWindow::GetVisData(float * pWaveformData, unsigned long ulNumSamples)
{
	return CCoreAudio::Instance()->GetVisData(pWaveformData, ulNumSamples);
}

void *	CVisualWindow::GetVariable(Variable eVar)
{
	return tuniacApp.m_PluginManager.GetVariable(eVar);
}
