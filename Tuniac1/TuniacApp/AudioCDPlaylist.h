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
#include "iplaylist.h"

#include "ntddcdrm.h"
#include "devioctl.h"

class CAudioCDPlaylist :
	public IPlaylist
{
protected:
	TCHAR						m_AlbumTitle[256];
	char						m_DriveLetter;

	int							m_ActiveItem;
	
	EntryArray					m_TrackList;

	CDROM_TOC					m_TOC;

	bool GetCDInfo(void);
	bool ReadTOC(void);

public:
	CAudioCDPlaylist(char cDriveLetter);
	~CAudioCDPlaylist(void);

	char GetDriveLetter(void) { return m_DriveLetter; }

	unsigned long		GetNumCDTracks(void);
	IPlaylistEntry *	GetEntryAtIndex(unsigned long Index);

	unsigned long		GetActiveIndex();
	bool				SetActiveIndex(int iIndex);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	int					GetNextIndex(void);
	bool				Next(void);

	IPlaylistEntry	*	GetActiveItem(void);
	unsigned long		GetActiveEntryID(void);
};
