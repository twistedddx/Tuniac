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

#pragma once

#include "FileAssoc.h"
#include "History.h"

enum RepeatMode
{
	RepeatNone = 0,
	RepeatOne,
	RepeatAll,
	RepeatAllQueued
};

enum TrayIconMode
{
	TrayIconNever = 0,
	TrayIconMinimize,
	TrayIconOnly,
	TrayIconBoth
};

enum ScreenSaveMode
{
	ScreenSaveAllow = 0,
	ScreenSavePrevent,
	ScreenSavePreventFull
};

class CPreferences
{
protected:
	HWND		m_hPage;
	RECT		m_PagePos;

	typedef struct
	{
		TCHAR *			pszName;
		int				iParent;
		DLGTEMPLATE *	pTemplate;
		DLGPROC			pDialogFunc;
		HTREEITEM		hTreeItem;
	} PrefPage;
	PrefPage	m_Pages[9];
	int			m_StartPage;

	HWND		m_hTextFormatToolTip;

	int			m_iSourceViewDividerX;
	int			m_iActiveWindow;

	BOOL		m_bMainWindowMaximized;
	BOOL		m_bMainWindowMinimized;
	BOOL		m_bMinimizeOnClose;
	BOOL		m_bAlwaysOnTop;

	BOOL		m_bPauseOnScreensave;
	BOOL		m_bPauseOnLock;
	BOOL		m_bPauseOnSwitch;
	BOOL		m_bResumeOnScreensave;
	BOOL		m_bResumeOnLock;
	BOOL		m_bResumeOnSwitch;
	BOOL		m_bRememberPos;
	int			m_iDelayInSecs;

	BOOL		m_bShowAlbumArt;
	BOOL		m_bArtOnSelection;
	BOOL		m_bFollowCurrentSong;
	BOOL		m_bSmartSorting;
	BOOL		m_bSkipStreams;
	BOOL		m_bShuffleState;
	BOOL		m_bSkipPlaylistImport;
	BOOL		m_bPlaylistSorting;
	BOOL		m_bAutoAddPlaylist;
	BOOL		m_bAddSingleStream;
	BOOL		m_bAutoSoftPause;

	BOOL		m_bCrossfadeEnabled;
	int			m_iCrossfadeTime;

	int			m_iAudioBuffering;
	BOOL		m_bReplayGain;
	BOOL		m_bReplayGainAlbum;

	int			m_iVolPercent;

	BOOL		m_bEQEnabled;
	float		m_fEQLow;
	float		m_fEQMid;
	float		m_fEQHigh;
	float		m_fAmpGain;

	TCHAR		m_szWindowFormatString[256];
	TCHAR		m_szPluginFormatString[256];
	TCHAR		m_szListFormatString[256];

	RECT		m_MainWindowRect;

	TrayIconMode	m_eTrayIconMode;
	RepeatMode		m_eRepeatMode;
	ScreenSaveMode	m_eScreenSaveMode;

	int			m_iPlaylistViewNumColumns;
	int			m_PlaylistViewColumnIDs[FIELD_MAXFIELD];
	int			m_PlaylistViewColumnWidths[FIELD_MAXFIELD];

	int			m_iVisualFPS;
	int			m_iCurrentVisual;
	BOOL		m_bShowVisArt;

	int			m_iFileAssocType;

	int			m_iHistoryListSize;
	int			m_iFutureListSize;

	void		BuildTree(HWND hTree, int iPage);
	int			FindNthTreeLeaf(int i, int iParent);


	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// general
	static LRESULT CALLBACK GeneralProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK FormattingProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LibraryProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK FileAssocProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// plugins
	static LRESULT CALLBACK PluginsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CoreAudioProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK VisualsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// audio
	static LRESULT CALLBACK AudioProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK EQProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CPreferences(void);
	~CPreferences(void);

	CFileAssoc	m_FileAssoc;

	bool	DefaultPreferences(void);

	bool	LoadPreferences(void);
	bool	SavePreferences(void);
	void	CleanPreferences(void);

