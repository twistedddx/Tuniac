#include "stdafx.h"
#include "m4aaudiosourcesupplier.h"

Cm4aAudioSourceSupplier::Cm4aAudioSourceSupplier(void)
{
}

Cm4aAudioSourceSupplier::~Cm4aAudioSourceSupplier(void)
{
}

void			Cm4aAudioSourceSupplier::Destroy(void)
{
	delete this;
}

void			Cm4aAudioSourceSupplier::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			Cm4aAudioSourceSupplier::GetName(void)
{
	return TEXT("M4A Decoder");
}

GUID			Cm4aAudioSourceSupplier::GetPluginID(void)
{
	// {97C3ABAC-64A8-490e-9D44-C1707BBDB325}
	static const GUID GUID_M4APLUGIN = { 0x97c3abac, 0x64a8, 0x490e, { 0x9d, 0x44, 0xc1, 0x70, 0x7b, 0xbd, 0xb3, 0x25 } };

	return GUID_M4APLUGIN;
}

unsigned long	Cm4aAudioSourceSupplier::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			Cm4aAudioSourceSupplier::About(HWND hParent)
{
	MessageBox(hParent, TEXT("M4A Decoder Plugin for Tuniac.\n\nUsing the FAAD2 decoder from Nero.\nCopyright (C) 2003-2005 M. Bakker"), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			Cm4aAudioSourceSupplier::Configure(HWND hParent)
{
	return false;
}

bool			Cm4aAudioSourceSupplier::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return(false);

	for(unsigned int x=0; x<GetNumCommonExts(); x++)
	{
		if(!StrCmpI(GetCommonExt(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}

unsigned long	Cm4aAudioSourceSupplier::GetNumCommonExts(void)
{
	return 3;
}

LPTSTR			Cm4aAudioSourceSupplier::GetCommonExt(unsigned long ulIndex)
{
	static LPTSTR exts[] = 
	{
		TEXT(".m4a"),
		TEXT(".mp4"),
		TEXT(".3gp")
	};

	return exts[ulIndex];
}

IAudioSource *	Cm4aAudioSourceSupplier::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	Cm4aAudioSource * source = new Cm4aAudioSource;

	if(source->Open(szSource))
	{
		return source;
	}

	source->Destroy();
	return NULL;
}
