// mpcPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "BASSDecoderPlugin.h"


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateDecoderPlugin(void)
{
	IAudioSourceSupplier * t = new CBASSDecoderPlugin;

	return t;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacAudioSourceVersion(void)
{
	return ITUNIACAUDIOSOURCE_VERSION;
}