#pragma once

#include "array.h"

#define FIELD_URL							0
#define FIELD_FILENAME						1
#define FIELD_ARTIST						2
#define FIELD_ALBUM							3
#define FIELD_TITLE							4
#define FIELD_TRACKNUM						5
#define FIELD_GENRE							6
#define FIELD_YEAR							7
#define FIELD_PLAYBACKTIME					8
#define FIELD_KIND							9
#define FIELD_FILESIZE						10
#define FIELD_DATEADDED						11
#define FIELD_DATEFILECREATION				12
#define FIELD_DATELASTPLAYED				13
#define FIELD_PLAYCOUNT						14
#define FIELD_RATING						15
#define FIELD_COMMENT						16
#define FIELD_BITRATE						17
#define FIELD_SAMPLERATE					18
#define FIELD_NUMCHANNELS					19
#define FIELD_FILEEXTENSION					20
#define FIELD_PLAYORDER						21
//#define FIELD_STATIONNAME					22


#define FIELD_MAXFIELD						22


#define PLAYLIST_TYPE_UNKNOWN				0
#define PLAYLIST_TYPE_MEDIALIBRARY			1
#define PLAYLIST_TYPE_STANDARDPLAYLIST		2
#define PLAYLIST_TYPE_SMARTPLAYLIST			3
#define PLAYLIST_TYPE_CD					4
//#define PLAYLIST_TYPE_RADIO					5
#define PLAYLIST_TYPE_USER					6

#define INVALID_PLAYLIST_INDEX			0xffffffff

#define PLAYLIST_FLAGS_CANRENAME		0x00000001
#define PLAYLIST_FLAGS_EXTENDED			0x00000002

#define PLAYLISTEX_FLAGS_CANFILTER		0x00010000
#define PLAYLISTEX_FLAGS_CANSORT		0x00020000
#define PLAYLISTEX_FLAGS_CANADD			0x00040000
#define PLAYLISTEX_FLAGS_CANDELETE		0x00080000
#define PLAYLISTEX_FLAGS_CANMOVE		0x00100000

class IPlaylistEntry
{
public:	
	virtual unsigned long	GetEntryID(void)							= 0;

	virtual void *	GetField(unsigned long ulFieldID)					= 0;
	virtual bool	SetField(unsigned long ulFieldID, void * pNewData)	= 0;

	virtual bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars)		= 0;
};

typedef Array<int, 100>						IndexArray;
typedef Array<IPlaylistEntry *, 100>		EntryArray;

class IPlaylist
{
public:
	virtual unsigned long		GetFlags(void)								= 0;
	virtual unsigned long		GetPlaylistType(void)						= 0;

	virtual bool				SetPlaylistName(LPTSTR szPlaylistName)		= 0;
	virtual LPTSTR				GetPlaylistName(void)						= 0;

	virtual bool				Previous(void)								= 0;
	virtual bool				Next(void)									= 0;


	virtual IPlaylistEntry	*	GetActiveItem(void)							= 0;
};

class IPlaylistEX : public IPlaylist
{
public:
	virtual unsigned long		GetNumItems(void)													= 0;

	virtual bool				ApplyFilter(void)													= 0;
	virtual void				RebuildPlaylistArrays(void)													= 0;

	virtual bool				SetActiveFilteredIndex(unsigned long ulFilteredIndex)				= 0;
	virtual unsigned long		GetActiveFilteredIndex(void)										= 0;
	virtual bool				SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex)	= 0;
	virtual unsigned long		GetActiveNormalFilteredIndex(void)									= 0;

	virtual IPlaylistEntry *	GetItemAtFilteredIndex(unsigned long ulFilteredIndex)				= 0;
	virtual IPlaylistEntry *	GetItemAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex)			= 0;
	virtual unsigned long		GetFilteredIndexforItem(IPlaylistEntry	* pEntry)					= 0;

	virtual unsigned long		GetNextFilteredIndex(unsigned long ulFilteredIndex, bool bFollowSelected, bool bFollowQueue)			= 0;
	virtual unsigned long		GetPlayOrder(unsigned long ulNormalFilteredIndex)				= 0;

	virtual bool				SetTextFilter(LPTSTR	szFilterString)			= 0;
	virtual LPTSTR				GetTextFilter(void)								= 0;
	virtual bool				SetTextFilterField(unsigned long ulFieldID)		= 0;
	virtual unsigned long		GetTextFilterField(void)						= 0;
	virtual bool				SetTextFilterReversed(bool bReverse)			= 0;
	virtual bool				GetTextFilterReversed(void)						= 0;

	virtual void				GetFieldFilteredList(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse) = 0;

	virtual bool				Sort(unsigned long ulSortBy)					= 0;

	virtual bool				AddEntryArray(EntryArray & entryArray)			= 0;

	virtual bool				DeleteItemArray(IndexArray &	indexArray)		= 0;
	virtual bool				MoveItemArray(unsigned long ToIndex, IndexArray &	indexArray) = 0;

	virtual bool				DeleteAllItemsWhereIDEquals(unsigned long ID)	= 0;
	virtual bool				UpdateIndex(unsigned long ulRealIndex)			= 0;
};



