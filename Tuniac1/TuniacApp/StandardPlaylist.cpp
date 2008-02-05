// A standand playlist is not playlist 0 the LibraryPlaylist(.cpp) or the low number AudioCDLibrary(.cpp) when they exist


#include "stdafx.h"
#include ".\standardplaylist.h"

CStandardPlaylist::CStandardPlaylist(void)
{
}

CStandardPlaylist::~CStandardPlaylist(void)
{
}

unsigned long		CStandardPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_STANDARDPLAYLIST;
}

unsigned long		CStandardPlaylist::GetRealCount(void)
{
	return m_PlaylistArray.GetCount();
}

unsigned long		CStandardPlaylist::GetIDAtRealIndex(unsigned long ulRealIndex)
{
	return m_PlaylistArray[ulRealIndex].pEntry->GetEntryID();
}
