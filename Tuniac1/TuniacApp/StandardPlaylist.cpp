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

unsigned long		CStandardPlaylist::GetIDAtRealIndex(int iIndex)
{
	return m_PlaylistArray[iIndex].pEntry->GetEntryID();
}

bool				CStandardPlaylist::DeleteAllItemsWhereIDEquals(unsigned long ID)
{
	IndexArray		indexesToDelete;

	for(int x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pEntry->GetEntryID() == ID)
		{
			int t = NormalRealIndexToFilteredIndex(x);
			indexesToDelete.AddTail(t);
		}
	}

	return DeleteItemArray(indexesToDelete);
}