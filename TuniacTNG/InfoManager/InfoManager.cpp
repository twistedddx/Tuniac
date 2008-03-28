// InfoManager.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GenericInfoHandler.h"

extern "C" __declspec(dllexport) IInfoHandler * CreateInfoHandler(void)
{
	CGenericInfoHandler	 * pHandler = new CGenericInfoHandler();

	if(pHandler)
	{
		return static_cast<IInfoHandler*>(pHandler);
	}

	return NULL;
}