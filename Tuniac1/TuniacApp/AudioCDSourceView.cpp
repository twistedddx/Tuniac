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

#include "stdafx.h"
#include "audiocdsourceview.h"
#include "resource.h"

#define WM_UPDATE					(WM_APP+2)
#define WM_SHOWACTIVEITEM			(WM_APP+3)

CAudioCDSourceView::CAudioCDSourceView(void)
{
}

CAudioCDSourceView::~CAudioCDSourceView(void)
{
}

bool	CAudioCDSourceView::CreateSourceView(HWND hWndParent)
{
	m_AudioCDSourceWnd = CreateDialogParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_CDAUDIOSOURCE), hWndParent, (DLGPROC)WndProcStub, (LPARAM)this);

	if(!m_AudioCDSourceWnd)
		return false;

	return (true);
}

bool	CAudioCDSourceView::DestroySourceView(void)
{
	DestroyWindow(m_AudioCDSourceWnd);

	return (true);
}

bool	CAudioCDSourceView::ShowSourceView(bool bShow)
{
	if(bShow)
		ShowWindow(m_AudioCDSourceWnd, SW_SHOW);
	else
		ShowWindow(m_AudioCDSourceWnd, SW_HIDE);

	return (true);
}

bool	CAudioCDSourceView::MoveSourceView(int x, int y, int w, int h)
{
	MoveWindow(m_AudioCDSourceWnd, x, y, w, h, TRUE);
	return (true);
}

bool	CAudioCDSourceView::ShowSourceViewOptions(HWND hWndParent)
{
	return (true);
}

bool	CAudioCDSourceView::Update(void)
{
	PostMessage(m_AudioCDSourceWnd, WM_UPDATE, 0, 0);
	return (true);
}

