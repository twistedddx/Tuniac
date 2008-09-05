// mpcPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <shnplay.h>
#include "shnDecoderPlugin.h"
#include "SHNInfoManager.h"


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return 1;
}

extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateDecoderPlugin(void)
{
	IAudioSourceSupplier * t = new CshnDecoderPlugin;

	return t;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacAudioSourceVersion(void)
{
	return ITUNIACAUDIOSOURCE_VERSION;
}

extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CSHNInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}