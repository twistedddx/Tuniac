#include "StdAfx.h"
#include "FLACAudioSourceSupplier.h"
#include "FLACAudioSource.h"

CFLACAudioSourceSupplier::CFLACAudioSourceSupplier(void)
{
}

CFLACAudioSourceSupplier::~CFLACAudioSourceSupplier(void)
{
}

void			CFLACAudioSourceSupplier::Destroy(void)
{
	delete this;
}

void			CFLACAudioSourceSupplier::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CFLACAudioSourceSupplier::GetName(void)
{
	return TEXT("FLAC Decoder");
}

GUID			CFLACAudioSourceSupplier::GetPluginID(void)
{
// {AD2EB8A5-AB9A-450e-AB05-4CA1E73BF5C3}
static const GUID FLAC_GUID = 
{ 0xad2eb8a5, 0xab9a, 0x450e, { 0xab, 0x5, 0x4c, 0xa1, 0xe7, 0x3b, 0xf5, 0xc3 } };

	return FLAC_GUID;
}

unsigned long	CFLACAudioSourceSupplier::GetFlags(void)
{
	return FLAGS_PROVIDESTANDARDFILEIO | FLAGS_ABOUT;
}

bool			CFLACAudioSourceSupplier::About(HWND hParent)
{
	MessageBox(hParent, TEXT("FLAC Decoder Plugin for Tuniac.\n\nUsing the libFLAC decoder from Xiph.\nCopyright (c) 2004,2005,2006,2007,2008 Josh Coalson"), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}
bool			CFLACAudioSourceSupplier::Configure(HWND hParent)
{
	return false;
}

bool			CFLACAudioSourceSupplier::CanHandle(LPTSTR szSource)
{
	if(!StrCmpI(TEXT(".flac"), PathFindExtension(szSource)))
	{
		return(true);
	}

	return(false);
}

unsigned long	CFLACAudioSourceSupplier::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CFLACAudioSourceSupplier::GetCommonExt(unsigned long ulIndex)
{
	return TEXT(".flac");
}

IAudioSource *	CFLACAudioSourceSupplier::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CFLACAudioSource * pSource = new CFLACAudioSource;

	if(pSource->Open(pFileIO))
	{
		return (IAudioSource*)pSource;
	}

	delete pSource;
	return NULL;
}