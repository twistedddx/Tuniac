#include "StdAfx.h"
#include "oggdecoderplugin.h"
#include "OGGDecoder.h"

COGGDecoderPlugin::COGGDecoderPlugin(void)
{
}

COGGDecoderPlugin::~COGGDecoderPlugin(void)
{
}

void			COGGDecoderPlugin::Destroy(void)
{
	delete this;
}

void			COGGDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			COGGDecoderPlugin::GetName(void)
{
	return(TEXT("OGG Decoder"));
}

GUID			COGGDecoderPlugin::GetPluginID(void)
{
	static const GUID oggPluginGUID = {0x65f0bdf3, 0x2938, 0x41b9, {0xb1, 0x6e, 0x98, 0x5d, 0xb6, 0xe1, 0xe2, 0x36} };
	return(oggPluginGUID);
}

unsigned long	COGGDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			COGGDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("OGG Decoder Plugin for Tuniac.\n\nUsing the OggVorbis decoder from Xiph.\nCopyright 1994-2007 Xiph.Org Foundation."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			COGGDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			COGGDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-4];
	if(!lstrcmpi(TEXT(".ogg"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	COGGDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			COGGDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".ogg");
}


IAudioSource *		COGGDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	COGGDecoder *	pDec = new COGGDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
