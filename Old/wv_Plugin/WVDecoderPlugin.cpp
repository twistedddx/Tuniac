#include "StdAfx.h"
#include "wvdecoderplugin.h"
#include "WVDecoder.h"

CWVDecoderPlugin::CWVDecoderPlugin(void)
{
}

CWVDecoderPlugin::~CWVDecoderPlugin(void)
{
}

void			CWVDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CWVDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CWVDecoderPlugin::GetName(void)
{
	return(TEXT("WV Decoder"));
}

GUID			CWVDecoderPlugin::GetPluginID(void)
{
	static const GUID mpcPluginGUID = {0x65d0eaf3, 0x21f8, 0x4db9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(mpcPluginGUID);
}

unsigned long	CWVDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CWVDecoderPlugin::About(HWND hParent)
{
    MessageBox(hParent, TEXT("WavPack Plugin \nCopyright (c) 2006 Conifer Software "), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CWVDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CWVDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-3];
	if(!lstrcmpi(TEXT(".wv"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	CWVDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CWVDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".wv");
}


IAudioSource *		CWVDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CWVDecoder *	pDec = new CWVDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
