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
	Copyright (C) 2003-2014 Brett Hoyle
*/

//every type of playlist is a base playlist. Playlist 0 is the LibraryPlaylist.cpp, low numbers playlists may be AudioCDPlaylist.cpp

#include "stdafx.h"
#include "baseplaylist.h"

CBasePlaylist::CBasePlaylist(void)
{
	m_ActiveRealIndex	= INVALID_PLAYLIST_INDEX;
	m_SortType		= SORTED_UNSORTED;
	m_LastSortBy	= -1;

	StringCchCopy(m_szPlaylistName, 128, TEXT(""));

	StringCchCopy(m_szTextFilter, 128, TEXT(""));
	m_ulTextFilterField = FIELD_MAXFIELD;
	m_bTextFilterReversed = false;
}

CBasePlaylist::~CBasePlaylist(void)
{
}

bool				CBasePlaylist::ApplyFilter(void)
{
	TCHAR szTemp[260];

	//if filter text exists
	if(wcsnlen_s(m_szTextFilter, 128))
	{
		//check items that match text
		for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
		{
			//if reversed, we flip each files filterness before the filter to make it the opposite result
			//meaning all files are filtered until they are unfiltered in the filter below
			//default false = include, true = exclude
			m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? false : true;

			//default filter(FIELD_MAXFIELD) is any title, artist, album and filename matches
			if(m_ulTextFilterField == FIELD_MAXFIELD)
			{

				for (unsigned int i = 0; i < tuniacApp.m_Preferences.GetUserSearchFieldNum(); i++)
				{
					unsigned long ulField = tuniacApp.m_Preferences.GetUserSearchFieldIDAtIndex(i);

					if (ulField == FIELD_ARTIST
						|| m_ulTextFilterField == FIELD_ALBUM
						|| m_ulTextFilterField == FIELD_ALBUMARTIST
						|| m_ulTextFilterField == FIELD_COMPOSER
						|| m_ulTextFilterField == FIELD_TITLE
						|| m_ulTextFilterField == FIELD_GENRE
						|| m_ulTextFilterField == FIELD_COMMENT
						|| m_ulTextFilterField == FIELD_URL
						|| m_ulTextFilterField == FIELD_FILENAME
						|| m_ulTextFilterField == FIELD_FILEEXTENSION)
					{
						if (StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(ulField), m_szTextFilter))
						{
							m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
							continue;
						}
					}
					else
					{
						m_PlaylistArray[x].pIPE->GetTextRepresentation(ulField, szTemp, 260);

						if (StrStrI(szTemp, m_szTextFilter))
						{
							m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
							continue;
						}

					}
				}

				/*
				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(FIELD_TITLE), m_szTextFilter))
				{
					//Toggles the filteredness if matched.
					//Watch if was "filtered true" already before as mentioned, making it now false again
					//set this entry as: false = include, true = exclude
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(FIELD_ARTIST), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(FIELD_ALBUM), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(FIELD_ALBUMARTIST), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}

				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(FIELD_URL), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}
				*/
			}

			//single field filter selected. Will include or exclude based solely on the single field
			else if(m_ulTextFilterField == FIELD_ARTIST
					|| m_ulTextFilterField == FIELD_ALBUM
					|| m_ulTextFilterField == FIELD_ALBUMARTIST
					|| m_ulTextFilterField == FIELD_COMPOSER
					|| m_ulTextFilterField == FIELD_TITLE
					|| m_ulTextFilterField == FIELD_GENRE
					|| m_ulTextFilterField == FIELD_COMMENT
					|| m_ulTextFilterField == FIELD_URL
					|| m_ulTextFilterField == FIELD_FILENAME
					|| m_ulTextFilterField == FIELD_FILEEXTENSION)
			{
				if(StrStrI((LPTSTR)m_PlaylistArray[x].pIPE->GetField(m_ulTextFilterField), m_szTextFilter))
				{
					m_PlaylistArray[x].bFiltered = m_bTextFilterReversed ? true : false;
					continue;
				}
			}
			else
			{
				m_PlaylistArray[x].pIPE->GetTextRepresentation(m_ulTextFilterField, szTemp, 260);

				if (StrStrI(szTemp, m_szTextFilter))
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

	if (tuniacApp.m_LogWindow)
	{
		if (tuniacApp.m_LogWindow->GetLogOn())
		{
			tuniacApp.m_LogWindow->LogMessage(TEXT("BasePlaylist"), TEXT("ApplyFilter"));
		}
	}

	RebuildPlaylistArrays();
	tuniacApp.RebuildFutureMenu();
	if (tuniacApp.m_SourceSelectorWindow)
		tuniacApp.m_SourceSelectorWindow->UpdateView();

	return true;
}

void				CBasePlaylist::RebuildPlaylistArrays(void)
{
	//remake our filtered lists(normal and shuffle) based on new filter
	g_Rand.RandomInit(time(NULL));

	m_NormalIndexArray.RemoveAll();
	m_RandomIndexArray.RemoveAll();

	unsigned long ulCount = 0;
	unsigned long ulCurrentIndex = 0;

	bool bValid = false;
	int iStartPos = 0;

	//for each item, if not set as filtered, add
	for(unsigned long y=0; y<m_PlaylistArray.GetCount(); y++)
	{
		if(m_PlaylistArray[y].bFiltered == false)
		{
			m_NormalIndexArray.AddTail(y);

			//create shuffle list
			if(y == m_ActiveRealIndex)
			{
				if(ulCount)
				{
					bValid = true;
					ulCurrentIndex = y;
				}
				else
				{
					m_RandomIndexArray.AddTail(y);
					iStartPos = 1;
					ulCount = 1;
				}
				continue;
			}
			if(ulCount)
			{
				unsigned long ulRandNum = g_Rand.IRandom(iStartPos, ulCount);
				if(ulRandNum == ulCount)
					m_RandomIndexArray.AddTail(y);
				else
					m_RandomIndexArray.InsertBefore(ulRandNum, y);
			}
			else
			{
				m_RandomIndexArray.AddTail(y);
			}
			ulCount++;
		}
	}
	if(bValid)
		m_RandomIndexArray.AddHead(ulCurrentIndex);
}


/////////////////////////////////////////////////////////////////////////////////
//
//			Playlist transposing code
//realindex = m_PlaylistArray
//normalfilteredindex = m_NormalIndexArray (these are valid playback files with shuffle off)
//randomfilteredindex = m_RandomIndexArray (these are valid playback files with shuffle on)

//m_MediaLibrary = FULL ML array. Contains all file info
//based on m_MediaLibrary
//m_PlaylistArray = full current(view) playlist array. Contains entries from m_MediaLibrary
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
	if(!CheckFilteredIndex(ulNormalFilteredIndex))
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
	if(!CheckFilteredIndex(ulRandomFilteredIndex))
		return INVALID_PLAYLIST_INDEX;

	return m_RandomIndexArray[ulRandomFilteredIndex];
}



///////////////////////////////////////////////////////////////////////
//
//	General Crap

unsigned long		CBasePlaylist::GetFlags(void)
{
	if(tuniacApp.m_Preferences.GetCanPlaylistsSort())
		return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_EXTENDED | PLAYLISTEX_FLAGS_CANFILTER | PLAYLISTEX_FLAGS_CANADD | PLAYLISTEX_FLAGS_CANDELETE | PLAYLISTEX_FLAGS_CANMOVE | PLAYLISTEX_FLAGS_CANSORT;
	else
		return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_EXTENDED | PLAYLISTEX_FLAGS_CANFILTER | PLAYLISTEX_FLAGS_CANADD | PLAYLISTEX_FLAGS_CANDELETE | PLAYLISTEX_FLAGS_CANMOVE;
}

unsigned long		CBasePlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_UNKNOWN;
}

