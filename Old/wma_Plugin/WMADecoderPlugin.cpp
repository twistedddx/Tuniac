#include "StdAfx.h"
#include "wmadecoderplugin.h"
#include "WMADecoder.h"

CWMADecoderPlugin::CWMADecoderPlugin(void)
{
}

CWMADecoderPlugin::~CWMADecoderPlugin(void)
{
}

void			CWMADecoderPlugin::Destroy(void)
{
	delete this;
}

void			CWMADecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CWMADecoderPlugin::GetName(void)
{
	return(TEXT("WMA Decoder"));
}

GUID			CWMADecoderPlugin::GetPluginID(void)
{
	static const GUID mpcPluginGUID = {0x65d0eaf3, 0x21f8, 0x4db9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(mpcPluginGUID);
}

unsigned long	CWMADecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CWMADecoderPlugin::About(HWND hParent)
{
    MessageBox(hParent, TEXT("WMA Plugin \nUsing wmfsdk"), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CWMADecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CWMADecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-4];
	if(!lstrcmpi(TEXT(".wma"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	CWMADecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CWMADecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".wma");
}


IAudioSource *		CWMADecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CWMADecoder *	pDec = new CWMADecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
