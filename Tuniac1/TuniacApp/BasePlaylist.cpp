#include "stdafx.h"
#include ".\baseplaylist.h"

CBasePlaylist::CBasePlaylist(void)
{
	m_ActiveIndex	= INVALID_PLAYLIST_INDEX;
	m_SortType		= SORTED_UNSORTED;
	m_LastSortBy	= -1;

	StrCpy(m_szPlaylistName, TEXT(""));

	StrCpy(m_szTextFilter, TEXT(""));
	m_ulTextFilterField = FIELD_MAXFIELD;
	m_bTextFilterReversed = false;
}

CBasePlaylist::~CBasePlaylist(void)
{
}

bool				CBasePlaylist::ApplyFilter(void)
{
	TCHAR szTemp[512];

	if(lstrlen(m_szTextFilter))
	{
		for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
		{
			m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? false : true;

			if(m_ulTextFilterField == FIELD_MAXFIELD)
			{
				if(StrStrI((LPTSTR)m_PlaylistArray[x].pEntry->GetField(FIELD_TITLE), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pEntry->GetField(FIELD_ARTIST), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pEntry->GetField(FIELD_ALBUM), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}
			} else if(m_ulTextFilterField == FIELD_ARTIST
					|| m_ulTextFilterField == FIELD_ALBUM
					|| m_ulTextFilterField == FIELD_TITLE
					|| m_ulTextFilterField == FIELD_GENRE
					|| m_ulTextFilterField == FIELD_COMMENT
					|| m_ulTextFilterField == FIELD_URL
					|| m_ulTextFilterField == FIELD_FILENAME
					|| m_ulTextFilterField == FIELD_FILEEXTENSION)
			{
				if(StrStrI((LPTSTR)m_PlaylistArray[x].pEntry->GetField(m_ulTextFilterField), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}
			}
			else
			{
				m_PlaylistArray[x].pEntry->GetTextRepresentation(m_ulTextFilterField, szTemp, 512);
				if (StrCmpI(m_szTextFilter, szTemp) == 0)
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

			}
		}
	}
	else
	{
		for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
		{
			m_PlaylistArray[x].bFiltered = false;
		}
	}

	g_Rand.RandomInit(GetTickCount());

	m_RandomIndexArray.RemoveAll();
	m_NormalIndexArray.RemoveAll();

	int CurrentIndex = 0;

	for(unsigned long y=0; y<m_PlaylistArray.GetCount(); y++)
	{
		if(m_PlaylistArray[y].bFiltered == false)
		{
			m_NormalIndexArray.AddTail(y);
			m_RandomIndexArray.AddTail(y);
			if(y == m_ActiveIndex)
			{
				CurrentIndex = m_RandomIndexArray.GetCount()-1;
			}
		}
	}

	int x, temp;
	for(int i = m_RandomIndexArray.GetCount() - 1; i > 0; i--)
	{
		x = g_Rand.IRandom(0, i);
		temp = m_RandomIndexArray[x];
		m_RandomIndexArray[x] = m_RandomIndexArray[i];
		m_RandomIndexArray[i] = temp;
		if(x == CurrentIndex)
			CurrentIndex = x;
		else if(i == CurrentIndex)
			CurrentIndex = i;
	}

	if(m_RandomIndexArray.GetCount() > 0)
	{
		temp = m_RandomIndexArray[0];
		m_RandomIndexArray[0] = m_RandomIndexArray[CurrentIndex];
		m_RandomIndexArray[CurrentIndex] = temp;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
//			Playlist transposing code

int					CBasePlaylist::NormalRealIndexToFilteredIndex(int iIndex)
{
	for(int x=0; x<m_NormalIndexArray.GetCount(); x++)
	{
		if(m_NormalIndexArray[x] == iIndex)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

int					CBasePlaylist::NormalFilteredIndexToRealIndex(int iIndex)
{
	return m_NormalIndexArray[iIndex];
}


int					CBasePlaylist::RandomRealIndexToFilteredIndex(int iIndex)
{
	for(int x=0; x<m_RandomIndexArray.GetCount(); x++)
	{
		if(m_RandomIndexArray[x] == iIndex)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

int					CBasePlaylist::RandomFilteredIndexToRealIndex(int iIndex)
{
	return m_RandomIndexArray[iIndex];
}

///////////////////////////////////////////////////////////////////////
//
//	General Crap

unsigned long		CBasePlaylist::GetFlags(void)
{
	return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_EXTENDED | PLAYLISTEX_FLAGS_CANFILTER | PLAYLISTEX_FLAGS_CANSORT | PLAYLISTEX_FLAGS_CANADD | PLAYLISTEX_FLAGS_CANDELETE | PLAYLISTEX_FLAGS_CANMOVE;
}

unsigned long		CBasePlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_UNKNOWN;
}

bool				CBasePlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	if(szPlaylistName)
	{
		StrCpyN(m_szPlaylistName, szPlaylistName, 255);
		return true;
	}

	return false;
}

LPTSTR				CBasePlaylist::GetPlaylistName(void)
{
	return m_szPlaylistName;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	playlist controls

bool				CBasePlaylist::Previous(void)
{
	if(m_ActiveIndex == INVALID_PLAYLIST_INDEX)
		return false;

	if(m_PlaylistArray[m_ActiveIndex].bFiltered)
	{
		SetActiveIndex(0);
		return true;
	}

	int iFilteredIndex;

	if(tuniacApp.GetShuffleState())
	{
		iFilteredIndex = RandomRealIndexToFilteredIndex(m_ActiveIndex);
		if(iFilteredIndex == 0)
		{
			if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
				iFilteredIndex = m_RandomIndexArray.GetCount()-1;
			else
				return false;
		}
		else if(iFilteredIndex != -1)
			SetActiveIndex(NormalRealIndexToFilteredIndex(m_RandomIndexArray[iFilteredIndex-1]));
	}
	else
	{
		iFilteredIndex = NormalRealIndexToFilteredIndex(m_ActiveIndex);
		if(iFilteredIndex == 0)
			return false;
		else
			SetActiveIndex(iFilteredIndex-1);
	}

	return true;
}

bool				CBasePlaylist::Next(void)
{
	if(!CheckNext())
		return false;

	int iFilteredIndex;

	if(tuniacApp.GetShuffleState())
	{
		iFilteredIndex = RandomRealIndexToFilteredIndex(m_ActiveIndex);
		if(iFilteredIndex == INVALID_PLAYLIST_INDEX)
			SetActiveIndex(0);
		else
			SetActiveIndex(NormalRealIndexToFilteredIndex(m_RandomIndexArray[iFilteredIndex + 1]));
	}
	//if repeatone the next song is just the current song
	if(tuniacApp.GetRepeatMode() == RepeatOne)
	{
		return true;
	}
	//if repeatall and we are at the last song, the next song is the first song
	else if(tuniacApp.GetRepeatMode() == RepeatAll)
	{
		if(tuniacApp.GetShuffleState() && RandomRealIndexToFilteredIndex(m_ActiveIndex) >= m_RandomIndexArray.GetCount() - 1)
		{
			SetActiveIndex(0);
			return true;
		}
		else if(NormalRealIndexToFilteredIndex(m_ActiveIndex) >= m_NormalIndexArray.GetCount() - 1)
		{
			SetActiveIndex(0);
			return true;
		}
	}
	iFilteredIndex = NormalRealIndexToFilteredIndex(m_ActiveIndex);
	if(iFilteredIndex == INVALID_PLAYLIST_INDEX)
		SetActiveIndex(0);
	else
		SetActiveIndex(iFilteredIndex + 1);

	return true;
}

bool				CBasePlaylist::CheckNext(void)
{
	if(m_ActiveIndex == INVALID_PLAYLIST_INDEX)
		return false;

	int iFilteredIndex;
	if(tuniacApp.GetShuffleState())
		iFilteredIndex = RandomRealIndexToFilteredIndex(m_ActiveIndex);
	else
		iFilteredIndex = NormalRealIndexToFilteredIndex(m_ActiveIndex);

	if(iFilteredIndex == INVALID_PLAYLIST_INDEX)
		return true;

	return CheckNext(iFilteredIndex);
}

bool				CBasePlaylist::CheckNext(int iBaseIndex)
{
	if(iBaseIndex == INVALID_PLAYLIST_INDEX)
		return false;

	if(tuniacApp.GetRepeatMode() == RepeatOne)
		return true;
	if(tuniacApp.GetRepeatMode() == RepeatAll)
		return true;

	if(iBaseIndex >= m_NormalIndexArray.GetCount() - 1)
		return false;

	return true;
}

int				CBasePlaylist::GetNextIndex(int iBaseIndex, bool * pbWrapped)
{
	if(pbWrapped) *pbWrapped = false;
	if(m_NormalIndexArray.GetCount() == 0)
	{
		return INVALID_PLAYLIST_INDEX;
	}
	if(m_NormalIndexArray.GetCount() == 1)
	{
		if(pbWrapped) *pbWrapped = true;
		return 0;
	}
	if(tuniacApp.GetRepeatMode() == RepeatOne)
	{
		return iBaseIndex;
	}
	else if(tuniacApp.GetRepeatMode() == RepeatAll)
	{
		if(tuniacApp.GetShuffleState() && RandomRealIndexToFilteredIndex(m_ActiveIndex) >= m_RandomIndexArray.GetCount() - 1)
		{
			return RandomRealIndexToFilteredIndex(m_RandomIndexArray[0]);
		}
		if(NormalRealIndexToFilteredIndex(m_ActiveIndex) >= m_NormalIndexArray.GetCount() - 1)
		{
			return NormalRealIndexToFilteredIndex(m_NormalIndexArray[0]);
		}
	}


	if(tuniacApp.GetShuffleState())
	{
		int iIndex = RandomRealIndexToFilteredIndex(NormalFilteredIndexToRealIndex(iBaseIndex));
		if(iIndex >= m_RandomIndexArray.GetCount() - 1)
		{
			if(pbWrapped) *pbWrapped = true;
			iIndex = -1;
		}
		return NormalRealIndexToFilteredIndex(m_RandomIndexArray[iIndex + 1]);
	}
	else
	{
		if(iBaseIndex >= m_NormalIndexArray.GetCount() - 1)
		{
			if(pbWrapped) *pbWrapped = true;
			return 0;
		}
		return iBaseIndex + 1;
	}
}
//////////////////////////////////////////////////////////

IPlaylistEntry	*	CBasePlaylist::GetActiveItem(void)
{
	if(m_ActiveIndex == INVALID_PLAYLIST_INDEX)
		return NULL;
/*
	if(tuniacApp.GetShuffleState())
	{
		int finalIndex = RandomRealIndexToFilteredIndex(m_ActiveIndex);

		if(finalIndex == INVALID_PLAYLIST_INDEX)
			return NULL;

		return m_PlaylistArray[RandomFilteredIndexToRealIndex(finalIndex)].pEntry;
	}
	else
	{
		int finalIndex = NormalRealIndexToFilteredIndex(m_ActiveIndex);

		if(finalIndex == INVALID_PLAYLIST_INDEX)
			return NULL;

		return m_PlaylistArray[NormalFilteredIndexToRealIndex(finalIndex)].pEntry;
	}

	return NULL;*/
	return m_PlaylistArray[m_ActiveIndex].pEntry;
}

unsigned long		CBasePlaylist::GetItemPlayOrder(unsigned long ulIndex)
{
	if(tuniacApp.GetShuffleState())
	{
		int iRealIndex = NormalFilteredIndexToRealIndex(ulIndex);
		return RandomRealIndexToFilteredIndex(iRealIndex);
	}
	else
	{
		return ulIndex;
	}
}


unsigned long		CBasePlaylist::GetNumItems(void)
{
	return m_NormalIndexArray.GetCount();
}

bool				CBasePlaylist::SetActiveIndex(int iIndex)
{
	if(iIndex >= m_NormalIndexArray.GetCount())
	{
		m_ActiveIndex = INVALID_PLAYLIST_INDEX;
		return false;
	}

	if(m_ActiveIndex == INVALID_PLAYLIST_INDEX)
	{
		int iRandIndex = RandomRealIndexToFilteredIndex(iIndex);
		m_RandomIndexArray.CycleToStart(iRandIndex);
	}

	m_ActiveIndex = NormalFilteredIndexToRealIndex(iIndex);

	return true;
}

int					CBasePlaylist::GetActiveIndex(void)
{
	if(m_ActiveIndex == INVALID_PLAYLIST_INDEX)
		return INVALID_PLAYLIST_INDEX;

	return NormalRealIndexToFilteredIndex(m_ActiveIndex);
}

IPlaylistEntry *	CBasePlaylist::GetItemAtIndex(int iIndex)
{
	int RealIndex = NormalFilteredIndexToRealIndex(iIndex);

	if(RealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[RealIndex].pEntry;
	}

	return NULL;
}

bool				CBasePlaylist::SetTextFilter(LPTSTR	szFilterString)
{
	StrCpyN(m_szTextFilter, szFilterString, 255);

	ApplyFilter();

	return true;
}

LPTSTR				CBasePlaylist::GetTextFilter(void)
{
	return m_szTextFilter;
}

bool				CBasePlaylist::SetTextFilterField(unsigned long ulFieldID)
{
	if(ulFieldID > FIELD_MAXFIELD)
		return false;
	m_ulTextFilterField = ulFieldID;
	ApplyFilter();
	return true;
}

unsigned long		CBasePlaylist::GetTextFilterField(void)
{
	return m_ulTextFilterField;
}

bool				CBasePlaylist::SetTextFilterReversed(bool bReverse)
{
	m_bTextFilterReversed = bReverse;
	ApplyFilter();
	return true;
}

bool				CBasePlaylist::GetTextFilterReversed(void)
{
	return m_bTextFilterReversed;
}

void			CBasePlaylist::GetFieldFilteredList(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse)
{
	PlaylistEntry PE;
	entryArray.RemoveAll();
	TCHAR szTemp[512];
	for(unsigned long x = 0; x < m_PlaylistArray.GetCount(); x++)
	{
		m_PlaylistArray[x].pEntry->GetTextRepresentation(ulFieldID, szTemp, 512);
		if (bReverse ? StrCmpI(szFilterString, szTemp) != 0 : StrCmpI(szFilterString, szTemp) == 0)
		{
			entryArray.AddTail(m_PlaylistArray[x].pEntry);
		}
	}
}

bool				CBasePlaylist::Sort(unsigned long ulSortBy)
{

	bool reversesort = false;

	if(m_LastSortBy == ulSortBy)
	{
		if(m_SortType == SORTED_ASCENDING)
		{
			m_SortType = SORTED_DESCENDING;
			reversesort = true;
		}
		else
		{
			m_SortType = SORTED_ASCENDING;
		}
	}
	else
	{
		m_SortType = SORTED_ASCENDING;
	}

	m_LastSortBy = ulSortBy;

	if (m_PlaylistArray.GetCount() < 2)
		return true;

	Sort_Algorithm(0, m_PlaylistArray.GetCount() - 1, ulSortBy, reversesort);

	ApplyFilter();

	return true;
}

void				CBasePlaylist::Sort_Algorithm(unsigned long begin, unsigned long end, unsigned long ulSortBy, bool reverse)
{	// quicksort

	IPlaylistEntry * pTemp;


	// find a better pivot to avoid obvious worse cases (ie, reverse sorting)
	// move best pivot to the end
	if((end - begin) >= 5)
	{
		int middle = (end + begin) / 2;
		int iBeginEnd = Sort_CompareItems(m_PlaylistArray[begin].pEntry, m_PlaylistArray[end].pEntry, ulSortBy);
		int iBeginMiddle = Sort_CompareItems(m_PlaylistArray[begin].pEntry, m_PlaylistArray[middle].pEntry, ulSortBy);
		int iMiddleEnd = Sort_CompareItems(m_PlaylistArray[middle].pEntry, m_PlaylistArray[end].pEntry, ulSortBy);
		
		if(iBeginEnd < 0 && iBeginMiddle < 0)
		{ //best pivot is middle

			pTemp = m_PlaylistArray[end].pEntry;
			m_PlaylistArray[end].pEntry = m_PlaylistArray[middle].pEntry;
			m_PlaylistArray[middle].pEntry = pTemp;

			if(m_ActiveIndex == end)
				m_ActiveIndex = middle;
			else if(m_ActiveIndex == middle)
				m_ActiveIndex = end;

		}
		else if(iBeginMiddle > 0 && iBeginEnd < 0)
		{ //best pivot is begin

			pTemp = m_PlaylistArray[begin].pEntry;
			m_PlaylistArray[begin].pEntry = m_PlaylistArray[end].pEntry;
			m_PlaylistArray[end].pEntry = pTemp;

			if(m_ActiveIndex == begin)
				m_ActiveIndex = end;
			else if(m_ActiveIndex == end)
				m_ActiveIndex = begin;

		}
	}

	int iComp = 0;
	int m = begin;
	IPlaylistEntry * pPivot = m_PlaylistArray[end].pEntry;

	for (int i = begin; i < end; i++)
	{
		iComp = Sort_CompareItems(m_PlaylistArray[i].pEntry, pPivot, ulSortBy);
		if(reverse)
			iComp = iComp * (-1);
		if (iComp < 0)
		{
			pTemp = m_PlaylistArray[m].pEntry;
			m_PlaylistArray[m].pEntry = m_PlaylistArray[i].pEntry;
			m_PlaylistArray[i].pEntry = pTemp;

			if(m_ActiveIndex == m)
				m_ActiveIndex = i;
			else if(m_ActiveIndex == i)
				m_ActiveIndex = m;

			m++;
		}
	}

	pTemp = m_PlaylistArray[end].pEntry;
	m_PlaylistArray[end].pEntry = m_PlaylistArray[m].pEntry;
	m_PlaylistArray[m].pEntry = pTemp;

	if(m_ActiveIndex == end)
		m_ActiveIndex = m;
	else if(m_ActiveIndex == m)
		m_ActiveIndex = end;

	if ((m - 1) > 0 && begin < (m - 1))
		Sort_Algorithm(begin, m - 1, ulSortBy, reverse);
	if ((m + 1) > 0 && (m + 1) < end)
		Sort_Algorithm(m + 1, end, ulSortBy, reverse);


}

int				CBasePlaylist::Sort_CompareItems(IPlaylistEntry * pItem1, IPlaylistEntry * pItem2, unsigned long ulSortBy)
{

	switch(ulSortBy)
	{
		case FIELD_DATEADDED:
		case FIELD_DATEFILECREATION:
			{
				FILETIME		ft[2];

				SystemTimeToFileTime((LPSYSTEMTIME)pItem1->GetField(ulSortBy),		&ft[0]);
				SystemTimeToFileTime((LPSYSTEMTIME)pItem2->GetField(ulSortBy),		&ft[1]);

				long x = CompareFileTime(&ft[0], &ft[1]);
				if(x < 0)
					return -1;
				else if(x > 0)
					return 1;
			}
			break;

		case FIELD_DATELASTPLAYED:
			{
				if(pItem1->GetField(FIELD_PLAYCOUNT))
				{
					FILETIME		ft[2];

					SystemTimeToFileTime((LPSYSTEMTIME)pItem1->GetField(ulSortBy),		&ft[0]);
					SystemTimeToFileTime((LPSYSTEMTIME)pItem2->GetField(ulSortBy),		&ft[1]);

					long x = CompareFileTime(&ft[0], &ft[1]);
					
					if(x < 0)
						return -1;
					else if(x > 0)
						return 1;
				}
				else if(pItem2->GetField(FIELD_PLAYCOUNT))
				{
					return -1;
				}
			}
			break;

		case FIELD_PLAYBACKTIME:
		case FIELD_BITRATE:
		case FIELD_SAMPLERATE:
		case FIELD_NUMCHANNELS:
		case FIELD_RATING:
		case FIELD_FILESIZE:
		case FIELD_PLAYCOUNT:
			{
				if(pItem1->GetField(ulSortBy) > pItem2->GetField(ulSortBy))
				{
					return 1;
				}
				else if(pItem1->GetField(ulSortBy) < pItem2->GetField(ulSortBy))
				{
					return -1;
				}
			}
			break;

		case FIELD_YEAR:
			{
				if(pItem1->GetField(FIELD_YEAR) > pItem2->GetField(FIELD_YEAR))
				{
					return 1;
				}
				else if(pItem1->GetField(FIELD_YEAR) < pItem2->GetField(FIELD_YEAR))
				{
					return -1;
				}
				return Sort_CompareItems(pItem1, pItem2, FIELD_ALBUM);
			}
			break;
		
		case FIELD_COMMENT:
		case FIELD_GENRE:
		case FIELD_TITLE:
		case FIELD_FILENAME:
		case FIELD_URL:
			{
				return(StrCmpI((LPTSTR)pItem1->GetField(ulSortBy), (LPTSTR)pItem2->GetField(ulSortBy)));
				
			}
			break;

		case FIELD_ARTIST:
			{

				int iItem1Offset = 0, iItem2Offset = 0;
				if(tuniacApp.m_Preferences.GetSmartSortingEnabled())
				{
					if(StrNCmpI((LPTSTR)pItem1->GetField(FIELD_ARTIST), L"A ", 2) == 0)
						iItem1Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem1->GetField(FIELD_ARTIST), L"THE ", 4) == 0)
						iItem1Offset = 4;
					
					if(StrNCmpI((LPTSTR)pItem2->GetField(FIELD_ARTIST), L"A ", 2) == 0)
						iItem2Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem2->GetField(FIELD_ARTIST), L"THE ", 4) == 0)
						iItem2Offset = 4;
				}

				int x = StrCmpI((LPTSTR)pItem1->GetField(FIELD_ARTIST) + iItem1Offset, (LPTSTR)pItem2->GetField(FIELD_ARTIST) + iItem2Offset);

				if(x > 0)
				{
					return 1;
				}
				else if (x < 0)
				{
					return -1;
				}

				return Sort_CompareItems(pItem1, pItem2, FIELD_ALBUM);

				
			}
			break;

		case FIELD_ALBUM:
			{
				int iItem1Offset = 0, iItem2Offset = 0;
				if(tuniacApp.m_Preferences.GetSmartSortingEnabled())
				{
					if(StrNCmpI((LPTSTR)pItem1->GetField(FIELD_ALBUM), L"A ", 2) == 0)
						iItem1Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem1->GetField(FIELD_ALBUM), L"THE ", 4) == 0)
						iItem1Offset = 4;
					
					if(StrNCmpI((LPTSTR)pItem2->GetField(FIELD_ALBUM), L"A ", 2) == 0)
						iItem2Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem2->GetField(FIELD_ALBUM), L"THE ", 4) == 0)
						iItem2Offset = 4;
				}

				int y = StrCmpI((LPTSTR)pItem1->GetField(FIELD_ALBUM) + iItem1Offset, (LPTSTR)pItem2->GetField(FIELD_ALBUM) + iItem2Offset);

				if(y > 0)
				{
					return 1;
				}
				else if (y < 0)
				{
					return -1;
				}
				return Sort_CompareItems(pItem1, pItem2, FIELD_TRACKNUM);
			}
			break;

		case FIELD_TRACKNUM:
			{
				short * pTrack1 = (short *)pItem1->GetField(FIELD_TRACKNUM);
				short * pTrack2 = (short *)pItem2->GetField(FIELD_TRACKNUM);

					if(pTrack1[0] > pTrack2[0])
					{
						return 1;
					}
					else if (pTrack1[0] < pTrack2[0])
					{
						return -1;
					}
			}
			break;
	}

	return(0);

}

