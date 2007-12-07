#pragma once

#include "PluginVariable.h"

#define MAXVISSAMPLES		4096


class ITuniacVisHelper
{
public:
	virtual bool	GetVisData(float * pWaveformData, unsigned long ulNumSamples) = 0;

	virtual void *	GetVariable(Variable eVar) = 0;
};

#define PLUGINFLAGS_ABOUT	0x00000001
#define PLUGINFLAGS_CONFIG	0x00000002

#define VISUAL_MOUSEFUNCTION_MOVE		0
#define VISUAL_MOUSEFUNCTION_DOWN		1
#define VISUAL_MOUSEFUNCTION_UP			2

class ITuniacVisPlugin
{
public:
	virtual void	Destroy(void)										= 0;

	virtual LPTSTR	GetPluginName(void)									= 0;
	virtual unsigned long GetFlags(void)								= 0;

	virtual bool	SetHelper(ITuniacVisHelper *pHelper)				= 0;
	
	virtual bool	Attach(HDC hDC)										= 0;
	virtual bool	Detach()											= 0;

	virtual bool	Render(int w, int h)								= 0;

	virtual bool	About(HWND hWndParent)								= 0;
	virtual bool	Configure(HWND hWndParent)							= 0;

	virtual bool	Notify(unsigned long Notification)					= 0;
	virtual bool	MouseFunction(unsigned long function, int x, int y)	= 0;
};

typedef ITuniacVisPlugin * (*CreateTuniacVisPluginFunc)(void);

extern "C" __declspec(dllexport) ITuniacVisPlugin * CreateTuniacVisPlugin(void);