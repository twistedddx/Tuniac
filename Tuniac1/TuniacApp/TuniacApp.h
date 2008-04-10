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

#pragma once

#include "medialibrary.h"
#include "Preferences.h"

#include "IWindow.h"
#include "CoreAudio.h"

#include "PlaylistManager.h"

#include "SourceSelectorWindow.h"
#include "LogWindow.h"

#include "PlayControls.h"

#include "SysEvents.h"
#include "Taskbar.h"
#include "History.h"
#include "CurrentlyPlaying.h"
#include "TuniacSkin.h"

#include "PluginManager.h"

#include "AlbumArt.h"

#define FONT_SIZE_LARGE				0
#define FONT_SIZE_MEDIUM			1
#define FONT_SIZE_SMALL_MEDIUM		2
#define FONT_SIZE_SMALL				3
#define FONT_SIZE_TINY				4

#define NOTIFY_UPDATEWINDOWTITLE	10
#define NOTIFY_PLAYLISTSCHANGED		11

#define MENU_BASE					(50000)
#define PREFERENCESMENU_BASE		(50100)
#define PLAYLISTMENU_BASE			(50200)
#define HISTORYMENU_BASE			(50300)
#define FUTUREMENU_BASE				(50400)

#define WM_TRAYICON 				WM_USER + 1

class CTuniacApp
{
protected:

	HINSTANCE							m_hInstance;

	HACCEL								m_hAccel;

	HWND								m_hWnd;
	HWND								m_hWndStatus;

	WNDCLASSEX							m_wc;
	WNDCLASSEX							m_wcp;

	HMENU								m_hPopupMenu;
	HMENU								m_hFutureMenu;
	HMENU								m_TrayMenu;

	HANDLE								m_hOneInstanceOnlyMutex;

	HFONT								m_LargeFont;
	HFONT								m_MediumFont;
	HFONT								m_SmallMediumFont;
	HFONT								m_SmallFont;
	HFONT								m_TinyFont;

	ATOM								m_aPlay; 
	ATOM								m_aStop; 
	ATOM								m_aNext; 
	ATOM								m_aRandNext; 
	ATOM								m_aPrev; 
	ATOM								m_aPrevByHist;
	ATOM								m_aVolUp; 
	ATOM								m_aVolDn; 
	ATOM								m_aSeekForward; 
	ATOM								m_aSeekBack; 
	ATOM								m_aShuffle; 
	ATOM								m_aRepeat; 

	bool								m_bSavePrefs;
	bool								m_bSaveML;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK		WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK		WndParentProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK		AddOtherProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool						RegisterHotkeys(void);



public:
	CMediaLibrary				m_MediaLibrary;
	CPlaylistManager			m_PlaylistManager;

	CPreferences				m_Preferences;

	CPlayControls				m_PlayControls;

	CSourceSelectorWindow	*	m_SourceSelectorWindow;
	CLogWindow				*	m_LogWindow;

	Array<IWindow *,3>			m_WindowArray;

	//CCoreAudio					m_CoreAudio;

	CPluginManager				m_PluginManager;
	CSysEvents					m_SysEvents;
	CTaskbar					m_Taskbar;
	CHistory					m_History;
	CCurrentlyPlaying			m_CurrentlyPlaying;

	CTuniacSkin					m_Skin;

	CAlbumArt					m_TestArt;


	struct
	{
		bool bNow;
		unsigned long ulAt;
	}							m_SoftPause;
	IndexArray					m_PlaySelected;
	IndexArray					m_FutureMenu;


public:
	CTuniacApp();
	virtual ~CTuniacApp();

	bool				Initialize(HINSTANCE hInstance, LPTSTR szCommandLine);
	bool				Shutdown(void);

	bool				Run(void);

	HINSTANCE			getMainInstance(void)	{ return m_hInstance; }
	HWND				getMainWindow(void)		{ return m_hWnd; }
	bool				getSavePrefs(void)		{ return m_bSavePrefs; }

	bool				SetStatusText(LPTSTR szStatusText);

	HFONT				GetTuniacFont(int size);

	bool				CoreAudioMessage(unsigned long Message, void * Params);

	bool				FormatSongInfo(LPTSTR szDest, unsigned int iDestSize, IPlaylistEntry * pIPE, LPTSTR szFormat, bool bPlayState);
	bool				EscapeMenuItemString(LPTSTR szSource, LPTSTR szDest,  unsigned int iDestSize);
	bool				DoSoftPause(void);

	HMENU				GetFutureMenu(void);

	IPlaylistEntry *	GetFuturePlaylistEntry(int iFromCurrent);
	void				BuildFuturePlaylistArray(void);
	void				RebuildFutureMenu(void);
	void				UpdateQueues(void);
	bool				SetArt(void);
	void				SetupReplayGain(IPlaylistEntry * pIPE);


};
