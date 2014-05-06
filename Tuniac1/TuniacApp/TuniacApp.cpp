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

// TuniacApp.cpp: implementation of the CTuniacApp class.
//m_hInstance
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <winsock2.h>
//#include <windows.h>
//#include <shellapi.h>
//#include <shobjidl.h>

//#include <strsafe.h>

#include "tuniacapp.h"
#include "resource.h"

#include "AboutWindow.h"


#define szClassName			TEXT("TUNIACWINDOWCLASS")

static UINT WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTuniacApp::CTuniacApp()
{
}

CTuniacApp::~CTuniacApp()
{
}

bool CTuniacApp::Initialize(HINSTANCE hInstance, LPTSTR szCommandLine)
{
	//load tuniac
	//SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	m_hInstance = hInstance;

	//one instance
	m_hOneInstanceOnlyMutex = CreateMutex(NULL, FALSE, szClassName);
	if(GetLastError() == ERROR_ALREADY_EXISTS) 
	{		
		//get the other instance of tuniac so we can send command lines to it
		unsigned long counttry = 0;
		HWND hOtherWnd = FindWindow(szClassName, NULL);
		while(!hOtherWnd)
		{
			Sleep(100);
			hOtherWnd = FindWindow(szClassName, NULL);
			if(counttry++>1000)
				return(0);
		}

		TCHAR szFilename[MAX_PATH];
		GetModuleFileName(NULL, szFilename, MAX_PATH);

		//if we have no command lines so just restore tuniac
		//the commandline gets quotation marks and a space, so remove 3 chars
		if((wcslen(szCommandLine) - 3) == wcsnlen_s(szFilename, 260))
			wcscat(szCommandLine, L"-restore");

		//relay command line to existing tuniacapp.exe window
		COPYDATASTRUCT cds;

		cds.dwData = 0;
		cds.cbData = (wcslen(szCommandLine) + 1) * sizeof(TCHAR);
		cds.lpData = szCommandLine;

		SendMessage(hOtherWnd, WM_COPYDATA, NULL, (LPARAM)&cds);

		CloseHandle(m_hOneInstanceOnlyMutex);
		return(0);
	}

	CoInitialize(NULL);
	InitCommonControls();

	WORD w = MAKEWORD(1,1);
	WSADATA wsadata;
	::WSAStartup(w, &wsadata); 

	m_hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWINDOWACCEL));

	/*
	HKEY		hCPUKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\3", 0, KEY_QUERY_VALUE, &hCPUKey) == ERROR_SUCCESS)
		m_iCPUCount = 4;
	else if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\2", 0, KEY_QUERY_VALUE, &hCPUKey) == ERROR_SUCCESS)
		m_iCPUCount = 3;
	else if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\1", 0, KEY_QUERY_VALUE, &hCPUKey) == ERROR_SUCCESS)
		m_iCPUCount = 2;
	else
		m_iCPUCount = 1;
	*/

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	m_dwWinVer = osvi.dwMajorVersion;

	//these are used for the portable switch to force not saving to disk
	m_bSavePrefs = true;
	m_bSaveML = true;

	//used to pause/resume on Windows suspends like sleep or hibernate or user switch/lock/logoff
	m_WasPlaying = false;
	m_WasPlayingTime = 0;
	m_bScreensaveActive = false;
	m_iMuteVol = 0;

	//load everything saved including windows size/postion/ontop
	m_Preferences.LoadPreferences();

	//start the skin engine
	m_Skin.Initialize();

	m_SoftPause.bNow = m_Preferences.GetAutoSoftPause();
	m_SoftPause.ulAt = INVALID_PLAYLIST_INDEX;

	m_iFailedSongRetry = 0;

	m_ActiveScreen = m_Preferences.GetActiveWindow();

	//fail if we cant start something
	if(!CCoreAudio::Instance()->Startup())
		return false;

	CCoreAudio::Instance()->SetVolumePercent(m_Preferences.GetVolumePercent());
	CCoreAudio::Instance()->EnableEQ(m_Preferences.GetEQEnabled());
	CCoreAudio::Instance()->SetEQGain(m_Preferences.GetEQLowGain(), m_Preferences.GetEQMidGain(), m_Preferences.GetEQHighGain());
	CCoreAudio::Instance()->SetAmpGain(m_Preferences.GetAmpGain());
	CCoreAudio::Instance()->SetAudioBufferSize(m_Preferences.GetAudioBuffering());
	CCoreAudio::Instance()->EnableReplayGain(m_Preferences.ReplayGainEnabled());
	CCoreAudio::Instance()->ReplayGainUseAlbumGain(m_Preferences.ReplayGainUseAlbumGain());

	if (!m_History.Initialize())
		return false;

	if(!m_MediaLibrary.Initialize())
		return false;

	if(!m_PlaylistManager.Initialize())
		return false;

	IWindow * t;

	//create the main medialibrary window	
	m_SourceSelectorWindow = new CSourceSelectorWindow;
	t = m_SourceSelectorWindow;
	m_WindowArray.AddTail(t);

	//create visual window
	m_VisualWindow = new CVisualWindow;
	t = m_VisualWindow;
	m_WindowArray.AddTail(t);

	//create log window
	m_LogWindow = new CLogWindow();
	t = m_LogWindow;
	m_WindowArray.AddTail(t);

	//theme the tray icon?
	m_wc.cbSize			= sizeof(WNDCLASSEX); 
	m_wc.style			= 0;
	m_wc.lpfnWndProc	= (WNDPROC)WndProcStub;
	m_wc.cbClsExtra		= 0;
	m_wc.cbWndExtra		= 0;
	m_wc.hInstance		= hInstance;
	m_wc.hIcon			= m_Skin.GetIcon(THEMEICON_WINDOW);
	m_wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	m_wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	m_wc.hIconSm		= m_Skin.GetIcon(THEMEICON_WINDOW_SMALL);
	m_wc.lpszMenuName	= MAKEINTRESOURCE(IDR_TUNIAC_MENU);
	m_wc.lpszClassName	= szClassName;
	if(!RegisterClassEx(&m_wc))
		return false;

	//move tuniac back on screen if moved off
	RECT r;
	CopyRect(&r, m_Preferences.GetMainWindowRect());

	if(r.top < 0)
		r.top	= 0;
	if(r.left < 0)
		r.left	= 0;
	if(r.right < 530)
		r.right = 530;
	if(r.bottom < 400)
		r.bottom = 400;

	//create our main window
	m_hWnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_APPWINDOW,
							szClassName, 
							TEXT("Tuniac"), 
							WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							r.left, 
							r.top, 
							r.right, 
							r.bottom, 
							NULL,
							NULL, 
							hInstance,
							this);

	//window did not create!
	if(!m_hWnd)
		return false;

	if(!m_PluginManager.Initialize())
		return false;

	//create tray menu
	m_TrayMenu = GetSubMenu(LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_TRAYMENU)), 0);
	CheckMenuItem(m_TrayMenu, ID_PLAYBACK_TOGGLE_SHUFFLE, m_Preferences.GetShuffleState() ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
	CheckMenuItem(m_TrayMenu, ID_PLAYBACK_SOFTPAUSE, m_SoftPause.bNow ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
	CheckMenuRadioItem(GetSubMenu(m_TrayMenu, 3), 0, 3, m_Preferences.GetRepeatMode(), MF_BYPOSITION);
	EnableMenuItem(GetSubMenu(m_TrayMenu, 3), RepeatAllQueued, MF_BYPOSITION | (m_Queue.GetCount() == 0 ? MF_GRAYED : MF_ENABLED));

	m_Taskbar.Initialize(m_hWnd, WM_TRAYICON);

	if(m_Preferences.GetTrayIconMode() == TrayIconMinimize)
		m_Taskbar.Show();

	//use any command lines given(initial open only all others in one instance code)
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = (wcslen(szCommandLine) + 1) * sizeof(TCHAR);
	cds.lpData = szCommandLine;
	SendMessage(m_hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);

	if(m_Preferences.GetMainWindowMaximized())
	{
		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	}
	else if(m_Preferences.GetMainWindowMinimized())
	{
		ShowWindow(m_hWnd, SW_SHOWMINIMIZED);
		if(m_Preferences.GetTrayIconMode() == TrayIconMinimize)
			ShowWindow(m_hWnd, SW_HIDE);
	}
	else
	{
		ShowWindow(m_hWnd, SW_SHOWNORMAL);
	}

	//register our own hotkeys
	RegisterHotkeys();

	//show where we are upto in the playlist
	m_SourceSelectorWindow->ShowPlaylistAtIndex(m_PlaylistManager.GetActivePlaylistIndex());
	IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
	if(pPlaylist)
	{
		if(pPlaylist->GetActiveFilteredIndex() == INVALID_PLAYLIST_INDEX)
		{
			TCHAR szURL[MAX_PATH];
			GetModuleFileName(NULL, szURL, MAX_PATH);
			PathRemoveFileSpec(szURL);
			m_AlbumArtPanel.SetCurrentArtSource(szURL);
			PathAppend(szURL, TEXT("NoAlbumArt.jpg"));
			m_AlbumArtPanel.SetSource(szURL);
		}
		else
		{
			//notice of setting song from playlist load
			m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_INIT, NULL, NULL);
		}
	}

	if(m_Preferences.GetFollowCurrentSongMode())
	{
		if(m_SourceSelectorWindow)
			m_SourceSelectorWindow->ShowCurrentlyPlaying();
	}
	if(m_Preferences.GetShowVisArt())
	{
		if(m_VisualWindow)
			m_VisualWindow->Show();
	}

	m_WindowArray[m_Preferences.GetActiveWindow()]->Show();

	//set window title
	UpdateTitles();

	//set always ontop state
	if(m_Preferences.GetAlwaysOnTop())
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	//notify that before we created the playlist window
	PostMessage(m_hWnd, WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);

	WTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_THIS_SESSION);
	SetTimer(m_hWnd, SYSEVENTS_TIMERID, 5000, NULL);

	return true;
}

bool CTuniacApp::Shutdown()
{
	//close tuniac

	//if allowed to save pl+ml, check if %appdata%/Tuniac needs to be created
	if(m_bSaveML)
	{
		TCHAR				szURL[MAX_PATH];
		if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szURL ) ) )
		{
			PathAppend( szURL, TEXT("\\Tuniac") );
			CreateDirectory( szURL, 0);
		}
	}

	//save if allowed
	m_PlaylistManager.Shutdown(m_bSaveML);
	m_MediaLibrary.Shutdown(m_bSaveML);

	m_PluginManager.Shutdown();

	m_Preferences.SetActiveWindow(m_ActiveScreen);

	while(m_WindowArray.GetCount())
	{
		m_WindowArray[0]->Destroy();
		m_WindowArray.RemoveAt(0);
	}

	//save prefs if allowed
	if(m_bSavePrefs)
	{
		m_Preferences.SetVolumePercent(CCoreAudio::Instance()->GetVolumePercent());
		m_Preferences.SavePreferences();
	}

	CCoreAudio::Instance()->Shutdown();

	m_Taskbar.Shutdown();

	WTSUnRegisterSessionNotification(m_hWnd);
	KillTimer(m_hWnd, SYSEVENTS_TIMERID);

	::WSACleanup();

	CoUninitialize();

	return true;
}

bool CTuniacApp::Run()
{
	MSG	msg;

	while(GetMessage(&msg, NULL, 0 , 0))
	{
		if(!TranslateAccelerator(m_hWnd, m_hAccel, &msg))
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

LRESULT CALLBACK CTuniacApp::WndParentProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, message, wParam, lParam));
}

