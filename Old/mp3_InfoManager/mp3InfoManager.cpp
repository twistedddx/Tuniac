////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//


#include "stdafx.h"

#include "id3.h"
#include "id3/tag.h"

#include ".\mp3infomanager.h"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CID3InfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}


// actual class definition here

CID3InfoManager::CID3InfoManager(void)
{
}

CID3InfoManager::~CID3InfoManager(void)
{
}

void			CID3InfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CID3InfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CID3InfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	return TEXT("MP3");
}

bool			CID3InfoManager::CanHandle(LPTSTR szSource)
{
	if(StrStrI(PathFindExtension(szSource), TEXT("MP3")))
		return true;

	return false;
}

int GetField(ID3_Tag & myTag, ID3_FrameID frameID, LPTSTR szToHere, unsigned long ulLength)
{
	ID3_Frame	*	frame = myTag.Find(frameID);
	if(frame)
	{
		frame->GetField(ID3FN_TEXT)->SetEncoding(ID3TE_ASCII);
		if(ID3TE_IS_SINGLE_BYTE_ENC(frame->GetField(ID3FN_TEXT)->GetEncoding()))
		{
			char myBuffer[1024];

			if(frame->GetField(ID3FN_TEXT)->Get(myBuffer, 1024))
				MultiByteToWideChar(CP_ACP, 0, myBuffer, -1, szToHere, ulLength);
		}
		/*else if(ID3TE_IS_DOUBLE_BYTE_ENC(frame->GetField(ID3FN_TEXT)->GetEncoding()))
		{
			WCHAR myBuffer[1024];
			frame->GetField(ID3FN_TEXT)->SetEncoding(ID3TE_UTF8);

			if(frame->GetField(ID3FN_TEXT)->Get((unicode_t *)myBuffer, 1024))
			{
				StrCpyN(szToHere, myBuffer, ulLength);
			}
		}*/
	}

	return true;
}

bool			CID3InfoManager::GetInfo(LibraryEntry * libEnt)
{
	char ConvertBuffer[512];

	WideCharToMultiByte(CP_ACP, 0, libEnt->szURL, 512, ConvertBuffer, 512, NULL, NULL);
    ID3_Tag myTag;
	myTag.Link(ConvertBuffer);

	GetField(myTag, ID3FID_TITLE,		libEnt->szTitle,	128);
	GetField(myTag, ID3FID_LEADARTIST,	libEnt->szArtist,	128);
	GetField(myTag, ID3FID_ALBUM,		libEnt->szAlbum,	128);
	GetField(myTag, ID3FID_COMMENT,		libEnt->szComment,	128);

	if(GetField(myTag, ID3FID_CONTENTTYPE,		libEnt->szGenre,	128))
	{
		if(libEnt->szGenre[0] == TEXT('('))
		{
			MultiByteToWideChar(CP_ACP, 0, ID3_v1_genre_description[min(_wtoi(&libEnt->szGenre[1]), 146)], 128, libEnt->szGenre, 128);
		}
	}

	TCHAR tstr[256];

	if(GetField(myTag, ID3FID_YEAR,		tstr,	128))
	{
		libEnt->iYear				= _wtoi(tstr);
	}

	if(GetField(myTag, ID3FID_TRACKNUM,		tstr,	128))
	{
	//	LPTSTR	p;

	//	libEnt->dwTrack[1] = 0;
		libEnt->dwTrack[0] = _wtoi(tstr);

	//	p = tstr;
/*
		while(*p != TEXT('\0'))
		{
			if(*p == TEXT('/'))
			{
				libEnt->dwTrack[1] = _wtoi(p+1);
			}
			p++;
		}
		*/
	}

	// stuff we work out here!
	const Mp3_Headerinfo * header = myTag.GetMp3HeaderInfo();
	if(header)
	{
		libEnt->iChannels			= header->channelmode == MP3CHANNELMODE_SINGLE_CHANNEL ? 1 : 2;
		libEnt->iSampleRate			= header->frequency;
		if(header->vbr_bitrate)
		{
			libEnt->iBitRate			= header->vbr_bitrate;
			// note: this gives same result? id3lib calculates it differently though. but this is more like what the mp3 decoder does
			double bs[3]				= { 384.0, 1152.0, 1152.0 };
			double TimePerFrame			= (double)bs[header->layer] / (((double)header->frequency / 1000.0));
			libEnt->iPlaybackTime		= TimePerFrame * header->frames;
		}
		else
		{
			libEnt->iBitRate			= header->bitrate;
			libEnt->iPlaybackTime		= header->time * 1000;
		}
	}
	else
	{
		libEnt->iBitRate			= 0;
		libEnt->iChannels			= 0;
		libEnt->iSampleRate			= 0;
		libEnt->iPlaybackTime		= 0;
	}
	return true;
}

bool			CID3InfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}