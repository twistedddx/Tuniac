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

#include "stdafx.h"
#include "RadioSourceView.h"
#include "resource.h"

CRadioSourceView::CRadioSourceView(void)
{
}

CRadioSourceView::~CRadioSourceView(void)
{
}

bool	CRadioSourceView::CreateSourceView(HWND hWndParent)
{
	m_RadioSourceWnd = CreateDialogParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_RADIOSOURCEVIEW), hWndParent, (DLGPROC)WndProcStub, (LPARAM)this);

	if(!m_RadioSourceWnd)
		return false;

	return (true);
}

bool	CRadioSourceView::DestroySourceView(void)
{
	DestroyWindow(m_RadioSourceWnd);

	return (true);
}

bool	CRadioSourceView::ShowSourceView(bool bShow)
{
	if(bShow)
	{
		ShowWindow(m_RadioSourceWnd, SW_SHOW);
	}
	else
	{
		ShowWindow(m_RadioSourceWnd, SW_HIDE);
	}

	return (true);
}

bool	CRadioSourceView::MoveSourceView(int x, int y, int w, int h)
{
	MoveWindow(m_RadioSourceWnd, x, y, w, h, TRUE);
	return (true);
}

bool	CRadioSourceView::ShowSourceViewOptions(HWND hWndParent)
{
	return (true);
}

bool	CRadioSourceView::Update(void)
{
	return false;
}

bool	CRadioSourceView::EditTrackInfo()
{
	return false;
}

bool	CRadioSourceView::ShowCurrentItem()
{
	return false;
}

bool CRadioSourceView::SetPlaylistSource(unsigned long ulPlaylistIndex)
{
	IPlaylist * t = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(ulPlaylistIndex);

	if(t->GetPlaylistType() == PLAYLIST_TYPE_RADIO)
	{
		m_ulActivePlaylistIndex = ulPlaylistIndex;

		m_pRadioPlaylist = (CRadioTunerPlaylist *)t;
		return true;
	}

	return false;
}

bool CRadioSourceView::ParseRadioList(void)
{
	return false;
}

LRESULT CALLBACK			CRadioSourceView::WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CRadioSourceView * pSSW = (CRadioSourceView *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pSSW->WndProc(hDlg, message, wParam, lParam));
}

LRESULT CALLBACK			CRadioSourceView::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(	GetDlgItem(hDlg, IDC_RADIOSOURCE_LIST),	
							0,
							28, 
							Width,
							Height - 64, 
							TRUE);

				MoveWindow(	GetDlgItem(hDlg, IDC_RADIOSOURCE_REFRESH),
							0,
							Height - 29,
							75,
							25,
							TRUE);

				MoveWindow(	GetDlgItem(hDlg, IDC_RADIOSOURCE_TUNEIN),
							Width-75,
							Height - 29,
							75,
							25,
							TRUE);

				MoveWindow(GetDlgItem(hDlg, IDC_RADIOSOURCE_LIST),							0,			28,		Width-2,	Height-30,	TRUE);

				MoveWindow(GetDlgItem(hDlg, IDC_RADIOSOURCE_TUNEIN),						Width-79,	4,		75,			20,			TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_RADIOSOURCE_REFRESH),						Width-159,	4,		75,			20,			TRUE);


			}
			break;

		case WM_PAINT:
			{
				RECT	rcUpdateRect;
				RECT	rcTopBarRect;

				if(GetUpdateRect(hDlg, &rcUpdateRect, FALSE))
				{
					PAINTSTRUCT PS;

					GetClientRect(hDlg, &rcTopBarRect);
					rcTopBarRect.bottom = 28;

					BeginPaint(hDlg, &PS);

					FillRect(PS.hdc, &rcTopBarRect, GetSysColorBrush(COLOR_BTNFACE));

					SetBkMode(PS.hdc, TRANSPARENT);
					SelectObject(PS.hdc, tuniacApp.GetTuniacFont(FONT_SIZE_LARGE));

					rcTopBarRect.left += 15;

					DrawText(PS.hdc, m_pRadioPlaylist->GetPlaylistName(), lstrlen(m_pRadioPlaylist->GetPlaylistName()), &rcTopBarRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

					EndPaint(hDlg, &PS);
				}
			}
			break;


		default:
			return FALSE;
			break;
	}

	return TRUE;
}
