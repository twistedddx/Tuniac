#include "stdafx.h"
#include "../ape_SDK/All.h"
#include "../ape_SDK/MACLib.h"
#include "../ape_SDK/apetag.h"
#include "apeinfomanager.h"

#define APE_TAG_FIELD_REPLAYGAIN_TRACK_GAIN	L"replaygain_track_gain"
#define APE_TAG_FIELD_REPLAYGAIN_TRACK_PEAK	L"replaygain_track_peak"
#define APE_TAG_FIELD_REPLAYGAIN_ALBUM_GAIN	L"replaygain_album_gain"
#define APE_TAG_FIELD_REPLAYGAIN_ALBUM_PEAK	L"replaygain_album_peak"



// actual class definition here

CAPEInfoManager::CAPEInfoManager(void)
{
}

CAPEInfoManager::~CAPEInfoManager(void)
{
}

void			CAPEInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CAPEInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CAPEInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	return TEXT("APE");
}

bool			CAPEInfoManager::CanHandle(LPTSTR szSource)
{
	if(StrStrI(PathFindExtension(szSource), TEXT("APE")))
		return true;
	return false;
}

bool			CAPEInfoManager::GetInfo(LibraryEntry * libEnt)
{
    int nRetVal;
	IAPEDecompress * MACDecompressor = CreateIAPEDecompress(libEnt->szURL, &nRetVal);
	if (MACDecompressor == NULL){
		return false;
	}

	// Get some information about the APE file

	CAPETag* MACTag = (CAPETag*)MACDecompressor->GetInfo(APE_INFO_TAG);

	BOOL bHasID3Tag = MACTag->GetHasID3Tag();
	BOOL bHasAPETag = MACTag->GetHasAPETag();

	if (bHasID3Tag || bHasAPETag)
	{
		CAPETagField *field;
		int idx = 0;

		while((field = MACTag->GetTagField(idx)) != NULL) {
			idx++;

			const wchar_t *field_name;
			const char *field_value;
			int field_size;

			field_name = field->GetFieldName();

			if(wcscmp(field_name, APE_TAG_FIELD_TITLE) == 0)
			{
				field_value = field->GetFieldValue();
				field_size = field->GetFieldSize();
				MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szTitle, 128);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_ALBUM) == 0)
			{
				field_value = field->GetFieldValue();
				field_size = field->GetFieldSize();
				MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szAlbum, 128);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_ARTIST) == 0)
			{
				field_value = field->GetFieldValue();
				field_size = field->GetFieldSize();
				MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szArtist, 128);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_COMMENT) == 0)
			{
				field_value = field->GetFieldValue();
				field_size = field->GetFieldSize();
				MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szComment, 128);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_GENRE) == 0)
			{
				field_value = field->GetFieldValue();
				field_size = field->GetFieldSize();
				MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szGenre, 128);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_YEAR) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->iYear = atoi(field_value);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_TRACK) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->dwTrack[0] = atoi(field_value);
				continue;
			}
			
			if(wcscmp(field_name, APE_TAG_FIELD_REPLAYGAIN_TRACK_GAIN) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->fReplayGain_Track_Gain = atof(field_value);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_REPLAYGAIN_TRACK_PEAK) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->fReplayGain_Track_Peak = atof(field_value);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_REPLAYGAIN_ALBUM_GAIN) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->fReplayGain_Album_Gain = atof(field_value);
				continue;
			}
			if(wcscmp(field_name, APE_TAG_FIELD_REPLAYGAIN_ALBUM_PEAK) == 0)
			{
				field_value = field->GetFieldValue();
				libEnt->fReplayGain_Album_Peak = atof(field_value);
				continue;
			}
			
		}
	}

	double bitrate = MACDecompressor->GetInfo(APE_INFO_AVERAGE_BITRATE);
	libEnt->iBitRate			= (unsigned long)(bitrate * 1000.0);
	libEnt->iChannels			= MACDecompressor->GetInfo(APE_INFO_CHANNELS);
	libEnt->iSampleRate			= MACDecompressor->GetInfo(APE_INFO_SAMPLE_RATE);
	libEnt->iPlaybackTime		= MACDecompressor->GetInfo(APE_INFO_LENGTH_MS);

	delete MACDecompressor;

	return true;
}

bool			CAPEInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CAPEInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			CAPEInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CAPEInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