bool				CBasePlaylist::AddEntryArray(EntryArray & entryArray)
{
	PlaylistEntry PE;

	PE.bFiltered	= false;

	for(unsigned long x=0; x<entryArray.GetCount(); x++)
	{
		PE.pEntry		= entryArray[x];
		m_PlaylistArray.AddTail(PE);
	}

	ApplyFilter();
	return true;
}

bool				CBasePlaylist::DeleteItemArray(IndexArray &	indexArray)
{
	for(unsigned long x=0; x<indexArray.GetCount(); x++)
	{
		indexArray[x] = NormalFilteredIndexToRealIndex(indexArray[x]);
	}

	while(indexArray.GetCount())
	{
		for(unsigned long x=1; x<indexArray.GetCount(); x++)
		{
			if(indexArray[0] < indexArray[x])
				indexArray[x]--;
		}

		int realIndex = indexArray[0];

		if(m_ActiveIndex != INVALID_PLAYLIST_INDEX)
		{
			if(realIndex < m_ActiveIndex)
			{
				m_ActiveIndex--;
			}
			else if(realIndex == m_ActiveIndex)
			{
				m_ActiveIndex = INVALID_PLAYLIST_INDEX;
			}
		}

		m_PlaylistArray.RemoveAt(realIndex);
		indexArray.RemoveAt(0);
	}

	ApplyFilter();

	return true;
}

