// GenericInfoReader.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "GenericInfoHandler.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

extern "C" __declspec(dllexport) void * CreateTuniacPlugin(GUID InterfaceID, unsigned long InterfaceNumber)
{
	if(InterfaceID == GUID_TUNIACINFOHANDLER)
	{
		if(InterfaceNumber == 0)
		{
			return new CGenericInfoHandler;
		}
	}

	return NULL;
}