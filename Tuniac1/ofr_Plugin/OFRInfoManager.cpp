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

	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, libEnt->szURL, -1, tempname, _MAX_PATH, 0, 0);

	OptimFROG_infoFile(tempname, &iInfo, &iTags);

	for(unsigned int x=0; x<iTags.keyCount; x++)
	{
		if(strcmp(iTags.keys[x], "Album") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[x], strlen(iTags.values[x]), libEnt->szAlbum, 128);
			continue;
		}
		if(strcmp(iTags.keys[x], "Title") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[x], strlen(iTags.values[x]), libEnt->szTitle, 128);
			continue;
		}
		if(strcmp(iTags.keys[x], "Artist") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[x], strlen(iTags.values[x]), libEnt->szArtist, 128);
			continue;
		}
		if(strcmp(iTags.keys[x], "Genre") == 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, iTags.values[x], strlen(iTags.values[x]), libEnt->szGenre, 128);
			continue;
		}
		if(strcmp(iTags.keys[x], "Year") == 0)
		{
			libEnt->iYear = atoi(iTags.values[x]);
			continue;
		}
		if(strcmp(iTags.keys[x], "Track") == 0)
		{
			libEnt->dwTrack[0] = atoi(iTags.values[x]);
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
