#pragma once

#define INVALID_ENTRY_ID	0xffffffff

#define PLAYTIME_CONTINUOUS	(-1)

#define ENTRY_KIND_FILE					0
#define ENTRY_KIND_URL					1

#define AVAILABLILITY_AVAILABLE			0
#define AVAILABLILITY_UNAVAILABLE		1
#define AVAILABLILITY_UNKNOWN			2

typedef struct
{
public:
	unsigned long		dwKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		dwFilesize;

	unsigned long		dwAvailablility;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		dwPlayCount;
	unsigned long		dwRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	int					iYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

	//extra info
	int					iPlaybackTime;

	int					iBitRate;
	int					iSampleRate;
	int					iChannels;
} LibraryEntry;
