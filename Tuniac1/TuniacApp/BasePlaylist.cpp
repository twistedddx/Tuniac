//every type of playlist is a base playlist. Playlist 0 is the LibraryPlaylist.cpp, low numbers playlists may be AudioCDPlaylist.cpp

#include "stdafx.h"
#include ".\baseplaylist.h"

CBasePlaylist::CBasePlaylist(void)
{
	m_ActiveRealIndex	= INVALID_PLAYLIST_INDEX;
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

	//if filter text exists
	if(lstrlen(m_szTextFilter))
	{
		//check items that match text
		for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
		{
			//if reversed, we flip each files filterness before the filter to make it the opposite result
			//meaning all files are filtered until they are unfiltered in the filter below
			//default false = include, true = exclude
			m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? false : true;

			//default filter(FIELD_MAXFIELD) is any title, artist and album matches
			if(m_ulTextFilterField == FIELD_MAXFIELD)
			{
				if(StrStrI((LPTSTR)m_PlaylistArray[x].pEntry->GetField(FIELD_TITLE), m_szTextFilter))
				{
					//Toggles the filteredness if matched.
					//Watch if was "filtered true" already before as mentioned, making it now false again
					//set this entry as: false = include, true = exclude
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

			//single field filter selected. Will include or exclude based solely on the single field
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

				//reverse filter selected, this will inverse the current filter
				if (StrCmpI(m_szTextFilter, szTemp) == 0)
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

			}
		}
	}

	//no filter text, so unfilter all items
	else
	{
		for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
		{
			m_PlaylistArray[x].bFiltered = false;
		}
	}


	//remake our filtered lists(normal and shuffle) based on new filter
	g_Rand.RandomInit(GetTickCount());

	m_RandomIndexArray.RemoveAll();
	m_NormalIndexArray.RemoveAll();

	unsigned long ulCurrentIndex = 0;

	//for each item, if not set as filtered, add
	for(unsigned long y=0; y<m_PlaylistArray.GetCount(); y++)
	{
		if(m_PlaylistArray[y].bFiltered == false)
		{
			m_NormalIndexArray.AddTail(y);
			m_RandomIndexArray.AddTail(y);
			//get the random filter list index where our current song index is
			//used later to move this to the first index in out randomlist
			if(y == m_ActiveRealIndex)
				ulCurrentIndex = m_RandomIndexArray.GetCount()-1;
		}
	}

	//shuffle our random filter list
	int x, temp;
	for(int i = m_RandomIndexArray.GetCount() - 1; i > 0; i--)
	{
		//get a random number between 0 and our random filter list size
		x = g_Rand.IRandom(0, i);

		//get the real random index value at the filtered random list index
		temp = m_RandomIndexArray[x];

		//move real random index value to another filtered random list index (should be unbias)
		m_RandomIndexArray[x] = m_RandomIndexArray[i];

		//move the 2nd real random index value to original random filtered random list index
		m_RandomIndexArray[i] = temp;

		//if our current song index is in the random or logical next filtered random list index, keep track of where it is now
		if(i == ulCurrentIndex)
			ulCurrentIndex = x;
		else if(x == ulCurrentIndex)
			ulCurrentIndex = i;
	}

	//if filtered random list has atleast a valid entry swap the first index with where we know our current song is
	if(m_RandomIndexArray.GetCount() > 0)
	{
		temp = m_RandomIndexArray[0];
		m_RandomIndexArray[0] = m_RandomIndexArray[ulCurrentIndex];
		m_RandomIndexArray[ulCurrentIndex] = temp;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
//			Playlist transposing code
//realindex = m_MediaLibrary
//normalfilteredindex = m_NormalIndexArray (these are valid playback files with shuffle off)
//randomfilteredindex = m_RandomIndexArray (these are valid playback files with shuffle on)

//m_MediaLibrary = FULL ML array. Contains all file info
//based on m_MediaLibrary
//m_PlaylistArray = full current(view) playlist array. Contains only indexes of m_MediaLibrary
//based on m_PlaylistArray
//m_NormalIndexArray = filtered normal m_PlaylistArray. Contains only indexes of m_MediaLibrary
//m_RandomIndexArray = filtered random m_PlaylistArray. Contains only indexes of m_MediaLibrary


//send index in m_MediaLibrary and return the index in m_NormalIndexArray
unsigned long		CBasePlaylist::RealIndexToNormalFilteredIndex(unsigned long ulRealIndex)
{
	for(unsigned long x=0; x<m_NormalIndexArray.GetCount(); x++)
	{
		if(m_NormalIndexArray[x] == ulRealIndex)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

//send index in m_NormalIndexArray and return the index in m_MediaLibrary
unsigned long		CBasePlaylist::NormalFilteredIndexToRealIndex(unsigned long ulNormalFilteredIndex)
{
	//m_NormalIndexArray is valid between 0 and m_NormalIndexArray.GetCount() -1
	if(ulNormalFilteredIndex < 0)
		return INVALID_PLAYLIST_INDEX;

	if(ulNormalFilteredIndex >= m_NormalIndexArray.GetCount())
		return INVALID_PLAYLIST_INDEX;

		return m_NormalIndexArray[ulNormalFilteredIndex];
}

//send index in m_MediaLibrary and return the index in m_NormalIndexArray
unsigned long		CBasePlaylist::RealIndexToRandomFilteredIndex(unsigned long ulRealIndex)
{
	for(unsigned long x=0; x<m_RandomIndexArray.GetCount(); x++)
	{
		if(m_RandomIndexArray[x] == ulRealIndex)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

//send index in m_NormalIndexArray and return the index in m_MediaLibrary
unsigned long		CBasePlaylist::RandomFilteredIndexToRealIndex(unsigned long ulRandomFilteredIndex)
{
	//m_NormalIndexArray is valid between 0 and m_NormalIndexArray.GetCount() -1
	if(ulRandomFilteredIndex < 0)
		return INVALID_PLAYLIST_INDEX;

	if(ulRandomFilteredIndex >= m_RandomIndexArray.GetCount())
		return INVALID_PLAYLIST_INDEX;

		return m_RandomIndexArray[ulRandomFilteredIndex];
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

//will set m_ActiveRealIndex to the previous song and return that songs index or -1 if it fails
bool				CBasePlaylist::Previous(void)
{

	if(tuniacApp.m_CoreAudio.GetPosition() < 3500)
	{
		unsigned long ulFilteredIndex;

		//check if we are repeatingall and play last song in list
		if( GetActiveFilteredIndex() == 0 && tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll )
			//m_NormalIndexArray/m_RandomIndexArray should always be the same length
			ulFilteredIndex = m_NormalIndexArray.GetCount() - 1;
		else
			//try to set active as -1 of current active
			ulFilteredIndex = GetActiveFilteredIndex() - 1;

		if(CheckFilteredIndex(ulFilteredIndex))
		{
			SetActiveFilteredIndex(ulFilteredIndex);

			IPlaylistEntry * pIPE = GetActiveItem();

			//are we crossfading?
			if(tuniacApp.m_Preferences.GetCrossfadeTime())
				//crossfade to next song
				tuniacApp.m_CoreAudio.TransitionTo(pIPE);

			else
				//move straight to next song
				tuniacApp.m_CoreAudio.SetSource(pIPE);
			
			tuniacApp.m_CoreAudio.Play();
			tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUAL, NULL, NULL);
		}
		//no valid previous song(start of playlist?), simply rewind
		else
		{
			tuniacApp.m_CoreAudio.Stop();
			tuniacApp.m_CoreAudio.SetPosition(0);
		}
	}
	//still at start, simply rewind
	else
	{
		tuniacApp.m_CoreAudio.SetPosition(0);
	}

	return true;
}

//will set m_ActiveRealIndex to the next song and return that songs index or -1 if it fails
bool				CBasePlaylist::Next(void)
{
	//get the next index
	unsigned long ulFilteredIndex = GetNextFilteredIndex(GetActiveFilteredIndex(), 1, 1);

	//check that index is valid
	bool bCheck = CheckFilteredIndex(ulFilteredIndex);

	//if we got an index and it's valid, set it
	if(bCheck)
	{
		//set our found next index
		SetActiveFilteredIndex(ulFilteredIndex);

		//activeitem should now be the NEXT song

		IPlaylistEntry * pIPE = GetActiveItem();

		//are we crossfading?
		if(tuniacApp.m_Preferences.GetCrossfadeTime())
			//crossfade to next song
			tuniacApp.m_CoreAudio.TransitionTo(pIPE);

		else
			//move straight to next song
			tuniacApp.m_CoreAudio.SetSource(pIPE);


		tuniacApp.m_CoreAudio.Play();
		tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUAL, NULL, NULL);


		//check if we were set to stop at this next song
		tuniacApp.DoSoftPause();

		return true;
	}
	else
		return false;
}

bool				CBasePlaylist::CheckFilteredIndex(unsigned long ulFilteredIndex)
{
	//too low
	if(ulFilteredIndex < 0)
		return false;

	//too high, m_NormalIndexArray/m_RandomIndexArray should be the same length
	if(ulFilteredIndex > m_NormalIndexArray.GetCount() - 1)
		return false;

	return true;
}

//will return the next song index and return -1 the index given is not valid
unsigned long		CBasePlaylist::GetNextFilteredIndex(unsigned long ulFilteredIndex, bool bFollowSelected, bool bFollowQueue)
{
///////////////// normal logic

	//check that we are checking a valid index
	if(!CheckFilteredIndex(ulFilteredIndex))
		return -1;

	//if repeatone the next song is just the current song
	if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatOne)
		return ulFilteredIndex;


	unsigned long ulActiveIndex = GetActiveFilteredIndex();

///////////////// play selected logic
		//if PlaySelected is valid check

	if(bFollowSelected)
	{
		int iPlaySize = tuniacApp.m_PlaySelected.GetCount();

		IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();
		if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED && iPlaySize)
		{

			//if we are checking the current song, the next song is our playselected
			if(ulFilteredIndex == ulActiveIndex )
				return tuniacApp.m_PlaySelected[0];

			//see if found within m_PlaySelected 0 thru GetCount - 1
			int iFound = -1;
			for(int x=0; x < iPlaySize; x++)
			{
				if(tuniacApp.m_PlaySelected[x] == ulFilteredIndex)
				{
					iFound = x;
					break;
				}
			}

			//if it was found to be the last playselected, but watch we are not queueing(must be handle yourself check rebuildfuturemenu for example)
			if(iFound == (iPlaySize - 1))
				return ulFilteredIndex + 1;

			//if found we want it's next
			if(iFound != -1)
				return tuniacApp.m_PlaySelected[iFound + 1];
		}
	}

///////////////// play queue logic

	if(bFollowQueue)
	{
		int iQueueSize = tuniacApp.m_MediaLibrary.m_Queue.GetCount();

		//if play queue play is valid check
		if (iQueueSize)
		{
			//if we are checking the current song the next song is our queue
			if(ulFilteredIndex == ulActiveIndex )
			{
				IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.m_Queue.GetItemAtIndex(0);
				//check if the first queued song is actually the current song(bizarre? :P)
				if(GetFilteredIndexforItem(pIPE) != ulActiveIndex)
					return GetFilteredIndexforItem(pIPE);
			}

			//see if found within m_Queue 0 thru GetCount - 1
			int iFound = -1;
			for(int x=0; x < iQueueSize ; x++)
			{
				IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.m_Queue.GetItemAtIndex(x);
				if(GetFilteredIndexforItem(pIPE) == ulFilteredIndex)
				{
					iFound = x;
					break;
				}
			}

			//if it was found to be the last queueing
			if(iFound == (tuniacApp.m_MediaLibrary.m_Queue.GetCount() - 1))
				return ulFilteredIndex + 1;

			//if found we want it's next
			if(iFound != -1)
			{
				//remove dead entries
				for(int x=0; x < iQueueSize; x++)
				{
					IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.m_Queue.GetItemAtIndex(iFound + 1);
					if(GetFilteredIndexforItem(pIPE) != INVALID_PLAYLIST_INDEX)
						return GetFilteredIndexforItem(pIPE);
					else
						tuniacApp.m_MediaLibrary.m_Queue.Remove(iFound + 1);
				}
			}
		}
	}

///////////////// normal logic

	//if repeatall and we are at the last song, the next song is the first song
	//m_NormalIndexArray/m_RandomIndexArray should be the same length
	if(ulFilteredIndex == (m_NormalIndexArray.GetCount() - 1) && tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
		return 0;
	
	//try to set active as +1 of current active
	if(ulFilteredIndex <= m_NormalIndexArray.GetCount() - 2)
		return ulFilteredIndex + 1;
	

	//no files
	return -1;
}

//////////////////////////////////////////////////////////

//get the active item(any active item should be in the current playlist?
IPlaylistEntry	*	CBasePlaylist::GetActiveItem(void)
{
	if(m_ActiveRealIndex == INVALID_PLAYLIST_INDEX)
		return NULL;

	return m_PlaylistArray[m_ActiveRealIndex].pEntry;
}

//play order is simply the order of m_NormalIndexArray/m_RandomIndexArray 
unsigned long		CBasePlaylist::GetPlayOrder(unsigned long ulNormalFilteredIndex)
{
	if(tuniacApp.m_Preferences.GetShuffleState())
	{
		unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(ulNormalFilteredIndex);
		return RealIndexToRandomFilteredIndex(ulRealIndex);
	}
	else
	{
		return ulNormalFilteredIndex;
	}
}


unsigned long		CBasePlaylist::GetNumItems(void)
{
	return m_NormalIndexArray.GetCount();
}

//set active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
bool				CBasePlaylist::SetActiveFilteredIndex(unsigned long ulFilteredIndex)
{
	if(ulFilteredIndex < 0)
		return false;

	if(ulFilteredIndex >= m_NormalIndexArray.GetCount())
		return false;


	if(tuniacApp.m_Preferences.GetShuffleState())
		m_ActiveRealIndex = RandomFilteredIndexToRealIndex(ulFilteredIndex);
	else
		m_ActiveRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	return true;
}

//set active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
bool				CBasePlaylist::SetActiveNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	if(ulFilteredIndex < 0)
		return false;

	if(ulFilteredIndex >= m_NormalIndexArray.GetCount())
		return false;

	m_ActiveRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	return true;
}

//get active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
unsigned long		CBasePlaylist::GetActiveFilteredIndex(void)
{
	if(m_ActiveRealIndex == INVALID_PLAYLIST_INDEX)
		return INVALID_PLAYLIST_INDEX;

	if(tuniacApp.m_Preferences.GetShuffleState())
		return RealIndexToRandomFilteredIndex(m_ActiveRealIndex);
	else
		return RealIndexToNormalFilteredIndex(m_ActiveRealIndex);
}

//get active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
unsigned long		CBasePlaylist::GetActiveNormalFilteredIndex(void)
{
	if(m_ActiveRealIndex == INVALID_PLAYLIST_INDEX)
		return INVALID_PLAYLIST_INDEX;

	return RealIndexToNormalFilteredIndex(m_ActiveRealIndex);
}

//get the item at filtered ulIndex for m_NormalIndexArray/m_RandomIndexArray
IPlaylistEntry *	CBasePlaylist::GetItemAtFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = INVALID_PLAYLIST_INDEX;
	if(tuniacApp.m_Preferences.GetShuffleState())
		ulRealIndex = RandomFilteredIndexToRealIndex(ulFilteredIndex);
	else
		ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);
	
	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pEntry;
	}

	return NULL;
}

//get the item at filtered ulIndex for m_NormalIndexArray/m_RandomIndexArray
IPlaylistEntry *	CBasePlaylist::GetItemAtNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pEntry;
	}

	return NULL;
}