bool				CBasePlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	if(szPlaylistName)
	{
		StringCchCopy(m_szPlaylistName, 128, szPlaylistName);
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

//return the previous song index
unsigned long		CBasePlaylist::Previous(void)
{
	//there is no files, there is no next
	if(GetNumItems() == 0)
		return INVALID_PLAYLIST_INDEX;

	unsigned long ulActiveFilteredIndex = GetActiveFilteredIndex();

	if(ulActiveFilteredIndex == INVALID_PLAYLIST_INDEX)
	{
		//no active, start from the start
		if(tuniacApp.m_Preferences.GetSkipStreams() && tuniacApp.m_Preferences.GetShuffleState() && GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
		{
			for(unsigned long x=0; x<m_NormalIndexArray.GetCount(); x++)
			{
				IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
				if(pIPE)
				{
					if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
					return x;
				}
			}
		}
		else
			return 0;
	}

	if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatOne)
		return ulActiveFilteredIndex;

	if(tuniacApp.m_Preferences.GetSkipStreams() && tuniacApp.m_Preferences.GetShuffleState() && GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
	{
		for(unsigned long x=ulActiveFilteredIndex; x>0; x--)
		{
			IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x-1);
			if(pIPE)
			{
				if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
					return x-1;
			}
		}
		//if we didn't find anything and we are repeating start going through the list again
		if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
		{
			for(unsigned long x=m_NormalIndexArray.GetCount(); x>0; x--)
			{
				IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x-1);
				if(pIPE)
				{
					if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
						return x-1;
				}
			}
		}
	}
	else
	{
		if(ulActiveFilteredIndex == 0)
		{
			if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
				return m_NormalIndexArray.GetCount() - 1;
		}
		else
			//try to set active as -1 of current active
			return ulActiveFilteredIndex - 1;
	}

	return INVALID_PLAYLIST_INDEX;
}

