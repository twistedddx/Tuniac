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
	CMediaLibraryPlaylistEntry(LibraryEntry * pEntry);
	~CMediaLibraryPlaylistEntry(void);

	unsigned long GetEntryID(void);
	void SetEntryID(unsigned long ulEntryID);

	bool			IsDirty(void);

	LibraryEntry *	GetLibraryEntry(void) { return & m_LibraryEntry; }

public:	
	void *	GetField(unsigned long ulFieldID);
	bool	SetField(unsigned long ulFieldID, void * pNewData);
	bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars);
};
