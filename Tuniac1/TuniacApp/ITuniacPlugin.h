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

#pragma once
#include "PluginVariable.h"

#define ITUNIACPLUGIN_VERSION	MAKELONG(0,3)

class ITuniacPluginHelper
{
public:
	virtual void *		GetVariable(Variable eVar)						= 0;
	virtual void		GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iFromCurrent) = 0;

	virtual bool		Navigate(int iFromCurrent)						= 0;
	virtual HINSTANCE	GetMainInstance(void)							= 0;
	virtual HWND		GetMainWindow(void)								= 0;

	virtual bool		PreferencesGet(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = 0;
	virtual bool		PreferencesSet(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData) = 0;
};


#define PLUGINFLAGS_ABOUT				0x00000001
#define PLUGINFLAGS_CONFIG				0x00000002

#define PLUGINNOTIFY_SONGCHANGE				WM_USER + 0x00000001
#define PLUGINNOTIFY_SONGCHANGE_BLIND		WM_USER + 0x00000002
#define PLUGINNOTIFY_SONGCHANGE_MANUAL		WM_USER + 0x00000003
#define PLUGINNOTIFY_SONGCHANGE_MANUALBLIND	WM_USER + 0x00000004
#define PLUGINNOTIFY_SONGCHANGE_INIT		WM_USER + 0x00000005
#define PLUGINNOTIFY_SONGPAUSE				WM_USER + 0x00000006
#define PLUGINNOTIFY_SONGPLAY				WM_USER + 0x00000007
#define PLUGINNOTIFY_SONGINFOCHANGE			WM_USER + 0x00000008
#define PLUGINNOTIFY_SEEK_MANUAL			WM_USER + 0x00000009
#define PLUGINNOTIFY_SONGPLAYED				WM_USER + 0x00000010


class ITuniacPlugin
{
public:
	virtual void			Destroy(void)							= 0;

	virtual LPTSTR			GetPluginName(void)						= 0;
	virtual unsigned long	GetFlags(void)							= 0;

	virtual bool			SetHelper(ITuniacPluginHelper *pHelper)	= 0;

	virtual HANDLE			CreateThread(LPDWORD lpThreadId)		= 0;
	virtual	HWND			GetMainWindow(void)						= 0;

	virtual bool			About(HWND hWndParent)					= 0;
	virtual bool			Configure(HWND hWndParent)				= 0;
};

typedef ITuniacPlugin *	(*CreateTuniacPluginFunc)(void);
extern "C" __declspec(dllexport) ITuniacPlugin *	CreateTuniacPlugin(void);

typedef unsigned long (*GetTuniacPluginVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void);
