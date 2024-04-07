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
#include "iplaylist.h"

class CMicPlaylist :
	public IPlaylist
{
protected:

	bool						m_IsEnabled;
	unsigned long				m_ulPlaylistID;

	TCHAR						m_PlaylistName[256];
	char						m_MicInput;

	unsigned long				m_ActiveItem;
	
	EntryArray					m_TrackList;

	bool GetMicInfo(void);

public:
	CMicPlaylist(char cMicInput);
	~CMicPlaylist(void);

	bool				IsEnabled(void);
	void				SetEnabled(bool bEnabled);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	void				SetPlaylistID(unsigned long ulPlaylistID);
	unsigned long		GetPlaylistID(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	unsigned long		Previous(void);
	unsigned long		Next(void);

	bool				CheckFilteredIndex(unsigned long ulFilteredIndex);

	unsigned long		GetActiveFilteredIndex(void);
	bool				SetActiveFilteredIndex(unsigned long ulFilteredIndex);
	unsigned long		GetActiveNormalFilteredIndex(void);
	bool				SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex);

	unsigned long		GetNextFilteredIndexForFilteredIndex(unsigned long ulFilteredIndex);

	IPlaylistEntry	*	GetActiveEntry(void);

	IPlaylistEntry *	GetEntryAtFilteredIndex(unsigned long ulFilteredIndex);
	IPlaylistEntry *	GetEntryAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex);

	unsigned long		GetFilteredIndexforEntry(IPlaylistEntry * pIPE);
	unsigned long		GetNormalFilteredIndexforEntry(IPlaylistEntry * pIPE);

	unsigned long		GetNumItems(void);
};
