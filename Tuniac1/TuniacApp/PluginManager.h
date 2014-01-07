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
	Copyright (C) 2003-2012 Brett Hoyle
*/

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
	TCHAR						m_PluginPath[MAX_PATH];

public:
	CPluginManager(void);
	~CPluginManager(void);

	bool			Initialize(void);
	bool			Shutdown(void);

	unsigned int	GetNumPlugins(void);
	PluginEntry *	GetPluginAtIndex(unsigned int iPlugin);

	bool			IsPluginEnabled(unsigned int iPlugin);
	bool			EnablePlugin(unsigned int iPlugin, bool bEnabled);

	void			PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

	void *			GetVariable(Variable eVar);
	void			GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iIndex);
	bool			Navigate(int iFromCurrent);

	HINSTANCE		GetMainInstance(void);
	HWND			GetMainWindow(void);

	bool			PreferencesGet(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	bool			PreferencesSet(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData);

};
