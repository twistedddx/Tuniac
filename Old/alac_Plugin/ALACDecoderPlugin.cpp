#include "StdAfx.h"
#include "alacdecoderplugin.h"
#include "ALACDecoder.h"

CALACDecoderPlugin::CALACDecoderPlugin(void)
{
}

CALACDecoderPlugin::~CALACDecoderPlugin(void)
{
}

void			CALACDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CALACDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CALACDecoderPlugin::GetName(void)
{
	return(TEXT("ALAC Decoder"));
}

GUID			CALACDecoderPlugin::GetPluginID(void)
{
	static const GUID alacPluginGUID = {0x65d026f3, 0x2978, 0x41b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(alacPluginGUID);
}

unsigned long	CALACDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CALACDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("ALAC Decoder Plugin for Tuniac. Using ALAC decoder by David Hammerton 2006"), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CALACDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CALACDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-5];
	if(!lstrcmpi(TEXT(".alac"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	CALACDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CALACDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".alac");
}


IAudioSource *		CALACDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CALACDecoder *	pDec = new CALACDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
