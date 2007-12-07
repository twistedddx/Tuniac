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
	PrefPage	m_Pages[8];
	int			m_StartPage;

	HWND		m_hTextFormatToolTip;

	int			m_SourceViewDividerX;

	int			m_CrossfadeEnabled;
	int			m_CrossfadeTime;

	int			m_OutputDevice;

	float		m_VolumeScale;

	RECT		m_MainWindowRect;
	BOOL		m_MainWindowMaximized;

	TCHAR		m_Theme[128];

	TCHAR		m_WindowFormatString[256];
	TCHAR		m_PluginFormatString[256];
	TCHAR		m_ListFormatString[256];

	TrayIconMode	m_eTrayIconMode;
	BOOL		m_MinimizeOnClose;

	BOOL		m_AlwaysOnTop;
	BOOL		m_PauseOnLock;
	BOOL		m_PauseOnScreensave;
	BOOL		m_FollowCurrentSong;
	BOOL		m_SmartSorting;
	BOOL		m_RandomState;

	int			m_PlaylistViewNumColumns;
	int			m_PlaylistViewColumnIDs[15];
	int			m_PlaylistViewColumnWidths[15];

	RepeatMode	m_RepeatMode;

	int			m_VisualFPS;

	int			m_FileAssocType;

	int			m_HistoryListSize;
	int			m_FutureListSize;

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

public:
	CPreferences(void);
	~CPreferences(void);

	CFileAssoc	m_FileAssoc;

	bool	RecursiveDeleteReg(HKEY hKeyRoot, LPTSTR lpSubKey);

	bool	DefaultPreferences(void);

	bool	LoadPreferences(void);
	bool	SavePreferences(void);
	bool	CleanPreferences(void);

	bool	ShowPreferences(HWND hParentWnd, unsigned int iStartPage);

	unsigned int	GetPreferencesPageCount(void);
	bool	GetPreferencesPageName(unsigned int iPage, LPTSTR szDest, unsigned long iSize);

	bool	PluginGetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool	PluginSetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);

public:
// preference accessor methods here!
	void	SetSourceViewDividerX(int x);
	int		GetSourceViewDividerX(void);

	void	SetMainWindowRect(RECT * lpRect);
	RECT *	GetMainWindowRect(void);

	void	SetMainWindowMaximized(bool bMaximized);
	bool	GetMainWindowMaximized(void);

	int		GetCrossfadeTime(void);
	void	SetCrossfadeTime(int time);

	int		GetOutputDevice(void);

	int		GetVolumePercent(void);
	void	SetVolumeScale(float Scale);

	int		GetPlaylistViewNumColumns(void);
	void	SetPlaylistViewNumColumns(int iColumns);

	int		GetPlaylistViewColumnIDAtIndex(int index);
	void	SetPlaylistViewColumnIDAtIndex(int index, int ID);

	int		GetPlaylistViewColumnWidthAtIndex(int index);
	void	SetPlaylistViewColumnWidthAtIndex(int index, int Width);

	LPTSTR	GetTheme(void);

	bool	GetRandomState(void);
	void	SetRandomState(bool bEnabled);

	RepeatMode	GetRepeatMode(void);
	void		SetRepeatMode(RepeatMode eMode);

	void		SetFollowCurrentSongMode(bool bEnabled);
	bool		GetFollowCurrentSongMode(void);

	bool		GetSmartSortingEnabled(void);

	bool		GetPauseOnLock(void);
	bool		GetPauseOnScreensave(void);

	LPTSTR		GetWindowFormatString(void);
	LPTSTR		GetPluginFormatString(void);
	LPTSTR		GetListFormatString(void);

	unsigned long	GetVisualFPS(void);

	TrayIconMode	GetTrayIconMode(void);
	void		SetTrayIconMode(TrayIconMode eMode);
	bool		GetMinimizeOnClose(void);

	bool		GetAlwaysOnTop(void);
	void		SetAlwaysOnTop(bool bEnabled);

	int			GetHistoryListSize(void);
	int			GetFutureListSize(void);
};
