#include "stdafx.h"
#include "Tuniac2.h"

#include "TuniacApp.h"

CTuniacApp		g_tuniacApp;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if(!g_tuniacApp.Initialise(hInstance))
	{
		return -1;
	}

	if(!g_tuniacApp.Run())
	{
		MessageBeep(-1);
	}

	g_tuniacApp.Shutdown();
	return 1;
}
