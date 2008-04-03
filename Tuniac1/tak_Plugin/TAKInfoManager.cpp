#include "stdafx.h"
#include <wchar.h>
#include "takinfomanager.h"
// actual class definition here

CTAKInfoManager::CTAKInfoManager(void)
{
}

CTAKInfoManager::~CTAKInfoManager(void)
{
}

void			CTAKInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CTAKInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CTAKInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".tak")
	};

	return exts[ulExtentionNum];
}

bool			CTAKInfoManager::CanHandle(LPTSTR szSource)
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

bool			CTAKInfoManager::GetInfo(LibraryEntry * libEnt)
{

	TtakSSDOptions				Options;
	TtakSeekableStreamDecoder	Decoder;
	TtakAPEv2Tag				TagInfo;
	Ttak_str_StreamInfo			StreamInfo;

	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, libEnt->szURL, -1, tempname, _MAX_PATH, 0, 0);

	Options.Cpu   = tak_Cpu_Any;

	Decoder = tak_SSD_Create_FromFile (tempname, &Options, NULL, NULL);
	if (Decoder == NULL)
		return false;
	if (tak_SSD_Valid (Decoder) != tak_True)
		return false;
	if (tak_SSD_GetStreamInfo (Decoder, &StreamInfo) != tak_res_Ok) 
		return false;

	TagInfo = tak_SSD_GetAPEv2Tag(Decoder);
	if(tak_APE_Valid(TagInfo))
	{
		char buffer[256];
		int idx = 0;
		int size =0;
		if(tak_APE_GetIndexOfKey(TagInfo, "Track", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			libEnt->dwTrack[0] = atoi(buffer);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Year", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			libEnt->iYear = atoi(buffer);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Artist", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			MultiByteToWideChar(CP_UTF8, 0, buffer, size, libEnt->szArtist, 128);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Title", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			MultiByteToWideChar(CP_UTF8, 0, buffer, size, libEnt->szTitle, 128);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Album", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			MultiByteToWideChar(CP_UTF8, 0, buffer, size, libEnt->szAlbum, 128);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Genre", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			MultiByteToWideChar(CP_UTF8, 0, buffer, size, libEnt->szGenre, 128);
		}
		if(tak_APE_GetIndexOfKey(TagInfo, "Comment", &idx) != tak_res_ape_NotAvail)
		{
			tak_APE_GetItemValue(TagInfo, idx, buffer, 256, &size);
			MultiByteToWideChar(CP_UTF8, 0, buffer, size, libEnt->szGenre, 128);
		}
	}

	HANDLE hFile = CreateFile(libEnt->szURL, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
				return false;

	unsigned long filesize = GetFileSize(hFile, NULL);

	CloseHandle(hFile);
	unsigned long time = StreamInfo.Sizes.SampleNum/StreamInfo.Audio.SampleRate;

	libEnt->iBitRate			= ((filesize / time) / 128) * 1000.0;
	libEnt->iChannels			= StreamInfo.Audio.ChannelNum;
	libEnt->iSampleRate			= StreamInfo.Audio.SampleRate;
	libEnt->iPlaybackTime		= time*1000;


	return true;
}

bool			CTAKInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CTAKInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			CTAKInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CTAKInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
