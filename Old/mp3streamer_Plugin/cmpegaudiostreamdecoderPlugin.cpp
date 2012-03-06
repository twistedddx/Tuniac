#include "StdAfx.h"
#include "cmpegaudiostreamdecoderplugin.h"
#include "cmpegaudiostreamdecoder.h"

CMpegAudioStreamDecoderPlugin::CMpegAudioStreamDecoderPlugin(void)
{
}

CMpegAudioStreamDecoderPlugin::~CMpegAudioStreamDecoderPlugin(void)
{
}

void			CMpegAudioStreamDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CMpegAudioStreamDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
	m_pHelper = pHelper;
}

LPTSTR			CMpegAudioStreamDecoderPlugin::GetName(void)
{
	return(TEXT("MP3 Decoder"));
}

GUID			CMpegAudioStreamDecoderPlugin::GetPluginID(void)
{
	static const GUID t = {0x9d6e7d5d, 0xe210, 0x4af8, {0xb8, 0xa7, 0x99, 0x4d, 0xed, 0xd9, 0x9e, 0x82} };

	return(t);
}

unsigned long	CMpegAudioStreamDecoderPlugin::GetFlags(void)
{
	return FLAGS_PROVIDEHTTPFILEIO | FLAGS_ABOUT;
}

bool			CMpegAudioStreamDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("MP3 Streamer Decoder Plugin for Tuniac.\n\nUsing Tony Million's MP3 Decoder.\nCopyright (c) 2002-2008 Tony Million."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMpegAudioStreamDecoderPlugin::Configure(HWND hParent)
{
	return false;
}

bool			CMpegAudioStreamDecoderPlugin::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource) || !StrCmpN(szSource, TEXT("ShoutCast://"), 12))
	{
		return(true);
	}

	return(false);
}

unsigned long	CMpegAudioStreamDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CMpegAudioStreamDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".mp3");
}

IAudioSource *		CMpegAudioStreamDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CMpegAudioDecoder * pSource = new CMpegAudioDecoder;

	if(pSource->Open(pFileIO))
	{
		return (IAudioSource*)pSource;
	}

	delete pSource;
	return NULL;
}
