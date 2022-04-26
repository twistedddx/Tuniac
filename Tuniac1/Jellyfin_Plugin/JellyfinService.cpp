#include "stdafx.h"
#include "JellyfinService.h"
#include "resource.h"
#include "rapidjson/document.h"

using namespace rapidjson;

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ulReason,
	LPVOID lpReserved
)
{
	return TRUE;
}

extern "C" __declspec(dllexport) ITuniacServicePlugin * CreateTuniacServicePlugin(void)
{
	ITuniacServicePlugin* pPlugin = new CJellyfinService;
	return pPlugin;
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacPluginVersion(void)
{
	return ITUNIACSERVICEPLUGIN_VERSION;
}


CJellyfinService::CJellyfinService()
{

}

CJellyfinService::~CJellyfinService()
{

}

void			CJellyfinService::Destroy(void)
{
	delete this;
}

LPTSTR			CJellyfinService::GetPluginName(void)
{
	return TEXT("Jellyfin");
}

unsigned long	CJellyfinService::GetFlags(void)
{
	return SERVICEPLUGINFLAGS_ABOUT | SERVICEPLUGINFLAGS_CONFIG;
}

bool			CJellyfinService::SetHelper(ITuniacServicePluginHelper* pHelper)
{
	m_pHelper = pHelper;
	return true;
}

//pref dialog
LRESULT CALLBACK	CJellyfinService::DlgProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CJellyfinService* pPopupNotify = (CJellyfinService*)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pPopupNotify->DlgProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK	CJellyfinService::DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CJellyfinService* pPopupNotify = (CJellyfinService*)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
			pPopupNotify = (CJellyfinService*)lParam;
		}
		break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
				{
					EndDialog(hDlg, wParam);
					return TRUE;
				}
				break;
				case IDC_LOGIN:
					Login();
					break;
			}
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

bool			CJellyfinService::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Jellyfin service plugin for Tuniac.\r\nBy Harteex, 2022.\r\n\r\nThis plugin enables streaming music from a Jellyfin server."), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CJellyfinService::Configure(HWND hWndParent)
{
	DialogBoxParam(GetModuleHandle(L"Jellyfin_Plugin.dll"), MAKEINTRESOURCE(IDD_NOTIFYPREFWINDOW), hWndParent, (DLGPROC)DlgProcStub, (DWORD_PTR)this);

	return true;
}

void CJellyfinService::Login()
{
	/*std::string json = DoHttpRequest();
	Document document;
	document.Parse(json.c_str());

	if (document.HasParseError())
		return;*/

	// TODO
}

std::string CJellyfinService::DoHttpRequest(LPCWSTR host, bool https, LPCWSTR path)
{
	HINTERNET hSession = InternetOpen(L"Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hConnect = InternetConnect(hSession, host, https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, L"", L"", INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hHttpFile = HttpOpenRequest(hConnect, L"GET", path, NULL, NULL, NULL, INTERNET_FLAG_RELOAD | (https ? INTERNET_FLAG_SECURE : 0), 0);

	if (!HttpSendRequest(hHttpFile, NULL, 0, 0, 0))
	{
		InternetCloseHandle(hHttpFile);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		return "";
	}

	DWORD dwFileSize;
	dwFileSize = BUFSIZ;

	char* buffer;
	buffer = new char[dwFileSize + 1];

	std::string data = "";

	while (true) {
		DWORD dwBytesRead;
		BOOL bRead;

		bRead = InternetReadFile(
			hHttpFile,
			buffer,
			dwFileSize + 1,
			&dwBytesRead);

		if (dwBytesRead == 0)
			break;

		if (!bRead) {
			printf("InternetReadFile error : <%lu>\n", GetLastError());
		}
		else {
			buffer[dwBytesRead] = 0;
			data.append(buffer);
			printf("Retrieved %lu data bytes: %s\n", dwBytesRead, buffer);
		}
	}

	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);

	return data;
}