LRESULT CALLBACK CTuniacApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_TIMER:
		{
			if(wParam == SYSEVENTS_TIMERID)
			{
				BOOL bActive = false;
				SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bActive, 0);
				if(bActive)
				{
					m_bScreensaveActive = true;
					if(m_Preferences.GetPauseOnScreensave())
					{
						if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
						{
							m_WasPlaying = true;
							SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
							if(m_Preferences.GetRememberPos())
								m_WasPlayingTime = CCoreAudio::Instance()->GetPosition();
						}
					}
				}
				else if(m_bScreensaveActive)
				{
					m_bScreensaveActive = false;
					if(m_Preferences.GetResumeOnScreensave())
						SetTimer(m_hWnd, DELAYEDPLAY_TIMERID, (m_Preferences.GetDelayInSecs()*1000), NULL);
				}
				break;
			}
			if(wParam == DELAYEDPLAY_TIMERID)
			{
				KillTimer(m_hWnd, DELAYEDPLAY_TIMERID);
				IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
				if(pPlaylist)
				{
					IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();
					if(pIPE)
					{
						PlayEntry(pIPE, m_WasPlaying, true);
						if(m_Preferences.GetRememberPos())
							CCoreAudio::Instance()->SetPosition(m_WasPlayingTime);
						m_WasPlayingTime = 0;
						m_WasPlaying = false;
					}
				}
				break;
			}
		}
		break;

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
				m_PlayControls.Create(hWnd);

				HDC hDC = GetDC(hWnd);

				m_TinyFont =  CreateFont(	-MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
											0,
											0,
											0,
											0,
											0,
											0,
											0,
											DEFAULT_CHARSET,
											OUT_TT_PRECIS,
											CLIP_DEFAULT_PRECIS,
											PROOF_QUALITY,
											DEFAULT_PITCH | FF_DONTCARE, 
											TEXT("Trebuchet MS"));

				m_SmallFont =  CreateFont(	-MulDiv(9, GetDeviceCaps(hDC, LOGPIXELSY), 72),
											0,
											0,
											0,
											0,
											0,
											0,
											0,
											DEFAULT_CHARSET,
											OUT_TT_PRECIS,
											CLIP_DEFAULT_PRECIS,
											PROOF_QUALITY,
											DEFAULT_PITCH | FF_DONTCARE, 
											TEXT("Trebuchet MS"));

				m_SmallMediumFont =  CreateFont(	-MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72),
													0,
													0,
													0,
													0,
													0,
													0,
													0,
													DEFAULT_CHARSET,
													OUT_TT_PRECIS,
													CLIP_DEFAULT_PRECIS,
													PROOF_QUALITY,
													DEFAULT_PITCH | FF_DONTCARE, 
													TEXT("Trebuchet MS"));


				m_MediumFont =  CreateFont(	-MulDiv(13, GetDeviceCaps(hDC, LOGPIXELSY), 72),
											0,
											0,
											0,
											0,
											0,
											0,
											0,
											DEFAULT_CHARSET,
											OUT_TT_PRECIS,
											CLIP_DEFAULT_PRECIS,
											PROOF_QUALITY,
											DEFAULT_PITCH | FF_DONTCARE, 
											TEXT("Trebuchet MS"));

				m_LargeFont =  CreateFont(	-MulDiv(15, GetDeviceCaps(hDC, LOGPIXELSY), 72),
											0,
											0,
											0,
											0,
											0,
											0,
											0,
											DEFAULT_CHARSET,
											OUT_TT_PRECIS,
											CLIP_DEFAULT_PRECIS,
											PROOF_QUALITY,
											DEFAULT_PITCH | FF_DONTCARE, 
											TEXT("Trebuchet MS"));

				ReleaseDC(hWnd, hDC);

				//get main window menus
				m_hPopupMenu = GetMenu(hWnd);

				//remove dummy "Home" menu item
				DeleteMenu(GetSubMenu(m_hPopupMenu, 3), 0, MF_BYPOSITION);

				for(unsigned long x=0; x<m_WindowArray.GetCount(); x++)
				{
					if(m_WindowArray[x]->CreatePluginWindow(hWnd, m_hInstance))
					{
						AppendMenu(GetSubMenu(m_hPopupMenu, 3), MF_ENABLED, MENU_BASE+x, m_WindowArray[x]->GetName());
					}
					else
					{
						if (tuniacApp.m_LogWindow)
						{
							tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("Error Creating Plugin Window."));
						}
						MessageBox(hWnd, TEXT("Error Creating Plugin Window."), TEXT("Non Fatal Error..."), MB_OK | MB_ICONSTOP);

						m_WindowArray[x]->Destroy();
						m_WindowArray.RemoveAt(x);
						x--;
					}
				}

				//create prefs window
				TCHAR szPrefPageTitle[64];
				for(unsigned int iPrefPage = 0; iPrefPage < m_Preferences.GetPreferencesPageCount(); iPrefPage++)
				{
					if(!m_Preferences.GetPreferencesPageName(iPrefPage, szPrefPageTitle, 64))
						break;
					AppendMenu(GetSubMenu(GetSubMenu(m_hPopupMenu, 1), 6), MF_ENABLED, PREFERENCESMENU_BASE + iPrefPage, szPrefPageTitle);
				}

				//create the future menu(right click "next" button)
				m_hFutureMenu = CreatePopupMenu();
				if (tuniacApp.m_LogWindow)
				{
					m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("Initialization Complete"));
				}
			}
			break;

		case WM_DESTROY:
			{
				WINDOWPLACEMENT wp;
				wp.length = sizeof(WINDOWPLACEMENT);
				GetWindowPlacement(hWnd, &wp);
				
				RECT r = wp.rcNormalPosition;
				//GetWindowRect(hWnd, &r);

				SetRect(&r, r.left, r.top, r.right - r.left, r.bottom - r.top);
				m_Preferences.SetMainWindowRect(&r);

				for(unsigned long x=0; x<m_WindowArray.GetCount(); x++)
				{
					m_WindowArray[x]->DestroyPluginWindow();
				}

				DestroyWindow(m_hWndStatus);
				DestroyMenu(m_hFutureMenu);

				PostQuitMessage(0);
			}
			break;

			//a shutdown is in progress
		case WM_ENDSESSION:
			{
				if(wParam)
					//DestroyWindow(hWnd);
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_FILE_FORCESAVEMEDIALIBRARY, 0), 0);
			}
			break;

		case WM_WTSSESSION_CHANGE:
			{
				if(wParam == WTS_SESSION_LOCK && m_Preferences.GetPauseOnLock() ||
					wParam == WTS_CONSOLE_DISCONNECT && m_Preferences.GetPauseOnSwitch())
				{
					if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
					{
						m_WasPlaying = true;
						SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
						if(m_Preferences.GetRememberPos())
							m_WasPlayingTime = CCoreAudio::Instance()->GetPosition();
					}
				}

				if(wParam == WTS_SESSION_UNLOCK && m_Preferences.GetResumeOnLock() ||
					wParam == WTS_CONSOLE_CONNECT && m_Preferences.GetResumeOnSwitch())
				{
					SetTimer(m_hWnd, DELAYEDPLAY_TIMERID, (m_Preferences.GetDelayInSecs()*1000), NULL);
				}
			}
			break;

			//the cross at top right of window clicked
		case WM_CLOSE:
			{
				//check if minimize to tray on close, still close if holding control key
				if(m_Preferences.GetMinimizeOnClose() && !(GetKeyState(VK_CONTROL) & 0x8000))
				{
					m_Preferences.SetMainWindowMinimized(true);
					ShowWindow(hWnd, SW_MINIMIZE);
				}
				else
				{
					DestroyWindow(hWnd);
				}
			}
			break;
		/*
		case WM_POWERBROADCAST:
			{
				if(wParam == PBT_APMSUSPEND)
				{
					//pause at suspend
					if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
					{
						m_WasPlaying = true;
						SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
					}
				}

				if(wParam == PBT_APMRESUMEAUTOMATIC)
				{
					//resume
					IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
					if(pPlaylist)
					{
						IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();
						if(pIPE)
						{
							Sleep(3000);
							PlayEntry(pIPE, m_WasPlaying, true);
							m_WasPlaying = false;
						}
					}
				}

			}
			break;
		*/
			//resize limits
		case WM_GETMINMAXINFO:
			{
				if(lParam)
				{	
					LPMINMAXINFO lpMinMaxInfo = (LPMINMAXINFO)lParam;
					lpMinMaxInfo->ptMinTrackSize.x = 500;
					lpMinMaxInfo->ptMinTrackSize.y = 445;
				}
			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);
				RECT playcontrolsrect = {0,0, Width, 60};
				RECT statusRect;

				if(wParam == SIZE_MINIMIZED)
				{
					m_VisualWindow->SetFullscreen(false);
					m_Preferences.SetMainWindowMinimized(true);
					if(m_Preferences.GetTrayIconMode() == TrayIconMinimize)
						ShowWindow(hWnd, SW_HIDE);
				}
				else
					m_Preferences.SetMainWindowMinimized(false);

				if(wParam==SIZE_MAXIMIZED)
					m_Preferences.SetMainWindowMaximized(true);
				else
					m_Preferences.SetMainWindowMaximized(false);

				m_PlayControls.Move(playcontrolsrect.left, playcontrolsrect.top, playcontrolsrect.right, playcontrolsrect.bottom);

				SendMessage(m_hWndStatus, message, wParam, lParam);
				SendMessage(m_hWndStatus, SB_GETRECT, 0, (LPARAM)&statusRect);

				for(unsigned long x=0; x < m_WindowArray.GetCount(); x++)
				{
					if(m_Preferences.GetShowVisArt() && (wcscmp(GetActiveScreenName(), L"Source Selector") == 0) && (wcscmp(m_WindowArray[x]->GetName(), L"Visuals") == 0))
						continue;

					m_WindowArray[x]->SetPos(	0,
												playcontrolsrect.bottom,
												Width,
												Height - statusRect.bottom - playcontrolsrect.bottom);
				}
			}
			break;

		case WM_ERASEBKGND:
			{
				return true;
			}
			break;

			//file dropped on tuniacapp.exe
		case WM_DROPFILES:
			{
				if(wParam)
				{
					HDROP hDrop = (HDROP)wParam;

					TCHAR szURL[MAX_PATH];

					UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);

					if(m_MediaLibrary.BeginAdd(uNumFiles))
					{
						for(UINT file = 0; file<uNumFiles; file++)
						{
							DragQueryFile(hDrop, file, szURL, MAX_PATH);
							m_MediaLibrary.AddItem(szURL, false);
						}

						m_MediaLibrary.EndAdd();
					}

					DragFinish(hDrop);
				}
			}
			break;

			//set menu
		case WM_MENUSELECT:
			{
				CheckMenuItem(GetSubMenu(m_hPopupMenu, 1), ID_EDIT_ALWAYSONTOP, m_Preferences.GetAlwaysOnTop() ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);

				CheckMenuItem(GetSubMenu(m_hPopupMenu, 2), ID_PLAYBACK_TOGGLE_SHUFFLE, m_Preferences.GetShuffleState() ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
				CheckMenuItem(m_TrayMenu, ID_PLAYBACK_TOGGLE_SHUFFLE, m_Preferences.GetShuffleState() ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);

				CheckMenuRadioItem(GetSubMenu(GetSubMenu(m_hPopupMenu, 2), 10), 0, 3, m_Preferences.GetRepeatMode(), MF_BYPOSITION);
				EnableMenuItem(GetSubMenu(GetSubMenu(m_hPopupMenu, 2), 10), RepeatAllQueued, MF_BYPOSITION | (m_Queue.GetCount() == 0 ? MF_GRAYED : MF_ENABLED));
				CheckMenuRadioItem(GetSubMenu(m_TrayMenu, 3), 0, 3, m_Preferences.GetRepeatMode(), MF_BYPOSITION);
				EnableMenuItem(GetSubMenu(m_TrayMenu, 3), RepeatAllQueued, MF_BYPOSITION | (m_Queue.GetCount() == 0 ? MF_GRAYED : MF_ENABLED));

				EnableMenuItem(GetSubMenu(m_hPopupMenu, 2), ID_PLAYBACK_CLEARQUEUE,m_Queue.GetCount() == 0 ? MF_GRAYED : MF_ENABLED);
				EnableMenuItem(GetSubMenu(m_hPopupMenu, 2), ID_PLAYBACK_CLEARPAUSEAT, m_SoftPause.ulAt == INVALID_PLAYLIST_INDEX ? MF_GRAYED : MF_ENABLED);

				CheckMenuItem(GetSubMenu(m_hPopupMenu, 2), ID_PLAYBACK_SOFTPAUSE, m_SoftPause.bNow ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
				CheckMenuItem(m_TrayMenu, ID_PLAYBACK_SOFTPAUSE, m_SoftPause.bNow ? MF_CHECKED | MF_BYCOMMAND : MF_UNCHECKED | MF_BYCOMMAND);
			}
			break;

		//media keys
		case WM_KEYDOWN:
			{
				switch(wParam)
				{
					case VK_MEDIA_PLAY_PAUSE:
					{
							if (!tuniacApp.m_Preferences.GetNoVKHotkeys())
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("VK_MEDIA_PLAY_PAUSE event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
							}
						}
						break;

					case VK_MEDIA_STOP:
						{
							if (!tuniacApp.m_Preferences.GetNoVKHotkeys())
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("VK_MEDIA_STOP event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_STOP, 0), 0);
							}
						}
						break;

					case VK_MEDIA_NEXT_TRACK:
						{
							if (!tuniacApp.m_Preferences.GetNoVKHotkeys())
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("VK_MEDIA_NEXT_TRACK event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
							}
						}
						break;

					case VK_MEDIA_PREV_TRACK:
						{
							if (!tuniacApp.m_Preferences.GetNoVKHotkeys())
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("VK_MEDIA_PREV_TRACK event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
							}
						}
						break;
						/* these are handled in the OS almost always, so lets not double up
					case VK_VOLUME_UP:
						{
							SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEUP, 0), 0);
						}
						break;

					case VK_VOLUME_DOWN:
						{
							SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEDOWN, 0), 0);
						}
						break;

					case VK_VOLUME_MUTE:
						{
							SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEMUTE, 0), 0);
						}
						break;
						*/
				}
			}
			break;


			//multimedia keyboard keys
		case WM_APPCOMMAND:
			{
				if(lParam)
				{
					DWORD cmd  = GET_APPCOMMAND_LPARAM(lParam);

					switch(cmd)
					{
						case APPCOMMAND_MEDIA_PLAY_PAUSE:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_PLAY_PAUSE event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_STOP:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_STOP event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_STOP, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_PAUSE:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_PAUSE event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_PLAY:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_PLAY event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_NEXTTRACK:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_NEXTTRACK event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_PREVIOUSTRACK:
							{
								if (tuniacApp.m_LogWindow)
								{
									tuniacApp.m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("APPCOMMAND_MEDIA_PREVIOUSTRACK event"));
								}
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_FAST_FORWARD:
							{
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_SEEKFORWARD, 0), 0);
							}
							break;

						case APPCOMMAND_MEDIA_REWIND:
							{
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_SEEKBACK, 0), 0);
							}
							break;
						/* these are handled in the OS almost always, so lets not double up
						case APPCOMMAND_VOLUME_UP:
							{
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEUP, 0), 0);
							}
							break;

						case APPCOMMAND_VOLUME_DOWN:
							{
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEDOWN, 0), 0);
							}
							break;

						case APPCOMMAND_VOLUME_MUTE:
							{
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEMUTE, 0), 0);
							}
							break;
							*/
					}
				}
				return(DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;

		case WM_APP:
			{
				switch (wParam)
				{
					//start of crossfade. Next Song needed if required
					case NOTIFY_COREAUDIO_MIXPOINTREACHED:
						{
							// we only care about this if crossfading is enabled, Core Audio will notify us anyway, its up to us to ignore it!!!
							if(m_Preferences.CrossfadingEnabled())
							{
								IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
								if(pPlaylist)
								{
									//Check if we are allowed to crossfade eg dont crossfade cd playlist
									if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_DONTCROSSFADE)
										break;

									//do we have a valid next song, if not we have run out of songs(end of playlist?)
									unsigned long ulNextFilteredIndex = pPlaylist->Next();
									if(ulNextFilteredIndex != INVALID_PLAYLIST_INDEX)
										//play the song we got
										PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulNextFilteredIndex), true, true, false);
									//else
									//no next song??
								}
							}
						}
						break;

					//audiostream has finished a song
					case NOTIFY_COREAUDIO_PLAYBACKFINISHED:
						{
							// Core Audio will send this too. its up to us to decide what we want to do about it eh
							IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
							if(pPlaylist)
							{
								if(!m_Preferences.CrossfadingEnabled() || pPlaylist->GetFlags() & PLAYLIST_FLAGS_DONTCROSSFADE)
								{
									//do we have a valid next song, if not we have run out of songs(end of playlist?)
									unsigned long ulNextFilteredIndex = pPlaylist->Next();
									if(ulNextFilteredIndex != INVALID_PLAYLIST_INDEX)
										//play the song we got
										PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulNextFilteredIndex), true, true);
									else
										//no next song??
										UpdateState();
								}
								else
								{
									//no next song??
									if(pPlaylist->Next() == INVALID_PLAYLIST_INDEX)
										UpdateState();
								}
							}
								
							//clear out old streams from crossfades and last song
							CCoreAudio::Instance()->CheckOldStreams();

						}
						break;

					//audiostream has finished a song unexpectedly
					case NOTIFY_COREAUDIO_PLAYBACKFAILED:
						{
							IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
							if(pPlaylist)
							{
								IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();
								if(pIPE)
								{
									if (tuniacApp.m_LogWindow)
									{
										TCHAR szMessage[_MAX_PATH + 100];
										StringCchPrintf(szMessage, 128, TEXT("File %s failed to playback."),
											pIPE->GetField(FIELD_URL));
										m_LogWindow->LogMessage(TEXT("TuniacApp"), szMessage);
									}

									pIPE->SetField(FIELD_AVAILABILITY, (unsigned long)AVAILABLILITY_UNAVAILABLE);
									//redraw window
									m_SourceSelectorWindow->UpdateView();
								}

							}

							//CoreAudio couldnt open the last song we told it to, try again
							//after 10 failed songs in a row we stop automatically going to the next song
							if(m_iFailedSongRetry < 10)
							{
								m_iFailedSongRetry++;

								if(pPlaylist)
								{
									unsigned long ulNextFilteredIndex = pPlaylist->Next();
									if(ulNextFilteredIndex != INVALID_PLAYLIST_INDEX)
										//play the song we got
										PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulNextFilteredIndex), true, true, false);
									else
										//no next song??
										UpdateState();
								}

								//clear out old streams from crossfades and last song
								CCoreAudio::Instance()->CheckOldStreams();
							}
							else
							{
								if (tuniacApp.m_LogWindow)
								{
									m_LogWindow->LogMessage(TEXT("TuniacApp"), TEXT("10 files in a row failed playback. Drive offline?"));
								}
								UpdateState();
								m_SourceSelectorWindow->UpdateView();
							}
						}
						break;

					// coreaudio now classes this song as being played (1/4 played point)
					case NOTIFY_COREAUDIO_PLAYEDPOINTREACHED:
						{
						//update played count and date played when more than 1/4 the way through
							LPTSTR szURL = (LPTSTR)lParam;
							IPlaylistEntry * pIPE = m_MediaLibrary.GetEntryByURL(szURL);

							if(pIPE)
							{
								SYSTEMTIME st;
								GetLocalTime(&st);
								pIPE->SetField(FIELD_DATELASTPLAYED, &st);
								pIPE->SetField(FIELD_PLAYCOUNT, (unsigned long)pIPE->GetField(FIELD_PLAYCOUNT)+1);
								
								m_SourceSelectorWindow->UpdateView();
								m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
								m_PluginManager.PostMessage(PLUGINNOTIFY_SONGPLAYED, NULL, NULL);
							}
						}
						break;

					//playlistmanager changed it's view
					case NOTIFY_PLAYLISTSCHANGED:
						{
							HMENU tMenu = GetSubMenu(m_TrayMenu, 12);
							if(IsMenu(tMenu))
							{
								while(GetMenuItemCount(tMenu))
								{
									DeleteMenu(tMenu, 0, MF_BYPOSITION);
								}

								for(unsigned long item = 0; item < m_PlaylistManager.GetNumPlaylists(); item++)
								{
									AppendMenu(tMenu, MF_ENABLED, TRAYMENU_BASE+item, m_PlaylistManager.GetPlaylistAtIndex(item)->GetPlaylistName());
								}
							}
						}
						break;

					case NOTIFY_COREAUDIO_RESET:
						{
							bool bWasPlaying = false;
							unsigned long ulWasPlayingTime = 0;

							if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
							{
								bWasPlaying = true;
								ulWasPlayingTime = CCoreAudio::Instance()->GetPosition();
							}

							CCoreAudio::Instance()->Shutdown();
							CCoreAudio::Instance()->Startup();

							if(bWasPlaying)
							{
								IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
								if(pPlaylist)
									//play the current song
									PlayEntry(pPlaylist->GetActiveEntry(), true, false);

								CCoreAudio::Instance()->SetPosition(ulWasPlayingTime);
							}
							UpdateState();
						}
						break;

					//audiostream has started a song
					case NOTIFY_COREAUDIO_PLAYBACKSTARTED:
						{

							IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
							if(pPlaylist)
							{
								IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();
								if(pIPE)
								{
									if((int)pIPE->GetField(FIELD_AVAILABILITY) == 1)
									{
										pIPE->SetField(FIELD_AVAILABILITY, (unsigned long)AVAILABLILITY_AVAILABLE);
										//redraw window
										m_SourceSelectorWindow->UpdateView();
									}
								}

							}

							if(m_Preferences.GetAutoSoftPause())
								m_SoftPause.bNow = true;
							m_iFailedSongRetry = 0;
						}
						break;
				}
			}
			break;

		//our defined hotkeys
		case WM_HOTKEY:
		{
				if (wParam == HOTKEY_PLAY)
				{
					if (tuniacApp.m_LogWindow)
					{
						tuniacApp.m_LogWindow->LogMessage(TEXT("Hotkeys"), TEXT("HOTKEY_PLAY event"));
					}
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
				}

				if (wParam == HOTKEY_STOP)
				{
					if (tuniacApp.m_LogWindow)
					{
						tuniacApp.m_LogWindow->LogMessage(TEXT("Hotkeys"), TEXT("HOTKEY_STOP event"));
					}
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_STOP, 0), 0);
				}

				else if (wParam == HOTKEY_NEXT)
				{
					if (tuniacApp.m_LogWindow)
					{
						tuniacApp.m_LogWindow->LogMessage(TEXT("Hotkeys"), TEXT("HOTKEY_NEXT event"));
					}
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
				}

				else if (wParam == HOTKEY_RANDNEXT)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_RANDOMNEXT, 0), 0);
				}

				else if (wParam == HOTKEY_PREV)
				{
					if (tuniacApp.m_LogWindow)
					{
						tuniacApp.m_LogWindow->LogMessage(TEXT("Hotkeys"), TEXT("HOTKEY_PREV event"));
					}
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
				}

				else if (wParam == HOTKEY_PREVBYHISTORY)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS_BYHISTORY, 0), 0);
				}

				else if (wParam == HOTKEY_SHUFFLE)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_TOGGLE_SHUFFLE, 0), 0);
				}

				else if (wParam == HOTKEY_REPEAT)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_TOGGLE_REPEAT, 0), 0);
				}

				else if (wParam == HOTKEY_FIND)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_FIND, 0), 0);
				}

				else if (wParam == HOTKEY_VOLUP)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEUP, 0), 0);
				}

				else if (wParam == HOTKEY_VOLDOWN)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEDOWN, 0), 0);
				}

				else if (wParam == HOTKEY_SEEKFORWARD)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_SEEKFORWARD, 0), 0);
				}

				else if (wParam == HOTKEY_SEEKBACK)
				{
					SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_SEEKBACK, 0), 0);
				}

			}
			break;

		//command line from previous (or current) version
		case WM_COPYDATA:
			{
				if(lParam)
				{
					PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;

					switch(pCDS->dwData)
					{
						case 0:
							{ 
								LPWSTR		*szArglist;
								int			nArgs;
								szArglist	= CommandLineToArgvW((LPTSTR)pCDS->lpData, &nArgs);

								if(szArglist != NULL)
								{
									bool		bAddingFiles = false;
									bool		bPlayAddedFiles = false;
									bool		bQueueAddedFiles = false;
									bool		bExitAtEnd = false;
									bool		bWantFocus = false;

									unsigned long ulMLOldCount = m_MediaLibrary.GetCount();

									for(int i = 1; i < nArgs; i++)
									{
										//tuniac first assumes command line is a filepath
										//it checks if it would be a valid path it can use
										if(PathFileExists(szArglist[i]) || PathIsURL(szArglist[i]))
										{
											if(!bAddingFiles)
											{
												if(!m_MediaLibrary.BeginAdd(nArgs - 1)) break;
												bAddingFiles = true;
												bWantFocus = true;
											}

											m_MediaLibrary.AddItem(szArglist[i], false);
										}
										else if (StrCmpI(szArglist[i], TEXT("-queue")) == 0)
										{
											bQueueAddedFiles = true;
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-play")) == 0)
										{
											bPlayAddedFiles = true;
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-pause")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-togglepause")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-stop")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_STOP, 0), 0);
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-softpause")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_SOFTPAUSE, 0), 0);
										}

										else if (StrCmpI(szArglist[i], TEXT("-next")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-randomnext")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_RANDOMNEXT, 0), 0);
										}

										else if (StrCmpI(szArglist[i], TEXT("-prev")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
										}
										
										else if (StrCmpI(szArglist[i], TEXT("-toggleshuffle")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_TOGGLE_SHUFFLE, 0), 0);
										}

										else if (StrCmpI(szArglist[i], TEXT("-togglerepeat")) == 0)
										{
											SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_TOGGLE_REPEAT, 0), 0);
										}

										else if (StrCmpI(szArglist[i], TEXT("-dontsaveprefs")) == 0)
										{
											m_bSavePrefs = false;
										}

										else if(StrCmpI(szArglist[i], TEXT("-wipeprefs")) == 0)
										{
											m_Preferences.CleanPreferences();
											m_Preferences.DefaultPreferences();
										}

										else if(StrCmpI(szArglist[i], TEXT("-wipefileassoc")) == 0)
											m_Preferences.m_FileAssoc.CleanAssociations();

										else if(StrCmpI(szArglist[i], TEXT("-dontsaveml")) == 0)
											m_bSaveML = false;

										else if(StrCmpI(szArglist[i], TEXT("-restore")) == 0)
										{
											//TODO: Implement This
											//m_Taskbar.ShowTrayIcon(FALSE);
											bWantFocus = true;
											ShowWindow(hWnd, SW_RESTORE);
											ShowWindow(hWnd, SW_SHOW);
										}

										else if(StrCmpI(szArglist[i], TEXT("-minimize")) == 0)
										{
											//TODO: Implement This
											//m_Taskbar.ShowTrayIcon(TRUE);
											bWantFocus = false;
											ShowWindow(hWnd, SW_MINIMIZE);
											if(m_Preferences.GetTrayIconMode() == TrayIconMinimize)
												ShowWindow(m_hWnd, SW_HIDE);
										}

										else if(StrCmpI(szArglist[i], TEXT("-nofocus")) == 0)
											bWantFocus = false;

										else if(StrCmpI(szArglist[i], TEXT("-exit")) == 0)
										{
											bWantFocus = false;
											bExitAtEnd = true;
										}
									}

									if(bAddingFiles)
									{
										m_MediaLibrary.EndAdd();
									}

									if(bPlayAddedFiles)
									{
										if(m_MediaLibrary.GetCount() > ulMLOldCount)
										{
											CCoreAudio::Instance()->Reset();
											m_SourceSelectorWindow->ShowPlaylistAtIndex(0);
											m_PlaylistManager.SetActivePlaylist(0);
											if (m_PlaylistManager.GetActivePlaylist()->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
											{
												m_SourceSelectorWindow->m_PlaylistSourceView->ClearTextFilter();
												IPlaylist * pPlaylist = (IPlaylist *)m_PlaylistManager.GetActivePlaylist();
												PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulMLOldCount), true, false);
											}
										}
										SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
									}
									else if (bQueueAddedFiles)
									{
										if(m_MediaLibrary.GetCount() > ulMLOldCount)
										{
											m_PlaylistManager.SetActivePlaylist(0);
											for(unsigned long i = ulMLOldCount; i < m_MediaLibrary.GetCount(); i++)
											{
												m_Queue.Append(m_MediaLibrary.GetEntryIDByIndex(i));
											}
											RebuildFutureMenu();
										}
									}

									if(bWantFocus)
										SetForegroundWindow(hWnd);

									if(bExitAtEnd)
										SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_FILE_EXIT, 0), 0);


								}
								LocalFree(szArglist);

							}
							break;
					}
				}
			}
			break;

		//messages sent to tuniac window(some are via mouse clicks others are messages)
		//menu equal given where possible
		case WM_COMMAND:
			{
				if(wParam)
				{
					WORD wCmdID = LOWORD(wParam);

					//clicked a different view in menu
					if(wCmdID >= MENU_BASE && wCmdID <= (MENU_BASE + m_WindowArray.GetCount()))
					{
						m_ActiveScreen = wCmdID - MENU_BASE;

						for(unsigned long item = 0; item < m_WindowArray.GetCount(); item++)
						{
							//dont hide visual windows when showvisart
							 if(m_Preferences.GetShowVisArt() && wcscmp(GetActiveScreenName(), L"Source Selector") == 0 && wcscmp(m_WindowArray[item]->GetName(), L"Visuals") == 0)
								 continue;

							//resize visual window to full visual area
							if(wcscmp(GetActiveScreenName(), L"Visuals") == 0)
							{
								RECT		rcWindowRect;
								GetClientRect(hWnd, &rcWindowRect);
								SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rcWindowRect.right, rcWindowRect.bottom));
							}

							if(item == m_ActiveScreen)
								m_WindowArray[item]->Show();
							else
								m_WindowArray[item]->Hide();

							//reshow visual window in source view
							if(m_Preferences.GetShowVisArt() && wcscmp(GetActiveScreenName(), L"Source Selector") == 0)
								m_VisualWindow->Show();
						}

						return 0;
					}

					//clicked to set another playlist active
					if(wCmdID >= PLAYLISTMENU_BASE && wCmdID <= (PLAYLISTMENU_BASE + m_PlaylistManager.GetNumPlaylists()))
					{
						CCoreAudio::Instance()->Reset();
						m_SourceSelectorWindow->ShowPlaylistAtIndex(wCmdID - PLAYLISTMENU_BASE);
						m_PlaylistManager.SetActivePlaylist(wCmdID - PLAYLISTMENU_BASE);
						IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();

						if(pPlaylist)
							pPlaylist->SetActiveFilteredIndex(0);

						SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
						return 0;
					}

					//clicked item inside history menu("previous" button)
					if(wCmdID >= HISTORYMENU_BASE && wCmdID <= (HISTORYMENU_BASE + m_Preferences.GetHistoryListSize()))
					{
						m_History.PlayHistoryIndex(wCmdID - HISTORYMENU_BASE);
						return 0;
					}

					//clicked item inside future menu("next" button)
					if(wCmdID >= FUTUREMENU_BASE && wCmdID <= (FUTUREMENU_BASE + m_Preferences.GetFutureListSize()))
					{
						IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
						if(pPlaylist)
						{
							if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
								PlayEntry(((IPlaylistEX *)pPlaylist)->GetEntryByEntryID(m_FutureMenu[wCmdID - FUTUREMENU_BASE]), true, false);
						}
						return 0;
					}

					//clicked a different view in tray menu
					if(wCmdID >= TRAYMENU_BASE && wCmdID <= (TRAYMENU_BASE + m_PlaylistManager.GetNumPlaylists()))
					{
						unsigned long m_PlaylistIndex = wCmdID - TRAYMENU_BASE;
						m_SourceSelectorWindow->ShowPlaylistAtIndex(m_PlaylistIndex);
						m_PlaylistManager.SetActivePlaylist(m_PlaylistIndex);
						return 0;
					}


					switch(wCmdID)
					{
						//show tuniac
						case ID_APP_SHOW:
							{
								m_Preferences.SetMainWindowMinimized(false);
								ShowWindow(hWnd, SW_RESTORE);
								ShowWindow(hWnd, SW_SHOW);
								SetForegroundWindow(hWnd);
							}
							break;

						//file -> import files
						case ID_FILE_ADDFILES:
							{
								//setup open file dialog
								OPENFILENAME		ofn;
								TCHAR				szURLBuffer[64*MAX_PATH];

								ZeroMemory(&ofn, sizeof(OPENFILENAME));
								StringCchCopy(szURLBuffer, 64*MAX_PATH, TEXT(""));

								ofn.lStructSize			= sizeof(OPENFILENAME);
								ofn.hwndOwner			= hWnd;
								ofn.hInstance			= m_hInstance;
								ofn.lpstrFilter			= TEXT("All Files\0*.*\0");
								ofn.lpstrCustomFilter	= NULL;
								ofn.nMaxCustFilter		= 0;
								ofn.nFilterIndex		= 0;
								ofn.lpstrFile			= szURLBuffer;
								ofn.nMaxFile			= 64*MAX_PATH;
								ofn.lpstrFileTitle		= NULL;
								ofn.nMaxFileTitle		= 0;
								ofn.lpstrInitialDir		= NULL;
								ofn.lpstrTitle			= NULL;
								ofn.Flags				= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

								//something valid was selected to be opened
								if(GetOpenFileName(&ofn))
								{
									if(m_MediaLibrary.BeginAdd(BEGIN_ADD_UNKNOWNNUMBER))
									{
										//single file was selected(straight url to file)
										if(ofn.nFileOffset < wcsnlen_s(szURLBuffer, 16640))
										{
											m_MediaLibrary.AddItem(szURLBuffer, false);
										}
										//multiple files were selected(path + multiple filenames, need to join)
										else
										{
											TCHAR szFilePath[MAX_PATH];
											TCHAR szURL[MAX_PATH];

											LPTSTR	szOFNName = &szURLBuffer[ofn.nFileOffset];

											StringCchCopy( szFilePath, MAX_PATH, szURLBuffer );
											while( wcsnlen_s(szOFNName, MAX_PATH) != 0 )
											{
												StringCchCopy(szURL, MAX_PATH, szFilePath );
												StringCchCat(szURL, MAX_PATH, TEXT("\\") );
												StringCchCat(szURL, MAX_PATH, szOFNName );

												m_MediaLibrary.AddItem(szURL, false);

												szOFNName = &szOFNName[wcsnlen_s(szOFNName, MAX_PATH) + 1];
											}
										}

										m_MediaLibrary.EndAdd();
									}
								}
							}
							break;

						//file -> import directory
						case ID_FILE_ADDDIRECTORY:
							{
								LPMALLOC lpMalloc;  // pointer to IMalloc

								if(::SHGetMalloc(&lpMalloc) == NOERROR)
								{
									//setup open directory dialog

									BROWSEINFO browseInfo;
									LPITEMIDLIST lpItemIDList;

									browseInfo.hwndOwner		= hWnd;
									browseInfo.pidlRoot			= NULL; 
									browseInfo.pszDisplayName	= NULL;
									browseInfo.lpszTitle		= TEXT("Select a directory...");
									browseInfo.ulFlags			= BIF_USENEWUI;
									browseInfo.lpfn				= NULL;
									browseInfo.lParam			= 0; 

									if((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
									{
										TCHAR szBuffer[MAX_PATH];
										if(::SHGetPathFromIDList(lpItemIDList, szBuffer)) //something valid was selected for open
										{
											if(m_MediaLibrary.BeginAdd(BEGIN_ADD_UNKNOWNNUMBER))
											{
												//straight url to directory
												m_MediaLibrary.AddItem(szBuffer, false);
												m_MediaLibrary.EndAdd();
											}
										}
										else if(tuniacApp.m_dwWinVer >= 6) //possible library eg Music
										{
											LPTSTR szParsingName;
											::SHGetNameFromIDList(lpItemIDList, SIGDN_DESKTOPABSOLUTEPARSING, &szParsingName); //get parsing name for selected shell id

											IShellLibrary *pslLibrary;
											HRESULT hr =  ::SHLoadLibraryFromParsingName(szParsingName, STGM_READWRITE, IID_PPV_ARGS(&pslLibrary));  //load library for that parsing name
											if(SUCCEEDED(hr))
											{
												IShellItemArray *psiaFolders;
												hr = pslLibrary->GetFolders(LFF_STORAGEITEMS, IID_PPV_ARGS(&psiaFolders)); //get Shell Items for library (library folders)
												IEnumShellItems *penumShellItems;
												psiaFolders->EnumItems(&penumShellItems);

												DWORD dwCount = 0;
												psiaFolders->GetCount(&dwCount);
												IShellItem *psiFolder;
												for(DWORD dwIndex = 0; dwIndex < dwCount; ++dwIndex )
												{
													psiaFolders->GetItemAt(dwIndex, &psiFolder );
													LPTSTR strFolderName;
													hr = psiFolder->GetDisplayName(SIGDN_FILESYSPATH, &strFolderName); //get full path string for Shell Item
													if(SUCCEEDED(hr))
													{
														if(m_MediaLibrary.BeginAdd(BEGIN_ADD_UNKNOWNNUMBER))
														{
															//straight url to directory
															m_MediaLibrary.AddItem(strFolderName, false);
															m_MediaLibrary.EndAdd();
														}
													}
													psiFolder->Release();
												}
												psiaFolders->Release();
											}
											pslLibrary->Release();
										}

										lpMalloc->Free(lpItemIDList);
									}

									lpMalloc->Release();
								}

								/*
								IFileDialog *pfd = NULL;
								HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
													NULL, 
													CLSCTX_INPROC_SERVER, 
													IID_PPV_ARGS(&pfd));
								if (SUCCEEDED(hr))
								{
									FILEOPENDIALOGOPTIONS fileOptions;
									pfd->GetOptions(&fileOptions);
									fileOptions |= FOS_PICKFOLDERS;
									fileOptions &= ~FOS_FORCEFILESYSTEM;
									pfd->SetOptions(fileOptions);

									pfd->Show ( m_hWnd );
									IShellItem *psiResult;
									hr = pfd->GetResult(&psiResult);
									if ( SUCCEEDED(hr) )
									{
										LPTSTR pwsz = NULL;
										hr = psiResult->GetDisplayName (SIGDN_FILESYSPATH, &pwsz);
										if ( SUCCEEDED(hr) )
										{
											if(m_MediaLibrary.BeginAdd(BEGIN_ADD_UNKNOWNNUMBER))
											{
												m_MediaLibrary.AddItem(pwsz);
												m_MediaLibrary.EndAdd();
											}
											CoTaskMemFree(pwsz);
										}
										else
										{
											IEnumShellItems* pEnum = nullptr;
											hr = psiResult->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&pEnum));
											if (SUCCEEDED(hr))
											{
												IShellItem* pChildItem = nullptr;
												ULONG ulFetched = 0;
												do
												{
													hr = pEnum->Next(1, & pChildItem, &ulFetched);
													if (FAILED(hr)) break;
													if (ulFetched != 0)
													{
														LPTSTR szChildName = NULL;
														pChildItem->GetDisplayName(SIGDN_FILESYSPATH, &szChildName);
														if(m_MediaLibrary.BeginAdd(BEGIN_ADD_UNKNOWNNUMBER))
														{
															m_MediaLibrary.AddItem(szChildName);
															m_MediaLibrary.EndAdd();
														}
														CoTaskMemFree(szChildName);
														pChildItem ->Release();
													}
												} while (hr != S_FALSE);
												pEnum->Release();
											}
										}
									}
									pfd->Release();
								}
								*/
							}
							break;

						//file -> import stream
						case ID_FILE_ADDOTHER:
							{
								TCHAR szAddBuffer[2048] = TEXT("");
								if(DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ADDOTHER), hWnd, (DLGPROC)AddOtherProc, (LPARAM)szAddBuffer))
								{
									if(wcsnlen_s(szAddBuffer, 2048) > 0 && m_MediaLibrary.BeginAdd(1))
									{
										m_MediaLibrary.AddItem(szAddBuffer, false);
										m_MediaLibrary.EndAdd();
									}
								}
							}
							break;

						//file -> forcesave medialibrary
						case ID_FILE_FORCESAVEMEDIALIBRARY:
							{
								bool bOK;
								bOK = m_MediaLibrary.SaveMediaLibrary();
								bOK = m_PlaylistManager.SavePlaylistLibrary() && bOK;
							
								if (bOK && m_LogWindow)
								{
									TCHAR szMessage[128];
									StringCchPrintf(szMessage, 128, TEXT("MediaLibrary has been saved.\r\n		%u entries total.\r\n		%u standard playlists."), 
													m_MediaLibrary.GetCount(), 
													m_PlaylistManager.m_StandardPlaylists.GetCount());

									m_LogWindow->LogMessage(TEXT("MediaLibrary"), szMessage);
								}
							}
							break;

						//file -> new playlist -> normal playlist
						case ID_FILE_NEWPLAYLIST_NORMALPLAYLIST:
							{
								m_PlaylistManager.CreateNewStandardPlaylist(TEXT("Untitled Playlist"));
								m_SourceSelectorWindow->UpdateList();
							}
							break;

						//file -> new playlist -> smart playlist (not implemented)
						case ID_FILE_NEWPLAYLIST_SMARTPLAYLIST:
							{
							}
							break;

						//file -> export playlist (import/export plugin save)
						case ID_FILE_EXPORTPLAYLIST:
							{
								IPlaylist * pPlaylist = m_SourceSelectorWindow->GetVisiblePlaylist();
								IndexArray indexArray;
								EntryArray exportArray;
								/*
								if(m_SourceSelectorWindow->m_PlaylistSourceView->GetSelectedIndexes(indexArray))
								{
									for(unsigned long i = 0; i < indexArray.GetCount(); i++)
									{
										IPlaylistEntry * pIPE = pPlaylistEX->GetEntryAtFilteredIndex(indexArray[i]);
										if(pIPE)
											exportArray.AddTail(pIPE);
									}
								}
								else*/
								{
									// must want to export everything eh???

									for(unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
									{
										IPlaylistEntry * pIPE = pPlaylist->GetEntryAtFilteredIndex(i);
										if(pIPE)
											exportArray.AddTail(pIPE);
									}
								}
								m_MediaLibrary.m_ImportExport.Export(exportArray, NULL);
							}
							break;

						//file -> exit
						case ID_FILE_EXIT:
							{
								DestroyWindow(hWnd);
							}
							break;

						//edit -> edit track info (not implemented)
						case ID_EDIT_EDITFILEINFORMATION:
							{
								m_SourceSelectorWindow->GetVisibleView()->EditTrackInfo();
							}
							break;

						//edit -> show playlist column Selection
						case ID_EDIT_SHOWCOLUMNSELECTION:
							{
								m_SourceSelectorWindow->ShowActiveViewViewOptions(hWnd);
							}
							break;

						//edit -> always on top	
						case ID_EDIT_ALWAYSONTOP:
							{
								m_Preferences.SetAlwaysOnTop(!m_Preferences.GetAlwaysOnTop());
								SetWindowPos(hWnd, m_Preferences.GetAlwaysOnTop() ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
							}
							break;

						//edit -> preferences
						case ID_EDIT_PREFERENCES:
							{
								m_Preferences.ShowPreferences(hWnd, 0);

								CCoreAudio::Instance()->EnableReplayGain(m_Preferences.ReplayGainEnabled());
								CCoreAudio::Instance()->ReplayGainUseAlbumGain(m_Preferences.ReplayGainUseAlbumGain());
							}
							break;

						//help -> about tuniac
						case ID_HELP_ABOUT:
							{
								CAboutWindow about;

								about.Show();
							}
							break;

						//help -> about tuniac
						case ID_HELP_HOMEPAGE:
							{
								ShellExecute(NULL, NULL, TEXT("http://www.tuniac.org/"), NULL, NULL, SW_SHOW);
							}
							break;

						//help -> tuniac help
						case ID_HELP_TUNIACHELP:
							{
								//if local help exists, load it, otherwise goto url
								TCHAR szHelp[512];
								GetModuleFileName(NULL, szHelp, 512);
								PathRemoveFileSpec(szHelp);
								StringCchCat(szHelp, 512, TEXT("\\Guide\\index.html"));
								if(PathFileExists(szHelp) == FALSE)
									StringCchCopy(szHelp, 512, TEXT("http://www.tuniac.org/Guide/"));

								ShellExecute(NULL, NULL, szHelp, NULL, NULL, SW_SHOW);
							}
							break;

						//playback -> show currently paylist track (jump focus to current track)
						case ID_PLAYBACK_SHOWCURRENTLYPLAYINGTRACK:
							{
								for(unsigned long item = 0; item < m_WindowArray.GetCount(); item++)
								{
									//dont hide visual windows when showvisart
									if(m_Preferences.GetShowVisArt() && wcscmp(GetActiveScreenName(), L"Source Selector") == 0 && wcscmp(m_WindowArray[item]->GetName(), L"Visuals") == 0)
										continue;
									m_WindowArray[item]->Hide();
								}

								m_ActiveScreen = 0;

								//reshow visual window in source view
								if(m_Preferences.GetShowVisArt() && wcscmp(GetActiveScreenName(), L"Source Selector") == 0)
									m_VisualWindow->Show();

								if(m_SourceSelectorWindow)
								{
									m_SourceSelectorWindow->Show();
									m_SourceSelectorWindow->ShowCurrentlyPlaying();
								}
							}
							break;

						//playback -> play / pause
						case ID_PLAYBACK_PLAYPAUSE:
							{
								//if currently playing, PAUSE
								if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
								{
									SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
								}
								//if not playing, PLAY
								else
								{
									SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
								}
							}
							break;

						//playback -> stop (coreaudio stop is a "pause", a stop is a "pause" and "rewind")
						case ID_PLAYBACK_STOP:
							{
								CCoreAudio::Instance()->Stop();
								CCoreAudio::Instance()->SetPosition(0);
								UpdateState();
							}
							break;

						//playback -> pause (coreaudio stop is a "pause")
						case ID_PLAYBACK_PAUSE:
							{
								if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
								{
									CCoreAudio::Instance()->Stop();
									UpdateState();
								}
							}
							break;

						//playback -> play
						case ID_PLAYBACK_PLAY:
							{
								//if only paused we can simply play again
								if(CCoreAudio::Instance()->Play())
								{
									UpdateState();
									m_PluginManager.PostMessage(PLUGINNOTIFY_SONGPLAY, NULL, NULL);
								}
								else
								{
									IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
									if(pPlaylist)
										//play the current song
										PlayEntry(pPlaylist->GetActiveEntry(), true, false);
								}

							}
							break;

						//playback -> softpause (on/off)
						case ID_PLAYBACK_SOFTPAUSE:
							{
								m_SoftPause.bNow = !m_SoftPause.bNow;
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
							}
							break;

						//playback -> next
						case ID_PLAYBACK_NEXT:
							{
								IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
								if(pPlaylist)
								{
									//try next song for non crossfade mode
									//do we have a valid next song, if not we have run out of songs(end of playlist?), start again
									unsigned long ulNextFilteredIndex = pPlaylist->Next();
									if(ulNextFilteredIndex == INVALID_PLAYLIST_INDEX)
										ulNextFilteredIndex = 0;

									//play the current song
									PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulNextFilteredIndex), CCoreAudio::Instance()->GetState(), HIWORD(wParam));
								}
							}
							break;

						//playback -> random next (toggle random for a moment to get a new random song, or if already random, next logical song)
						case ID_PLAYBACK_RANDOMNEXT:
							{
								bool bWasShuffle = m_Preferences.GetShuffleState();
								m_Preferences.SetShuffleState(!bWasShuffle);
								SendMessage(hWnd, WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, HIWORD(wParam)), 0);
								m_Preferences.SetShuffleState(bWasShuffle);
							}
							break;

						//playback -> previous
						case ID_PLAYBACK_PREVIOUS:
							{


								IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
								if(pPlaylist)
								{
									IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();
									if(pIPE)
									{
										if(PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)) && ((int)pIPE->GetField(FIELD_PLAYBACKTIME) == LENGTH_STREAM))
										{
											//do we have a valid next song, if not we have run out of songs(end of playlist?)
											unsigned long ulPreviousFilteredIndex = pPlaylist->Previous();
											if(ulPreviousFilteredIndex == INVALID_PLAYLIST_INDEX)
												ulPreviousFilteredIndex = pPlaylist->GetNumItems()-1;

											//play the current song
											PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulPreviousFilteredIndex), CCoreAudio::Instance()->GetState(), HIWORD(wParam));
											break;
										}
									}
									//still at start, skip backwards or a stream
									if(CCoreAudio::Instance()->GetPosition() < 3500)
									{
											//do we have a valid next song, if not we have run out of songs(end of playlist?)
											unsigned long ulPreviousFilteredIndex = pPlaylist->Previous();
											if(ulPreviousFilteredIndex == INVALID_PLAYLIST_INDEX)
												ulPreviousFilteredIndex = pPlaylist->GetNumItems()-1;

											//play the current song
											PlayEntry(pPlaylist->GetEntryAtFilteredIndex(ulPreviousFilteredIndex), CCoreAudio::Instance()->GetState(), HIWORD(wParam));
									}
									//past start of song, simply rewind
									else
									{
										CCoreAudio::Instance()->SetPosition(0);
									}
								}
							}
							break;

						//playback -> previous by history
						case ID_PLAYBACK_PREVIOUS_BYHISTORY:
							{
								if(m_History.GetCount() > 1)
									m_History.PlayHistoryIndex(1);
							}
							break;

						//playback -> seek forward (750ms at a time)
						case ID_PLAYBACK_SEEKFORWARD:
							{
								unsigned long pos = CCoreAudio::Instance()->GetPosition() + 750;
								CCoreAudio::Instance()->SetPosition(pos);
								m_PluginManager.PostMessage(PLUGINNOTIFY_SEEK_MANUAL, NULL, NULL);
							}
							break;

						//playback -> seekback (1300ms at a time)
						case ID_PLAYBACK_SEEKBACK:
							{
								unsigned long pos;
								if(CCoreAudio::Instance()->GetPosition() < 1300)
									pos = 0;
								else
									pos = CCoreAudio::Instance()->GetPosition() - 1300;
								CCoreAudio::Instance()->SetPosition(pos);
								m_PluginManager.PostMessage(PLUGINNOTIFY_SEEK_MANUAL, NULL, NULL);
							}
							break;

						case ID_PLAYBACK_VOLUMEUP:
							{
								if(m_iMuteVol)
								{
									CCoreAudio::Instance()->SetVolumePercent(m_iMuteVol + 2);
									m_iMuteVol = 0;
								}
								else
								{
									CCoreAudio::Instance()->SetVolumePercent((CCoreAudio::Instance()->GetVolumePercent()) + 2);
								}
								m_PlayControls.UpdateVolume();
							}
							break;

						case ID_PLAYBACK_VOLUMEDOWN:
							{
								if(m_iMuteVol)
								{
									CCoreAudio::Instance()->SetVolumePercent(m_iMuteVol - 2);
									m_iMuteVol = 0;
								}
								else
								{
									CCoreAudio::Instance()->SetVolumePercent((CCoreAudio::Instance()->GetVolumePercent()) - 2);
								}
								m_PlayControls.UpdateVolume();
							}
							break;

						case ID_PLAYBACK_VOLUMEMUTE:
							{
								if(m_iMuteVol)
								{
									CCoreAudio::Instance()->SetVolumePercent(m_iMuteVol);
									m_iMuteVol = 0;
								}
								else
								{
									m_iMuteVol = CCoreAudio::Instance()->GetVolumePercent();
									CCoreAudio::Instance()->SetVolumePercent(0);
								}
								m_PlayControls.UpdateVolume();
							}
							break;

						//toggle shuffle mode
						case ID_PLAYBACK_TOGGLE_SHUFFLE:
							{
								m_Preferences.SetShuffleState(!m_Preferences.GetShuffleState());
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//toggle repeat mode
						case ID_PLAYBACK_TOGGLE_REPEAT:
							{
								if(m_Preferences.GetRepeatMode() == RepeatNone)
									m_Preferences.SetRepeatMode(RepeatOne);
								else if(m_Preferences.GetRepeatMode() == RepeatOne)
									m_Preferences.SetRepeatMode(RepeatAll);
								else if(m_Preferences.GetRepeatMode() == RepeatAll)
									m_Preferences.SetRepeatMode(RepeatNone);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//clear play queue
						case ID_PLAYBACK_CLEARQUEUE:
							{
								m_Queue.Clear();
								if(m_Preferences.GetRepeatMode() == RepeatAllQueued)
								{
									m_Preferences.SetRepeatMode(RepeatAll);
									SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								}
								RebuildFutureMenu();
							}
							break;

						//clear soft pause
						case ID_PLAYBACK_CLEARPAUSEAT:
							{
								m_SoftPause.ulAt = INVALID_PLAYLIST_INDEX;
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//turn off repeat
						case ID_REPEAT_OFF:
							{
								m_Preferences.SetRepeatMode(RepeatNone);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//turn on repeat track
						case ID_REPEAT_ONETRACK:
							{
								m_Preferences.SetRepeatMode(RepeatOne);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//turn on repeat all
						case ID_REPEAT_ALLTRACKS:
							{
								m_Preferences.SetRepeatMode(RepeatAll);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//turn on repeat queued
						case ID_REPEAT_ALLQUEUED:
							{
								m_Preferences.SetRepeatMode(RepeatAllQueued);
								SendMessage(hWnd, WM_MENUSELECT, 0, 0);
								RebuildFutureMenu();
							}
							break;

						//toggle repeat mode
						case ID_FIND:
							{
								if(m_Preferences.GetMainWindowMinimized())
								{
									ShowWindow(hWnd, SW_RESTORE);
									m_Preferences.SetMainWindowMinimized(false);
								}

								ShowWindow(hWnd, SW_SHOW);
								ShowWindowAsync(hWnd, SW_SHOW);
								SetForegroundWindow(hWnd);

								m_SourceSelectorWindow->m_PlaylistSourceView->SetFocusFilter();

							}
							break;
					}
				}
			}
			break;

		case WM_SYSCOLORCHANGE:
			{
				m_PlayControls.SysColorChange(wParam, lParam);
			}
			break;

		case WM_TRAYICON:
			{
				UINT uID; 
				UINT uMouseMsg; 
 
				uID = (UINT) wParam; 
				uMouseMsg = (UINT) lParam; 

				switch (uMouseMsg)
				{
					//selected by keyboard in windows 2000 and up
					case NIN_KEYSELECT:
					//left double click tray icon (restore window)
					case WM_LBUTTONDBLCLK:
						{
							m_Preferences.SetMainWindowMinimized(false);
							ShowWindow(hWnd, SW_RESTORE);
							ShowWindow(hWnd, SW_SHOW);
							SetForegroundWindow(hWnd);
						}
						break;

					//right click tray icon (menu)
					case WM_CONTEXTMENU:
						{
							POINT pt;
							GetCursorPos(&pt);
							SetForegroundWindow(hWnd);
							TrackPopupMenu(m_TrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
							PostMessage(hWnd, WM_NULL, 0, 0);

						}
						break;
				}

			}
			break;

		case WM_SYSCOMMAND:
			{
				if(wParam == SC_SCREENSAVE || (wParam == SC_MONITORPOWER && lParam != -1))
				{
					//prevent when focused
					if(tuniacApp.m_Preferences.GetScreenSaveMode() == 1)
						return true;

					//prevent when fullscreen
					if(tuniacApp.m_VisualWindow)
					{
						if(tuniacApp.m_Preferences.GetScreenSaveMode() == 2 && tuniacApp.m_VisualWindow->GetFullscreen())
							return true;
					}
				}
			}

		default:
			//restore trayicon after explorer restart
			if(message == WM_TASKBARCREATED)
			{
				if(m_Preferences.GetTrayIconMode() == TrayIconMinimize)
				{
					m_Taskbar.Show();
				}
			}
			
			return(DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}

	return(0);
}

LPTSTR CTuniacApp::GetActiveScreenName(void)
{
	return m_WindowArray[m_ActiveScreen]->GetName();
}

bool CTuniacApp::SetStatusText(LPTSTR szStatusText)
{
	SendMessage(m_hWndStatus, SB_SETTEXT, 0, (LPARAM)szStatusText);
	return true;
}

LRESULT CALLBACK CTuniacApp::AddOtherProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static LPTSTR szString = (LPTSTR)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(message)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
				szString = (LPTSTR)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_ADDOTHER_ADD:
						{
							GetDlgItemText(hWnd, IDC_ADDOTHER_EDIT, szString, 2048);
							EndDialog(hWnd, 1);
						}
						break;

					case IDC_ADDOTHER_CANCEL:
						{
							EndDialog(hWnd, 0);
						}
						break;
				}
			}
			break;

		case WM_CLOSE:
			{
				EndDialog(hWnd, 0);
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

bool CTuniacApp::CoreAudioMessage(unsigned long Message, void * Params)
{
	PostMessage(m_hWnd, WM_APP, Message, LPARAM(Params));
	return true;
}

bool CTuniacApp::RegisterHotkeys(void)
{
	//our hotkeys
	if (RegisterHotKey(m_hWnd, HOTKEY_PLAY, MOD_WIN, VK_NUMPAD5) == 0)
	{
		if (m_LogWindow)
		{
			m_LogWindow->LogMessage(TEXT("HotKey Register"), TEXT("Error registering hotkey"));
		}
	}

	RegisterHotKey(m_hWnd, HOTKEY_STOP,			MOD_WIN, VK_NUMPAD0);
	RegisterHotKey(m_hWnd, HOTKEY_NEXT,			MOD_WIN, VK_NUMPAD6);
	RegisterHotKey(m_hWnd, HOTKEY_RANDNEXT,		MOD_WIN, VK_NUMPAD9);
	RegisterHotKey(m_hWnd, HOTKEY_PREV,			MOD_WIN, VK_NUMPAD4);
	RegisterHotKey(m_hWnd, HOTKEY_PREVBYHISTORY,	MOD_WIN + MOD_CONTROL, VK_NUMPAD4);

	RegisterHotKey(m_hWnd, HOTKEY_VOLUP,		MOD_WIN, VK_NUMPAD8);
	RegisterHotKey(m_hWnd, HOTKEY_VOLDOWN,		MOD_WIN, VK_NUMPAD2);

	RegisterHotKey(m_hWnd, HOTKEY_SEEKBACK,		MOD_WIN, VK_NUMPAD1);
	RegisterHotKey(m_hWnd, HOTKEY_SEEKFORWARD,	MOD_WIN, VK_NUMPAD3);

	RegisterHotKey(m_hWnd, HOTKEY_SHUFFLE,		MOD_WIN + MOD_CONTROL, 'S');
	RegisterHotKey(m_hWnd, HOTKEY_REPEAT,		MOD_WIN + MOD_CONTROL, 'R');
	RegisterHotKey(m_hWnd, HOTKEY_FIND,			MOD_WIN, VK_DECIMAL);
	return true;
}

HFONT	CTuniacApp::GetTuniacFont(int size)
{
	switch(size)
	{
		case FONT_SIZE_LARGE:
			{
				return m_LargeFont;
			}
			break;

		case FONT_SIZE_MEDIUM:
			{
				return m_MediumFont;
			}
			break;
		case FONT_SIZE_SMALL_MEDIUM:
			{
				return m_SmallMediumFont;
			}
			break;
		case FONT_SIZE_SMALL:
			{
				return m_SmallFont;
			}
			break;
		case FONT_SIZE_TINY:
			{
				return m_TinyFont;
			}
			break;
	}

	return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

LPTSTR	CTuniacApp::GetTuniacBuild(void)
{
	return TEXT(__DATE__);
}

bool	CTuniacApp::FormatSongInfo(LPTSTR szDest, unsigned int iDestSize, IPlaylistEntry * pIPE, LPTSTR szFormat, bool bPlayState)
{
	memset(szDest, '\0', iDestSize);
	if(pIPE == NULL)
		return false;

	LPTSTR a = szFormat, b = StrStr(a, TEXT("@"));
	while (b != NULL) {
		if(wcsnlen_s(szDest, iDestSize) >= iDestSize - 1)
			break;

		StringCchCopyN(szDest + wcslen(szDest), iDestSize - wcslen(szDest), a, wcslen(a) - wcslen(b));

		int lField = 0xFFFE;
		bool bShort = false;
		WCHAR cFmt = b[1];
		switch(cFmt)
		{
			case '\0':
				lField = 0xFFFF;
				break;
			case '@':
				lField = 0xFFFD;
				break;
			case '!':
				{
					if(bPlayState)
						lField = -1;
					else
						lField = 0xFFFE;
				}
				break;
			case 'U':
				lField = FIELD_URL;
				break;
			case 'F':
				lField = FIELD_FILENAME;
				break;
			case 'X':
				lField = FIELD_FILEEXTENSION;
				break;
			case 'x':
				lField = FIELD_FILEEXTENSION;
				bShort = true;
				break;
			case 'A':
				lField = FIELD_ARTIST;
				break;
			case 'L':
				lField = FIELD_ALBUM;
				break;
			case 'T':
				lField = FIELD_TITLE;
				break;
			case '#':
				lField = FIELD_TRACKNUM;
				break;
			case 'G':
				lField = FIELD_GENRE;
				break;
			case 'Y':
				lField = FIELD_YEAR;
				break;
			case 'I':
				lField = FIELD_PLAYBACKTIME;
				break;
			case 'i':
				lField = FIELD_PLAYBACKTIME;
				bShort = true;
				break;
			case 'K':
				lField = FIELD_KIND;
				break;
			case 'S':
				lField = FIELD_FILESIZE;
				break;
			case 'D':
				lField = FIELD_DATEADDED;
				break;
			case 'E':
				lField = FIELD_DATEFILECREATION;
				break;
			case 'P':
				lField = FIELD_DATELASTPLAYED;
				break;
			case 'Z':
				lField = FIELD_PLAYCOUNT;
				break;
			case 'R':
				lField = FIELD_RATING;
				break;
			case 'C':
				lField = FIELD_COMMENT;
				break;
			case 'B':
				lField = FIELD_BITRATE;
				break;
			case 'M':
				lField = FIELD_SAMPLERATE;
				break;
			case 'N':
				lField = FIELD_NUMCHANNELS;
				break;
			case 'V':
				lField = FIELD_BPM;
				break;
			case 'Q':
				lField = FIELD_ALBUMARTIST;
				break;
		}

		if (lField == 0xFFFF)
		{
			StringCchPrintf(szDest + wcsnlen_s(szDest, iDestSize), iDestSize - wcsnlen_s(szDest, iDestSize), TEXT("@"));
			break;
		}
		else if (lField == 0xFFFE)
		{
			StringCchPrintf(szDest + wcsnlen_s(szDest, iDestSize), iDestSize - wcsnlen_s(szDest, iDestSize), TEXT("@"));
			b += 1;
		}
		else if (lField == 0xFFFD) 
		{
			StringCchPrintf(szDest + wcsnlen_s(szDest, iDestSize), iDestSize - wcsnlen_s(szDest, iDestSize), TEXT("@"));
			b += 2;
		}
		else if (lField == -1)
		{
			TCHAR szPlayState[16];
			switch(CCoreAudio::Instance()->GetState())
			{
				case STATE_STOPPED:
					{
						StringCchPrintf(szPlayState, 16, TEXT("Paused"));
					}
					break;

				case STATE_PLAYING:
					{
						StringCchPrintf(szPlayState, 16, TEXT("Playing"));
					}
					break;

				default:
					{
						StringCchPrintf(szPlayState, 16, TEXT("Stopped"));
					}
					break;
			}
			StringCchPrintf(szDest + wcsnlen_s(szDest, iDestSize), iDestSize - wcsnlen_s(szDest, iDestSize), szPlayState);
			b += 2;
		}
		else
		{
			TCHAR szFieldData[256];
			pIPE->GetTextRepresentation(lField, szFieldData, 256);

			if(bShort)
			{
				switch(lField)
				{
					case FIELD_PLAYBACKTIME:
						{
							unsigned long i = 0;
							while(szFieldData[i] == L'0' && szFieldData[i+1] == L'0')
							{
								i += 3;
								if(i >= wcsnlen_s(szFieldData, 256) - 1)
								{
									i = wcsnlen_s(szFieldData, 256) - 2;
									break;
								}
							}
							if(i > 0)
							{
								if(szFieldData[i] == L'0')
									i++;
								StringCchCopy(szFieldData, 256, szFieldData + i);
							}
						}
						break;

					case FIELD_FILEEXTENSION:
						{
							if(wcsnlen_s(szFieldData, 256) > 0 && szFieldData[0] == L'.')
								StringCchCopy(szFieldData, 256, szFieldData + 1);
						}
						break;
				}
			}

			StringCchPrintf(szDest + wcsnlen_s(szDest, iDestSize), iDestSize - wcsnlen_s(szDest, iDestSize), TEXT("%s"), szFieldData);
			b += 2;
		}

		a = b;
		b = StrStr(a, TEXT("@"));
	}

	StringCchCopyN(szDest + wcslen(szDest), iDestSize - wcslen(szDest), a, wcslen(a));
	return true;
}

bool	CTuniacApp::EscapeMenuItemString(LPTSTR szSource, LPTSTR szDest,  unsigned int iDestSize)
{
	memset(szDest, L'\0', iDestSize);

	LPTSTR szFound;
	szFound = StrChr(szSource, L'&');

	if (szFound == NULL)
	{
		StringCchCat(szDest, iDestSize, szSource);
	}
	else
	{
		int iRead = 0;
		int iFound = 0;
		while (szFound)
		{
			iFound = wcslen(szSource) - wcslen(szFound) + 1;
			StringCchCat(szDest, iFound + 1, szSource + iRead);
			iRead = iRead + iFound;
			StringCchCat(szDest, iDestSize, TEXT("&"));
			szFound = StrChr(szFound + 1, L'&');
		}
		StringCchCat(szDest, iDestSize, szSource + iRead);
	}

	return true;
}

IPlaylistEntry *		CTuniacApp::GetFuturePlaylistEntry(int iFromCurrent)
{
	if(iFromCurrent >= 0 && iFromCurrent < m_FutureMenu.GetCount())
	{
		IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
		if (pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
		{
			IPlaylistEntry * pIPE = pPlaylist->GetEntryAtFilteredIndex(((IPlaylistEX *)pPlaylist)->GetFilteredIndexforEntryID(m_FutureMenu[iFromCurrent]));
			return pIPE;
		}
	}
	return NULL;
}

unsigned long		CTuniacApp::GetFuturePlaylistEntryID(int iFromCurrent)
{
	if(iFromCurrent >= 0 && iFromCurrent < m_FutureMenu.GetCount())
		return m_FutureMenu[iFromCurrent];

	return NULL;
}

void			CTuniacApp::BuildFuturePlaylistArray(void)
{
	m_FutureMenu.RemoveAll();

	if(!m_PlaylistManager.GetActivePlaylist())
		return;

	IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();

	if(pPlaylist)
	{

		if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
		{
			IPlaylistEX * pPlaylistEX = (IPlaylistEX *)pPlaylist;
			int iSize = m_Preferences.GetFutureListSize();

			for(int i = 0; i < m_PlaySelected.GetCount(); i++)
			{
				if(iSize)
				{
					IPlaylistEntry * pIPE = pPlaylist->GetEntryAtFilteredIndex(m_PlaySelected[i]);
					//test that we could get the item in this playlist.
					if(pIPE)
					{
						unsigned long ulEntryID = pIPE->GetEntryID();
						m_FutureMenu.AddTail(ulEntryID);
						iSize--;
					}
				}
				else
					break;
			}

			for(int i = 0; i < m_Queue.GetCount(); i++)
			{
				if(iSize)
				{
					IPlaylistEntry * pIPE = pPlaylist->GetEntryAtFilteredIndex(pPlaylistEX->GetFilteredIndexforEntryID(m_Queue.GetEntryIDAtIndex(i)));
					//test that we could get the item in this playlist.
					if(pIPE)
					{
						unsigned long ulEntryID = pIPE->GetEntryID();
						m_FutureMenu.AddTail(ulEntryID);
						iSize--;
					}
				}
				else
					break;
			}

			unsigned long ulIndex = pPlaylist->GetActiveFilteredIndex();

			if(m_FutureMenu.GetCount())
				ulIndex = pPlaylistEX->GetFilteredIndexforEntryID(m_FutureMenu[m_FutureMenu.GetCount()-1]);
			else
				ulIndex = pPlaylist->GetActiveFilteredIndex();

			for(int i = 0; i < iSize; i++)
			{
				ulIndex = pPlaylist->GetNextFilteredIndexForFilteredIndex(ulIndex);
				if(ulIndex == INVALID_PLAYLIST_INDEX)
					break;

				unsigned long ulEntryID = pPlaylistEX->GetEntryIDAtFilteredIndex(ulIndex);
				m_FutureMenu.AddTail(ulEntryID);
			}
		}
	}
}

void	CTuniacApp::RebuildFutureMenu(void)
{
	while(GetMenuItemCount(m_hFutureMenu) > 0)
		DeleteMenu(m_hFutureMenu, 0, MF_BYPOSITION);

	BuildFuturePlaylistArray();

	for(int i = 0; i < m_FutureMenu.GetCount(); i++)
	{
		TCHAR szDetail[52];
		TCHAR szItem[64];
		TCHAR szTime[12];

		//memset(szDetail, L'\0',52);
		//memset(szTime, L'\0', 12);


		IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
		if(pPlaylist)
		{
			if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
			{
				IPlaylistEntry * pIPE = pPlaylist->GetEntryAtFilteredIndex(((IPlaylistEX *)pPlaylist)->GetFilteredIndexforEntryID(m_FutureMenu[i]));
				if(pIPE)
				{
					//format info of file
					FormatSongInfo(szDetail, 52, pIPE, m_Preferences.GetListFormatString(), true);
					EscapeMenuItemString(szDetail, szItem, 52);
					FormatSongInfo(szTime, 12, pIPE, TEXT("\t[@I]"), true);
					StringCchCat(szItem, 64, szTime);

					//add menu item
					AppendMenu(m_hFutureMenu, MF_STRING, FUTUREMENU_BASE + i, szItem);
				}
			}
		}
	}
}

HMENU	CTuniacApp::GetFutureMenu(void)
{
	return m_hFutureMenu;
}

//update taskbar and titlebar
void	CTuniacApp::UpdateTitles(void)
{
	//set title of window/tray
	TCHAR szWinTitle[512];
	IPlaylistEntry * pIPE = m_PlaylistManager.GetActivePlaylist()->GetActiveEntry();
	
	if(pIPE)
		FormatSongInfo(szWinTitle, 512, pIPE, m_Preferences.GetWindowFormatString(), true);
	else
		StringCchPrintf(szWinTitle, 512, TEXT("Tuniac"));

	SetWindowText(m_hWnd, szWinTitle);
	m_Taskbar.SetTitle(szWinTitle);
}

void	CTuniacApp::UpdateState(void)
{
	UpdateTitles();
	m_PlayControls.UpdateState();

	if(m_PlaylistManager.GetActivePlaylist())
	{
		if(m_PlaylistManager.GetActivePlaylist()->GetActiveEntry())
		{
			if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGPLAY, NULL, NULL);
			else if(CCoreAudio::Instance()->GetState() == STATE_STOPPED)
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGPAUSE, NULL, NULL);
		}
	}
}

//update streamtitle eg for mp3 streams
void	CTuniacApp::UpdateMetaData(LPTSTR szURL, void * pNewData, unsigned long ulFieldID)
{
	IPlaylistEntry * pIPE = m_MediaLibrary.GetEntryByURL(szURL);

	if(pIPE)
	{
		pIPE->SetField(ulFieldID, pNewData);

		UpdateTitles();
		//make sure the source selector window exists we can get here before its created
		if(m_SourceSelectorWindow)
			m_SourceSelectorWindow->UpdateView();
		if(PathIsURL(szURL) && ulFieldID == FIELD_ARTIST)
		{
			if(GetForegroundWindow() == m_hWnd && !IsIconic(m_hWnd))
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE, NULL, NULL);
			else
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_BLIND, NULL, NULL);
		}

		m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
	}
}

