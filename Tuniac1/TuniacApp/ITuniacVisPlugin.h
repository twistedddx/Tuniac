/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2014 Brett Hoyle
*/

#pragma pack(16)
#pragma once

#include "PluginVariable.h"

#define MAXVISSAMPLES		4096
#define ITUNIACVISPLUGIN_VERSION	MAKELONG(0,1)

class ITuniacVisHelper
{
public:
	virtual unsigned long	GetVisData(float * pWaveformData, unsigned long ulNumSamples) = 0;

	virtual void *	GetVariable(Variable eVar) = 0;

	virtual bool	GetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = 0;
	virtual bool	SetVisualPref(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData) = 0;
	virtual void	GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iFromCurrent) = 0;

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

typedef unsigned long (*GetTuniacVisPluginVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long		GetTuniacVisPluginVersion(void);