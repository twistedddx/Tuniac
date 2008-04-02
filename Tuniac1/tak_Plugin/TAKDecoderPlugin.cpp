#include "StdAfx.h"
#include "takdecoderplugin.h"
#include "TAKDecoder.h"

CTAKDecoderPlugin::CTAKDecoderPlugin(void)
{
}

CTAKDecoderPlugin::~CTAKDecoderPlugin(void)
{
}

void			CTAKDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CTAKDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CTAKDecoderPlugin::GetName(void)
{
	return(TEXT("TAK Decoder"));
}

GUID			CTAKDecoderPlugin::GetPluginID(void)
{
	static const GUID takPluginGUID = {0x65d335f3, 0x2920, 0x47b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(takPluginGUID);
}

unsigned long	CTAKDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CTAKDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("TAK Decoder Plugin for Tuniac."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CTAKDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CTAKDecoderPlugin::CanHandle(LPTSTR szSource)
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


unsigned long	CTAKDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CTAKDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	static LPTSTR exts[] = 
	{
		TEXT(".tak")
	};

	return exts[ulIndex];
}


IAudioSource *		CTAKDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CTAKDecoder *	pDec = new CTAKDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