bool				CBasePlaylist::MoveItemArray(unsigned long ToIndex, IndexArray &	indexArray)
{
	EntryArray		theEntries;
	bool			bIDActive = false;
	int				iIDActive = -1;

	for(unsigned long x=0; x<indexArray.GetCount(); x++)
	{
		int realindex = NormalFilteredIndexToRealIndex(indexArray[x]);
		theEntries.AddTail(m_PlaylistArray[realindex].pEntry);

		if((indexArray[x] <= ToIndex) && (ToIndex > 0))
			ToIndex--;

		if(realindex == m_ActiveIndex)
		{
			bIDActive = true;
			iIDActive = x;
		}
	}


	DeleteItemArray(indexArray);

	if(ToIndex > m_PlaylistArray.GetCount())
	{
		ToIndex = m_PlaylistArray.GetCount();
	}

	for(unsigned long id = 0; id < theEntries.GetCount(); id ++)
	{
		PlaylistEntry	PLE;
		PLE.pEntry		= theEntries[id];

		m_PlaylistArray.InsertBefore(ToIndex + id, PLE);
		if(m_ActiveIndex >= ToIndex + id)
		{
			m_ActiveIndex++;
		}

		if(bIDActive)
		{
			if(id == iIDActive)
			{
				m_ActiveIndex = ToIndex + id;
				bIDActive = false;
			}
		}
	}


	ApplyFilter();
	return true;
}
