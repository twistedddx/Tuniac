#include "StdAfx.h"
#include "APEdecoderplugin.h"
#include "APEDecoder.h"

CAPEDecoderPlugin::CAPEDecoderPlugin(void)
{
}

CAPEDecoderPlugin::~CAPEDecoderPlugin(void)
{
}

void			CAPEDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CAPEDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CAPEDecoderPlugin::GetName(void)
{
	return(TEXT("APE Decoder"));
}

GUID			CAPEDecoderPlugin::GetPluginID(void)
{
    // {5bda1570-7305-11d9-9669-0800200c9a66}
	static const GUID APEPluginGUID = {0x5bda1570, 0x7305, 0x11d9, {0x96, 0x69, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66} };
	return(APEPluginGUID);
}

unsigned long	CAPEDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CAPEDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("APE Decoder Plugin for Tuniac.\n\nUsing the Monkey's Audio decoder.\nCopyright (C) 2000-2003 by Matthew T. Ashland."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CAPEDecoderPlugin::Configure(HWND hParent)
{
	return false ;
}

bool			CAPEDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-4];
	if(!lstrcmpi(TEXT(".ape"), t))
	{
		return (true);
	}

	return(false);
}

unsigned long	CAPEDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CAPEDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".ape");
}

IAudioSource *		CAPEDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CAPEDecoder *	pDec = new CAPEDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}


	return pDec;
}