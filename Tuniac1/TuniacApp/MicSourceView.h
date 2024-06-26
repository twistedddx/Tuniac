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
	Copyright (C) 2003-2014 Brett Hoyle
*/

#pragma once
#include "isourceview.h"
#include "MicPlaylist.h"

class CMicSourceView :
	public ISourceView
{
protected:
	CMicPlaylist		*	m_pMicPlaylist;
	HWND					m_MicSourceWnd;
	unsigned long			m_ulActivePlaylistIndex;

	LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CMicSourceView(void);
	~CMicSourceView(void);

	bool SetPlaylistSource(unsigned long ulPlaylistIndex);

	void SetEnabled(bool bEnabled);

public:

	bool	CreateSourceView(HWND hWndParent);
	bool	DestroySourceView(void);
	bool	ShowSourceView(bool bShow);
	bool	MoveSourceView(int x, int y, int w, int h);
	bool	ShowSourceViewOptions(HWND hWndParent);
	bool	Update(void);
	bool	EditTrackInfo();
	bool	ShowCurrentItem();
};
