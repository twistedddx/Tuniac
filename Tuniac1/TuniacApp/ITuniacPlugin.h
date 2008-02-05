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

	bool				PreferencesGet(LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool				PreferencesSet(LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);
};


#define PLUGINFLAGS_ABOUT				0x00000001
#define PLUGINFLAGS_CONFIG				0x00000002

#define PLUGINNOTIFY_SONGCHANGE			WM_USER + 0x00000001
#define PLUGINNOTIFY_SONGCHANGE_MANUAL	WM_USER + 0x00000002
#define PLUGINNOTIFY_SONGPAUSE			WM_USER + 0x00000003
#define PLUGINNOTIFY_SONGSTOP			WM_USER + 0x00000004
#define PLUGINNOTIFY_SONGINFOCHANGE		WM_USER + 0x00000011
#define PLUGINNOTIFY_SEEK_MANUAL		WM_USER + 0x00000101

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
