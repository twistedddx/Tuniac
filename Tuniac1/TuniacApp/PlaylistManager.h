#pragma once

#include "LibraryPlaylist.h"
#include "StandardPlaylist.h"
#include "AudioCDPlaylist.h"
//#include "RadioTunerPlaylist.h"

class CPlaylistManager
{
protected:
	HANDLE						m_hThread;
	unsigned long				m_dwThreadID;

	static unsigned long __stdcall PMThreadStub(void * in);
	unsigned long PMThreadProc(void);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool			AddCDWithDriveLetter(char cDriveLetter);
	bool			DeleteCDWithDriveLetter(char cDriveLetter);

public:
	IPlaylist			*			m_ActivePlaylist;

	CLibraryPlaylist				m_LibraryPlaylist;
	
//	CRadioTunerPlaylist				m_RadioPlaylist;

	Array<CAudioCDPlaylist *, 3>	m_CDPlaylists;

	Array<CStandardPlaylist *, 3>	m_StandardPlaylists;

public:
	CPlaylistManager(void);
	~CPlaylistManager(void);

	bool			Initialize(void);
	bool			Shutdown(bool bSave = true);

	bool			SavePlaylistLibrary(void);
	bool			LoadPlaylistLibrary(void);


	unsigned long	GetNumPlaylists(void);

	IPlaylist *		GetPlaylistAtIndex(unsigned long ulIndex);
	IPlaylist *		GetActivePlaylist(void);

	bool			SetActiveByEntry(IPlaylistEntry * pEntry);
	bool			SetActivePlaylist(unsigned long ulPlaylistNumber);
	int				GetActivePlaylistIndex(void);

	bool			CreateNewStandardPlaylist(LPTSTR szName);
	bool			CreateNewStandardPlaylistWithIDs(LPTSTR szName, EntryArray & newIDs);
	bool			MoveStandardPlaylist(unsigned long ulIndex, unsigned long ulNewIndex);

	bool			DeletePlaylistAtIndex(unsigned long ulPlaylistNumber);
};
