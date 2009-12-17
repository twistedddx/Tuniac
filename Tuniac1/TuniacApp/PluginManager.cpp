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

#include "stdafx.h"
#include "PluginManager.h"

CPluginManager::CPluginManager(void)
{
}

CPluginManager::~CPluginManager(void)
{
}

bool			CPluginManager::Initialize(void)
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szFilename[512];

	DWORD				lpRegType = REG_DWORD;
	DWORD				iRegSize = sizeof(int);
	BOOL				bRegEnabled;

	GetModuleFileName(NULL, m_PluginPath, 512);
	PathRemoveFileSpec(m_PluginPath);
	PathAddBackslash(m_PluginPath);
	StrCat(m_PluginPath, TEXT("plugins"));
	PathAddBackslash(m_PluginPath);
	StrCpy(szFilename, m_PluginPath);
	StrCat(szFilename, TEXT("*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR szDll[512];

			StrCpy(szDll, m_PluginPath);
			StrCat(szDll, w32fd.cFileName);

			HMODULE hDLL = LoadLibrary(szDll);
			if(hDLL)
			{
				GetTuniacPluginVersionFunc pGTPVF = (GetTuniacPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacPluginVersion");
				if(pGTPVF == NULL)
				{
					FreeLibrary(hDLL);
					continue;
				}
				if(pGTPVF() != ITUNIACPLUGIN_VERSION)
				{
					TCHAR szError[512];
					wnsprintf(szError, 512, TEXT("Incompatable plugin found: \\plugins\\%s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
					MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONWARNING);
					FreeLibrary(hDLL);
					continue;
				}

				CreateTuniacPluginFunc pCTPF = (CreateTuniacPluginFunc)GetProcAddress(hDLL, "CreateTuniacPlugin");
				if(pCTPF)
				{
					PluginEntry PE;
					PE.hDLL = hDLL;
					PE.pPlugin = pCTPF();
					if(PE.pPlugin == NULL)
					{
						FreeLibrary(hDLL);
						continue;
					}

					wnsprintf(PE.szDllFile, 64, TEXT("%s"), w32fd.cFileName);
					wnsprintf(PE.szName, 64, TEXT("%s"), PE.pPlugin->GetPluginName());
					PE.ulFlags = PE.pPlugin->GetFlags();

					PE.bEnabled = false;
					if(tuniacApp.m_Preferences.PluginGetValue(TEXT("plugins"), PE.szDllFile, &lpRegType, (LPBYTE)&bRegEnabled, &iRegSize))
						PE.bEnabled = bRegEnabled == TRUE;

					if(!PE.bEnabled)
					{
						PE.pPlugin->Destroy();
						PE.pPlugin = NULL;
						FreeLibrary(PE.hDLL);
						PE.hDLL = NULL;
					}
					else
					{ 
						PE.hThread = PE.pPlugin->CreateThread(&PE.dwThreadId);
						SetThreadPriority(PE.hThread, THREAD_PRIORITY_LOWEST);
						PE.pPlugin->SetHelper(this);
					
					}
					m_PluginArray.AddTail(PE);
				}
				else
				{
					FreeLibrary(hDLL);
				}
			}

		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}
	return true;
}

bool			CPluginManager::Shutdown(void)
{
	BOOL	bRegEnabled;

	while(m_PluginArray.GetCount())
	{
		bRegEnabled = m_PluginArray[0].bEnabled ? TRUE : FALSE;
		tuniacApp.m_Preferences.PluginSetValue(TEXT("plugins"), m_PluginArray[0].szDllFile, REG_DWORD, (LPBYTE)&bRegEnabled, sizeof(int));

		EnablePlugin(0, false);

		FreeLibrary(m_PluginArray[0].hDLL);
		m_PluginArray.RemoveAt(0);
	}

	return true;
}

unsigned int	CPluginManager::GetNumPlugins(void)
{
	return m_PluginArray.GetCount();
}

PluginEntry *	CPluginManager::GetPluginAtIndex(unsigned int iPlugin)
{
	if(iPlugin >= m_PluginArray.GetCount())
		return NULL;
	return &m_PluginArray[iPlugin];
}

bool			CPluginManager::IsPluginEnabled(unsigned int iPlugin)
{
	if(iPlugin >= m_PluginArray.GetCount())
		return false;
	return m_PluginArray[iPlugin].bEnabled;
}

bool			CPluginManager::EnablePlugin(unsigned int iPlugin, bool bEnabled)
{
	if(iPlugin >= m_PluginArray.GetCount())
		return false;
	if(m_PluginArray[iPlugin].bEnabled == bEnabled)
		return bEnabled;

	bool bOK = true;

	if(bEnabled)
	{
		TCHAR szDllFile[512];
		wnsprintf(szDllFile, 512, TEXT("%s%s"), m_PluginPath, m_PluginArray[iPlugin].szDllFile);
		m_PluginArray[iPlugin].hDLL = LoadLibrary(szDllFile);

		if(m_PluginArray[iPlugin].hDLL == NULL)
			bOK = false;

		CreateTuniacPluginFunc pCTPF = (CreateTuniacPluginFunc)GetProcAddress(m_PluginArray[iPlugin].hDLL, "CreateTuniacPlugin");
		if(bOK && pCTPF)
		{
			ITuniacPlugin * pPlugin = pCTPF();

			if(pPlugin)
			{
				m_PluginArray[iPlugin].pPlugin = pPlugin;
				m_PluginArray[iPlugin].hThread = m_PluginArray[iPlugin].pPlugin->CreateThread(&m_PluginArray[iPlugin].dwThreadId);

				if(m_PluginArray[iPlugin].hThread == NULL)
				{
					m_PluginArray[iPlugin].pPlugin->Destroy();
					m_PluginArray[iPlugin].pPlugin = NULL;
					bOK = false;
				}
				else
				{
					SetThreadPriority(m_PluginArray[iPlugin].hThread, THREAD_PRIORITY_LOWEST);
					m_PluginArray[iPlugin].pPlugin->SetHelper(this);
					wnsprintf(m_PluginArray[iPlugin].szName, 64, TEXT("%s"), m_PluginArray[iPlugin].pPlugin->GetPluginName());
					m_PluginArray[iPlugin].ulFlags = m_PluginArray[iPlugin].pPlugin->GetFlags();
					m_PluginArray[iPlugin].bEnabled = true;
				}
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

		if(!bOK)
		{
			if(m_PluginArray[iPlugin].hDLL != NULL)
			{
				FreeLibrary(m_PluginArray[iPlugin].hDLL);
				m_PluginArray[iPlugin].hDLL = NULL;
			}
			TCHAR szError[512];
			wnsprintf(szError, 512, TEXT("Error reloading plugin: %s"), m_PluginArray[iPlugin].szDllFile);
			MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONERROR);
			return false;
		}

	}
	else
	{
		int iCount = 100;
		HWND hPluginWnd;
		if( m_PluginArray[iPlugin].pPlugin != NULL)
			hPluginWnd = m_PluginArray[iPlugin].pPlugin->GetMainWindow();

		if(hPluginWnd)
		{
			while((hPluginWnd = m_PluginArray[iPlugin].pPlugin->GetMainWindow()) != NULL && iCount--)
			{
				::PostMessage(hPluginWnd, WM_QUIT, 0, 0);
				Sleep(5);
			}
			if((hPluginWnd = m_PluginArray[iPlugin].pPlugin->GetMainWindow()) != NULL)
				DestroyWindow(hPluginWnd);
		}
		else
		{
			while(::PostThreadMessage(m_PluginArray[iPlugin].dwThreadId, WM_QUIT, 0, 0) && iCount--)
				Sleep(5);
		}

		if(WaitForSingleObject(m_PluginArray[iPlugin].hThread, 10000) == WAIT_TIMEOUT)
			TerminateThread(m_PluginArray[iPlugin].hThread, 0);

		CloseHandle(m_PluginArray[iPlugin].hThread);
		m_PluginArray[iPlugin].hThread = NULL;

		if(m_PluginArray[iPlugin].pPlugin != NULL)
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

void			CPluginManager::PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].bEnabled == false || m_PluginArray[i].pPlugin == NULL)
			continue;

		HWND hPluginWnd = m_PluginArray[i].pPlugin->GetMainWindow();
		if(hPluginWnd)
			::PostMessage(hPluginWnd, Msg, wParam, lParam);
		else
			PostThreadMessage(m_PluginArray[i].dwThreadId, Msg, wParam, lParam);
	}
}

void *			CPluginManager::GetVariable(Variable eVar)
{

	switch(eVar)
	{
		case Variable_NumChannels:
			{
				return (void*)(CCoreAudio::Instance()->GetChannels());
			}
			break;

		case Variable_SampleRate:
			{
				return (void*)(CCoreAudio::Instance()->GetSampleRate());
			}
			break;

		case Variable_PositionMS:
			{
				return (void*)(CCoreAudio::Instance()->GetPosition());
			}
			break;

		case Variable_LengthMS:
			{
				return (void*)(CCoreAudio::Instance()->GetLength());
			}
			break;

		case Variable_SongTitle:
			{
				IPlaylist * pList = tuniacApp.m_PlaylistManager.GetActivePlaylist();
				if(pList)
				{
					IPlaylistEntry * pEntry = pList->GetActiveItem();
					
					if(pEntry)
						return (void *)pEntry->GetField(FIELD_TITLE);
				}
			}
			break;

		case Variable_Artist:
			{
				IPlaylist * pList = tuniacApp.m_PlaylistManager.GetActivePlaylist();
				if(pList)
				{
					IPlaylistEntry * pEntry = pList->GetActiveItem();
					
					if(pEntry)
						return (void *)pEntry->GetField(FIELD_ARTIST);
				}
			}
			break;
	}
	return NULL;
}

void		CPluginManager::GetTrackInfo(LPTSTR szDest, unsigned int iDestSize, LPTSTR szFormat, unsigned int iFromCurrent)
{ // szFormat=NULL to use full format from preferences

	IPlaylistEntry * pEntry = NULL;

	memset(szDest, L'\0', iDestSize);
	if(iFromCurrent == 0)
	{
		IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();
		IPlaylistEX * pPlaylistEX = (IPlaylistEX *)pPlaylist;
		pEntry = pPlaylist->GetActiveItem();
	}
	else
		pEntry = tuniacApp.GetFuturePlaylistEntry(iFromCurrent - 1);

	if(pEntry == NULL)
		return;

	tuniacApp.FormatSongInfo(szDest, iDestSize, pEntry, szFormat == NULL ? tuniacApp.m_Preferences.GetPluginFormatString() : szFormat, true);
}

bool			CPluginManager::Navigate(int iFromCurrent)
{

	if(iFromCurrent < 0)
	{
		if(!tuniacApp.m_History.PlayHistoryItem(0 - iFromCurrent))
			return false;
	}
	else if(iFromCurrent > 0)
	{
		IPlaylistEntry * pEntry = tuniacApp.GetFuturePlaylistEntry(iFromCurrent - 1);
		if(pEntry == NULL)
			return false;

		tuniacApp.m_SoftPause.bNow = false;
		//open for art before opening for decode.
		tuniacApp.SetArt(pEntry);
		unsigned long ulState = CCoreAudio::Instance()->GetState();
		if(!CCoreAudio::Instance()->SetSource(pEntry))
			return false;

		if(ulState == STATE_PLAYING)
		{
			CCoreAudio::Instance()->Play();
			PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUAL, NULL, NULL);
		}
		
		tuniacApp.m_PlaylistManager.SetActiveByEntry(pEntry);
	}
	else
	{
		CCoreAudio::Instance()->SetPosition(0);
		PostMessage(PLUGINNOTIFY_SEEK_MANUAL, NULL, NULL);

	}
	return true;
}
HINSTANCE		CPluginManager::GetMainInstance(void)
{
	return tuniacApp.getMainInstance();
}

