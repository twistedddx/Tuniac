#pragma once

#include <singleton.h>

class CTuniacApp : public CSingleton<CTuniacApp>
{
protected:
	WNDCLASSEX		m_WCEX;
	HWND			m_hWnd;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK		WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CTuniacApp(void);
	~CTuniacApp(void);

	bool Initialize(HINSTANCE hInst, LPTSTR lpCmdLine);
	bool Shutdown(void);

	bool Run(void);
};
