#include "stdafx.h"
#include "All.h"
#include "MACLib.h"
#include "apetag.h"
#include "apeinfomanager.h"

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
			wchar_t buffer[256];
			int chars = 256;

			if (MACTag->GetFieldString(APE_TAG_FIELD_TITLE, buffer, &chars) == 0)
				swprintf(libEnt->szTitle, 256, L"%s", buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_ALBUM, buffer, &chars) == 0)
				swprintf(libEnt->szAlbum, 256, L"%s", buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_ARTIST, buffer, &chars) == 0)
				swprintf(libEnt->szArtist, 256, L"%s", buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_COMMENT, buffer, &chars) == 0)
				swprintf(libEnt->szComment, 256, L"%s", buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_GENRE, buffer, &chars) == 0)
				swprintf(libEnt->szGenre, 256, L"%s", buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_YEAR, buffer, &chars) == 0)
				libEnt->iYear = _wtoi(buffer);

			if (MACTag->GetFieldString(APE_TAG_FIELD_TRACK, buffer, &chars) == 0)
				libEnt->dwTrack[0] = _wtoi(buffer);
	}

	double bitrate = MACDecompressor->GetInfo(APE_INFO_AVERAGE_BITRATE);
	libEnt->iBitRate			= (unsigned long)(bitrate * 1024.0);
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