HWND			CPluginManager::GetMainWindow(void)
{
	return tuniacApp.getMainWindow();
}

LPTSTR			CPluginManager::GetCurrentPluginDll(void)
{
	DWORD dwCurrentThreadId = GetCurrentThreadId();
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].dwThreadId == dwCurrentThreadId)
			return m_PluginArray[i].szDllFile;
	}
	return NULL;
}

bool			CPluginManager::PreferencesGet(LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LPTSTR szCurrent = GetCurrentPluginDll();
	if(szCurrent == NULL)
		return false;

	TCHAR szSubKey[128];
	wnsprintf(szSubKey, 128, TEXT("plugins\\%s"), szCurrent);
	return tuniacApp.m_Preferences.PluginGetValue(szSubKey, lpValueName, lpType, lpData, lpcbData);
}

bool			CPluginManager::PreferencesSet(LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
	if(lpValueName == NULL || wcslen(lpValueName) == 0)
		return false;

	LPTSTR szCurrent = GetCurrentPluginDll();
	if(szCurrent == NULL)
		return false;

	TCHAR szSubKey[128];
	wnsprintf(szSubKey, 128, TEXT("plugins\\%s"), szCurrent);
	return tuniacApp.m_Preferences.PluginSetValue(szSubKey, lpValueName, dwType, lpData, cbData);
}
