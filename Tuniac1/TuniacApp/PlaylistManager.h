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
	unsigned long					m_ulActivePlaylistIndex;

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
	unsigned long	GetActivePlaylistIndex(void);

	bool			SetActiveByEntry(IPlaylistEntry * pEntry);
	bool			SetActivePlaylist(unsigned long ulPlaylistNumber);


	bool			CreateNewStandardPlaylist(LPTSTR szName);
	bool			CreateNewStandardPlaylistWithIDs(LPTSTR szName, EntryArray & newIDs);
	bool			MoveStandardPlaylist(unsigned long ulIndex, unsigned long ulNewIndex);

	bool			DeletePlaylistAtIndex(unsigned long ulPlaylistNumber);
};
