#include "stdafx.h"
#include "ServicePluginManager.h"

CServicePluginManager::CServicePluginManager(void)
{
}

CServicePluginManager::~CServicePluginManager(void)
{
}

bool			CServicePluginManager::Initialize(void)
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szFilename[MAX_PATH];

	DWORD				lpRegType = REG_DWORD;
	DWORD				iRegSize = sizeof(int);
	BOOL				bRegEnabled;

	GetModuleFileName(NULL, m_PluginPath, MAX_PATH);
	PathRemoveFileSpec(m_PluginPath);
	PathAddBackslash(m_PluginPath);
	StringCchCat(m_PluginPath, MAX_PATH, TEXT("services"));
	PathAddBackslash(m_PluginPath);
	StringCchCopy(szFilename, MAX_PATH, m_PluginPath);
	StringCchCat(szFilename, MAX_PATH, TEXT("*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0)
				continue;

			TCHAR szDll[MAX_PATH];

			StringCchCopy(szDll, MAX_PATH, m_PluginPath);
			StringCchCat(szDll, MAX_PATH, w32fd.cFileName);

			HMODULE hDLL = LoadLibrary(szDll);
			if (hDLL)
			{
				GetTuniacPluginVersionFunc pGTPVF = (GetTuniacPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacPluginVersion");
				if (pGTPVF == NULL)
				{
					FreeLibrary(hDLL);
					continue;
				}
				if (pGTPVF() != ITUNIACPLUGIN_VERSION)
				{
					TCHAR szError[MAX_PATH + 100];
					StringCchPrintf(szError, MAX_PATH + 100, TEXT("Incompatable plugin found: \\services\\%s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
					if (tuniacApp.m_LogWindow)
					{
						if (tuniacApp.m_LogWindow->GetLogOn())
							tuniacApp.m_LogWindow->LogMessage(TEXT("ServicePluginManager"), szError);
					}
					MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONWARNING);
					FreeLibrary(hDLL);
					continue;
				}

				CreateTuniacServicePluginFunc pCTPF = (CreateTuniacServicePluginFunc)GetProcAddress(hDLL, "CreateTuniacServicePlugin");
				if (pCTPF)
				{
					ServicePluginEntry PE;
					PE.hDLL = hDLL;
					PE.pPlugin = pCTPF();
					if (PE.pPlugin == NULL)
					{
						FreeLibrary(hDLL);
						continue;
					}

					StringCchCopy(PE.szDllFile, 64, w32fd.cFileName);
					StringCchCopy(PE.szName, 64, PE.pPlugin->GetPluginName());
					PE.ulFlags = PE.pPlugin->GetFlags();

					PE.bEnabled = false;
					if (tuniacApp.m_Preferences.PluginGetValue(TEXT("plugins"), PE.szDllFile, &lpRegType, (LPBYTE)&bRegEnabled, &iRegSize)) // FIXME??
						PE.bEnabled = bRegEnabled == TRUE;

					if (!PE.bEnabled)
					{
						PE.pPlugin->Destroy();
						PE.pPlugin = NULL;
						FreeLibrary(PE.hDLL);
						PE.hDLL = NULL;
					}
					else
					{
						PE.pPlugin->SetHelper(this);
					}
					m_PluginArray.AddTail(PE);
				}
				else
				{
					FreeLibrary(hDLL);
				}
			}

		} while (FindNextFile(hFind, &w32fd));

		FindClose(hFind);
	}
	return true;
}

bool			CServicePluginManager::Shutdown(void)
{
	BOOL	bRegEnabled;

	while (m_PluginArray.GetCount())
	{
		bRegEnabled = m_PluginArray[0].bEnabled ? TRUE : FALSE;
		tuniacApp.m_Preferences.PluginSetValue(TEXT("plugins"), m_PluginArray[0].szDllFile, REG_DWORD, (LPBYTE)&bRegEnabled, sizeof(int));

		EnablePlugin(0, false);

		FreeLibrary(m_PluginArray[0].hDLL);
		m_PluginArray.RemoveAt(0);
	}

	return true;
}

unsigned int	CServicePluginManager::GetNumPlugins(void)
{
	return m_PluginArray.GetCount();
}

ServicePluginEntry* CServicePluginManager::GetPluginAtIndex(unsigned int iPlugin)
{
	if (iPlugin >= m_PluginArray.GetCount())
		return NULL;
	return &m_PluginArray[iPlugin];
}

bool			CServicePluginManager::IsPluginEnabled(unsigned int iPlugin)
{
	if (iPlugin >= m_PluginArray.GetCount())
		return false;
	return m_PluginArray[iPlugin].bEnabled;
}

bool			CServicePluginManager::EnablePlugin(unsigned int iPlugin, bool bEnabled)
{
	if (iPlugin >= m_PluginArray.GetCount())
		return false;
	if (m_PluginArray[iPlugin].bEnabled == bEnabled)
		return bEnabled;

	bool bOK = true;

	if (bEnabled)
	{
		TCHAR szDllFile[512];
		StringCchPrintf(szDllFile, MAX_PATH, TEXT("%s%s"), m_PluginPath, m_PluginArray[iPlugin].szDllFile);
		m_PluginArray[iPlugin].hDLL = LoadLibrary(szDllFile);

		if (m_PluginArray[iPlugin].hDLL == NULL)
			bOK = false;

		CreateTuniacServicePluginFunc pCTPF = (CreateTuniacServicePluginFunc)GetProcAddress(m_PluginArray[iPlugin].hDLL, "CreateTuniacServicePlugin");
		if (bOK && pCTPF)
		{
			ITuniacServicePlugin* pPlugin = pCTPF();

			if (pPlugin)
			{
				m_PluginArray[iPlugin].pPlugin = pPlugin;

				m_PluginArray[iPlugin].pPlugin->SetHelper(this);
				StringCchCopy(m_PluginArray[iPlugin].szName, 64, m_PluginArray[iPlugin].pPlugin->GetPluginName());
				m_PluginArray[iPlugin].ulFlags = m_PluginArray[iPlugin].pPlugin->GetFlags();
				m_PluginArray[iPlugin].bEnabled = true;
			}
			else
			{
				bOK = false;
			}
		}
		else
		{
			bOK = false;
		}

		if (!bOK)
		{
			if (m_PluginArray[iPlugin].hDLL != NULL)
			{
				FreeLibrary(m_PluginArray[iPlugin].hDLL);
				m_PluginArray[iPlugin].hDLL = NULL;
			}
			TCHAR szError[MAX_PATH + 20];
			StringCchPrintf(szError, MAX_PATH + 20, TEXT("Error reloading service plugin: %s"), m_PluginArray[iPlugin].szDllFile);
			if (tuniacApp.m_LogWindow)
			{
				if (tuniacApp.m_LogWindow->GetLogOn())
					tuniacApp.m_LogWindow->LogMessage(TEXT("ServicePluginManager"), szError);
			}
			MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONERROR);
			return false;
		}

	}
	else
	{
		if (m_PluginArray[iPlugin].pPlugin != NULL)
		{
			m_PluginArray[iPlugin].pPlugin->Destroy();
			m_PluginArray[iPlugin].pPlugin = NULL;
		}

		FreeLibrary(m_PluginArray[iPlugin].hDLL);
		m_PluginArray[iPlugin].hDLL = NULL;

		m_PluginArray[iPlugin].bEnabled = false;
	}

	return true;
}

void			CServicePluginManager::LogMessage(LPTSTR szModuleName, LPTSTR szMessage)
{
	if (tuniacApp.m_LogWindow)
	{
		if (tuniacApp.m_LogWindow->GetLogOn())
			tuniacApp.m_LogWindow->LogMessage(szModuleName, szMessage);
	}
}

bool			CServicePluginManager::PreferencesGet(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	TCHAR szPluginsSubKey[128];
	StringCchPrintf(szPluginsSubKey, 128, TEXT("plugins\\%s"), szSubKey);
	return tuniacApp.m_Preferences.PluginGetValue(szPluginsSubKey, lpValueName, lpType, lpData, lpcbData);
}

bool			CServicePluginManager::PreferencesSet(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
	TCHAR szPluginsSubKey[128];
	StringCchPrintf(szPluginsSubKey, 128, TEXT("plugins\\%s"), szSubKey);
	return tuniacApp.m_Preferences.PluginSetValue(szPluginsSubKey, lpValueName, dwType, lpData, cbData);
}