//return the next song index
unsigned long		CBasePlaylist::Next(void)
{
	//there is no files, there is no next
	if(GetNumItems() == 0)
		return INVALID_PLAYLIST_INDEX;

	if(tuniacApp.m_PlaySelected.GetCount())
	{
		//playselected is always stored as a normal filtered index, so will need to switch for shuffle
		if(tuniacApp.m_Preferences.GetShuffleState())
			return RealIndexToRandomFilteredIndex(NormalFilteredIndexToRealIndex(tuniacApp.m_PlaySelected[0]));
		else
			return tuniacApp.m_PlaySelected[0];
	}

	if (tuniacApp.m_Queue.GetCount())
	{
		unsigned long ulFilteredIndex = GetFilteredIndexforEntryID(tuniacApp.m_Queue.GetEntryIDAtIndex(0));
		//check the queue is for a valid file
		if(CheckFilteredIndex(ulFilteredIndex))
			return ulFilteredIndex;
	}

	unsigned long ulActiveFilteredIndex = GetActiveFilteredIndex();
	if(ulActiveFilteredIndex == INVALID_PLAYLIST_INDEX)
	{
		//no active, start from the start
		if(tuniacApp.m_Preferences.GetSkipStreams() && tuniacApp.m_Preferences.GetShuffleState() && GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
		{
			for(unsigned long x=0; x<m_NormalIndexArray.GetCount(); x++)
			{
				IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
				if(pIPE)
				{
					if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
					return x;
				}
			}
		}
		else
		{
			return 0;
		}
	}

	if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatOne)
		return ulActiveFilteredIndex;

	if(tuniacApp.m_Preferences.GetSkipStreams() && tuniacApp.m_Preferences.GetShuffleState() && GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
	{
		for(unsigned long x=(ulActiveFilteredIndex + 1); x<m_NormalIndexArray.GetCount(); x++)
		{
			IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
			if(pIPE)
			{
				if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
					return x;
			}
		}
		//if we didn't find anything and we are repeating start going through the list again
		if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
		{
			for(unsigned long x=0; x<m_NormalIndexArray.GetCount(); x++)
			{
				IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
				if(pIPE)
				{
					if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
						return x;
				}
			}
		}
	}
	else
	{
		if(ulActiveFilteredIndex == m_NormalIndexArray.GetCount() - 1)
		{
			if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
				return 0;
		}
		else
			//try to set active as -1 of current active
			return ulActiveFilteredIndex + 1;
	}

	//this is the end of the line
	return INVALID_PLAYLIST_INDEX;
}

bool				CBasePlaylist::CheckFilteredIndex(unsigned long ulFilteredIndex)
{
	//invalid
	if(ulFilteredIndex == INVALID_PLAYLIST_INDEX)
		return false;

	//too low
	if(ulFilteredIndex < 0)
		return false;

	//too high, m_NormalIndexArray/m_RandomIndexArray should be the same length
	if(ulFilteredIndex >= m_NormalIndexArray.GetCount())
		return false;

	return true;
}

//will return the next song index for the index given. DOES NOT follow queues
unsigned long		CBasePlaylist::GetNextFilteredIndexForFilteredIndex(unsigned long ulFilteredIndex)
{
	//there is no files, there is no next
	if(GetNumItems() == 0)
		return INVALID_PLAYLIST_INDEX;

	//no valid start, so just start from the beginning again
	if(!CheckFilteredIndex(ulFilteredIndex))
		return 0;

	if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatOne)
		return ulFilteredIndex;


	if(tuniacApp.m_Preferences.GetSkipStreams() && tuniacApp.m_Preferences.GetShuffleState() && GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
	{
		for(unsigned long x=(ulFilteredIndex + 1); x<m_NormalIndexArray.GetCount(); x++)
		{
			IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
			if(pIPE)
			{
				if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
					return x;
			}
		}
		//if we didn't find anything and we are repeating start going through the list again
		if(tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
		{
			for(unsigned long x=0; x<m_NormalIndexArray.GetCount(); x++)
			{
				IPlaylistEntry * pIPE = GetEntryAtFilteredIndex(x);
				if(pIPE)
				{
					if(!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)))
						return x;
				}
			}
		}

	}
	else
	{
		//we are at the last song
		//m_NormalIndexArray/m_RandomIndexArray should be the same length
		if(ulFilteredIndex == (m_NormalIndexArray.GetCount() - 1) && tuniacApp.m_Preferences.GetRepeatMode() == RepeatAll)
			return 0;
	
		//try to set active as +1 of current active
		if(ulFilteredIndex < m_NormalIndexArray.GetCount() - 1)
			return ulFilteredIndex + 1;
	}
	
	//no more files
	return 0;
}

