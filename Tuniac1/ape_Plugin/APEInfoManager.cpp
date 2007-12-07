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

	TCHAR	szFileTitle[128];
	GetFileTitle(libEnt->szURL, szFileTitle, 128);
	if (bHasID3Tag || bHasAPETag)
	{
			TCHAR tstr[256];
			CAPETagField* tempField = MACTag->GetTagField (APE_TAG_FIELD_TITLE);
			if ( tempField )
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, libEnt->szTitle, 128);
			else
			{
				StrCpy(libEnt->szTitle, szFileTitle);
			}
			tempField = MACTag->GetTagField (APE_TAG_FIELD_ALBUM);
			if(tempField)
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, libEnt->szAlbum, 128);

			tempField = MACTag->GetTagField (APE_TAG_FIELD_ARTIST);
			if(tempField)
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, libEnt->szArtist, 128);

			tempField = MACTag->GetTagField (APE_TAG_FIELD_COMMENT);
			if(tempField)
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, libEnt->szComment, 128);

			tempField = MACTag->GetTagField (APE_TAG_FIELD_GENRE);
			if(tempField)
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, libEnt->szGenre, 128);

			tempField = MACTag->GetTagField (APE_TAG_FIELD_YEAR);
			if(tempField)
			{
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, tstr, 128);
				libEnt->iYear = _wtoi(tstr);
			}

			tempField = MACTag->GetTagField (APE_TAG_FIELD_TRACK);
			if(tempField)
			{
				MultiByteToWideChar(CP_ACP, 0, tempField->GetFieldValue(), 128, tstr, 128);
				libEnt->dwTrack[0] = _wtoi(tstr);
			}
	}
	else 
	{
		StrCpy(libEnt->szTitle, szFileTitle);
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