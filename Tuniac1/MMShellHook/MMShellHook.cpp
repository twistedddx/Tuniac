#include "stdafx.h"
#include "MMShellHook.h"


#pragma data_seg(".shared")
HWND hNotifyWnd = NULL;
HHOOK hShellHook = NULL;
HHOOK hKeyboardHook = NULL;
#pragma data_seg( )

HINSTANCE hInstance = NULL;

LRESULT CALLBACK ShellProc (int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc (int nCode, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		hInstance = (HINSTANCE)hInst;
		return TRUE;

	case DLL_PROCESS_DETACH:
		return TRUE;

	default:
		return TRUE;
	}
}

DllExport BOOL SetMMShellHook(HWND hWnd)
{
	if (hWnd == NULL)
		return FALSE;
	
	if (hNotifyWnd != NULL)
		return FALSE;

	hShellHook = SetWindowsHookEx(
					WH_SHELL,
					(HOOKPROC) ShellProc,
					hInstance,
					0L
					);

	hKeyboardHook = SetWindowsHookEx(
					WH_KEYBOARD,
					(HOOKPROC) KeyboardProc,
					hInstance,
					0L
					);

	if (hShellHook != NULL)
	{
		hNotifyWnd = hWnd;
		return TRUE;
	}

	return FALSE;
}

DllExport BOOL UnSetMMShellHook(HWND hWnd)
{

	BOOL unHooked = TRUE;
	
	if (hWnd == NULL)
		return false;

	if (hWnd != hNotifyWnd)
		return FALSE;

	if (hNotifyWnd != NULL)
	{
		unHooked = UnhookWindowsHookEx(hShellHook);
		hShellHook = NULL;
		UnhookWindowsHookEx(hKeyboardHook);
		hKeyboardHook = NULL;
	}

	if (unHooked)
	{
		hNotifyWnd = NULL;
	}

	return unHooked;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (hNotifyWnd != NULL && hNotifyWnd != (HWND)lParam)
		{
			switch (wParam)
			{
				case VK_MEDIA_NEXT_TRACK:
				case VK_MEDIA_PREV_TRACK:
				case VK_MEDIA_PLAY_PAUSE:
				case VK_MEDIA_STOP:
				/* these are handled in the OS almost always, so lets not double up
				case VK_VOLUME_UP:
				case VK_VOLUME_DOWN:
				case VK_VOLUME_MUTE:
				*/
					::PostMessage(hNotifyWnd,WM_KEYDOWN,wParam,(LPARAM)hNotifyWnd);
					return 1;
			}
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HSHELL_APPCOMMAND)
	{
		if (hNotifyWnd != NULL && hNotifyWnd != (HWND)wParam)
		{
			short AppCommand = GET_APPCOMMAND_LPARAM(lParam);
			switch (AppCommand)
			{
				case APPCOMMAND_MEDIA_PLAY:
				case APPCOMMAND_MEDIA_PAUSE:
				case APPCOMMAND_MEDIA_PLAY_PAUSE:
				case APPCOMMAND_MEDIA_STOP:
				case APPCOMMAND_MEDIA_NEXTTRACK:
				case APPCOMMAND_MEDIA_PREVIOUSTRACK:
				case APPCOMMAND_MEDIA_FAST_FORWARD:
				case APPCOMMAND_MEDIA_REWIND:
				/* these are handled in the OS almost always, so lets not double up
				case APPCOMMAND_VOLUME_DOWN:
				case APPCOMMAND_VOLUME_UP:
				case APPCOMMAND_VOLUME_MUTE:
				*/
					::PostMessage(hNotifyWnd,WM_APPCOMMAND,(WPARAM)hNotifyWnd,lParam);
					return 1;

			}
		}
	}

	return CallNextHookEx (hShellHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) ITuniacPlugin * CreateTuniacPlugin(void)
{
	ITuniacPlugin * pPlugin = new CMMShellHook;

	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACPLUGIN_VERSION;
}


CMMShellHook::CMMShellHook(void)
{
}

CMMShellHook::~CMMShellHook(void)
{
}

void			CMMShellHook::Destroy(void)
{
	delete this;
}

LPTSTR			CMMShellHook::GetPluginName(void)
{
	return TEXT("MMShellHook");
}

unsigned long	CMMShellHook::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool			CMMShellHook::SetHelper(ITuniacPluginHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

HANDLE			CMMShellHook::CreateThread(LPDWORD lpThreadId)
{
	m_hThread = ::CreateThread(	NULL,
										16384,
										this->ThreadStub,
										this,
										0,
										lpThreadId
									);

	if(m_hThread == NULL)
		return NULL;

	return m_hThread;
}

HWND			CMMShellHook::GetPluginWindow(void)
{
	return NULL;
}

unsigned long	CMMShellHook::ThreadStub(void * in)
{
	CMMShellHook * pPlugin = (CMMShellHook *)in;
	return(pPlugin->ThreadProc());
}

unsigned long	CMMShellHook::ThreadProc(void)
{
	HWND hTuniacWnd = FindWindow(L"TUNIACWINDOWCLASS", NULL);
	if(!SetMMShellHook(hTuniacWnd))
		return 0;

	BOOL bIsWow64 = false;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);

    SYSTEM_INFO systemInfo;
    ZeroMemory(&systemInfo, sizeof(systemInfo));
    GetNativeSystemInfo(&systemInfo);

	BOOL bIsReal64 = false;
	if(systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		bIsReal64 = true;


	if(bIsWow64 || bIsReal64)
	{
		TCHAR szHelperPath[512];
		GetModuleFileName(NULL, szHelperPath, 512);
		PathRemoveFileSpec(szHelperPath);
		PathAddBackslash(szHelperPath);
		StrCat(szHelperPath, TEXT("plugins\\MMShellHookHelper.exe"));

		ShellExecute(NULL, L"open", szHelperPath, NULL, NULL, SW_HIDE);
	}

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnSetMMShellHook(hTuniacWnd);

	if(bIsWow64)
	{
		HWND hTuniacHelperWnd = FindWindow(L"TUNIACHELPER", NULL);
		if(hTuniacHelperWnd)
			PostMessage(hTuniacHelperWnd, WM_CLOSE, 0, 0);
	}

	return 0;
}

bool			CMMShellHook::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("MMShellHook for Tuniac.\r\nBrett H, 2013\r\n\r\nAttempt to hook multimedia keys where possible"), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CMMShellHook::Configure(HWND hWndParent)
{
	return true;
}
