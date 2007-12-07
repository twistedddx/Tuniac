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

	int										m_ActiveIndex;

	unsigned long							m_SortType;
	unsigned long							m_LastSortBy;

	TCHAR									m_szPlaylistName[128];

	TCHAR									m_szTextFilter[128];
	unsigned long							m_ulTextFilterField;
	bool									m_bTextFilterReversed;

	bool									CheckNext(int iBaseIndex);

public:
	CBasePlaylist(void);
	~CBasePlaylist(void);

	bool				ApplyFilter(void);

	int					NormalRealIndexToFilteredIndex(int iIndex);	// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	int					NormalFilteredIndexToRealIndex(int iIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX

	int					RandomRealIndexToFilteredIndex(int iIndex);	// returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	int					RandomFilteredIndexToRealIndex(int iIndex);	// returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX


public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	bool				Next(void);
	bool				CheckNext(void);
	int					GetNextIndex(int iBaseIndex, bool * pbWrapped = NULL);

	IPlaylistEntry	*	GetActiveItem(void);
	unsigned long		GetItemPlayOrder(unsigned long ulIndex);

public:
	unsigned long		GetNumItems(void);

	bool				SetActiveIndex(int iIndex);
	int					GetActiveIndex(void);

	IPlaylistEntry *	GetItemAtIndex(int iIndex);

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

};