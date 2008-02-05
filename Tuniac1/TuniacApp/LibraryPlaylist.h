#pragma once
#include "baseplaylist.h"

class CLibraryPlaylist :
	public CBasePlaylist
{
public:
	CLibraryPlaylist(void);
	~CLibraryPlaylist(void);

public:
	unsigned long		GetPlaylistType(void);
	unsigned long		GetFlags(void);

	bool				AddEntryToPlaylist(IPlaylistEntry * lpPLE);
	bool				DeleteItemArray(IndexArray &	indexArray);

	unsigned long		GetRealCount(void);
	unsigned long		GetIDAtRealIndex(unsigned long ulIndex);

	bool				RebuildPlaylist(void);
};
