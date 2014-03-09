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

#include "iplaylist.h"
#include "LibraryEntry.h"

class CMediaLibraryPlaylistEntry :
	public IPlaylistEntry
{
protected:
	unsigned long		m_ulEntryID;
	LibraryEntry		m_LibraryEntry;
	unsigned long		m_ulUrlCycled;

	bool				m_bDirty;

public:
	CMediaLibraryPlaylistEntry(LibraryEntry * pIPE);
	~CMediaLibraryPlaylistEntry(void);

	unsigned long GetEntryID(void);
	void SetEntryID(unsigned long ulEntryID);

	bool			IsDirty(void);

	LibraryEntry *	GetLibraryEntry(void) { return & m_LibraryEntry; }

public:	
	void *	GetField(unsigned long ulFieldID);
	bool	SetField(unsigned long ulFieldID, void * pNewData);
	bool	SetField(unsigned long ulFieldID, unsigned long pNewData);
	bool	SetField(unsigned long ulFieldID, float pNewData);
	bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars);
};