//////////////////////////////////////////////////////////

//get the active item(any active item should be in the current playlist?
IPlaylistEntry	*	CBasePlaylist::GetActiveEntry(void)
{
	if(m_ActiveRealIndex == INVALID_PLAYLIST_INDEX)
		return NULL;

	return m_PlaylistArray[m_ActiveRealIndex].pIPE;
}

//get the active entryid(any active entryid should be in the current playlist?
unsigned long		CBasePlaylist::GetActiveEntryID(void)
{
	if(m_ActiveRealIndex == INVALID_PLAYLIST_INDEX)
		return NULL;

	return m_PlaylistArray[m_ActiveRealIndex].pIPE->GetEntryID();
}

//play order is simply the order of m_NormalIndexArray/m_RandomIndexArray 
unsigned long		CBasePlaylist::GetPlayOrder(unsigned long ulNormalFilteredIndex)
{
	if(tuniacApp.m_Preferences.GetShuffleState())
		return RealIndexToRandomFilteredIndex(NormalFilteredIndexToRealIndex(ulNormalFilteredIndex));
	else
		return ulNormalFilteredIndex;
}


unsigned long		CBasePlaylist::GetNumItems(void)
{
	return m_NormalIndexArray.GetCount();
}

//set active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
bool				CBasePlaylist::SetActiveRealIndex(unsigned long ulRealIndex)
{
	m_ActiveRealIndex = ulRealIndex;

	return true;
}

//set active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
bool				CBasePlaylist::SetActiveFilteredIndex(unsigned long ulFilteredIndex)
{
	if(tuniacApp.m_Preferences.GetShuffleState())
		m_ActiveRealIndex = RandomFilteredIndexToRealIndex(ulFilteredIndex);
	else
		m_ActiveRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	return true;
}

//set active song, this is the index location for m_NormalIndexArray/m_RandomIndexArray 
bool				CBasePlaylist::SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex)
{
	//check that index is valid
	if(!CheckFilteredIndex(ulNormalFilteredIndex))
		return false;

	m_ActiveRealIndex = NormalFilteredIndexToRealIndex(ulNormalFilteredIndex);

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
IPlaylistEntry *	CBasePlaylist::GetEntryAtFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = INVALID_PLAYLIST_INDEX;
	if(tuniacApp.m_Preferences.GetShuffleState())
		ulRealIndex = RandomFilteredIndexToRealIndex(ulFilteredIndex);
	else
		ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);
	
	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pIPE;
	}

	return NULL;
}

//get the EntryID at filtered ulIndex for m_NormalIndexArray/m_RandomIndexArray
unsigned long	CBasePlaylist::GetEntryIDAtFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = INVALID_PLAYLIST_INDEX;
	if(tuniacApp.m_Preferences.GetShuffleState())
		ulRealIndex = RandomFilteredIndexToRealIndex(ulFilteredIndex);
	else
		ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);
	
	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pIPE->GetEntryID();
	}

	return INVALID_PLAYLIST_INDEX;
}

//get the item at filtered ulIndex for m_NormalIndexArray/m_RandomIndexArray
IPlaylistEntry *	CBasePlaylist::GetEntryAtNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pIPE;
	}

	return NULL;
}

//get the EntryID at filtered ulIndex for m_NormalIndexArray/m_RandomIndexArray
unsigned long	CBasePlaylist::GetEntryIDAtNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(ulFilteredIndex);

	if(ulRealIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_PlaylistArray[ulRealIndex].pIPE->GetEntryID();
	}

	return INVALID_PLAYLIST_INDEX;
}

IPlaylistEntry *	CBasePlaylist::GetEntryByEntryID(unsigned long ulEntryID)
{
	for(unsigned long x=0; x < m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pIPE->GetEntryID() == ulEntryID)
				return m_PlaylistArray[x].pIPE;
	}
	return NULL;
}

