#include "stdafx.h"
#include "mIRCSpammer.h"

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
	ITuniacPlugin * pPlugin = new CmIRCSpammer;

	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACPLUGIN_VERSION;
}


CmIRCSpammer::CmIRCSpammer(void)
{
}

CmIRCSpammer::~CmIRCSpammer(void)
{
}

void			CmIRCSpammer::Destroy(void)
{
	delete this;
}

LPTSTR			CmIRCSpammer::GetPluginName(void)
{
	return TEXT("mIRC Spammer");
}

unsigned long	CmIRCSpammer::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool			CmIRCSpammer::SetHelper(ITuniacPluginHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

HANDLE			CmIRCSpammer::CreateThread(LPDWORD lpThreadId)
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

HWND			CmIRCSpammer::GetPluginWindow(void)
{
	return NULL;
}

unsigned long	CmIRCSpammer::ThreadStub(void * in)
{
	CmIRCSpammer * pPlugin = (CmIRCSpammer *)in;
	return(pPlugin->ThreadProc());
}

unsigned long	CmIRCSpammer::ThreadProc(void)
{
	m_bThreadActive = true;

	MSG		msg;
	bool	Done = false;

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

						/* eh... this is probably incomplete.  I did not test it as
						far as using another application that communicates with mIRC at
						the same time. */
					
						HWND hwnd = FindWindow(L"mIRC", NULL);

						if (hwnd != NULL) {
							
							int iLen = wcsnlen_s(szSong, 512);

							HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE,
											NULL,
											PAGE_READWRITE,
											0,
											4096,
											L"mIRC9");

							if (GetLastError() == ERROR_ALREADY_EXISTS)
							{
								DWORD dSize = 0;
								
								if(hMap)
								{
									if (GetFileSize(hMap, &dSize) == NO_ERROR) {
										iLen = (int) dSize;
									}
								}
							}

							if (hMap != NULL) {
								char* szContents = (char*) MapViewOfFile(hMap,
														FILE_MAP_ALL_ACCESS,
														0,
														0,
														iLen);

								if(szContents)
								{

									// update the file map with the command
									WideCharToMultiByte(CP_ACP, 0, szSong, -1, szContents, 512, NULL, FALSE);

									// issue the configured mirc command to the current instance of mIRC.
									long lRes =  SendMessage(hwnd,  WM_MCOMMAND, 1, 9L);

									UnmapViewOfFile(szContents);
								}
								CloseHandle(hMap);
							}
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
	
	m_bThreadActive = false;
	return 0;
}

bool			CmIRCSpammer::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("mIRC Spammer Plugin for Tuniac.\r\nBy Blur and Brett Hoyle, 2005-2010.\r\n\r\nWill output the current song to mIRC"), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CmIRCSpammer::Configure(HWND hWndParent)
{
	return true;
}
