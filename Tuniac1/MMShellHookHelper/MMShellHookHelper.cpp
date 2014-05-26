// MMShellHookHelper.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MMShellHookHelper.h"

#include "ituniacplugin.h"

#define MAX_LOADSTRING 100
#define szClassName			TEXT("TUNIACHELPER")

#define TUNIACAPP_CHECK_TIMER	0x500

HINSTANCE			hInst;
HWND				hWnd;
HANDLE				m_hOneInstanceOnlyMutex;
HACCEL				hAccel;
DWORD				dwThreadId;
HANDLE				hThread;
ITuniacPlugin *		pPlugin;


LRESULT CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

bool				Initialize(HINSTANCE hInstance, LPTSTR szCommandLine);
bool				Run(void);
bool				Shutdown(void);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if(!Initialize(hInstance, GetCommandLine()))
		return(0);

	Run();

	Shutdown();

	return 0;

}

bool Initialize(HINSTANCE hInstance, LPTSTR szCommandLine)
{
	hInst = NULL;
	hWnd = NULL;
	dwThreadId = NULL;
	hThread = NULL;
	pPlugin = NULL;
	hAccel = NULL;

	m_hOneInstanceOnlyMutex = CreateMutex(NULL, FALSE, szClassName);
	if(GetLastError() == ERROR_ALREADY_EXISTS) 
		return false;

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MMSHELLHOOKHELPER));

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MMSHELLHOOKHELPER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= szClassName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MMSHELLHOOKHELPER);
	if(!RegisterClassEx(&wcex))
		return false;

	hInst = hInstance;

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
				szClassName, 
				TEXT("TuniacHelper"), 
				WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				NULL,
				NULL, 
				hInstance,
				NULL);

	if (!hWnd)
		return false;

	HMODULE hDLL = LoadLibrary(L"MMShellHookHelper.dll");
	if(hDLL)
	{
		GetTuniacPluginVersionFunc pGTPVF = (GetTuniacPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacPluginVersion");
		if(pGTPVF == NULL)
		{
			FreeLibrary(hDLL);
			return false;
		}

		if(pGTPVF() != ITUNIACPLUGIN_VERSION)
		{
			MessageBox(hWnd, TEXT("Incompatable plugin found: \\plugins\\MMShellHookHelper.dll\n\nThis Plugin must be updated before you can use it."), TEXT("Error"), MB_OK | MB_ICONWARNING);
			FreeLibrary(hDLL);
			return false;
		}

		CreateTuniacPluginFunc pCTPF = (CreateTuniacPluginFunc)GetProcAddress(hDLL, "CreateTuniacPlugin");
		if(pCTPF)
		{
			pPlugin = pCTPF();
			if(pPlugin == NULL)
			{
				FreeLibrary(hDLL);
				return false;
			}

			bool bEnabled = false;

			HKEY hTuniacPrefKey;
			DWORD lpRegType = REG_DWORD;
			DWORD iRegSize = sizeof(int);
			BOOL bRegEnabled = false;

			if(RegOpenKeyEx(	HKEY_CURRENT_USER,
								L"Software\\Tuniac\\plugins",
								0,
								KEY_QUERY_VALUE,
								&hTuniacPrefKey) == ERROR_SUCCESS)
			{
				RegQueryValueEx(	hTuniacPrefKey,
									L"MMShellHook_Plugin.dll",
									NULL,
									&lpRegType,
									(LPBYTE)&bRegEnabled,
									&iRegSize);

				RegCloseKey(hTuniacPrefKey);
				bEnabled = bRegEnabled == TRUE;
			}

			if(!bEnabled)
			{
				pPlugin->Destroy();
				pPlugin = NULL;
				FreeLibrary(hDLL);
				return false;
			}
			else
			{ 
				hThread = pPlugin->CreateThread(&dwThreadId);
				SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
			}
		}
		else
		{
			FreeLibrary(hDLL);
			return false;
		}
	}

	SetTimer(hWnd, TUNIACAPP_CHECK_TIMER, 5000, NULL);

	return true;
}

bool Run(void)
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return true;
}

bool Shutdown(void)
{
	if(dwThreadId)
		::PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);

	if(WaitForSingleObject(hThread, 10000) == WAIT_TIMEOUT)
		TerminateThread(hThread, 0);

	if(hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}

	if(pPlugin != NULL)
	{
		pPlugin->Destroy();
		pPlugin = NULL;
	}

	KillTimer(hWnd, TUNIACAPP_CHECK_TIMER);

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_COMMAND:
			{
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
				switch (wmId)
				{
					case IDM_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
						break;
					case IDM_EXIT:
						DestroyWindow(hWnd);
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
				break;
			}

		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_TIMER:
			{
				if(wParam == TUNIACAPP_CHECK_TIMER)
				{
					if(!FindWindow(TEXT("TUNIACWINDOWCLASS"), NULL))
						PostQuitMessage(0);
				}
			}
			break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

		case WM_CLOSE:
			{
				DestroyWindow(hWnd);
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}