//gets real index then returns filtered index for item 
unsigned long		CBasePlaylist::GetFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{

	for(unsigned long x=0; x < m_NormalIndexArray.GetCount(); x++)
	{
		if(tuniacApp.m_Preferences.GetShuffleState())
		{
			if(m_PlaylistArray[m_RandomIndexArray[x]].pIPE == pIPE)
				return x;
		}
		else
		{
			if(m_PlaylistArray[m_NormalIndexArray[x]].pIPE == pIPE)
				return x;
		}
	}

	return INVALID_PLAYLIST_INDEX;
}

//gets real index then returns filtered index for EntryID 
unsigned long		CBasePlaylist::GetFilteredIndexforEntryID(unsigned long ulEntryID)
{

	for(unsigned long x=0; x < m_NormalIndexArray.GetCount(); x++)
	{
		if(tuniacApp.m_Preferences.GetShuffleState())
		{
			if(m_PlaylistArray[m_RandomIndexArray[x]].pIPE->GetEntryID() == ulEntryID)
				return x;
		}
		else
		{
			if(m_PlaylistArray[m_NormalIndexArray[x]].pIPE->GetEntryID() == ulEntryID)
				return x;
		}
	}

	return INVALID_PLAYLIST_INDEX;
}

//gets real index then returns filtered index for item 
unsigned long		CBasePlaylist::GetNormalFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{
	for(unsigned long x=0; x < m_NormalIndexArray.GetCount(); x++)
	{
		if(m_PlaylistArray[m_NormalIndexArray[x]].pIPE == pIPE)
		{
			return x;
		}
	}

	return INVALID_PLAYLIST_INDEX;
}

//gets real index then returns filtered index for EntryID 
unsigned long		CBasePlaylist::GetNormalFilteredIndexforEntryID(unsigned long ulEntryID)
{
	for(unsigned long x=0; x < m_NormalIndexArray.GetCount(); x++)
	{
		if(m_PlaylistArray[m_NormalIndexArray[x]].pIPE->GetEntryID() == ulEntryID)
		{
			return x;
		}
	}

	return INVALID_PLAYLIST_INDEX;
}

