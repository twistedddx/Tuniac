#pragma once
#include "iplaylist.h"

#define SORTED_ASCENDING	0
#define SORTED_DESCENDING	1
#define SORTED_UNSORTED		2

class CBasePlaylist :
	public IPlaylistEX
{

protected:
	int					Sort_CompareItems(IPlaylistEntry * pItem1, IPlaylistEntry * pItem2, unsigned long ulSortBy); // returns comparison between feild of playlist entries
	void				Sort_Algorithm(unsigned long begin, unsigned long end, unsigned long ulSortBy, bool reverse);

protected:

	typedef struct
	{
		bool				bFiltered;
		IPlaylistEntry *	pEntry;
	} PlaylistEntry;

	Array<PlaylistEntry, 100>				m_PlaylistArray;

	Array<unsigned long, 100>				m_RandomIndexArray;
	Array<unsigned long, 100>				m_NormalIndexArray;

	int										m_ActiveRealIndex;

	unsigned long							m_SortType;
	unsigned long							m_LastSortBy;

	TCHAR									m_szPlaylistName[128];

	TCHAR									m_szTextFilter[128];
	unsigned long							m_ulTextFilterField;
	bool									m_bTextFilterReversed;

public:
	CBasePlaylist(void);
	~CBasePlaylist(void);

	bool				ApplyFilter(void);

	unsigned long		RealIndexToNormalFilteredIndex(unsigned long ulRealIndex);				// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	unsigned long		NormalFilteredIndexToRealIndex(unsigned long ulNormalFilteredIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX
	unsigned long		RealIndexToRandomFilteredIndex(unsigned long ulRealIndex);				// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	unsigned long		RandomFilteredIndexToRealIndex(unsigned long ulRandomFilteredIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	bool				Next(void);
	bool				CheckFilteredIndex(unsigned long ulFilteredIndex);

	unsigned long		GetNextFilteredIndex(unsigned long ulFilteredIndex, bool bFollowSelected, bool bFollowQueue);
	unsigned long		GetPlayOrder(unsigned long ulNormalFilteredIndex);

	IPlaylistEntry	*	GetActiveItem(void);

public:
	unsigned long		GetNumItems(void);

	bool				SetActiveFilteredIndex(unsigned long ulFilteredIndex);
	unsigned long		GetActiveFilteredIndex(void);

	bool				SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex);
	unsigned long		GetActiveNormalFilteredIndex(void);

	IPlaylistEntry *	GetItemAtFilteredIndex(unsigned long ulFilteredIndex);
	IPlaylistEntry *	GetItemAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex);
	unsigned long		GetFilteredIndexforItem(IPlaylistEntry * pEntry);

	bool				SetTextFilter(LPTSTR	szFilterString);
	LPTSTR				GetTextFilter(void);
	bool				SetTextFilterField(unsigned long ulFieldID);
	unsigned long		GetTextFilterField(void);
	bool				SetTextFilterReversed(bool bReverse);
	bool				GetTextFilterReversed(void);

	void				GetFieldFilteredList(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse);

	bool				Sort(unsigned long ulSortBy);

	bool				AddEntryArray(EntryArray & entryArray);

	bool				DeleteItemArray(IndexArray &	indexArray);
	bool				MoveItemArray(unsigned long ToIndex, IndexArray &	indexArray);

	bool				DeleteAllItemsWhereIDEquals(unsigned long ID);
	bool				UpdateIndex(unsigned long ulRealIndex);
};