#include "StdAfx.h"
#include "ttadecoderplugin.h"
#include "TTADecoder.h"

CTTADecoderPlugin::CTTADecoderPlugin(void)
{
}

CTTADecoderPlugin::~CTTADecoderPlugin(void)
{
}

void			CTTADecoderPlugin::Destroy(void)
{
	delete this;
}

void			CTTADecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CTTADecoderPlugin::GetName(void)
{
	return(TEXT("TrueAudio Decoder"));
}

GUID			CTTADecoderPlugin::GetPluginID(void)
{
	static const GUID ttaPluginGUID = {0x65d396f3, 0x2920, 0x47b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(ttaPluginGUID);
}

unsigned long	CTTADecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CTTADecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("TTA Decoder Plugin for Tuniac."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CTTADecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CTTADecoderPlugin::CanHandle(LPTSTR szSource)
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


unsigned long	CTTADecoderPlugin::GetNumCommonExts(void)
{
	return 2;
}

LPTSTR			CTTADecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	static LPTSTR exts[] = 
	{
		TEXT(".tta"),
		TEXT(".ofs")
	};

	return exts[ulIndex];
}


IAudioSource *		CTTADecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CTTADecoder *	pDec = new CTTADecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
