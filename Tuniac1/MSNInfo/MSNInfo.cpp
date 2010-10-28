#include "stdafx.h"
#include "MSNInfo.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ulReason, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) ITuniacPlugin * CreateTuniacPlugin(void)
{
	ITuniacPlugin * pPlugin = new CMSNInfo;
	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACPLUGIN_VERSION;
}


CMSNInfo::CMSNInfo(void)
{
}

CMSNInfo::~CMSNInfo(void)
{
}

void			CMSNInfo::Destroy(void)
{
	delete this;
}

LPTSTR			CMSNInfo::GetPluginName(void)
{
	return TEXT("MSN v7+ Info");
}

unsigned long	CMSNInfo::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool			CMSNInfo::SetHelper(ITuniacPluginHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

HANDLE			CMSNInfo::CreateThread(LPDWORD lpThreadId)
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

HWND			CMSNInfo::GetMainWindow(void)
{
	return NULL;
}

unsigned long	CMSNInfo::ThreadStub(void * in)
{
	CMSNInfo * pPlugin = (CMSNInfo *)in;
	return(pPlugin->ThreadProc());
}

unsigned long	CMSNInfo::ThreadProc(void)
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
						BOOL bShow = TRUE;
						WCHAR buffer[512];

						LPTSTR szArtist = (LPTSTR)m_pHelper->GetVariable(Variable_Artist);
						LPTSTR szTitle = (LPTSTR)m_pHelper->GetVariable(Variable_SongTitle);

						wnsprintf(buffer, 512, TEXT("\\0Music\\0%d\\0%s\\0%s\\0%s\\0%s\\0"), bShow, TEXT("{0} - {1}"), szArtist, szTitle, TEXT("WMContentID"));

						COPYDATASTRUCT msndata;
						msndata.dwData = 0x547;
						msndata.lpData = &buffer;
						msndata.cbData = (wcslen(buffer) + 1) * sizeof(TCHAR);

						HWND msnui = NULL;
						while (msnui = FindWindowEx(NULL, msnui, TEXT("MsnMsgrUIManager"), NULL))
						{
							SendMessage(msnui, WM_COPYDATA, NULL, (LPARAM)&msndata);
						}
					}
					break;

				case WM_QUIT:
					{
						TCHAR buffer[512];
						BOOL bShow = FALSE;

						wnsprintf(buffer, 512, TEXT("\\0Music\\0%d\\0%s\\0"), bShow, TEXT("WMContentID"));

						COPYDATASTRUCT msndata;
						msndata.dwData = 0x547;
						msndata.lpData = &buffer;
						msndata.cbData = (wcslen(buffer) + 1) * sizeof(TCHAR);

						HWND msnui = NULL;
						while (msnui = FindWindowEx(NULL, msnui, TEXT("MsnMsgrUIManager"), NULL))
						{
							SendMessage(msnui, WM_COPYDATA, NULL, (LPARAM)&msndata);
						}
						Done = true;
					}
					break;

			}
		} else {
			Sleep(5);
		}
	}
	m_bThreadActive = false;
	return 0;
}

bool			CMSNInfo::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("MSN Info Plugin for Tuniac.\r\nBy Bits & Blur, 2005-2008.\r\n\r\nWill update your MSN Messenger (v7 and above) info with the current song."), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMSNInfo::Configure(HWND hWndParent)
{
	return false;
}
