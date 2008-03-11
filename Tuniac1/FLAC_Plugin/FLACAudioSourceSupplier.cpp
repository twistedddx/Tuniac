#include "StdAfx.h"
#include "FLACAudioSourceSupplier.h"

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
	return TEXT("FLAC Audio Decoder");
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
	return(0);
}

bool			CFLACAudioSourceSupplier::About(HWND hParent)
{
	return false;
}
bool			CFLACAudioSourceSupplier::Configure(HWND hParent)
{
	return false;
}

bool			CFLACAudioSourceSupplier::CanHandle(LPTSTR szSource)
{
	return false;
}

unsigned long	CFLACAudioSourceSupplier::GetNumCommonExts(void)
{
	return 1;
}

LPTSTR			CFLACAudioSourceSupplier::GetCommonExt(unsigned long ulIndex)
{
	return TEXT("FLAC");
}

IAudioSource *	CFLACAudioSourceSupplier::CreateAudioSource(LPTSTR szSource)
{
	return NULL;
}