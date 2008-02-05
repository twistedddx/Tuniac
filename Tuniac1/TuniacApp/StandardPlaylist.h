#pragma once
#include "baseplaylist.h"

class CStandardPlaylist :
	public CBasePlaylist
{
public:
	CStandardPlaylist(void);
	~CStandardPlaylist(void);

	unsigned long		GetRealCount(void);
	unsigned long		GetIDAtRealIndex(unsigned long ulRealIndex);

public:
	unsigned long		GetPlaylistType(void);
};
