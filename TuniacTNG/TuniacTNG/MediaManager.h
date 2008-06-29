#pragma once

#include <IInfoHandler.h>
#include <Singleton.h>
#include <sqlite3x.hpp>

enum
{
	PLAYBACK_TIME_ACCURACY_GUESSED = 0,
	PLAYBACK_TIME_ACCURACY_PLAYED = 1,
};

typedef struct _MEDIAITEM_
{
	unsigned __int64		ulItemID;

	__int64					dateAdded;
	__int64					fileOnline;

	String					filename;
	__int64					ullFilesize;
	__int64					ullFileModifiedTime;

	String					title;	
	String					artist;
	String					disktitle;
	String					album;
	String					albumartist;
	String					composer;
	__int64					ulYear;
	String					genre;
	String					comment;


	__int64					ulTrack;
	__int64					ulMaxTrack;

	__int64					ulDisk;
	__int64					ulMaxDisk;

	__int64					ulRating;
	__int64					ulBPM;

	__int64					ulPlayTimeMS;
	__int64					ulPlaybackTimeAccuracy;

	__int64					ulSampleRate;
	__int64					ulChannelCount;
	__int64					ulBitRate;

	__int64					ullFirstPlayed;
	__int64					ullLastPlayed;
	__int64					ullPlayCount;

	float					fReplayGainTrack;
	float					fReplayPeakTrack;
	float					fReplayGainAlbum;
	float					fReplayPeakAlbum;

	__int64					ullEncoderDelay;
	__int64					ullEncoderPadding;


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

	bool ReaderMediaItem(sqlite3x::sqlite3_reader & reader, MediaItem & item);


public:
	CMediaManager(void);
	~CMediaManager(void);

public:

	bool Initialize(void);
	bool Shutdown(void);

	bool GetMediaDBLocation(String & strPath);

	unsigned __int64 GetNumEntries(void);

	bool GetAlbums(StringArray & albumList);
	bool GetArtists(StringArray & artistList);
	bool GetRange(unsigned long ulStart, unsigned long ulCount, MediaItemList & itemList);

	bool AddFile(String filename);
	bool AddFileArray(StringArray filenameArray);

	bool DeleteByID(__int64 ullID);
	bool DeleteByFilename(CStdString filename);
};
