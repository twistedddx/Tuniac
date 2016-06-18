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

HWND			CNowPlayingTxt::GetPluginWindow(void)
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
	RegisterHotKey(NULL, m_aCopy, MOD_WIN, VK_MULTIPLY);
	while(!Done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(msg.message)
			{
				case PLUGINNOTIFY_SONGCHANGE_INIT:
				case PLUGINNOTIFY_SONGCHANGE_MANUALBLIND:
				case PLUGINNOTIFY_SONGCHANGE_MANUAL:
				case PLUGINNOTIFY_SONGCHANGE:
				case PLUGINNOTIFY_SONGINFOCHANGE:
					{
						TCHAR szSong[512];
						m_pHelper->GetTrackInfo(szSong, 512, NULL, 0); //get current song, using default format

						TCHAR szURL[512];
						if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, szURL ) ) )
						{
							PathAppend( szURL, TEXT("\\NowPlaying.txt") );
						}
						else{
							//cant get appdata path
							break;
						}

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
						WriteFile(hOutFile, &szSong, (wcsnlen_s(szSong, 512)) * sizeof(TCHAR), &ulBytesWritten, NULL);
						CloseHandle(hOutFile);
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
							if(wcsnlen_s(szData, 512) == 0)
								break;

							HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, wcsnlen_s(szData, 512) * sizeof(TCHAR)); 
							if (hglbCopy == NULL) 
							{ 
								CloseClipboard(); 
								break;
							}
							
							LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
							if(lptstrCopy)
								memcpy_s(lptstrCopy, 512, szData, wcsnlen_s(szData, 512) * sizeof(TCHAR)); 
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
	MessageBox(hWndParent, TEXT("NowPlaying .Txt Plugin for Tuniac.\r\nBy Blur and Brett H, 2005-2010.\r\n\r\nWill output the current song to <My Documents>\\NowPlaying.txt"), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CNowPlayingTxt::Configure(HWND hWndParent)
{
	return true;
}
