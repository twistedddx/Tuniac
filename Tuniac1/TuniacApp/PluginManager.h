#pragma once
#include "ITuniacPlugin.h"

typedef struct
{
	HMODULE					hDLL;
	HANDLE					hThread;
	DWORD					dwThreadId;
	ITuniacPlugin *			pPlugin;
	TCHAR					szName[64];
	TCHAR					szDllFile[64];
	unsigned long			ulFlags;
	bool					bEnabled;
} PluginEntry;

class CPluginManager :
	public ITuniacPluginHelper
{
protected:
	Array<PluginEntry, 3>		m_PluginArray;
	TCHAR						m_PluginPath[512];

public:
	CPluginManager(void);
	~CPluginManager(void);

	bool			Initialize(void);
	bool			Shutdown(void);

	unsigned int	GetNumPlugins(void);
	PluginEntry *	GetPluginAtIndex(unsigned int iPlugin);
	LPTSTR			GetCurrentPluginDll(void);

	bool			IsPluginEnabled(unsigned int iPlugin);
	bool			EnablePlugin(unsigned int iPlugin, bool bEnabled);

	void			PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

	void *			GetVariable(Variable eVar);
	void			GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iFromCurrent);
	bool			Navigate(int iFromCurrent);

	HINSTANCE		GetMainInstance(void);
	HWND			GetMainWindow(void);

	bool			PreferencesGet(LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool			PreferencesSet(LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);

};