//update streamtitle eg for mp3 streams
void	CTuniacApp::UpdateMetaData(LPTSTR szURL, unsigned long pNewData, unsigned long ulFieldID)
{
	IPlaylistEntry * pIPE = m_MediaLibrary.GetEntryByURL(szURL);

	if(pIPE)
	{
		pIPE->SetField(ulFieldID, pNewData);

		UpdateTitles();
		//make sure the source selector window exists we can get here before its created
		if(m_SourceSelectorWindow)
			m_SourceSelectorWindow->UpdateView();
		if(PathIsURL(szURL) && ulFieldID == FIELD_ARTIST)
		{
			if(GetForegroundWindow() == m_hWnd && !IsIconic(m_hWnd))
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE, NULL, NULL);
			else
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_BLIND, NULL, NULL);
		}

		m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
	}
}

//update streamtitle eg for mp3 streams
void	CTuniacApp::UpdateMetaData(LPTSTR szURL, float pNewData, unsigned long ulFieldID)
{
	IPlaylistEntry * pIPE = m_MediaLibrary.GetEntryByURL(szURL);

	if(pIPE)
	{
		pIPE->SetField(ulFieldID, pNewData);

		UpdateTitles();
		//make sure the source selector window exists we can get here before its created
		if(m_SourceSelectorWindow)
			m_SourceSelectorWindow->UpdateView();
		if(PathIsURL(szURL) && ulFieldID == FIELD_ARTIST)
		{
			if(GetForegroundWindow() == m_hWnd && !IsIconic(m_hWnd))
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE, NULL, NULL);
			else
				m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_BLIND, NULL, NULL);
		}

		m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
	}
}

