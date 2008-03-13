#include "stdafx.h"
#include "NowPlayingTxt.h"

//not defined in vista?
#define WM_MCOMMAND   WM_USER + 200

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ulReason, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) ITuniacPlugin * CreateTuniacPlugin(void)
{
	ITuniacPlugin * pPlugin = new CNowPlayingTxt;

	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACPLUGIN_VERSION;
}


CNowPlayingTxt::CNowPlayingTxt(void)
{
}

CNowPlayingTxt::~CNowPlayingTxt(void)
{
}

void			CNowPlayingTxt::Destroy(void)
{
	delete this;
}

LPTSTR			CNowPlayingTxt::GetPluginName(void)
{
	return TEXT("Now Playing .Txt");
}

unsigned long	CNowPlayingTxt::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool			CNowPlayingTxt::SetHelper(ITuniacPluginHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

HANDLE			CNowPlayingTxt::CreateThread(LPDWORD lpThreadId)
{
	m_hThread = ::CreateThread(	NULL,
										16384,
										this->ThreadStub,
										this,
										0,
										&m_dwThreadId
									);

	if(m_hThread == NULL)
		return NULL;

	*lpThreadId = m_dwThreadId;
	return m_hThread;
}

HWND			CNowPlayingTxt::GetMainWindow(void)
{
	return NULL;
}

unsigned long	CNowPlayingTxt::ThreadStub(void * in)
{
	CNowPlayingTxt * pPlugin = (CNowPlayingTxt *)in;
	return(pPlugin->ThreadProc());
}

unsigned long	CNowPlayingTxt::ThreadProc(void)
{
	m_bThreadActive = true;

	MSG		msg;
	bool	Done = false;

	m_aCopy = GlobalAddAtom(TEXT("TUNIACNOWPLAYING_COPY"));
	RegisterHotKey(NULL, m_aCopy, MOD_WIN, VK_ADD);

	while(!Done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{

				case PLUGINNOTIFY_SONGCHANGE:
				case PLUGINNOTIFY_SONGCHANGE_MANUAL:
				case PLUGINNOTIFY_SONGINFOCHANGE:
					{
						TCHAR szSongW[512];
						m_pHelper->GetTrackInfo(szSongW, 512, NULL, 0); //get current song, using default format
						char szSong[512];
						WideCharToMultiByte(CP_ACP, 0, szSongW, -1, szSong, 512, NULL, FALSE);

						TCHAR szURL[512];
						GetModuleFileName(NULL, szURL, 512);
						PathRemoveFileSpec(szURL);
						PathAddBackslash(szURL);
						StrCat(szURL, TEXT("NowPlaying.txt"));

						HANDLE hOutFile = CreateFile(	szURL,
														GENERIC_WRITE, 
														0,
														NULL,
														CREATE_ALWAYS,
														FILE_ATTRIBUTE_NORMAL,
														NULL);

						if(hOutFile == INVALID_HANDLE_VALUE)
							break;
						unsigned long ulBytesWritten;
						WriteFile(hOutFile, &szSong, (strlen(szSong)) * sizeof(char), &ulBytesWritten, NULL);
						CloseHandle(hOutFile);

						//we need a real setting for this ;)
						bool bMirc = 0;

						if (bMirc) {
							/* eh... this is probably incomplete.  I did not test it as
							far as using another application that communicates with mIRC at
							the same time. */
						
							HWND hwnd = FindWindow(L"mIRC", NULL);

							if (hwnd != NULL) {
								
								int iLen = strlen(szSong);

								HANDLE hMap = CreateFileMapping(NULL,
												NULL,
												PAGE_READWRITE,
												0,
												iLen,
												L"mIRC");

								if (GetLastError() == ERROR_ALREADY_EXISTS) {
									DWORD dSize = 0;

									if (GetFileSize(hMap, &dSize) == NO_ERROR) {
										iLen = (int) dSize;
									}
								}

								if (hMap != NULL) {
									char* szContents = (char*) MapViewOfFile(hMap,
															FILE_MAP_ALL_ACCESS,
															0,
															0,
															iLen);

									// update the file map with the command
									strcpy(szContents, szSong);

									// issue the configured mirc command to the current instance of mIRC.
									long lRes =  SendMessage(hwnd,  WM_MCOMMAND, 1, 0L);

									UnmapViewOfFile(szContents);
									CloseHandle(hMap);
								}
							}
						}
						

					}
					break;

				case WM_HOTKEY:
					{
						if(msg.wParam == m_aCopy)
						{
							if (!OpenClipboard(m_pHelper->GetMainWindow())) 
								break; 
							EmptyClipboard(); 
							
							TCHAR szData[512];
							m_pHelper->GetTrackInfo(szData, 512, TEXT("@T - @A"), 0);
							if(wcslen(szData) == 0)
								break;

							HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (wcslen(szData) + 1) * sizeof(TCHAR)); 
							if (hglbCopy == NULL) 
							{ 
								CloseClipboard(); 
								break;
							}
							
							LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
							memcpy(lptstrCopy, szData, (wcslen(szData) + 1) * sizeof(TCHAR)); 
							GlobalUnlock(hglbCopy); 
  
							SetClipboardData(CF_UNICODETEXT, hglbCopy); 
							CloseClipboard();
							GlobalFree(hglbCopy);
						}
					}
					break;

				case WM_QUIT:
					{
						Done = true;
					}
					break;


			}
		}
		else
		{
			Sleep(5);
		}
	}
	
	UnregisterHotKey(NULL, m_aCopy);
	GlobalDeleteAtom(m_aCopy);

	m_bThreadActive = false;
	return 0;
}

bool			CNowPlayingTxt::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("NowPlaying .Txt Plugin for Tuniac.\r\nBy Blur, 2005-2008.\r\n\r\nWill output the current song to <Tuniac-dir>\\NowPlaying.txt"), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CNowPlayingTxt::Configure(HWND hWndParent)
{
	return false;
}
