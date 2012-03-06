// FLAC_Plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "FLACAudioSourceSupplier.h"


extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateDecoderPlugin(void)
{
	IAudioSourceSupplier * t = new CFLACAudioSourceSupplier;

	return t;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacAudioSourceVersion(void)
{
	return ITUNIACAUDIOSOURCE_VERSION;
}