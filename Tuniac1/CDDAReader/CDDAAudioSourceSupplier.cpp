#include "StdAfx.h"
#include ".\cddaaudiosourcesupplier.h"

#include "CDDAAudioSource.h"

CCDDAAudioSourceSupplier::CCDDAAudioSourceSupplier(void)
{
}

CCDDAAudioSourceSupplier::~CCDDAAudioSourceSupplier(void)
{
}

void			CCDDAAudioSourceSupplier::Destroy(void)
{
	delete this;
}

void			CCDDAAudioSourceSupplier::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CCDDAAudioSourceSupplier::GetName(void)
{
	return TEXT("CD Audio Source");
}

GUID			CCDDAAudioSourceSupplier::GetPluginID(void)
{
// {D1433411-D3BD-43c2-A837-12683893CB08}
static const GUID GUIDCDDAREADER = 
{ 0xd1433411, 0xd3bd, 0x43c2, { 0xa8, 0x37, 0x12, 0x68, 0x38, 0x93, 0xcb, 0x8 } };

	return GUIDCDDAREADER;
}

unsigned long	CCDDAAudioSourceSupplier::GetFlags(void)
{
	return 0;
}

bool			CCDDAAudioSourceSupplier::About(HWND hParent)
{
	return false;
}

bool			CCDDAAudioSourceSupplier::Configure(HWND hParent)
{
	return false;
}

bool			CCDDAAudioSourceSupplier::CanHandle(LPTSTR szSource)
{
	if(!StrCmpN(szSource, TEXT("AUDIOCD"), 7))
		return true;

	return false;
}

unsigned long	CCDDAAudioSourceSupplier::GetNumCommonExts(void)
{
	return 0;
}

LPTSTR			CCDDAAudioSourceSupplier::GetCommonExt(unsigned long ulIndex)
{
	return NULL;
}

IAudioSource *	CCDDAAudioSourceSupplier::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CCDDAAudioSource * pSource = new CCDDAAudioSource();

	if(pSource)
	{
		if(pSource->Open(szSource))
		{
			return pSource;
		}

		delete pSource;
	}

	return NULL;
}