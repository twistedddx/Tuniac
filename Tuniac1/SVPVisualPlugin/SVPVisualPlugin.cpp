// SVPVisualPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SVPRenderer.h"

HANDLE	hInst;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInst = hModule;

    return TRUE;
}

extern "C" __declspec(dllexport) ITuniacVisPlugin * CreateTuniacVisPlugin(void)
{
	ITuniacVisPlugin * t = new SVPRenderer;

	if(t)
	{
		return t;
	}

	return NULL;
}