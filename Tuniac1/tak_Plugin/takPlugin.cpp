// takPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TAKDecoderPlugin.h"
#include "TAKInfoManager.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateDecoderPlugin(void)
{
	IAudioSourceSupplier * t = new CTAKDecoderPlugin;

	return t;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacAudioSourceVersion(void)
{
	return ITUNIACAUDIOSOURCE_VERSION;
}

extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CTAKInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}
