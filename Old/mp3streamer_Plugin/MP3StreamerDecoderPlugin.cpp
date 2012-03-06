#include "StdAfx.h"
#include "mp3streamerdecoderplugin.h"
#include "mp3streamerdecoder.h"

CMP3StreamerDecoderPlugin::CMP3StreamerDecoderPlugin(void)
{
}

CMP3StreamerDecoderPlugin::~CMP3StreamerDecoderPlugin(void)
{
}

void			CMP3StreamerDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CMP3StreamerDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
	m_pHelper = pHelper;
}

LPTSTR			CMP3StreamerDecoderPlugin::GetName(void)
{
	return TEXT("MP3 Streamer");
}

GUID			CMP3StreamerDecoderPlugin::GetPluginID(void)
{
	// {6DA92204-2A37-4252-9D10-0E95923106F0}
	static const GUID GUID_MP3STREAMER = { 0x6da92204, 0x2a37, 0x4252, { 0x9d, 0x10, 0xe, 0x95, 0x92, 0x31, 0x6, 0xf0 } };

	return GUID_MP3STREAMER;
}

unsigned long	CMP3StreamerDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CMP3StreamerDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("Streaming MP3 Plugin for Tuniac.\n\nUsing Tony Million's MP3 Decoder.\nCopyright (c) 2002-2008 Tony Million."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMP3StreamerDecoderPlugin::Configure(HWND hParent)
{
	return false;
}

bool			CMP3StreamerDecoderPlugin::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource) || !StrCmpN(szSource, TEXT("ShoutCast://"), 12))
	{
		return true;
	}

	return false;
}

unsigned long	CMP3StreamerDecoderPlugin::GetNumCommonExts(void)
{
	return 0;
}

LPTSTR			CMP3StreamerDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return NULL;
}

IAudioSource *		CMP3StreamerDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CMP3StreamerDecoder *	t	= new CMP3StreamerDecoder(szSource, m_pHelper);
	if(!t->OpenStream())
	{
		delete t;
		return NULL;

	}

	return static_cast<IAudioSource *>(t);
}
