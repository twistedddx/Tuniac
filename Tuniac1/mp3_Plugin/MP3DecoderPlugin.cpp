#include "StdAfx.h"
#include "mp3decoderplugin.h"
#include "mp3decoder.h"

CMP3DecoderPlugin::CMP3DecoderPlugin(void)
{
}

CMP3DecoderPlugin::~CMP3DecoderPlugin(void)
{
}

void			CMP3DecoderPlugin::Destroy(void)
{
	delete this;
}

void			CMP3DecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
	m_pHelper = pHelper;
}

LPTSTR			CMP3DecoderPlugin::GetName(void)
{
	return(TEXT("MP3 Decoder"));
}

GUID			CMP3DecoderPlugin::GetPluginID(void)
{
	static const GUID t = {0x9d6e7d5d, 0xe210, 0x4af8, {0xb8, 0xa7, 0x99, 0x4d, 0xed, 0xd9, 0x9e, 0x82} };

	return(t);
}

unsigned long	CMP3DecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CMP3DecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("MP3 Decoder Plugin for Tuniac.\n\nUsing Tony Million's MP3 Decoder.\nCopyright (c) 2002 Tony Million."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMP3DecoderPlugin::Configure(HWND hParent)
{
	return false;
}

bool			CMP3DecoderPlugin::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return(false);

	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szSource)))
	{
		return(true);
	}

	return(false);
}

unsigned long	CMP3DecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CMP3DecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".mp3");
}

IAudioSource *		CMP3DecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CMP3Decoder * t = new CMP3Decoder(m_pHelper);

	if(t->Open(szSource))
	{
		return t;
	}

	delete t;
	return NULL;
}
