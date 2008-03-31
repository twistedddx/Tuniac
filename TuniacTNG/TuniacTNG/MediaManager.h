#pragma once

#include <IInfoHandler.h>
#include <Singleton.h>

typedef struct _MEDIAITEM_
{
	unsigned __int64		ulItemID;

	SYSTEMTIME				dateAdded;
	unsigned long			fileOnline;

	String					filename;
	unsigned long			ulFilesize;

	String					title;	
	String					artist;
	String					album;
	String					albumartist;
	String					composer;
	String					genre;
	String					comment;

	unsigned long			ulTrack;
	unsigned long			ulMaxTrack;

	unsigned long			ulDisk;
	unsigned long			ulMaxDisk;

	unsigned long			ulRating;
	unsigned __int64		ulBPM;

	unsigned long			ulPlayTimeMS;

	unsigned long			ulSampleRate;
	unsigned long			ulChannelCount;
	unsigned long			ulBitRate;
} MediaItem;

class CMediaManager : public CSingleton<CMediaManager>
{
protected:
	typedef struct
	{
		IInfoHandler *	pInfoHandler;
		HINSTANCE		hDLL;
	} InfoHandler;

	std::vector<InfoHandler>		m_vInfoHandlers;

	bool PopulateMediaItemFromAccessor(MediaItem & pItem, IInfoAccessor * pAccessor);

public:
	CMediaManager(void);
	~CMediaManager(void);

public:

	bool Initialize(void);
	bool Shutdown(void);

	bool GetMediaDBLocation(String & strPath);

	unsigned __int64 GetNumEntries(void);
	bool AddFile(String filename);
};
