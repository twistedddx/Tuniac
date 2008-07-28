#pragma once

#include "Stdstring.h"
#include <vector>
enum
{
	PLAYBACK_TIME_ACCURACY_GUESSED = 0,
	PLAYBACK_TIME_ACCURACY_PLAYED = 1,
};

class MediaItem
{
public:
	unsigned long long		ulItemID;

	long long				dateAdded;
	long long				fileOnline;

	CStdString					filename;
	long long				ullFilesize;
	long long				ullFileModifiedTime;

	CStdString					title;	
	CStdString					artist;
	CStdString					disktitle;
	CStdString					album;
	CStdString					albumartist;
	CStdString					composer;
	long long				ulYear;
	CStdString					genre;
	CStdString					comment;


	long long				ulTrack;
	long long				ulMaxTrack;

	long long				ulDisk;
	long long				ulMaxDisk;

	long long				ulRating;
	long long				ulBPM;

	long long				ulPlayTimeMS;
	long long				ulPlaybackTimeAccuracy;

	long long				ulSampleRate;
	long long				ulChannelCount;
	long long				ulBitRate;

	long long				ullFirstPlayed;
	long long				ullLastPlayed;
	long long				ullPlayCount;

	float					fReplayGainTrack;
	float					fReplayPeakTrack;
	float					fReplayGainAlbum;
	float					fReplayPeakAlbum;

	long long				ullEncoderDelay;
	long long				ullEncoderPadding;

};

typedef std::vector<MediaItem>			MediaItemList;