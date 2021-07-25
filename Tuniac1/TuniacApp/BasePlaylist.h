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

#pragma once
#include <time.h>
#include "IPlaylist.h"

#define SORTED_ASCENDING	0
#define SORTED_DESCENDING	1
#define SORTED_UNSORTED		2

class CBasePlaylist :
	public IPlaylistEX
{

protected:

	typedef struct
	{
		bool				bFiltered;
		IPlaylistEntry *	pIPE;
	} PlaylistEntry;

	Array<PlaylistEntry, 100>				m_PlaylistArray;

	Array<unsigned long, 100>				m_SavedPlaylistArray;

	Array<unsigned long, 100>				m_RandomIndexArray;
	Array<unsigned long, 100>				m_NormalIndexArray;

	unsigned long							m_ActiveRealIndex;

	unsigned long							m_SortType;
	unsigned long							m_LastSortBy;


	unsigned long							m_ulPlaylistID;
	TCHAR									m_szPlaylistName[128];

	TCHAR									m_szTextFilter[128];
	unsigned long							m_ulTextFilterField;
	bool									m_bTextFilterReversed;


protected:
	int					Sort_CompareItems(IPlaylistEntry * pItem1, IPlaylistEntry * pItem2, unsigned long ulSortBy); // returns comparison between feild of playlist entries
	void				Sort_Algorithm(unsigned long head, unsigned long tail, PlaylistEntry * scratch, unsigned long ulSortBy, bool reverse);
	void				Sort_Merge(unsigned long head, unsigned long tail, PlaylistEntry * scratch, unsigned long ulSortBy, bool reverse);


public:
	CBasePlaylist(void);
	~CBasePlaylist(void);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	void				SetPlaylistID(unsigned long ulPlaylistID);
	unsigned long		GetPlaylistID(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	unsigned long		Previous(void);
	unsigned long		Next(void);

	bool				CheckFilteredIndex(unsigned long ulFilteredIndex);

	unsigned long		GetActiveFilteredIndex(void);
	bool				SetActiveFilteredIndex(unsigned long ulFilteredIndex);
	unsigned long		GetActiveNormalFilteredIndex(void);
	bool				SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex);

	unsigned long		GetNextFilteredIndexForFilteredIndex(unsigned long ulFilteredIndex);

	IPlaylistEntry	*	GetActiveEntry(void);

	IPlaylistEntry *	GetEntryAtFilteredIndex(unsigned long ulFilteredIndex);
	IPlaylistEntry *	GetEntryAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex);
	
	unsigned long		GetFilteredIndexforEntry(IPlaylistEntry * pIPE);

	unsigned long		GetNormalFilteredIndexforEntry(IPlaylistEntry * pIPE);

	unsigned long		GetNumItems(void);

public:

	unsigned long		GetActiveEntryID(void);
	unsigned long		GetEntryIDAtFilteredIndex(unsigned long ulFilteredIndex);
	unsigned long		GetEntryIDAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex);
	unsigned long		GetFilteredIndexforEntryID(unsigned long ulEntryID);
	unsigned long		GetNormalFilteredIndexforEntryID(unsigned long ulEntryID);

	bool				SetActiveRealIndex(unsigned long ulRealIndex);

	IPlaylistEntry *	GetEntryByEntryID(unsigned long ulEntryID);

	unsigned long		RealIndexToNormalFilteredIndex(unsigned long ulRealIndex);				// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	unsigned long		NormalFilteredIndexToRealIndex(unsigned long ulNormalFilteredIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX
	unsigned long		RealIndexToRandomFilteredIndex(unsigned long ulRealIndex);				// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	unsigned long		RandomFilteredIndexToRealIndex(unsigned long ulRandomFilteredIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX

	bool				ApplyFilter(void);
	void				RebuildPlaylistArrays(void);

	unsigned long		GetPlayOrder(unsigned long ulNormalFilteredIndex);

	bool				SetTextFilter(LPTSTR	szFilterString);
	LPTSTR				GetTextFilter(void);
	bool				SetTextFilterField(unsigned long ulFieldID);
	unsigned long		GetTextFilterField(void);
	bool				SetTextFilterReversed(bool bReverse);
	bool				GetTextFilterReversed(void);

	unsigned long		GetRealIndexforEntry(IPlaylistEntry * pIPE);
	unsigned long		GetRealIndexforEntryID(unsigned long ulEntryID);

	void				GetFieldFilteredItemArray(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse);

	bool				Sort(unsigned long ulSortBy);

	bool				AddEntryArray(EntryArray & entryArray, bool bApplyFilter = true);

	bool				DeleteRealIndexArray(IndexArray &	indexArray);
	bool				DeleteNormalFilteredIndexArray(IndexArray &	indexArray);
	bool				MoveNormalFilteredIndexArray(unsigned long ToIndex, IndexArray &	indexArray);

	bool				DeleteAllItemsWhereEntryIDEquals(unsigned long ulEntryID);
	bool				UpdateRealIndex(unsigned long ulRealIndex);
	bool				DeleteRealIndex(unsigned long ulRealIndex);

	void				SaveOrder(void);
	void				RestoreOrder(void);
	bool				HasSavedOrder(void);

};