//gets real index for item 
unsigned long		CBasePlaylist::GetRealIndexforEntry(IPlaylistEntry	* pIPE)
{
	for(unsigned long x=0; x < m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pIPE == pIPE)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

//gets real index for EntryID 
unsigned long		CBasePlaylist::GetRealIndexforEntryID(unsigned long ulEntryID)
{
	for(unsigned long x=0; x < m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pIPE->GetEntryID() == ulEntryID)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

bool				CBasePlaylist::SetTextFilter(LPTSTR	szFilterString)
{
	StringCchCopy(m_szTextFilter, 128, szFilterString);
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
	return true;
}

unsigned long		CBasePlaylist::GetTextFilterField(void)
{
	return m_ulTextFilterField;
}

bool				CBasePlaylist::SetTextFilterReversed(bool bReverse)
{
	m_bTextFilterReversed = bReverse;
	return true;
}

bool				CBasePlaylist::GetTextFilterReversed(void)
{
	return m_bTextFilterReversed;
}

void			CBasePlaylist::GetFieldFilteredItemArray(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse)
{
	entryArray.RemoveAll();
	TCHAR szTemp[512];
	for(unsigned long x = 0; x < m_PlaylistArray.GetCount(); x++)
	{
		m_PlaylistArray[x].pIPE->GetTextRepresentation(ulFieldID, szTemp, 512);
		if (bReverse ? StrCmpI(szFilterString, szTemp) != 0 : StrCmpI(szFilterString, szTemp) == 0)
		{
			entryArray.AddTail(m_PlaylistArray[x].pIPE);
		}
	}
}

bool CBasePlaylist::Sort (unsigned long ulSortBy)
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
	{
		return true;
	}

	PlaylistEntry *scratch = new PlaylistEntry[m_PlaylistArray.GetCount()];

	if (scratch == NULL)
	{
	  return false;
	}
	unsigned long ulTempActive = GetActiveEntryID();
	Sort_Algorithm(0, m_PlaylistArray.GetCount() - 1, scratch, ulSortBy, reversesort);
	delete[] scratch;

	m_ActiveRealIndex = GetRealIndexforEntryID(ulTempActive);

	RebuildPlaylistArrays();
	tuniacApp.RebuildFutureMenu();
	tuniacApp.m_SourceSelectorWindow->UpdateView();

	return true;
}

void CBasePlaylist::Sort_Algorithm (unsigned long head, unsigned long tail, PlaylistEntry *scratch, unsigned long ulSortBy, bool reverse)
{
   // Merge Sort: O(nlogn) STABLE sort
   
   if (head != tail) // i.e. if we have more than 1 item
   {
	  unsigned long mid = (head+tail)/2;
	  Sort_Algorithm (head, mid, scratch, ulSortBy, reverse);
	  Sort_Algorithm (mid+1, tail, scratch, ulSortBy, reverse);
	  Sort_Merge (head, tail, scratch, ulSortBy, reverse);
   }
}

void CBasePlaylist::Sort_Merge (unsigned long head, unsigned long tail, PlaylistEntry *scratch, unsigned long ulSortBy, bool reverse)
{
   // because the two lists are always sequential we can create the boundary point by finding the middle
   unsigned long mid = (head+tail)/2;
   int order = Sort_CompareItems (m_PlaylistArray[mid].pIPE, m_PlaylistArray[mid+1].pIPE, ulSortBy);
   
   // only merge if not already in order
   // i.e. largest of left list is bigger than smallest of right list if normal order
   // i.e. largest of left list is smaller than smallest of right list if reversed
   if ((order == 1 && !reverse) || (order == -1 && reverse))
   {

		// if there are only two items, just swap them
		/*if (tail == (head+1))
		{
			// bitwise swap, doesn't need temp variable and is FAST
			m_PlaylistArray[head] ^= m_PlaylistArray[tail];
			m_PlaylistArray[tail] ^= m_PlaylistArray[head];
			m_PlaylistArray[head] ^= m_PlaylistArray[tail];
		}
		else
		{
*/
			// do normal merge
			unsigned long i = head;
			unsigned long j = mid+1;
			unsigned long k = 0;

			// merge until either list is empty or the scratch space is full
			while ((i <= mid || j <= tail) && k < m_PlaylistArray.GetCount())
			{
				// if both lists contain items
				if (i <= mid && j <= tail)
				{
				 order = Sort_CompareItems (m_PlaylistArray[i].pIPE, m_PlaylistArray[j].pIPE, ulSortBy);
				   // put the smallest of the two into the scratch list if not reverse
				   // IMPORTANT: if they are equal put the sequential first one into scratch list
				   // otherwise this sort will no longer be stable
				   if (order < 1 && !reverse)
					  scratch[k] = m_PlaylistArray[i++]; // hacky variable incrementation
				   else
					  scratch[k] = m_PlaylistArray[j++]; // hacky variable incrementation
				}
				// if only the left list contains items
				else if (i <= mid && j > tail)
				   // fill scratch list with left list
				   scratch[k] = m_PlaylistArray[i++];

				// if only the right list contains items
				else if (j <= tail && i > mid)
				   // fill scratch list with right list
				   scratch[k] = m_PlaylistArray[j++];

				k++;
			}
			// copy (ordered) scratch list back to playlist
			for (i = head; i <= tail; i++)
			{
				m_PlaylistArray[i] = scratch[i-head];
			}
		//}
	}
}

int CBasePlaylist::Sort_CompareItems (IPlaylistEntry * pItem1, IPlaylistEntry * pItem2, unsigned long ulSortBy)
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
					return -1;
			}
			break;

		case FIELD_REPLAYGAIN_TRACK_GAIN:
		case FIELD_REPLAYGAIN_TRACK_PEAK:
		case FIELD_REPLAYGAIN_ALBUM_GAIN:
		case FIELD_REPLAYGAIN_ALBUM_PEAK:
		case FIELD_BPM:
		case FIELD_PLAYBACKTIME:
		case FIELD_BITRATE:
		case FIELD_SAMPLERATE:
		case FIELD_NUMCHANNELS:
		case FIELD_RATING:
		case FIELD_FILESIZE:
		case FIELD_PLAYCOUNT:
			{
				if(pItem1->GetField(ulSortBy) > pItem2->GetField(ulSortBy))
					return 1;
				else if(pItem1->GetField(ulSortBy) < pItem2->GetField(ulSortBy))
					return -1;
			}
			break;
