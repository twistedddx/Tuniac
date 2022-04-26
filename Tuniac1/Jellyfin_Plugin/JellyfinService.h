#pragma once

#include "ITuniacServicePlugin.h"
#include <Shlwapi.h>
#include <WinInet.h>
#include <string>

class CJellyfinService :
	public ITuniacServicePlugin
{

protected:

	ITuniacServicePluginHelper* m_pHelper;

	static LRESULT CALLBACK	DlgProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK		DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CJellyfinService();
	~CJellyfinService();

	void				Destroy(void);

	LPTSTR				GetPluginName(void);
	unsigned long		GetFlags(void);

	bool				SetHelper(ITuniacServicePluginHelper* pHelper);

	bool				About(HWND hWndParent);
	bool				Configure(HWND hWndParent);

private:
	void				Login(void);
	std::string			DoHttpRequest(LPCWSTR host, bool https, LPCWSTR path);
};
