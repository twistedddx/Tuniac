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

class CRadioTunerPlaylist :
	public IPlaylist,
	public IPlaylistEntry
{
protected:


	TCHAR				m_StationName[256];

	TCHAR				m_CurrentSong[256];

	Array<LPTSTR, 3>	m_URLList;

public:
	CRadioTunerPlaylist(void);
	~CRadioTunerPlaylist(void);

	void	ResetURLS(void);
	void	AddURL(LPTSTR szURL);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	bool				Next(void);
	bool				CheckNext(void);

	IPlaylistEntry	*	GetActiveItem(void);

public:	
	unsigned long	GetEntryID(void);

	void *	GetField(unsigned long ulFieldID);
	bool	SetField(unsigned long ulFieldID, void * pNewData);

	bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars);
};