/*
		case FIELD_PLAYORDER:
			{
				if(GetPlayOrder(GetNormalFilteredIndexforEntry(pItem1)) > GetPlayOrder(GetNormalFilteredIndexforEntry(pItem2)))
					return 1;
				else if(GetPlayOrder(GetNormalFilteredIndexforEntry(pItem1)) < GetPlayOrder(GetNormalFilteredIndexforEntry(pItem2)))
					return -1;
			}
			break;
*/
		case FIELD_YEAR:
			{
				if(pItem1->GetField(FIELD_YEAR) > pItem2->GetField(FIELD_YEAR))
					return 1;
				else if(pItem1->GetField(FIELD_YEAR) < pItem2->GetField(FIELD_YEAR))
					return -1;
				return Sort_CompareItems(pItem1, pItem2, FIELD_ALBUM);
			}
			break;

		case FIELD_FILEEXTENSION:
		case FIELD_COMMENT:
		case FIELD_GENRE:
		case FIELD_TITLE:
		case FIELD_FILENAME:
		case FIELD_URL:
			{
				return(StrCmpI((LPTSTR)pItem1->GetField(ulSortBy), (LPTSTR)pItem2->GetField(ulSortBy)));
			}
			break;

		case FIELD_ALBUMARTIST:
		case FIELD_COMPOSER:
		case FIELD_ARTIST:
			{

				int iItem1Offset = 0, iItem2Offset = 0;
				if(tuniacApp.m_Preferences.GetSmartSortingEnabled())
				{
					if(StrNCmpI((LPTSTR)pItem1->GetField(ulSortBy), L"A ", 2) == 0)
						iItem1Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem1->GetField(ulSortBy), L"THE ", 4) == 0)
						iItem1Offset = 4;
					
					if(StrNCmpI((LPTSTR)pItem2->GetField(ulSortBy), L"A ", 2) == 0)
						iItem2Offset = 2;
					else if(StrNCmpI((LPTSTR)pItem2->GetField(ulSortBy), L"THE ", 4) == 0)
						iItem2Offset = 4;
				}

				int x = StrCmpI((LPTSTR)pItem1->GetField(ulSortBy) + iItem1Offset, (LPTSTR)pItem2->GetField(ulSortBy) + iItem2Offset);

				if(x > 0)
					return 1;
				else if (x < 0)
					return -1;

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
					return 1;
				else if (y < 0)
					return -1;
				return Sort_CompareItems(pItem1, pItem2, FIELD_TRACKNUM);
			}
			break;

		case FIELD_TRACKNUM:
		case FIELD_DISCNUM:
			{
				short * pTrack1 = (short *)pItem1->GetField(ulSortBy);
				short * pTrack2 = (short *)pItem2->GetField(ulSortBy);

				if(pTrack1[0] > pTrack2[0])
					return 1;
				else if (pTrack1[0] < pTrack2[0])
					return -1;
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
		PE.pIPE		= entryArray[x];
		m_PlaylistArray.AddTail(PE);
	}

	ApplyFilter();
	return true;
}

bool				CBasePlaylist::DeleteNormalFilteredIndexArray(IndexArray &	indexArray)
{
	//swap to real index
	for (unsigned long x = 0; x<indexArray.GetCount(); x++)
	{
		indexArray[x] = NormalFilteredIndexToRealIndex(indexArray[x]);
	}
	return DeleteRealIndexArray(indexArray);
}

bool				CBasePlaylist::DeleteRealIndexArray(IndexArray &	indexArray)
{
	//remove them all
	while (indexArray.GetCount())
	{
		DeleteRealIndex(indexArray[0]);

		//move indexes to compensate for now deleted files
		for (unsigned long i = 1; i<indexArray.GetCount(); i++)
		{
			if (indexArray[0] < indexArray[i])
				indexArray[i]--;
		}

		indexArray.RemoveAt(0);
	}

	return true;
}

bool				CBasePlaylist::MoveNormalFilteredIndexArray(unsigned long ToIndex, IndexArray &	indexArray)
{
	PlaylistEntry *scratch = new PlaylistEntry[m_PlaylistArray.GetCount()];

	unsigned long ulRealMoveIndex = NormalFilteredIndexToRealIndex(ToIndex);


	if(ToIndex == m_PlaylistArray.GetCount())
		ulRealMoveIndex = ToIndex;


	if(ulRealMoveIndex == INVALID_PLAYLIST_INDEX)
		return false;

	unsigned long ulRealMoveToIndex = ulRealMoveIndex;

	if(ToIndex != 0)
	{
		for(unsigned long x=0; x<indexArray.GetCount(); x++)
		{
			if(NormalFilteredIndexToRealIndex(indexArray[x]) < ulRealMoveIndex)
				ulRealMoveToIndex--;
		}
	}


	bool bActiveFound = false;
	unsigned long ulFound = 0;
	unsigned long ulOthers = 0;
	unsigned long ulRealIndex = NormalFilteredIndexToRealIndex(indexArray[0]);

	for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		if(ulRealIndex == x)
		{
			scratch[ulRealMoveToIndex + ulFound] = m_PlaylistArray[x];
			if(x == m_ActiveRealIndex && !bActiveFound)
			{
				bActiveFound = true;
				 m_ActiveRealIndex = ulRealMoveToIndex + ulFound;
			}
			 
			ulFound++;

			ulRealIndex = NormalFilteredIndexToRealIndex(indexArray[ulFound]);
		}
		else
		{
			if(ulOthers == ulRealMoveToIndex)
				ulOthers += indexArray.GetCount();

			scratch[ulOthers] = m_PlaylistArray[x];
			if(x == m_ActiveRealIndex && !bActiveFound)
			{
				bActiveFound = true;
				 m_ActiveRealIndex = ulOthers;
			}

			ulOthers++;

		}
	}

	for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		m_PlaylistArray[x] = scratch[x];
	}

	delete [] scratch;

	RebuildPlaylistArrays();
	tuniacApp.RebuildFutureMenu();
	tuniacApp.m_SourceSelectorWindow->UpdateView();

	return true;
}

