#include "StdAfx.h"
#include "mpcdecoderplugin.h"
#include "MPCDecoder.h"

CMPCDecoderPlugin::CMPCDecoderPlugin(void)
{
}

CMPCDecoderPlugin::~CMPCDecoderPlugin(void)
{
}

void			CMPCDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CMPCDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CMPCDecoderPlugin::GetName(void)
{
	return(TEXT("MPC Decoder"));
}

GUID			CMPCDecoderPlugin::GetPluginID(void)
{
	static const GUID mpcPluginGUID = {0x65d0bdf3, 0x2978, 0x41b9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(mpcPluginGUID);
}

unsigned long	CMPCDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CMPCDecoderPlugin::About(HWND hParent)
{
	MessageBox(hParent, TEXT("MPC Decoder Plugin for Tuniac.\n\nUsing the libmpc decoder from The Musepack Development Team.\nCopyright 2005 The Musepack Development Team."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMPCDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CMPCDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[lstrlen(szSource)-4];
	if(!lstrcmpi(TEXT(".mpc"), t))
	{
		return(true);
	}

	return(false);
}


unsigned long	CMPCDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CMPCDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".mpc");
}


IAudioSource *		CMPCDecoderPlugin::CreateAudioSource(LPTSTR szSource)
{
	CMPCDecoder *	pDec = new CMPCDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
