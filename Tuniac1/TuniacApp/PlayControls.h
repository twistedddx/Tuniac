#pragma once

class CPlayControls
{
protected:

	HWND						m_hWnd;

	HMENU						m_hFuture;

	HWND						m_hPrevWnd;
	HWND						m_hPlayWnd;
	HWND						m_hNextWnd;
	HWND						m_hVolumeWnd;

	HBITMAP						m_PreviousBitmap;
	HBITMAP						m_NextBitmap;
	HBITMAP						m_PlayBitmap;
	HBITMAP						m_PauseBitmap;

	RECT						m_SeekRect;

	WNDPROC						m_origPrevWndProc;
	static LRESULT CALLBACK		PrevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	WNDPROC						m_origNextWndProc;
	static LRESULT CALLBACK		NextWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CPlayControls(void);
	~CPlayControls(void);

	bool Create(HWND hWndParent);
	bool Destroy(void);

	bool Move(int x, int y, int w, int h);

	bool UpdateVolume(void);
	bool UpdateState(void);

	bool SysColorChange(WPARAM wParam, LPARAM lParam);
};
