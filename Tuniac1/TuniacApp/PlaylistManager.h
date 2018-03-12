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

#pragma once

#include "LibraryPlaylist.h"
#include "StandardPlaylist.h"
#include "AudioCDPlaylist.h"
//#include "RadioTunerPlaylist.h"

#define INVALID_PLAYLIST_INDEX			0xffffffff

class CPlaylistManager
{
protected:
	HANDLE						m_hThread;
	unsigned long				m_dwThreadID;

	unsigned long				m_ulPlaylistID;

	static unsigned long __stdcall PMThreadStub(void * in);
	unsigned long PMThreadProc(void);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool			AddCDWithDriveLetter(char cDriveLetter);
	bool			DeleteCDWithDriveLetter(char cDriveLetter);

public:
	IPlaylist			*			m_ActivePlaylist;
	unsigned long					m_ulActivePlaylistIndex;

	CLibraryPlaylist				m_LibraryPlaylist;
	
//	CRadioTunerPlaylist				m_RadioPlaylist;

	Array<CAudioCDPlaylist *, 3>	m_CDPlaylists;

	Array<CStandardPlaylist *, 3>	m_StandardPlaylists;

public:
	CPlaylistManager(void);
	~CPlaylistManager(void);

	bool			Initialize(LPTSTR szLibraryFolder);
	bool			Shutdown(LPTSTR szLibraryFolder, bool bSave = true);

	bool			SavePlaylistLibrary(LPTSTR szLibraryFolder);
	bool			LoadPlaylistLibrary(LPTSTR szLibraryFolder);


	unsigned long	GetNumPlaylists(void);

	IPlaylist *		GetPlaylistByIndex(unsigned long ulIndex);
	IPlaylist *		GetPlaylistByID(unsigned long ulPlaylistID);

	unsigned long	GetPlaylistIndexByID(unsigned long ulID);
	unsigned long	GetPlaylistIDByIndex(unsigned long ulIndex);

	IPlaylist *		GetActivePlaylist(void);
	unsigned long	GetActivePlaylistIndex(void);
	unsigned long	GetActivePlaylistID(void);


	bool			SetActiveByEntry(IPlaylistEntry * pIPE);
	bool			SetActivePlaylistByIndex(unsigned long ulIndex);
	bool			SetActivePlaylistByID(unsigned long ulID);

	bool			CreateNewStandardPlaylist(LPTSTR szName);
	bool			CreateNewStandardPlaylistWithIDs(LPTSTR szName, EntryArray & newIDs);
	bool			MoveStandardPlaylist(unsigned long ulIndex, unsigned long ulNewIndex);

	bool			DeletePlaylistByIndex(unsigned long ulIndex);
	bool			DeletePlaylistByID(unsigned long ulID);
};
