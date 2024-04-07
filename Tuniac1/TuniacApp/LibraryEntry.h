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

#define LENGTH_STREAM		0xfffffffe
#define LENGTH_UNKNOWN		0xffffffff

#define BITDEPTH_UNDEFINABLE	0xffffffff

#define BITRATE_UNKNOWN		0xffffffff

typedef struct
{
public:
	unsigned long		ulKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		ulFilesize;

	unsigned long		ulAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		ulPlayCount;
	unsigned long		ulRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	TCHAR				szAlbumArtist[128];
	TCHAR				szComposer[128];
	unsigned long		ulYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

	//extra info
	unsigned long		ulPlaybackTime;
	unsigned long		ulBitRate;
	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	unsigned long		ulBitsPerSample;

	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;

	unsigned long		ulBPM;

	TCHAR				szFileType[16];
} LibraryEntry;

//Past versions

typedef struct
{
public:
	unsigned long		ulKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		ulFilesize;

	unsigned long		ulAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		ulPlayCount;
	unsigned long		ulRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	TCHAR				szAlbumArtist[128];
	TCHAR				szComposer[128];
	unsigned long		ulYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

										//extra info
	unsigned long		ulPlaybackTime;
	unsigned long		ulBitRate;
	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;

	unsigned long		ulBPM;

	TCHAR				szFileType[16];
} LibraryEntry09;

typedef struct
{
public:
	unsigned long		ulKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		ulFilesize;

	unsigned long		ulAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		ulPlayCount;
	unsigned long		ulRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	TCHAR				szAlbumArtist[128];
	TCHAR				szComposer[128];
	unsigned long		ulYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

	//extra info
	unsigned long		ulPlaybackTime;
	unsigned long		ulBitRate;
	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;

	unsigned long		ulBPM;
} LibraryEntry08;

typedef struct
{
public:
	unsigned long		ulKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		ulFilesize;

	unsigned long		ulAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		ulPlayCount;
	unsigned long		ulRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	TCHAR				szAlbumArtist[128];
	unsigned long		ulYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

	//extra info
	unsigned long		ulPlaybackTime;
	unsigned long		ulBitRate;
	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;

	unsigned long		ulBPM;
} LibraryEntry07;

typedef struct
{
public:
	unsigned long		ulKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		ulFilesize;

	unsigned long		ulAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	unsigned long		ulPlayCount;
	unsigned long		ulRating;

	// filled in by the relevent media type handler
	// standard ID3 tag stuff
	TCHAR				szTitle[128];
	TCHAR				szArtist[128];
	TCHAR				szAlbum[128];
	TCHAR				szComment[128];
	TCHAR				szGenre[128];
	unsigned long		ulYear;

	unsigned short		dwTrack[2];		// index 0 == track : index 1 == max track (0 if unavailable)
	unsigned short		dwDisc[2];		// index 0 == disk  : index 1 == max disc  (0 is unavailable)

	//extra info
	unsigned long		ulPlaybackTime;
	unsigned long		ulBitRate;
	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;

	unsigned long		ulBPM;
} LibraryEntry06;

typedef struct
{
public:
	unsigned long		dwKind;
	TCHAR				szURL[MAX_PATH];
	unsigned long		dwFilesize;

	unsigned long		dwAvailability;

	SYSTEMTIME			stDateAdded;
	SYSTEMTIME			stFileCreationDate;
	SYSTEMTIME			stLastPlayed;

	int					iPlayCount;
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
	float				fReplayGain_Track_Gain;
	float				fReplayGain_Track_Peak;
	float				fReplayGain_Album_Gain;
	float				fReplayGain_Album_Peak;
} LibraryEntry05;