//gets real index then returns filtered index for item 
unsigned long		CBasePlaylist::GetFilteredIndexforItem(IPlaylistEntry	* pEntry)
{
	for(unsigned long x=0; x < m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pEntry == pEntry)
		{
		if(tuniacApp.m_Preferences.GetShuffleState())
			return RealIndexToRandomFilteredIndex(x);
		else
			return RealIndexToNormalFilteredIndex(x);
		}
	}

	return INVALID_PLAYLIST_INDEX;
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

			if(m_ActiveRealIndex == end)
				m_ActiveRealIndex = middle;
			else if(m_ActiveRealIndex == middle)
				m_ActiveRealIndex = end;

		}
		else if(iBeginMiddle > 0 && iBeginEnd < 0)
		{ //best pivot is begin

			pTemp = m_PlaylistArray[begin].pEntry;
			m_PlaylistArray[begin].pEntry = m_PlaylistArray[end].pEntry;
			m_PlaylistArray[end].pEntry = pTemp;

			if(m_ActiveRealIndex == begin)
				m_ActiveRealIndex = end;
			else if(m_ActiveRealIndex == end)
				m_ActiveRealIndex = begin;

		}
	}

	int iComp = 0;
	unsigned long m = begin;
	IPlaylistEntry * pPivot = m_PlaylistArray[end].pEntry;

	for (unsigned long i = begin; i < end; i++)
	{
		iComp = Sort_CompareItems(m_PlaylistArray[i].pEntry, pPivot, ulSortBy);
		if(reverse)
			iComp = iComp * (-1);
		if (iComp < 0)
		{
			pTemp = m_PlaylistArray[m].pEntry;
			m_PlaylistArray[m].pEntry = m_PlaylistArray[i].pEntry;
			m_PlaylistArray[i].pEntry = pTemp;

			if(m_ActiveRealIndex == m)
				m_ActiveRealIndex = i;
			else if(m_ActiveRealIndex == i)
				m_ActiveRealIndex = m;

			m++;
		}
	}

	pTemp = m_PlaylistArray[end].pEntry;
	m_PlaylistArray[end].pEntry = m_PlaylistArray[m].pEntry;
	m_PlaylistArray[m].pEntry = pTemp;

	if(m_ActiveRealIndex == end)
		m_ActiveRealIndex = m;
	else if(m_ActiveRealIndex == m)
		m_ActiveRealIndex = end;

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

		unsigned long ulRealIndex = indexArray[0];

		if(m_ActiveRealIndex != INVALID_PLAYLIST_INDEX)
		{
			if(ulRealIndex < m_ActiveRealIndex)
			{
				m_ActiveRealIndex--;
			}
			else if(ulRealIndex == m_ActiveRealIndex)
			{
				m_ActiveRealIndex = INVALID_PLAYLIST_INDEX;
			}
		}

		m_PlaylistArray.RemoveAt(ulRealIndex);
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
		unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(indexArray[x]);
		theEntries.AddTail(m_PlaylistArray[ulRealIndex].pEntry);

		if((indexArray[x] <= ToIndex) && (ToIndex > 0))
			ToIndex--;

		if(ulRealIndex == m_ActiveRealIndex)
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
		if(m_ActiveRealIndex >= ToIndex + id)
		{
			m_ActiveRealIndex++;
		}

		if(bIDActive)
		{
			if(id == iIDActive)
			{
				m_ActiveRealIndex = ToIndex + id;
				bIDActive = false;
			}
		}
	}


	ApplyFilter();
	return true;
}

bool				CBasePlaylist::DeleteAllItemsWhereIDEquals(unsigned long ID)
{
	IndexArray		indexesToDelete;

	for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pEntry->GetEntryID() == ID)
		{
			unsigned long t = RealIndexToNormalFilteredIndex(x);
			indexesToDelete.AddTail((int &)t);
		}
	}

	return DeleteItemArray(indexesToDelete);
}

bool				CBasePlaylist::UpdateIndex(unsigned long ulRealIndex)
{
	if(ulRealIndex < 0)
		return false;

	if(ulRealIndex >= m_PlaylistArray.GetCount())
		return false;
	
	PlaylistEntry PE;
	unsigned long ulEntry = m_PlaylistArray[ulRealIndex].pEntry->GetEntryID();
	PE.pEntry = tuniacApp.m_MediaLibrary.GetItemByID(ulEntry);
	m_PlaylistArray.RemoveAt(ulRealIndex);
	m_PlaylistArray.InsertBefore(ulRealIndex, PE);
	return true;
}