bool	CTuniacApp::GetArt(LPTSTR szSource)
{
	bool bArtSuccess = false;

	if(StrCmpI(szSource, m_AlbumArtPanel.GetCurrentArtSource()) == 0)
		return false;

	IInfoManager * pManager = m_MediaLibrary.GetInfoManagerForFilename(szSource);
	//Attempt art from infomanager(embedded art)
	if(pManager)
	{
		LPVOID art;
		unsigned long ulSize;
		TCHAR	szMimeType[128];
		unsigned long artType;
		if(pManager->GetAlbumArt(0, &art, &ulSize, szMimeType, &artType))
		{
			if(m_AlbumArtPanel.SetSource(art, ulSize, szMimeType))
			{
				bArtSuccess = true;
				m_AlbumArtPanel.SetCurrentArtSource(szSource);
			}

			pManager->FreeAlbumArt(art);
		}
	}
	//No art from infomanager. Try external file(folder.jpg/png)
	if(!bArtSuccess)
	{
		TCHAR		szJPGPath[MAX_PATH];
		TCHAR		szPNGPath[MAX_PATH];
		StringCchCopy(szJPGPath, MAX_PATH, szSource);

		PathRemoveFileSpec(szJPGPath);

		StringCchCopy(szPNGPath, MAX_PATH, szJPGPath);

		PathAppend(szJPGPath, TEXT("folder.jpg"));
		PathAppend(szPNGPath, TEXT("folder.png"));

		if((StrCmpI(szJPGPath, m_AlbumArtPanel.GetCurrentArtSource()) == 0) || (StrCmpI(szPNGPath, m_AlbumArtPanel.GetCurrentArtSource()) == 0))
			return false;

		if(m_AlbumArtPanel.SetSource(szJPGPath))
		{
			bArtSuccess = true;
			m_AlbumArtPanel.SetCurrentArtSource(szJPGPath);
		}
		else if(m_AlbumArtPanel.SetSource(szPNGPath))
		{
			bArtSuccess = true;
			m_AlbumArtPanel.SetCurrentArtSource(szPNGPath);
		}
	}

	//No art found embedded or external. Load default art in tuniac base dir
	if(!bArtSuccess)
	{
		TCHAR szURL[MAX_PATH];
		GetModuleFileName(NULL, szURL, MAX_PATH);
		PathRemoveFileSpec(szURL);
		PathAppend(szURL, TEXT("NoAlbumArt.jpg"));

		if (StrCmpI(szURL, m_AlbumArtPanel.GetCurrentArtSource()) == 0)
			return false;

		m_AlbumArtPanel.SetCurrentArtSource(szURL);
		m_AlbumArtPanel.SetSource(szURL);
	}

	if(m_SourceSelectorWindow)
		m_SourceSelectorWindow->Refresh();

	return bArtSuccess;
}

