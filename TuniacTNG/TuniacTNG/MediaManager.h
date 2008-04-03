#pragma once

#include <IInfoHandler.h>
#include <Singleton.h>
#include <sqlite3x.hpp>

typedef struct _MEDIAITEM_
{
	unsigned __int64		ulItemID;

	SYSTEMTIME				dateAdded;
	__int64					fileOnline;

	String					filename;
	__int64					ulFilesize;
	SYSTEMTIME				fileModifiedTime;

	String					title;	
	String					artist;
	String					album;
	String					albumartist;
	String					composer;
	String					genre;
	String					comment;

	__int64					ulYear;

	__int64					ulTrack;
	__int64					ulMaxTrack;

	__int64					ulDisk;
	__int64					ulMaxDisk;

	__int64					ulRating;
	__int64					ulBPM;

	__int64					ulPlayTimeMS;

	__int64					ulSampleRate;
	__int64					ulChannelCount;
	__int64					ulBitRate;
} MediaItem;

typedef std::vector<MediaItem>			MediaItemList;

class CMediaManager : public CSingleton<CMediaManager>
{
protected:
	typedef struct
	{
		IInfoHandler *	pInfoHandler;
		HINSTANCE		hDLL;
	} InfoHandler;

	std::vector<InfoHandler>		m_vInfoHandlers;

	bool PopulateMediaItemFromAccessor(String filename, MediaItem & pItem);
	bool InsertItemToMediaLibraryUsingConnection(sqlite3x::sqlite3_connection & con, MediaItem & pItem);
	bool IsFileInLibrary(sqlite3x::sqlite3_connection & con, String filename);

public:
	CMediaManager(void);
	~CMediaManager(void);

public:

	bool Initialize(void);
	bool Shutdown(void);

	bool GetMediaDBLocation(String & strPath);

	unsigned __int64 GetNumEntries(void);

	bool GetAlbums(StringArray & albumList);
	bool GetRange(unsigned long ulStart, unsigned long ulCount, MediaItemList & itemList);

	bool AddFile(String filename);
	bool AddFileArray(StringArray filenameArray);
};
