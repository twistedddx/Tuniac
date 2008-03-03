#include "stdafx.h"

#include "m4aAudioSourceSupplier.h"
#include "m4ainfomanager.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateDecoderPlugin(void)
{
	IAudioSourceSupplier * t = new Cm4aAudioSourceSupplier;

	return t;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacAudioSourceVersion(void)
{
	return ITUNIACAUDIOSOURCE_VERSION;
}


extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new Cm4aInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}
