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
#define FIELD_FILETYPE						20
#define FIELD_PLAYORDER						21
#define FIELD_REPLAYGAIN_TRACK_GAIN			22
#define FIELD_REPLAYGAIN_TRACK_PEAK			23
#define FIELD_REPLAYGAIN_ALBUM_GAIN			24
#define FIELD_REPLAYGAIN_ALBUM_PEAK			25
#define FIELD_AVAILABILITY					26
#define FIELD_BPM							27
#define FIELD_ALBUMARTIST					28
#define FIELD_COMPOSER						29
#define FIELD_DISCNUM						30
//#define FIELD_STATIONNAME					31


#define FIELD_MAXFIELD						31


#define PLAYLIST_TYPE_UNKNOWN				0
#define PLAYLIST_TYPE_MEDIALIBRARY			1
#define PLAYLIST_TYPE_STANDARDPLAYLIST		2
#define PLAYLIST_TYPE_SMARTPLAYLIST			3
#define PLAYLIST_TYPE_CD					4
//#define PLAYLIST_TYPE_RADIO					5
//#define PLAYLIST_TYPE_USER					6
#define PLAYLIST_TYPE_COUNT					5

#define INVALID_PLAYLIST_INDEX			0xffffffff

#define PLAYLIST_FLAGS_CANRENAME		0x00000001
#define PLAYLIST_FLAGS_EXTENDED			0x00000002
#define PLAYLIST_FLAGS_DONTCROSSFADE	0x00000004

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
	virtual bool	SetField(unsigned long ulFieldID, unsigned long pNewData)	= 0;
	virtual bool	SetField(unsigned long ulFieldID, float pNewData)	= 0;
	virtual bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars)		= 0;
};

typedef Array<unsigned long, 100>		IndexArray;
typedef Array<IPlaylistEntry *, 100>	EntryArray;

class IPlaylist
{
public:
	virtual unsigned long		GetFlags(void)								= 0;
	virtual unsigned long		GetPlaylistType(void)						= 0;

	virtual void				SetPlaylistID(unsigned long szPlaylistID)	= 0;
	virtual unsigned long		GetPlaylistID(void)							= 0;

	virtual bool				SetPlaylistName(LPTSTR szPlaylistName)		= 0;
	virtual LPTSTR				GetPlaylistName(void)						= 0;

	virtual unsigned long		Previous(void)								= 0;
	virtual unsigned long		Next(void)									= 0;

	virtual bool				CheckFilteredIndex(unsigned long ulFilteredIndex) = 0;

	virtual unsigned long		GetActiveFilteredIndex(void)										= 0;
	virtual bool				SetActiveFilteredIndex(unsigned long ulFilteredIndex)				= 0;
	virtual unsigned long		GetActiveNormalFilteredIndex(void)									= 0;
	virtual bool				SetActiveNormalFilteredIndex(unsigned long ulNormalFilteredIndex)	= 0;

	virtual unsigned long		GetNextFilteredIndexForFilteredIndex(unsigned long ulFilteredIndex)	= 0;

	virtual IPlaylistEntry	*	GetActiveEntry(void)							= 0;

	virtual IPlaylistEntry *	GetEntryAtFilteredIndex(unsigned long ulFilteredIndex)				= 0;
	virtual IPlaylistEntry *	GetEntryAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex)	= 0;

	virtual unsigned long		GetFilteredIndexforEntry(IPlaylistEntry	* pIPE)						= 0;

	virtual unsigned long		GetNormalFilteredIndexforEntry(IPlaylistEntry * pIPE)				= 0;

	virtual unsigned long		GetNumItems(void)							= 0;
};

class IPlaylistEX : public IPlaylist
{
public:

	virtual unsigned long		GetActiveEntryID(void)												= 0;
	virtual unsigned long		GetEntryIDAtFilteredIndex(unsigned long ulFilteredIndex)			= 0;
	virtual unsigned long		GetEntryIDAtNormalFilteredIndex(unsigned long ulNormalFilteredIndex)= 0;
	virtual unsigned long		GetFilteredIndexforEntryID(unsigned long ulEntryID)					= 0;
	virtual unsigned long		GetNormalFilteredIndexforEntryID(unsigned long ulEntryID)			= 0;

	virtual bool				SetActiveRealIndex(unsigned long ulRealIndex)						= 0;

	virtual IPlaylistEntry *	GetEntryByEntryID(unsigned long ulFilteredIndex)					= 0;

	virtual unsigned long		RealIndexToNormalFilteredIndex(unsigned long ulRealIndex)			= 0; // returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	virtual unsigned long		NormalFilteredIndexToRealIndex(unsigned long ulNormalFilteredIndex)	= 0; // returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX
	virtual unsigned long		RealIndexToRandomFilteredIndex(unsigned long ulRealIndex)			= 0; // returns a index based on a real (as in the whole playlist) index, or INVALID_PLAYLIST_INDEX
	virtual unsigned long		RandomFilteredIndexToRealIndex(unsigned long ulRandomFilteredIndex)	= 0; // returns a valid real index based on a playlist index or INVALID_PLAYLIST_INDEX

	virtual bool				ApplyFilter(void)								= 0;
	virtual void				RebuildPlaylistArrays(void)						= 0;

	virtual unsigned long		GetPlayOrder(unsigned long ulNormalFilteredIndex)	= 0;

	virtual bool				SetTextFilter(LPTSTR	szFilterString)			= 0;
	virtual LPTSTR				GetTextFilter(void)								= 0;
	virtual bool				SetTextFilterField(unsigned long ulFieldID)		= 0;
	virtual unsigned long		GetTextFilterField(void)						= 0;
	virtual bool				SetTextFilterReversed(bool bReverse)			= 0;
	virtual bool				GetTextFilterReversed(void)						= 0;

	virtual unsigned long		GetRealIndexforEntry(IPlaylistEntry * pIPE)		= 0;
	virtual unsigned long		GetRealIndexforEntryID(unsigned long ulEntryID)	= 0;

	virtual void				GetFieldFilteredItemArray(EntryArray & entryArray, unsigned long ulFieldID, LPTSTR szFilterString, bool bReverse) = 0;

	virtual bool				Sort(unsigned long ulSortBy)					= 0;

	virtual bool				AddEntryArray(EntryArray & entryArray, bool bApplyFilter = true)			= 0;

	virtual bool				DeleteRealIndexArray(IndexArray &	indexArray) = 0;
	virtual bool				DeleteNormalFilteredIndexArray(IndexArray &	indexArray)							= 0;
	virtual bool				MoveNormalFilteredIndexArray(unsigned long ToIndex, IndexArray &	indexArray) = 0;

	virtual bool				DeleteAllItemsWhereEntryIDEquals(unsigned long ID)	= 0;
	virtual bool				UpdateRealIndex(unsigned long ulRealIndex)			= 0;
	virtual bool				DeleteRealIndex(unsigned long ulRealIndex)			= 0;

	virtual void				SaveOrder(void)									= 0;
	virtual void				RestoreOrder(void)								= 0;
	virtual bool				HasSavedOrder(void)								= 0;
};



