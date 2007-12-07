// Tuniac.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

CTuniacApp		tuniacApp;
CRandom			g_Rand;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	if(!tuniacApp.Initialize(hInstance, GetCommandLine()))
		return(0);

	tuniacApp.Run();

	tuniacApp.Shutdown();

	return 0;
}



