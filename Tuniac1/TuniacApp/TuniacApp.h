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
#include "VisualWindow.h"
#include "LogWindow.h"

#include "PlayControls.h"

#include "SysEvents.h"
#include "Taskbar.h"
#include "History.h"
//#include "CurrentlyPlaying.h"
#include "TuniacSkin.h"

#include "PluginManager.h"

#include "AlbumArt.h"

#define FONT_SIZE_LARGE				0
#define FONT_SIZE_MEDIUM			1
#define FONT_SIZE_SMALL_MEDIUM		2
#define FONT_SIZE_SMALL				3
#define FONT_SIZE_TINY				4

#define HOTKEY_PLAY					0
#define HOTKEY_STOP					1
#define HOTKEY_NEXT					2
#define HOTKEY_RANDNEXT				3
#define HOTKEY_PREV					4
#define HOTKEY_PREVBYHISTORY		5
#define HOTKEY_VOLUP				6
#define HOTKEY_VOLDOWN				7
#define HOTKEY_SEEKFORWARD			8
#define HOTKEY_SEEKBACK				9
#define HOTKEY_SHUFFLE				10
#define HOTKEY_REPEAT				11

#define NOTIFY_UPDATEWINDOWTITLE	10
#define NOTIFY_PLAYLISTSCHANGED		11

#define MENU_BASE					(50000)
#define PREFERENCESMENU_BASE		(50100)
#define HISTORYMENU_BASE			(50200)
#define FUTUREMENU_BASE				(50300)
#define TRAYMENU_BASE				(50400)
#define PLAYLISTMENU_BASE			(50500)

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

	bool								m_bSavePrefs;
	bool								m_bSaveML;

	int									m_ActiveScreen;

	int									m_iFailedSongRetry;
	//int									m_iCPUCount;

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
	CVisualWindow			*	m_VisualWindow;
	CLogWindow				*	m_LogWindow;

	Array<IWindow *,3>			m_WindowArray;

	//CCoreAudio					m_CoreAudio;

	CPluginManager				m_PluginManager;
	CSysEvents					m_SysEvents;
	CTaskbar					m_Taskbar;
	CHistory					m_History;
	//CCurrentlyPlaying			m_CurrentlyPlaying;

	CTuniacSkin					m_Skin;

	CAlbumArt					m_AlbumArtPanel;


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

	LPTSTR				GetActiveScreenName(void);

	bool				SetStatusText(LPTSTR szStatusText);

	HFONT				GetTuniacFont(int size);

	bool				CoreAudioMessage(unsigned long Message, void * Params);

	bool				FormatSongInfo(LPTSTR szDest, unsigned int iDestSize, IPlaylistEntry * pIPE, LPTSTR szFormat, bool bPlayState);
	bool				EscapeMenuItemString(LPTSTR szSource, LPTSTR szDest,  unsigned int iDestSize);

	HMENU				GetFutureMenu(void);
	IPlaylistEntry *	GetFuturePlaylistEntry(int iFromCurrent);
	void				BuildFuturePlaylistArray(void);
	void				RebuildFutureMenu(void);

	void				UpdateState(void);
	void				UpdateTitles(void);
	void				UpdateStreamTitle(LPTSTR szURL, LPTSTR szTitle, unsigned long ulFieldID);
	void				UpdateQueues(void);
	bool				DoSoftPause(void);

	bool				SetArt(IPlaylistEntry * pIPE);
};