bool				CBasePlaylist::DeleteAllItemsWhereEntryIDEquals(unsigned long ulEntryID)
{
	for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		if(m_PlaylistArray[x].pIPE->GetEntryID() == ulEntryID)
		{

			return DeleteRealIndex(x);
		}
	}

	return false;
}

bool				CBasePlaylist::UpdateRealIndex(unsigned long ulRealIndex)
{
	if(ulRealIndex < 0)
		return false;

	if(ulRealIndex >= m_PlaylistArray.GetCount())
		return false;
	
	PlaylistEntry PE;
	unsigned long ulEntryID = m_PlaylistArray[ulRealIndex].pIPE->GetEntryID();
	PE.pIPE = tuniacApp.m_MediaLibrary.GetEntryByEntryID(ulEntryID);

	TCHAR szMessage[MAX_PATH + 20];
	StringCchPrintf(szMessage, MAX_PATH + 20, TEXT("Updating %s"), (LPTSTR)m_PlaylistArray[ulRealIndex].pIPE->GetField(FIELD_URL));
	if (tuniacApp.m_LogWindow)
	{
		if (tuniacApp.m_LogWindow->GetLogOn())
		{
			tuniacApp.m_LogWindow->LogMessage(TEXT("BasePlaylist"), szMessage);
		}
	}

	m_PlaylistArray.RemoveAt(ulRealIndex);
	m_PlaylistArray.InsertBefore(ulRealIndex, PE);
	return true;
}

bool				CBasePlaylist::DeleteRealIndex(unsigned long ulRealIndex)
{
	//move active song
	if (m_ActiveRealIndex != INVALID_PLAYLIST_INDEX)
	{
		if (ulRealIndex < m_ActiveRealIndex)
		{
			m_ActiveRealIndex--;
		}
		else if (ulRealIndex == m_ActiveRealIndex)
		{
			if (tuniacApp.m_PlaylistManager.GetActivePlaylist() == this)
				CCoreAudio::Instance()->Reset();

			m_ActiveRealIndex = INVALID_PLAYLIST_INDEX;
		}
	}

	TCHAR szMessage[MAX_PATH + 20];
	StringCchPrintf(szMessage, MAX_PATH + 20, TEXT("Deleting %s"), (LPTSTR)m_PlaylistArray[ulRealIndex].pIPE->GetField(FIELD_URL));
	if (tuniacApp.m_LogWindow)
	{
		if (tuniacApp.m_LogWindow->GetLogOn())
		{
			tuniacApp.m_LogWindow->LogMessage(TEXT("BasePlaylist"), szMessage);
		}
	}

	m_PlaylistArray.RemoveAt(ulRealIndex);

	return true;
}

bool				CBasePlaylist::HasSavedOrder(void)
{
	if(m_SavedPlaylistArray.GetCount() > 0)
		return true;

	return false;
}

void				CBasePlaylist::SaveOrder(void)
{
	m_SavedPlaylistArray.RemoveAll();
	unsigned long id;
	for(unsigned long x=0; x<m_PlaylistArray.GetCount(); x++)
	{
		id = m_PlaylistArray[x].pIPE->GetEntryID();
		m_SavedPlaylistArray.AddTail(id);
	}
}

void				CBasePlaylist::RestoreOrder(void)
{
	if(m_SavedPlaylistArray.GetCount() > 0)
	{
		m_PlaylistArray.RemoveAll();

		PlaylistEntry PE;
		PE.bFiltered	= false;
		for(unsigned long x=0; x<m_SavedPlaylistArray.GetCount(); x++)
		{
			PE.pIPE = tuniacApp.m_MediaLibrary.GetEntryByEntryID(m_SavedPlaylistArray[x]);
			m_PlaylistArray.AddTail(PE);
		}
		ApplyFilter();
	}
}