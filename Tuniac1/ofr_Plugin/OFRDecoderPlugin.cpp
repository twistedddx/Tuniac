#include "StdAfx.h"
#include "ofrdecoderplugin.h"
#include "OFRDecoder.h"

COFRDecoderPlugin::COFRDecoderPlugin(void)
{
}

COFRDecoderPlugin::~COFRDecoderPlugin(void)
{
}

void			COFRDecoderPlugin::Destroy(void)
{
	delete this;
}

void			COFRDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			COFRDecoderPlugin::GetName(void)
{
	return(TEXT("OptimFROG Decoder"));
}

GUID			COFRDecoderPlugin::GetPluginID(void)
{
	static const GUID ofrPluginGUID = {0x65d396f3, 0x2978, 0x47b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(ofrPluginGUID);
}

unsigned long	COFRDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			COFRDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("OFR Decoder Plugin for Tuniac. OptimFROG lib Copyright (C) 2005 Florin Ghido"), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			COFRDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			COFRDecoderPlugin::CanHandle(LPTSTR szSource)
{
	for(unsigned int x=0; x<GetNumCommonExts(); x++)
	{
		if(!StrCmpI(GetCommonExt(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}


unsigned long	COFRDecoderPlugin::GetNumCommonExts(void)
{
	return 2;
}

LPTSTR			COFRDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	static LPTSTR exts[] = 
	{
		TEXT(".ofr"),
		TEXT(".ofs")
	};

	return exts[ulIndex];
}


IAudioSource *		COFRDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	COFRDecoder *	pDec = new COFRDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