	bool	ShowPreferences(HWND hParentWnd, unsigned int iStartPage);

	unsigned int	GetPreferencesPageCount(void);
	bool	GetPreferencesPageName(unsigned int iPage, LPTSTR szDest, unsigned long iSize);

	bool	PluginGetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool	PluginSetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);

public:
// preference accessor methods here!
	void	SetSourceViewDividerX(int iPixels);
	int		GetSourceViewDividerX(void);

	void	SetActiveWindow(int iActiveWindow);
	int		GetActiveWindow(void);

	void	SetMainWindowRect(RECT * lpRect);
	RECT *	GetMainWindowRect(void);

	void	SetMainWindowMaximized(BOOL bMaximized);
	BOOL	GetMainWindowMaximized(void);

	void	SetMainWindowMinimized(BOOL bMinimized);
	BOOL	GetMainWindowMinimized(void);

	BOOL	CrossfadingEnabled(void);

	int		GetCrossfadeTime(void);
	void	SetCrossfadeTime(int time);

	int		GetAudioBuffering(void);

	BOOL	ReplayGainEnabled(void);
	BOOL	ReplayGainUseAlbumGain(void);

	int		GetVolumePercent(void);
	void	SetVolumePercent(int iVolPercent);

	BOOL	GetEQEnabled(void);
	void	SetEQEnabled(BOOL bEnabled);
	float	GetEQLowGain(void);
	float	GetEQMidGain(void);
	float	GetEQHighGain(void);
	void	SetEQGain(float fEQLow, float fEQMid, float fEQHigh);

	float	GetAmpGain(void);
	void	SetAmpGain(float gain);

	int		GetPlaylistViewNumColumns(void);
	void	SetPlaylistViewNumColumns(int iColumns);

	int		GetPlaylistViewColumnIDAtIndex(int index);
	void	SetPlaylistViewColumnIDAtIndex(int index, int ID);

	int		GetPlaylistViewColumnWidthAtIndex(int index);
	void	SetPlaylistViewColumnWidthAtIndex(int index, int Width);

	//LPTSTR	GetTheme(void);

	BOOL	GetShuffleState(void);
	void	SetShuffleState(BOOL bEnabled);

	RepeatMode	GetRepeatMode(void);
	void		SetRepeatMode(RepeatMode eMode);

	void		SetFollowCurrentSongMode(BOOL bEnabled);
	BOOL		GetFollowCurrentSongMode(void);

	BOOL		GetPauseOnScreensave(void);
	BOOL		GetPauseOnLock(void);
	BOOL		GetPauseOnSwitch(void);
	BOOL		GetResumeOnScreensave(void);
	BOOL		GetResumeOnLock(void);
	BOOL		GetResumeOnSwitch(void);
	BOOL		GetRememberPos(void);
	int			GetDelayInSecs(void);

	BOOL		GetShowAlbumArt(void);
	BOOL		GetArtOnSelection(void);

	LPTSTR		GetWindowFormatString(void);
	LPTSTR		GetPluginFormatString(void);
	LPTSTR		GetListFormatString(void);

	int			GetVisualFPS(void);
	int			GetCurrentVisual(void);
	void		SetCurrentVisual(int iVisual);
	BOOL		GetShowVisArt(void);

	TrayIconMode	GetTrayIconMode(void);
	void		SetTrayIconMode(TrayIconMode eMode);

	ScreenSaveMode	GetScreenSaveMode(void);
	void		SetScreenSaveMode(ScreenSaveMode eMode);

	BOOL		GetMinimizeOnClose(void);
	BOOL		GetAlwaysOnTop(void);
	void		SetAlwaysOnTop(BOOL bEnabled);

	int			GetHistoryListSize(void);
	int			GetFutureListSize(void);

	BOOL		GetSkipPlaylistImport(void);
	BOOL		GetCanPlaylistsSort(void);
	BOOL		GetAutoAddPlaylists(void);
	BOOL		GetAddSingleStream(void);
	BOOL		GetAutoSoftPause(void);
	BOOL		GetSmartSortingEnabled(void);
	BOOL		GetSkipStreams(void);

};
