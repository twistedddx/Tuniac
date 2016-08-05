#include "StdAfx.h"
#include "shndecoderplugin.h"
#include "shnDecoder.h"

CshnDecoderPlugin::CshnDecoderPlugin(void)
{
}

CshnDecoderPlugin::~CshnDecoderPlugin(void)
{
}

void			CshnDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CshnDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
	//	m_pHelper = pHelper;
}

LPTSTR			CshnDecoderPlugin::GetName(void)
{
	return(TEXT("SHN Decoder"));
}

GUID			CshnDecoderPlugin::GetPluginID(void)
{
	static const GUID mpcPluginGUID = {0x65d0eaf3, 0x21f8, 0x4db9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(mpcPluginGUID);
}

unsigned long	CshnDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CshnDecoderPlugin::About(HWND hParent)
{
    MessageBox(hParent, TEXT("SHN Plugin \nUsing libshnplay \nCopyright (c) 2005, Holger Stenger "), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CshnDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CshnDecoderPlugin::CanHandle(LPTSTR szSource)
{
	LPTSTR	t = &szSource[wcsnlen_s(szSource, MAX_PATH)-4];
	if(StrCmpI(TEXT(".shn"), t) == 0)
	{
		return(true);
	}

	return(false);
}

bool			CshnDecoderPlugin::Close(void)
{
	return true;
}

unsigned long	CshnDecoderPlugin::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CshnDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".shn");
}


IAudioSource *		CshnDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CshnDecoder *	pDec = new CshnDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
