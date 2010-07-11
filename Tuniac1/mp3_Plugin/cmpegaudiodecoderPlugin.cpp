#include "StdAfx.h"
#include "cmpegaudiodecoderplugin.h"
#include "cmpegaudiodecoder.h"

CMpegAudioDecoderPlugin::CMpegAudioDecoderPlugin(void)
{
}

CMpegAudioDecoderPlugin::~CMpegAudioDecoderPlugin(void)
{
}

void			CMpegAudioDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CMpegAudioDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
	m_pHelper = pHelper;
}

LPTSTR			CMpegAudioDecoderPlugin::GetName(void)
{
	return(TEXT("MP3 Decoder"));
}

GUID			CMpegAudioDecoderPlugin::GetPluginID(void)
{
	static const GUID t = {0x9d6e7d5d, 0xe210, 0x4af8, {0xb8, 0xa7, 0x99, 0x4d, 0xed, 0xd9, 0x9e, 0x82} };

	return(t);
}

unsigned long	CMpegAudioDecoderPlugin::GetFlags(void)
{
	return FLAGS_PROVIDESTANDARDFILEIO | FLAGS_ABOUT;
}

bool			CMpegAudioDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("MP3 Decoder Plugin for Tuniac.\n\nUsing Tony Million's MP3 Decoder.\nCopyright (c) 2002-2008 Tony Million."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMpegAudioDecoderPlugin::Configure(HWND hParent)
{
	return false;
}

bool			CMpegAudioDecoderPlugin::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return(false);

	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szSource)))
	{
		return(true);
	}

	return(false);
}

unsigned long	CMpegAudioDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CMpegAudioDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".mp3");
}

IAudioSource *		CMpegAudioDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CMpegAudioDecoder * pSource = new CMpegAudioDecoder;

	if(pSource->Open(pFileIO))
	{
		return (IAudioSource*)pSource;
	}

	delete pSource;
	return NULL;
}
