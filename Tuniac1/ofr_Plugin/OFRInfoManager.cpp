#include "stdafx.h"
#include "ofrinfomanager.h"

// actual class definition here

COFRInfoManager::COFRInfoManager(void)
{
}

COFRInfoManager::~COFRInfoManager(void)
{
}

void			COFRInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	COFRInfoManager::GetNumExtensions(void)
{
	return 2;
}

LPTSTR			COFRInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".ofr"),
		TEXT(".ofs")
	};

	return exts[ulExtentionNum];
}

bool			COFRInfoManager::CanHandle(LPTSTR szSource)
{
	for(unsigned int x=0; x<GetNumExtensions(); x++)
	{
		if(!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}

bool			COFRInfoManager::GetInfo(LibraryEntry * libEnt)
{
    OptimFROG_Info iInfo;
	OptimFROG_Tags iTags;

	char tempname[MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, libEnt->szURL, -1, tempname, MAX_PATH, 0, 0);

	OptimFROG_infoFile(tempname, &iInfo, &iTags);

	for(unsigned int i=0; i<iTags.keyCount; i++)
	{
		if(strcmp(iTags.keys[i], "Album") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[i], strlen(iTags.values[i]), libEnt->szAlbum, 128);
			continue;
		}
		if(strcmp(iTags.keys[i], "Title") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[i], strlen(iTags.values[i]), libEnt->szTitle, 128);
			continue;
		}
		if(strcmp(iTags.keys[i], "Artist") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[i], strlen(iTags.values[i]), libEnt->szArtist, 128);
			continue;
		}
		if(strcmp(iTags.keys[i], "Genre") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[i], strlen(iTags.values[i]), libEnt->szGenre, 128);
			continue;
		}
		if(strcmp(iTags.keys[i], "Comment") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[i], strlen(iTags.values[i]), libEnt->szComment, 128);
			continue;
		}
		if(strcmp(iTags.keys[i], "Year") == 0)
		{
			libEnt->iYear = atoi(iTags.values[i]);
			continue;
		}
		if(strcmp(iTags.keys[i], "Track") == 0)
		{
			libEnt->dwTrack[0] = atoi(iTags.values[i]);
			continue;
		}
		if(strcmp(iTags.keys[i], "replaygain_track_gain") == 0)
		{
			libEnt->fReplayGain_Track_Gain = atof(iTags.values[i]);
			continue;
		}
		if(strcmp(iTags.keys[i], "replaygain_track_peak") == 0)
		{
			libEnt->fReplayGain_Track_Peak = atof(iTags.values[i]);
			continue;
		}
		if(strcmp(iTags.keys[i], "replaygain_album_gain") == 0)
		{
			libEnt->fReplayGain_Album_Gain = atof(iTags.values[i]);
			continue;
		}
		if(strcmp(iTags.keys[i], "replaygain_album_peak") == 0)
		{
			libEnt->fReplayGain_Album_Peak = atof(iTags.values[i]);
			continue;
		}
	}

	libEnt->iBitRate			= (unsigned long)(iInfo.bitrate * 1000.0);
	libEnt->iChannels			= iInfo.channels;
	libEnt->iSampleRate			= iInfo.bitspersample;
	libEnt->iPlaybackTime		= iInfo.length_ms;

	return true;
}

bool			COFRInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	COFRInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			COFRInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			COFRInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
