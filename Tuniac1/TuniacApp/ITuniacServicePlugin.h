#pragma once

#define ITUNIACSERVICEPLUGIN_VERSION	MAKELONG(0,4)

class ITuniacServicePluginHelper
{
public:

	virtual void		LogMessage(LPTSTR szModuleName, LPTSTR szMessage) = 0;

	virtual bool		PreferencesGet(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = 0;
	virtual bool		PreferencesSet(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData) = 0;
};


#define SERVICEPLUGINFLAGS_ABOUT				0x00000001
#define SERVICEPLUGINFLAGS_CONFIG				0x00000002


class ITuniacServicePlugin
{
public:
	virtual void			Destroy(void) = 0;

	virtual LPTSTR			GetPluginName(void) = 0;
	virtual unsigned long	GetFlags(void) = 0;

	virtual bool			SetHelper(ITuniacServicePluginHelper* pHelper) = 0;

	virtual bool			About(HWND hWndParent) = 0;
	virtual bool			Configure(HWND hWndParent) = 0;
};

typedef ITuniacServicePlugin* (*CreateTuniacServicePluginFunc)(void);
extern "C" __declspec(dllexport) ITuniacServicePlugin * CreateTuniacServicePlugin(void);

typedef unsigned long (*GetTuniacPluginVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long		GetTuniacServicePluginVersion(void);
