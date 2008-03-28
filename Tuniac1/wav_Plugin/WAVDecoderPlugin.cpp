#include "StdAfx.h"
#include "wavdecoderplugin.h"
#include "WAVDecoder.h"

CWAVDecoderPlugin::CWAVDecoderPlugin(void)
{
}

CWAVDecoderPlugin::~CWAVDecoderPlugin(void)
{
}

void			CWAVDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CWAVDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CWAVDecoderPlugin::GetName(void)
{
	return(TEXT("WAV Decoder"));
}

GUID			CWAVDecoderPlugin::GetPluginID(void)
{
	static const GUID wavPluginGUID = {0x65d0bdf3, 0x2978, 0x41b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(wavPluginGUID);
}

unsigned long	CWAVDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CWAVDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("WAV Decoder Plugin for Tuniac."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CWAVDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CWAVDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-4];
	if(!lstrcmpi(TEXT(".wav"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	CWAVDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CWAVDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".wav");
}


IAudioSource *		CWAVDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CWAVDecoder *	pDec = new CWAVDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
