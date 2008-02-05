#pragma once

#include "Array.h"
#include "LibraryEntry.h"
#include "IPlaylist.h"

class CPlayQueue
{
protected:
	Array<IPlaylistEntry *, 3>	m_Queue;

public:
	CPlayQueue(void);
	~CPlayQueue(void);

	unsigned long		GetCount(void);

	IPlaylistEntry *	GetItemAtIndex(unsigned long ulIndex);

	bool				Prepend(IPlaylistEntry * pEntry);
	bool				Append(IPlaylistEntry * pEntry);

	bool				Remove(int iIndex);
	bool				RemoveItem(IPlaylistEntry * pEntry);
	void				Clear(void);
};