//bAuto		0 = user interaction caused song change
//			1 = auto selected song from Tuniac logic(eg track ended and next track plays)


bool	CTuniacApp::PlayEntry(IPlaylistEntry * pIPE, bool bStart, bool bAuto, bool bResetAudio)
{
	if(pIPE)
	{
		//open for art before opening for decode.
		if(m_Preferences.GetShowAlbumArt())
			GetArt((LPTSTR)pIPE->GetField(FIELD_URL));

		IPlaylist * pPlaylist = m_PlaylistManager.GetActivePlaylist();
		if(pPlaylist)
		if (!pPlaylist->SetActiveNormalFilteredIndex(pPlaylist->GetNormalFilteredIndexforEntry(pIPE)))
		{
			//history can pick songs that are filtered out
			IPlaylistEX * pPlaylistEX =  (IPlaylistEX *)pPlaylist;
			if (pPlaylistEX)
				pPlaylistEX->SetActiveRealIndex(pPlaylistEX->GetRealIndexforEntry(pIPE));
		}

		if(CCoreAudio::Instance()->SetSource((LPTSTR)pIPE->GetField(FIELD_URL), (float *)pIPE->GetField(FIELD_REPLAYGAIN_ALBUM_GAIN), (float *)pIPE->GetField(FIELD_REPLAYGAIN_TRACK_GAIN), bResetAudio))
		{
			if(bStart)
				CCoreAudio::Instance()->Play();

			//reset softpause on manual song changes
			if(!bAuto)
			{
				m_SoftPause.bNow = false;
				m_SoftPause.ulAt = INVALID_PLAYLIST_INDEX;
			}
			else if(m_SoftPause.bNow || m_SoftPause.ulAt == pIPE->GetEntryID())
			{
				//reset feature/GUI menu for  feature
				m_SoftPause.bNow = false;
				m_SoftPause.ulAt = INVALID_PLAYLIST_INDEX;
				SendMessage(m_hWnd, WM_MENUSELECT, 0, 0);
				//this will only stop the last stream, not all of them
				CCoreAudio::Instance()->StopLast();
			}

			UpdateState();

			if(m_SourceSelectorWindow)
			{
				m_SourceSelectorWindow->UpdateView();
				//focus current song if we are following playback
				if(m_SourceSelectorWindow->GetVisiblePlaylistIndex() == m_PlaylistManager.GetActivePlaylistIndex() && m_Preferences.GetFollowCurrentSongMode())
						m_SourceSelectorWindow->ShowCurrentlyPlaying();
			}

			//update queues
			if(pPlaylist)
			{
				if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
				{
					//add history
					m_History.AddEntryID(pIPE->GetEntryID());

					unsigned long ulIndex = pPlaylist->GetActiveNormalFilteredIndex();
					//remove playselected
					if(m_PlaySelected.GetCount())
					{
						if(ulIndex == m_PlaySelected[0])
						{
							m_PlaySelected.RemoveAt(0);
							m_SourceSelectorWindow->m_PlaylistSourceView->DeselectItem(ulIndex);
						}
					}

					//remove playqueue, find first oocurance and remove all before it if needed
					for(unsigned long i = 0; i < m_Queue.GetCount(); i++)
					{
						unsigned long ulEntryID = m_Queue.GetEntryIDAtIndex(i);
						unsigned long ulFilteredIndex = ((IPlaylistEX *)pPlaylist)->GetNormalFilteredIndexforEntryID(ulEntryID);
						if(ulFilteredIndex == ulIndex)
						{
							for(unsigned long x = 0; x <= i; x++)
							{
								m_Queue.Remove(0);

								if(m_Preferences.GetRepeatMode() == RepeatAllQueued)
									m_Queue.Append(ulEntryID);
							}
							break;
						}
					}
				}
			}

			RebuildFutureMenu();

			if(GetForegroundWindow() == m_hWnd && !IsIconic(m_hWnd))
			{
				if(bAuto)
					m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE, NULL, NULL);
				else
					m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUAL, NULL, NULL);
			}
			else
			{
				if(bAuto)
					m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_BLIND, NULL, NULL);
				//else if(::IsWindowVisible(m_hWnd) && !IsIconic(m_hWnd))
				else 
					m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUALBLIND, NULL, NULL);
			}

			return true;
		}
	}
	return false;
}