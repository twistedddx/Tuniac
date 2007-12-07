#pragma once
#include "baseplaylist.h"

class CStandardPlaylist :
	public CBasePlaylist
{
public:
	CStandardPlaylist(void);
	~CStandardPlaylist(void);

	unsigned long		GetRealCount(void);
	unsigned long		GetIDAtRealIndex(int iIndex);

	bool				DeleteAllItemsWhereIDEquals(unsigned long ID);

public:
	unsigned long		GetPlaylistType(void);
};
