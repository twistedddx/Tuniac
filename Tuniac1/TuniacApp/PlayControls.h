/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

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
