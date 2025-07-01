#include "stdafx.h"
#include "TuniacGoom.h"



HANDLE	hInst;

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	hInst = hModule;

	return TRUE;
}

extern "C" __declspec(dllexport) ITuniacVisPlugin* CreateTuniacVisPlugin(void)
{
	ITuniacVisPlugin* t = new TuniacGoom;

	if (t)
	{
		return t;
	}

	return NULL;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacVisPluginVersion(void)
{
	return ITUNIACVISPLUGIN_VERSION;
}