LRESULT CALLBACK			CAudioCDSourceView::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{
		case WM_INITDIALOG:
			{
				HWND hListView = GetDlgItem(hDlg, IDC_CDSOURCE_TRACKLIST);
	
				ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

				LVCOLUMN	lvC;   
				ZeroMemory(&lvC, sizeof(LVCOLUMN));

				lvC.mask	= LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;

				lvC.pszText = TEXT("");
				lvC.cx		= 24;
				lvC.fmt		= 0;
				ListView_InsertColumn(hListView, 0, &lvC);

				lvC.pszText		= TEXT("Track");
				lvC.cx			= 48;
				lvC.fmt			= LVCFMT_LEFT;
				lvC.iSubItem	= 1;
				ListView_InsertColumn(hListView, 1, &lvC);

				lvC.pszText		= TEXT("Time");
				lvC.cx			= 64;
				lvC.fmt			= LVCFMT_LEFT;
				lvC.iSubItem	= 1;
				ListView_InsertColumn(hListView, 2, &lvC);

				lvC.pszText		= TEXT("Artist");
				lvC.cx			= 250;
				lvC.fmt			= LVCFMT_LEFT;
				lvC.iSubItem	= 1;
				ListView_InsertColumn(hListView, 3, &lvC);

				lvC.pszText		= TEXT("Title");
				lvC.cx			= 250;
				lvC.fmt			= LVCFMT_LEFT;
				lvC.iSubItem	= 1;
				ListView_InsertColumn(hListView, 4, &lvC);

				HIMAGELIST hList  = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 3, 1); 

				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_NORMAL)); 
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_ACTIVE)); 
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_UNAVAILABLE)); 

				ListView_SetImageList(hListView, hList, LVSIL_SMALL);
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
					BeginPaint(hDlg, &PS);

					FillRect(PS.hdc, &rcTopBarRect, GetSysColorBrush(COLOR_BTNFACE));

					if(m_pCDPlaylist)
					{
						SetBkMode(PS.hdc, TRANSPARENT);
						SelectObject(PS.hdc, tuniacApp.GetTuniacFont(FONT_SIZE_LARGE));
						rcTopBarRect.bottom = 28;
						rcTopBarRect.left += 15;
						DrawText(PS.hdc, m_pCDPlaylist->GetPlaylistName(), wcsnlen_s(m_pCDPlaylist->GetPlaylistName(), 256), &rcTopBarRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
					}

					EndPaint(hDlg, &PS);
				}
			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(GetDlgItem(hDlg, IDC_CDSOURCE_TRACKLIST),							0,			28,		Width-4,	Height-32,	TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_CDSOURCE_IMPORT),						Width-150,	4,		150,		20,			TRUE);

			}
			break;

		case WM_ERASEBKGND:
			{
				return true;
			}
			break;

		case WM_NOTIFY:
			{
				LPNMHDR lpNotify = (LPNMHDR)lParam;
				HWND hListView = GetDlgItem(hDlg, IDC_CDSOURCE_TRACKLIST);

				switch(lpNotify->code)
				{
					//click an item in playlist
					case NM_CLICK:
						{

							if(tuniacApp.m_Preferences.GetArtOnSelection())
							{
								LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;
								IPlaylistEntry * pIPE = m_pCDPlaylist->GetEntryAtNormalFilteredIndex(lpnmitem->iItem);
								if(pIPE)
									tuniacApp.GetArt((LPTSTR)pIPE->GetField(FIELD_URL));
							}
						}
						break;

					case NM_DBLCLK:
						{
							LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;

							if(lpnmitem->iItem != -1)
							{
								tuniacApp.m_PlaylistManager.SetActivePlaylistByIndex(m_ulActivePlaylistIndex);
								if(m_pCDPlaylist->SetActiveFilteredIndex(lpnmitem->iItem))
								{
									tuniacApp.PlayEntry(m_pCDPlaylist->GetActiveEntry(), true, false);
								}

								Update();
							}
						}
						break;

					case LVN_GETDISPINFO:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *) lParam;
							if(pDispInfo->item.iItem != -1)
							{
								IPlaylistEntry * pIPE = m_pCDPlaylist->GetEntryAtNormalFilteredIndex(pDispInfo->item.iItem);

								if(pIPE)
								{
									if(pDispInfo->item.mask & LVIF_IMAGE)
									{
										if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == m_pCDPlaylist)
										{
											if(m_pCDPlaylist->GetActiveFilteredIndex() == pDispInfo->item.iItem)
											{
												pDispInfo->item.iImage = 1;
											}
										}
									}

									if(pDispInfo->item.mask & LVIF_TEXT)
									{
										if(pDispInfo->item.iSubItem == 1)
										{
											pIPE->GetTextRepresentation(	FIELD_TRACKNUM,
																			pDispInfo->item.pszText,
																			pDispInfo->item.cchTextMax);
										}
										else if(pDispInfo->item.iSubItem == 2)
										{
											pIPE->GetTextRepresentation(	FIELD_PLAYBACKTIME,
																			pDispInfo->item.pszText,
																			pDispInfo->item.cchTextMax);
										}
										else if(pDispInfo->item.iSubItem == 3)
										{
											pIPE->GetTextRepresentation(	FIELD_ARTIST,
																			pDispInfo->item.pszText,
																			pDispInfo->item.cchTextMax);
										}
										else if(pDispInfo->item.iSubItem == 4)
										{
											pIPE->GetTextRepresentation(	FIELD_TITLE,
																			pDispInfo->item.pszText,
																			pDispInfo->item.cchTextMax);
										}

									}

								}
							}
						}
						break;
				}
			}
			break;

		case WM_UPDATE:
			{
				HWND hListView = GetDlgItem(hDlg, IDC_CDSOURCE_TRACKLIST);

				ListView_SetItemCountEx(hListView, m_pCDPlaylist->GetNumItems(), LVSICF_NOSCROLL);
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

LRESULT CALLBACK			CAudioCDSourceView::WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CAudioCDSourceView * pSSW = (CAudioCDSourceView *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pSSW->WndProc(hDlg, message, wParam, lParam));
}

//Added Mark 7th Oct
bool CAudioCDSourceView::EditTrackInfo()
{
	return false;
}

bool CAudioCDSourceView::SetPlaylistSource(unsigned long ulPlaylistIndex)
{
	IPlaylist * t = tuniacApp.m_PlaylistManager.GetPlaylistByIndex(ulPlaylistIndex);

	if(t->GetPlaylistType() == PLAYLIST_TYPE_CD)
	{
		m_ulActivePlaylistIndex = ulPlaylistIndex;

		m_pCDPlaylist = (CAudioCDPlaylist*)t;
		return true;
	}

	return false;
}

bool	CAudioCDSourceView::ShowCurrentItem()
{
	return false;
}
