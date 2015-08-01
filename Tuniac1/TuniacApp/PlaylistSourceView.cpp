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
#include "playlistsourceview.h"

#include "resource.h"

#define WM_UPDATE					(WM_APP+2)
#define WM_SHOWACTIVEITEM			(WM_APP+3)
#define WM_SETFOCUSFILTER			(WM_APP+4)

#define DARKCOL						RGB(236, 243, 254)
#define BARCOLOR					GetSysColor(COLOR_MENUHILIGHT)

#define FILTERBYFIELD_MENUBASE		(7000)

typedef struct
{
	LPTSTR			szHeaderText;
	int				iHeaderWidth;
	unsigned long	ulFlags;
	bool			bEditable;
	bool			bFilterable;
} HeaderEntry;

static HeaderEntry HeaderEntries[FIELD_MAXFIELD] = 
{
	{
		TEXT("URL"),
		300,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Filename"),
		300,
		LVCFMT_LEFT,
		false,
		true
	},
	{
		TEXT("Artist"),
		200,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Album"),
		200,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Title"),
		320,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Track"),
		50,
		LVCFMT_RIGHT,
		true,
		true
	},
	{
		TEXT("Genre"),
		100,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Year"),
		40,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Time"),
		70,
		LVCFMT_RIGHT,
		false,
		true
	},
	{
		TEXT("Kind"),
		30,
		LVCFMT_LEFT,
		false,
		true
	},
	{
		TEXT("Size"),
		70,
		LVCFMT_RIGHT,
		false,
		true
	},
	{
		TEXT("Date Added"),
		115,
		LVCFMT_LEFT,
		false,
		false
	},
	{
		TEXT("File Creation Date"),
		115,
		LVCFMT_LEFT,
		false,
		false
	},
	{
		TEXT("Last Played Date"),
		115,
		LVCFMT_LEFT,
		false,
		false
	},
	{
		TEXT("Played"),
		50,
		LVCFMT_RIGHT,
		true,
		true
	},
	{
		TEXT("Rating"),
		50,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Comment"),
		240,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Bitrate"),
		70,
		LVCFMT_RIGHT,
		false,
		true
	},
	{
		TEXT("Sample Rate"),
		90,
		LVCFMT_RIGHT,
		false,
		true
	},
	{
		TEXT("Channels"),
		80,
		LVCFMT_CENTER,
		false,
		true
	},
	{
		TEXT("Ext"),
		80,
		LVCFMT_LEFT,
		false,
		true
	},
	{
		TEXT("Order"),
		80,
		LVCFMT_RIGHT,
		false,
		false
	},
	{
		TEXT("Track Gain"),
		80,
		LVCFMT_CENTER,
		true,
		true
	},
	{
		TEXT("Track Peak"),
		80,
		LVCFMT_CENTER,
		true,
		true
	},
	{
		TEXT("Album Gain"),
		80,
		LVCFMT_CENTER,
		true,
		true
	},
	{
		TEXT("Album Peak"),
		80,
		LVCFMT_CENTER,
		true,
		true
	},
	{
		TEXT("Availability"),
		80,
		LVCFMT_CENTER,
		false,
		true
	},
	{
		TEXT("BPM"),
		80,
		LVCFMT_CENTER,
		true,
		true
	},
	{
		TEXT("Album Artist"),
		300,
		LVCFMT_LEFT,
		true,
		true
	},
	{
		TEXT("Composer"),
		300,
		LVCFMT_LEFT,
		true,
		true
	}
};

CPlaylistSourceView::CPlaylistSourceView(void) :
	m_PlaylistSourceWnd(NULL),
	m_pPlaylist(NULL),
	m_Drag(false),
	m_ShowingHeaderFilter(false),
	m_iLastClickedItem(-1),
	m_iLastClickedSubitem(-1),
	m_origEditWndProc(NULL)
{
	m_ColumnIDArray.RemoveAll();

	for(int x=0; x<tuniacApp.m_Preferences.GetPlaylistViewNumColumns(); x++)
	{
		unsigned long t = tuniacApp.m_Preferences.GetPlaylistViewColumnIDAtIndex(x);
		m_ColumnIDArray.AddTail(t);
		HeaderEntries[tuniacApp.m_Preferences.GetPlaylistViewColumnIDAtIndex(x)].iHeaderWidth = tuniacApp.m_Preferences.GetPlaylistViewColumnWidthAtIndex(x);
	}
}

CPlaylistSourceView::~CPlaylistSourceView(void)
{
}

WNDPROC		CPlaylistSourceView::m_ViewOptionsListViewOldProc;

LRESULT CALLBACK CPlaylistSourceView::ViewOptionsListViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_GETDLGCODE:
			{
				if (lParam)
				{
					LPMSG lpmsg = (LPMSG)lParam;
					if (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == VK_ESCAPE)
						EndDialog(GetParent(hWnd), 0);

				}
			}
			break;
	}
	return CallWindowProc(m_ViewOptionsListViewOldProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CPlaylistSourceView::ViewOptionsWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{
		case WM_INITDIALOG:
			{
				LVCOLUMN	lvC;
				HWND		hListView = GetDlgItem(hDlg, IDC_PLAYLISTVIEWOPTIONS_LIST);

				m_ViewOptionsListViewOldProc = (WNDPROC)SetWindowLongPtr(hListView,  GWLP_WNDPROC, (LONG_PTR)ViewOptionsListViewProc);
				ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

				// Create columns.
				lvC.mask	= LVCF_WIDTH | LVCF_TEXT;
				lvC.cx		= 300;
				lvC.pszText	= TEXT("");
				ListView_InsertColumn(hListView, 0, &lvC);

				ListView_SetItemCountEx(hListView, FIELD_MAXFIELD, LVSICF_NOSCROLL);
				for(int x=0; x<FIELD_MAXFIELD; x++)
				{
					LVITEM	lvItem;

					ZeroMemory(&lvItem, sizeof(LVITEM));

					lvItem.mask		= LVIF_TEXT;
					lvItem.iItem	= x;
					lvItem.pszText	= HeaderEntries[x].szHeaderText;

					int item = ListView_InsertItem(hListView, &lvItem);

					for(unsigned long y=0; y<m_ColumnIDArray.GetCount(); y++)
					{
						if(m_ColumnIDArray[y] == x)
						{
							ListView_SetCheckState(hListView, item, TRUE);
						}
					}
				}
				ListView_SetColumnWidth(hListView, 0, LVSCW_AUTOSIZE_USEHEADER);
			}
			break;

		case WM_CLOSE:
			{
				SendMessage(hDlg, WM_COMMAND, MAKELONG(IDOK, 0), 0);
			}
			break;

		case WM_COMMAND:
			{
				int wmId, wmEvent;
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);

				switch(wmId)
				{
					//what columns to show after ok in column select window??
					case IDOK:
						{
							HWND		hCheckListView = GetDlgItem(hDlg, IDC_PLAYLISTVIEWOPTIONS_LIST);
							HWND		hPlayListView  = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);

							int			colarray[FIELD_MAXFIELD];

							HWND Header = ListView_GetHeader(hPlayListView);
							int NumCols = Header_GetItemCount(Header);
							if(NumCols)
							{
								Array<unsigned long, 3>		tempIDArray;			// the index here is actually a FIELD_ID from iplaylist.h to say which columns should be displayed.
								ListView_GetColumnOrderArray(hPlayListView, NumCols, colarray);

								for(int x=0; x<NumCols; x++)
								{
									unsigned long Column = colarray[x];
									if(Column)
									{
										unsigned long val = m_ColumnIDArray[Column-1];
										HeaderEntries[val].iHeaderWidth = ListView_GetColumnWidth(hPlayListView, colarray[x]);
										tempIDArray.AddTail(val);
									}
								}

								m_ColumnIDArray.RemoveAll();

								for(unsigned long x=0; x<tempIDArray.GetCount(); x++)
								{
									m_ColumnIDArray.AddTail(tempIDArray[x]);
								}
							}

							for(unsigned long x=0; x<FIELD_MAXFIELD; x++)
							{
								if(ListView_GetCheckState(hCheckListView, x))
								{
									bool bFound = false;

									for(unsigned long y=0; y<m_ColumnIDArray.GetCount(); y++)
									{
										if(m_ColumnIDArray[y] == x)
										{
											bFound = true;
											break;
										}
									}

									if(!bFound)
									{
										m_ColumnIDArray.AddTail(x);
									}
								}
								else
								{
									for(unsigned long y=0; y<m_ColumnIDArray.GetCount(); y++)
									{
										if(m_ColumnIDArray[y] == x)
										{
											m_ColumnIDArray.RemoveAt(y);
											y--;
										}
									}
								}

							}

							UpdateColumns();

							EndDialog(hDlg, 0);
						}
						break;
				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

LRESULT CALLBACK CPlaylistSourceView::ViewOptionsWndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
	{
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
	}

	CPlaylistSourceView * pLSV = (CPlaylistSourceView *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pLSV->ViewOptionsWndProc(hDlg, message, wParam, lParam));
}

bool	CPlaylistSourceView::ShowSourceViewOptions(HWND hWndParent)
{
	DialogBoxParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_PLAYLISTVIEW_VIEWOPTIONS), hWndParent, (DLGPROC)ViewOptionsWndProcStub, (DWORD_PTR)this);
	return false;
}

bool	CPlaylistSourceView::CreateSourceView(HWND hWndParent)
{
	m_PlaylistSourceWnd = CreateDialogParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_PLAYLISTSOURCE), hWndParent, (DLGPROC)WndProcStub, (LPARAM)this);

	if(!m_PlaylistSourceWnd)
		return false;

	UpdateColumns();
	
	m_ItemMenu			= GetSubMenu(LoadMenu(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDR_PLAYLISTITEM_MENU)), 0);
	m_HeaderMenu		= GetSubMenu(LoadMenu(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDR_PLAYLISTHEADER_MENU)), 0);

	m_FilterByFieldMenu	= CreatePopupMenu();
	AppendMenu(m_FilterByFieldMenu, MF_STRING | MF_CHECKED,		FILTERBYFIELD_MENUBASE + 0,		TEXT("&User defined"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING | MF_SEPARATOR,	FILTERBYFIELD_MENUBASE + 1,		NULL);
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 2,		TEXT("A&rtist"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 3,		TEXT("A&lbum"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 4,		TEXT("&Title"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 5,		TEXT("&Genre"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 6,		TEXT("&Year"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 7,		TEXT("&Comment"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 8,		TEXT("Al&bum Artist"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 9,		TEXT("C&omposer"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING | MF_SEPARATOR,	FILTERBYFIELD_MENUBASE + 10,	NULL);
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 11,	TEXT("&URL"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 12,	TEXT("&Filename"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 13,	TEXT("E&xtension"));
	AppendMenu(m_FilterByFieldMenu, MF_STRING | MF_SEPARATOR,	FILTERBYFIELD_MENUBASE + 14,	NULL);
	AppendMenu(m_FilterByFieldMenu, MF_STRING,					FILTERBYFIELD_MENUBASE + 15,	TEXT("Reverse &Filter"));

	return true;
}

bool	CPlaylistSourceView::DestroySourceView(void)
{
	HWND		hPlayListView  = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);

	HWND		Header = ListView_GetHeader(hPlayListView);
	int			NumCols = Header_GetItemCount(Header);
	int *		colarray;
	colarray = new int[NumCols];


	if(NumCols)
	{
		ListView_GetColumnOrderArray(hPlayListView, NumCols, colarray);

		tuniacApp.m_Preferences.SetPlaylistViewNumColumns(NumCols-1);

		int y = 0;

		for(int x=0; x<NumCols; x++)
		{
			if(colarray[x] != 0)
			{
				tuniacApp.m_Preferences.SetPlaylistViewColumnIDAtIndex(y, m_ColumnIDArray[colarray[x]-1]);
				tuniacApp.m_Preferences.SetPlaylistViewColumnWidthAtIndex(y, ListView_GetColumnWidth(hPlayListView, colarray[x]));
				y++;
			}
		}
	}
	delete[]colarray;

	DestroyWindow(m_PlaylistSourceWnd);
	DestroyMenu(m_ItemMenu);
	DestroyMenu(m_FilterByFieldMenu);

	return true;

}

bool	CPlaylistSourceView::ShowSourceView(bool bShow)
{
	if(bShow)
	{
		ShowWindow(m_PlaylistSourceWnd, SW_SHOW);
		return true;
		//InvalidateRect(m_PlaylistSourceWnd, NULL, TRUE);
	}
	else
	{
		ShowWindow(m_PlaylistSourceWnd, SW_HIDE);
		return false;
	}
}

bool	CPlaylistSourceView::MoveSourceView(int x, int y, int w, int h)
{
	MoveWindow(m_PlaylistSourceWnd, x, y, w, h, TRUE);

	RECT	rcTopBarRect;
	GetClientRect(m_PlaylistSourceWnd, &rcTopBarRect);
	rcTopBarRect.bottom = 28;
	RedrawWindow(m_PlaylistSourceWnd, &rcTopBarRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	return true;
}

LRESULT CALLBACK CPlaylistSourceView::EditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPlaylistSourceView * pPLSV = (CPlaylistSourceView *)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(uMsg)
	{
		case WM_WINDOWPOSCHANGING:
			{
				LPWINDOWPOS	lpWP = (LPWINDOWPOS)lParam;

				lpWP->x = pPLSV->m_editRect.left;
				lpWP->y = pPLSV->m_editRect.top;

				lpWP->cx = pPLSV->m_editRect.right - pPLSV->m_editRect.left;
				lpWP->cy = pPLSV->m_editRect.bottom - pPLSV->m_editRect.top;
			}
			break;

		default:
			return CallWindowProc(pPLSV->m_origEditWndProc, hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


LRESULT CALLBACK			CPlaylistSourceView::WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CPlaylistSourceView * pLSV = (CPlaylistSourceView *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pLSV->WndProc(hDlg, message, wParam, lParam));
}

LRESULT CALLBACK			CPlaylistSourceView::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
				SendMessage(GetDlgItem(hDlg, IDC_PLAYLIST_FILTER), EM_LIMITTEXT, 127, 0);
				m_AddCursor			= LoadCursor(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDC_ADDARROW));
				m_NewPlaylistCursor = LoadCursor(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDC_NEWPLAYLISTARROW));

				ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_HIDE);

				ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_PLAYLIST_LIST), LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

				HIMAGELIST hList  = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 3, 1); 

				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_NORMAL)); 
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_ACTIVE)); 
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_ITEM_UNAVAILABLE)); 

				ListView_SetImageList(GetDlgItem(hDlg, IDC_PLAYLIST_LIST), hList, LVSIL_SMALL);

			}
			break;

		case WM_DESTROY:
			{
			}
			break;

		case WM_PAINT:
			{
				RECT	rcTopBarRect;
				PAINTSTRUCT PS;
				CDoubleBuffer	doubleBuffer;

				GetClientRect(hDlg, &rcTopBarRect);
				HDC		hDC = BeginPaint(hDlg, &PS);
				doubleBuffer.Begin(hDC, &rcTopBarRect);

				FillRect(hDC, &rcTopBarRect, GetSysColorBrush(COLOR_BTNFACE));

				if(m_pPlaylist)
				{
					SetBkMode(hDC, TRANSPARENT);
					SelectObject(hDC, tuniacApp.GetTuniacFont(FONT_SIZE_LARGE));
					rcTopBarRect.bottom = 28;
					rcTopBarRect.left += 15;
					DrawText(hDC, m_pPlaylist->GetPlaylistName(), wcsnlen_s(m_pPlaylist->GetPlaylistName(), 256), &rcTopBarRect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
				}

				doubleBuffer.End(hDC);
				EndPaint(hDlg, &PS);
			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_CLEARFILTER),			Width-195,	4,		20,		20,		TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST),			Width-215,	4,		20,		20,		TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_SELECTFILTERBYFIELD),	Width-25,	4,		20,		20,		TRUE);
				MoveWindow(GetDlgItem(hDlg, IDC_PLAYLIST_FILTER),						Width-175,	4,		150,	20,		TRUE);

				MoveWindow(GetDlgItem(hDlg, IDC_PLAYLIST_LIST),							0,			28,		Width - 2,	Height - 30,	TRUE);

			}
			break;

		case WM_ERASEBKGND:
			{
				return true;
			}
			break;

		case WM_COMMAND:
			{
				int wmId, wmEvent;
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);

				if(wmId >= FILTERBYFIELD_MENUBASE && wmId <= FILTERBYFIELD_MENUBASE + 15)
				{
					if(wmId == FILTERBYFIELD_MENUBASE + 15)
					{
						bool bReverse = GetMenuState(m_FilterByFieldMenu, FILTERBYFIELD_MENUBASE + 15, MF_BYCOMMAND) & MF_CHECKED ? false : true;
						CheckMenuItem(m_FilterByFieldMenu, FILTERBYFIELD_MENUBASE + 15, MF_BYCOMMAND | (bReverse ? MF_CHECKED : MF_UNCHECKED));
						m_pPlaylist->SetTextFilterReversed(bReverse);
						m_pPlaylist->ApplyFilter();

						if(tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex() == tuniacApp.m_PlaylistManager.GetActivePlaylistIndex() && tuniacApp.m_Preferences.GetFollowCurrentSongMode())
						{
							if(tuniacApp.m_SourceSelectorWindow)
							{
								tuniacApp.m_SourceSelectorWindow->ShowCurrentlyPlaying();
							}
						}

						return TRUE;
					}

					unsigned long ulFilterByField = FIELD_MAXFIELD;

					if(wmId == FILTERBYFIELD_MENUBASE + 2)
						ulFilterByField = FIELD_ARTIST;
					else if(wmId == FILTERBYFIELD_MENUBASE + 3)
						ulFilterByField = FIELD_ALBUM;
					else if(wmId == FILTERBYFIELD_MENUBASE + 4)
						ulFilterByField = FIELD_TITLE;
					else if(wmId == FILTERBYFIELD_MENUBASE + 5)
						ulFilterByField = FIELD_GENRE;
					else if(wmId == FILTERBYFIELD_MENUBASE + 6)
						ulFilterByField = FIELD_YEAR;
					else if(wmId == FILTERBYFIELD_MENUBASE + 7)
						ulFilterByField = FIELD_COMMENT;
					else if(wmId == FILTERBYFIELD_MENUBASE + 8)
						ulFilterByField = FIELD_ALBUMARTIST;
					else if (wmId == FILTERBYFIELD_MENUBASE + 9)
						ulFilterByField = FIELD_COMPOSER;
					else if(wmId == FILTERBYFIELD_MENUBASE + 11)
						ulFilterByField = FIELD_URL;
					else if(wmId == FILTERBYFIELD_MENUBASE + 12)
						ulFilterByField = FIELD_FILENAME;
					else if(wmId == FILTERBYFIELD_MENUBASE + 13)
						ulFilterByField = FIELD_FILEEXTENSION;
					
					CheckMenuRadioItem(m_FilterByFieldMenu, 0, 13, wmId - FILTERBYFIELD_MENUBASE, MF_BYPOSITION);

					if(ulFilterByField != m_pPlaylist->GetTextFilterField())
					{
						m_pPlaylist->SetTextFilterField(ulFilterByField);
						m_pPlaylist->ApplyFilter();

						if(tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex() == tuniacApp.m_PlaylistManager.GetActivePlaylistIndex() && tuniacApp.m_Preferences.GetFollowCurrentSongMode())
						{
							if(tuniacApp.m_SourceSelectorWindow)
							{
								tuniacApp.m_SourceSelectorWindow->ShowCurrentlyPlaying();
							}
						}

					}
					return TRUE;
				}

				switch(wmId)
				{
					case IDC_PLAYLIST_SOURCE_CLEARFILTER:
						{
							SetDlgItemText(hDlg, IDC_PLAYLIST_FILTER, TEXT(""));
						}
						break;

					case IDC_PLAYLIST_SOURCE_MAKEPLAYLIST:
						{
							IPlaylistEntry	*	pPE = NULL;
							EntryArray			entryArray;

							TCHAR tBuffer[256];
							GetDlgItemText(hDlg, IDC_PLAYLIST_FILTER, tBuffer, 255);
							for(unsigned long x=0; x<m_pPlaylist->GetNumItems(); x++)
							{
								pPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(x);
								if(pPE)
								{
									entryArray.AddTail(pPE);
								}
							}

							StringCchCat(tBuffer, 256, TEXT(" Filterlist"));
							tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(tBuffer, entryArray);
							tuniacApp.m_SourceSelectorWindow->UpdateList();

							SetDlgItemText(hDlg, IDC_PLAYLIST_FILTER, TEXT(""));
							SendMessage(m_PlaylistSourceWnd, MAKELONG(IDC_PLAYLIST_FILTER, EN_CHANGE), 0, 0);
						}
						break;

					case IDC_PLAYLIST_SOURCE_SELECTFILTERBYFIELD:
						{
							POINT pt;
							GetCursorPos(&pt);

							TrackPopupMenu(m_FilterByFieldMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_PlaylistSourceWnd, NULL);
						}
						break;

					case IDC_PLAYLIST_FILTER:
						{
							switch(wmEvent)
							{
								case EN_CHANGE:
									{
										SetTimer(hDlg, DELAYEDAPPLYFILTER_TIMERID, 50, NULL);
									}
									break;
							}
						}
						break;

						//right click play
					case ID_PLAY:
						{
							if(m_iLastClickedItem < 0) break;

							tuniacApp.m_PlaylistManager.SetActivePlaylist(m_ulActivePlaylistIndex);
							tuniacApp.PlayEntry(m_pPlaylist->GetEntryAtNormalFilteredIndex(m_iLastClickedItem), true, false);
						}
						break;

						//right click play next (queue prepend)
					case ID_PLAYNEXT:
						{
							if(m_iLastClickedItem < 0) break;

							IndexArray	iaPlayArray;
							HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
							if(iPos == -1)
								break;

							while(iPos != -1)
							{
								iaPlayArray.AddTail((unsigned long &)iPos);
								iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
							}
							for(unsigned long i = iaPlayArray.GetCount(); i > 0; i--)
							{
								tuniacApp.m_Queue.Prepend(m_pPlaylist->GetEntryAtNormalFilteredIndex(iaPlayArray[i-1])->GetEntryID());
							}
							iaPlayArray.RemoveAll();
							tuniacApp.RebuildFutureMenu();
						}
						break;

						//right click queue (queue append)
					case ID_QUEUE:
						{
							if(m_iLastClickedItem < 0) break;

							HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
							if(iPos == -1)
								break;

							while(iPos != -1)
							{
								tuniacApp.m_Queue.Append(m_pPlaylist->GetEntryAtNormalFilteredIndex((unsigned long &)iPos)->GetEntryID());
								iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
							}
							tuniacApp.RebuildFutureMenu();
						}
						break;

						//right click pause here (soft pause)
					case ID_PAUSEHERE:
						{
							if(m_iLastClickedItem < 0) break;
							IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(m_iLastClickedItem);
							if(pIPE)
							{
								if(tuniacApp.m_SoftPause.ulAt == pIPE->GetEntryID())
									tuniacApp.m_SoftPause.ulAt = INVALID_PLAYLIST_INDEX;
								else
									tuniacApp.m_SoftPause.ulAt = pIPE->GetEntryID();
							}
						}
						break;

						//right click remove (remove item)
					case ID_REMOVE:
						{
							if(m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANDELETE)
							{
								IndexArray	m_DeleteArray;

								HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
								int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
								if(iPos == -1)
									break;

								while(iPos != -1)
								{
									m_DeleteArray.AddTail((unsigned long &)iPos);
									iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED);
								}
								ListView_SetItemState(hListViewWnd, -1, 0, LVIS_SELECTED);
								m_pPlaylist->DeleteNormalFilteredIndexArray(m_DeleteArray);
								m_DeleteArray.RemoveAll();

								//if deleted from ML it may have removed the file from numerous playlists, refresh them all
								if (m_pPlaylist->GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
								{
									for (unsigned long x = 0; x < tuniacApp.m_PlaylistManager.GetNumPlaylists(); x++)
									{
										IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
										if (pPlaylist)
										{
											//song tags may have changed, reapply filter
											if (pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
												((IPlaylistEX *)pPlaylist)->ApplyFilter();
										}
									}
								}
								else
								{
									m_pPlaylist->ApplyFilter();
								}

								if((m_pPlaylist->GetActiveEntry() == NULL) && (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist()))
								{
									CCoreAudio::Instance()->Reset();
								}
							}
						}
						break;

						//right click update (update item)
					case ID_UPDATE:
						{
							unsigned long deletedIndexes = 0;

							HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);

							if(iPos == -1)
								break;

							while(iPos != -1)
							{
								unsigned long ulEntryID = m_pPlaylist->GetEntryIDAtNormalFilteredIndex(iPos - deletedIndexes);
								if (tuniacApp.m_MediaLibrary.UpdateMLEntryByEntryID(ulEntryID))
								{
									//update success now update through playlists
									for (unsigned long list = 0; list < tuniacApp.m_PlaylistManager.GetNumPlaylists(); list++)
									{
										IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(list);
										if (pPlaylist)
										{
											if (pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
											{
												((IPlaylistEX *)pPlaylist)->UpdateRealIndex(((IPlaylistEX *)pPlaylist)->GetRealIndexforEntryID(ulEntryID));
											}

										}
									}
								}
								else
								{
									//update fail, remove the file from playlists. DeleteRealIndex in LibraryPlaylist will clear out all playlists
									tuniacApp.m_PlaylistManager.m_LibraryPlaylist.DeleteAllItemsWhereEntryIDEquals(ulEntryID);
									//ListView_SetItemState(hListViewWnd, iPos, 0, LVIS_SELECTED);
									deletedIndexes++;
								}
								iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
							}

							for (unsigned long x = 0; x < tuniacApp.m_PlaylistManager.GetNumPlaylists(); x++)
							{
								IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
								if (pPlaylist)
								{
									//song tags may have changed, reapply filter
									if (pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
										((IPlaylistEX *)pPlaylist)->ApplyFilter();
								}
							}

							if ((m_pPlaylist->GetActiveEntry() == NULL) && (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist()))
							{
								CCoreAudio::Instance()->Reset();
							}
						}
						break;


						//right click to new playlist (send selected to playlist)
					case ID_TONEWPLAYLIST:
						{
							IPlaylistEntry	*	pPE = NULL;
							EntryArray			entryArray;

							HWND				hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
							if(iPos == -1)
								break;
							while(iPos != -1)
							{
								pPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos);

								if(pPE)
								{
									entryArray.AddTail(pPE);
								}

								iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
							}

							tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(TEXT("New Dropped Playlist"), entryArray);
							tuniacApp.m_SourceSelectorWindow->UpdateList();
						}
						break;

						//right click show file (open explorer at file)
					case ID_SHOWFILE:
						{
							IPlaylistEntry	*	pPE = NULL;

							HWND				hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
							if(iPos == -1)
								break;
							if(iPos != -1)
							{
								pPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos);

								if(pPE)
								{
									LPITEMIDLIST lpItemIDList;

									LPMALLOC	pMalloc;
									if(SHGetMalloc(&pMalloc) == NOERROR)
									{
										if(SHILCreateFromPath((LPTSTR)pPE->GetField(FIELD_URL), &lpItemIDList, NULL) == S_OK)
										{
											SHOpenFolderAndSelectItems(lpItemIDList, 0, NULL, 0);
											pMalloc->Free(lpItemIDList);
										}
										pMalloc->Release();
									}
								}
							}
						}
						break;

						//right click filter by field (send field to new playlist)
					case ID_FILTERBYFIELD:
						{
							EntryArray			entryArray;
							IPlaylistEntry *	pIPE				= m_pPlaylist->GetEntryAtNormalFilteredIndex(m_iLastClickedItem);
							TCHAR				szFilterString[512];
							TCHAR				szFilterName[256];
							bool				bReverse			= GetKeyState(VK_CONTROL) & 0x8000;

							pIPE->GetTextRepresentation(m_ColumnIDArray[m_iLastClickedSubitem - 1], szFilterString, 512);
							
							if(bReverse)
								StringCchPrintf(szFilterName, 256, TEXT("Not %s"), szFilterString);
							else
								StringCchCopy(szFilterName, 256, szFilterString);

							if(wcsnlen_s(szFilterString, 512))
							{
								m_pPlaylist->GetFieldFilteredItemArray(entryArray, m_ColumnIDArray[m_iLastClickedSubitem - 1], szFilterString, bReverse); 

								tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(szFilterName, entryArray);
								tuniacApp.m_SourceSelectorWindow->UpdateList();
								tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(tuniacApp.m_PlaylistManager.GetNumPlaylists() - 1);
							}							
						}
						break;

						//right click queue by field (send field to queue append)
					case ID_QUEUEBYFIELD:
						{
							if(GetKeyState(VK_CONTROL) & 0x8000)
								tuniacApp.m_Queue.Clear();

							EntryArray			entryArray;
							IPlaylistEntry *	pIPE				= m_pPlaylist->GetEntryAtNormalFilteredIndex(m_iLastClickedItem);
							TCHAR				szFilterString[512];

							pIPE->GetTextRepresentation(m_ColumnIDArray[m_iLastClickedSubitem - 1], szFilterString, 512);

							if(wcsnlen_s(szFilterString, 512))
							{
								m_pPlaylist->GetFieldFilteredItemArray(entryArray, m_ColumnIDArray[m_iLastClickedSubitem - 1], szFilterString, false); 

								for(unsigned long i = 0; i < entryArray.GetCount(); i++)
								{
									tuniacApp.m_Queue.Append(entryArray[i]->GetEntryID());
								}
							}
							tuniacApp.RebuildFutureMenu();
						}
						break;

						//right click headers "options" (column views)
					case ID_EDIT_SHOWCOLUMNSELECTION:
						{
							ShowSourceViewOptions(tuniacApp.getMainWindow());
						}
						break;

						//right click headers "filter by" (filter by text)
					case ID_FILTERBY:
						{
							if(!HeaderEntries[m_ColumnIDArray[m_iLastClickedSubitem-1]].bFilterable)
								break;

							m_ShowingHeaderFilter = true;

							HWND hFilter = GetDlgItem(hDlg, IDC_PLAYLISTSOURCE_FILTERCOMBO);

							HWND hListView = GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
							HWND hListHeader = ListView_GetHeader(hListView);

							//int iWidth = ListView_GetColumnWidth(hListView, m_iLastClickedSubitem);


							//POINT pt, cpt;
							//GetCursorPos(&pt);
							//GetCursorPos(&cpt);
							//ScreenToClient(hListHeader, &cpt);
							RECT rc;
							Header_GetItemRect(hListHeader, m_iLastClickedSubitem, &rc);

							SCROLLINFO si;
							ZeroMemory(&si, sizeof(si));
							si.cbSize = sizeof(si);
							si.fMask = SIF_POS;
							GetScrollInfo(hListView, SB_HORZ, &si);

							SendMessage(hFilter, CB_RESETCONTENT, 0, 0);
							SetWindowPos(hFilter, HWND_TOP, rc.left - si.nPos, 30, rc.right - rc.left + 3, 13, SWP_SHOWWINDOW);
							RECT r;
							GetClientRect(hFilter, &r);
							r.left =- si.nPos;
							RedrawWindow(hFilter, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_VALIDATE | RDW_UPDATENOW);

							SendMessage(hFilter, CB_ADDSTRING, 0, (LPARAM)TEXT("..."));
							EnableWindow(hFilter, FALSE);

							FieldDataArray setArray;
							TCHAR szCurrent[64];
							int iLeft, iMid, iRight, iPos, iComp;
							for(unsigned long i = 0; i < m_pPlaylist->GetNumItems(); i++)
							{
								m_pPlaylist->GetEntryAtNormalFilteredIndex(i)->GetTextRepresentation(m_ColumnIDArray[m_iLastClickedSubitem - 1], szCurrent, 64);
								if(szCurrent[0] == TEXT('\0')) continue;

								iLeft = 0;
								iRight = setArray.GetCount() - 1;
								iPos = -1;
								while(iLeft <= iRight)
								{
									iMid = (iLeft + iRight) / 2;

									iComp = StrCmpI(szCurrent, setArray[iMid].szText);
									if(iComp < 0)
									{
										iRight = iMid - 1;
									}
									else if (iComp > 0)
									{
										iLeft = iMid + 1;
									}
									else
									{
										iPos = iMid;
										break;
									}
								}
								
								if(iRight < iLeft || iPos < 0)
								{
									FieldDataSet fdsCurrent;
									fdsCurrent.nCount = 1;
									StringCchCopy(fdsCurrent.szText, 64, szCurrent);
									setArray.InsertAfter(iRight, fdsCurrent);
								}
								else
								{
									setArray[iPos].nCount++;
								}

							}
							
							SendMessage(hFilter, CB_DELETESTRING, 0, 0);
							SendMessage(hFilter, CB_INITSTORAGE, setArray.GetCount(), 64 * sizeof(TCHAR));

							EnableWindow(hFilter, TRUE);
							for(unsigned long i = 0; i < setArray.GetCount(); i++)
							{
								SendMessage(hFilter, CB_ADDSTRING, 0, (LPARAM)setArray[i].szText);
							}
							SendMessage(hFilter, CB_SHOWDROPDOWN, TRUE, NULL);

							SetTimer(hDlg, DELAYEDREDRAW_TIMERID, 10, NULL);

						}
						break;

					case IDC_PLAYLISTSOURCE_FILTERCOMBO:
						{
							HWND hFilter = GetDlgItem(hDlg, IDC_PLAYLISTSOURCE_FILTERCOMBO);
							switch(wmEvent)
							{
								case CBN_SELENDCANCEL:
								case CBN_KILLFOCUS:
									{
										ShowWindow(hFilter, SW_HIDE);
										m_ShowingHeaderFilter = false;
									}
									break;

								case CBN_SELENDOK:
									{
										ShowWindow(hFilter, SW_HIDE);
										m_ShowingHeaderFilter = false;

										TCHAR szFilter[64];
										SendMessage(hFilter, WM_GETTEXT, 64, (LPARAM)szFilter);
										m_pPlaylist->SetTextFilter(szFilter);
										m_pPlaylist->SetTextFilterField(m_ColumnIDArray[m_iLastClickedSubitem - 1]);
										m_pPlaylist->SetTextFilterReversed(false);
										m_pPlaylist->ApplyFilter();
										SetPlaylistSource(m_ulActivePlaylistIndex);
									}
									break;

							}
						}
						break;

				}
			}
			break;


			//right click on playlist item
			//I dont get what this is for; removing 130224
			/*
		case WM_INITMENUPOPUP:
			{
				HMENU hMenu = GetSubMenu(m_ItemMenu, 7);
				if((HMENU)wParam == hMenu)
				{

					while(GetMenuItemCount(hMenu) > 0)
						DeleteMenu(hMenu, 0, MF_BYPOSITION);
					AppendMenu(hMenu, MF_STRING | MF_GRAYED, 0, L"Please wait...");

					FieldDataArray setArray;
					TCHAR szCurrent[64];
					int iLeft, iMid, iRight, iPos, iComp;
					for(unsigned long i = 0; i < m_pPlaylist->GetNumItems(); i++)
					{
						m_pPlaylist->GetEntryAtNormalFilteredIndex(i)->GetTextRepresentation(m_ColumnIDArray[m_iLastClickedSubitem - 1], szCurrent, 64);
						if(szCurrent == TEXT('\0')) continue;

						iLeft = 0;
						iRight = setArray.GetCount() - 1;
						iPos = -1;
						while(iLeft <= iRight)
						{
							iMid = (iLeft + iRight) / 2;

							iComp = StrCmpI(szCurrent, setArray[iMid].szText);
							if(iComp < 0)
							{
								iRight = iMid - 1;
							}
							else if (iComp > 0)
							{
								iLeft = iMid + 1;
							}
							else
							{
								iPos = iMid;
								break;
							}
						}
						
						if(iRight < iLeft || iPos < 0)
						{
							FieldDataSet fdsCurrent;
							fdsCurrent.nCount = 1;
							StringCchCopy(fdsCurrent.szText, szCurrent);
							setArray.InsertAfter(iRight, fdsCurrent);
						}
						else
						{
							setArray[iPos].nCount++;
						}

					}

					DeleteMenu(hMenu, 0, MF_BYPOSITION);

					TCHAR szItem[64];
					for(unsigned long i = 0; i < setArray.GetCount(); i++)
					{
						szItem[0] = L'\0';
						StringCchCat(szItem, setArray[i].szText);
						StringCchPrintf(szItem, 64, TEXT("%s\t%d items"), szItem, setArray[i].nCount);
						AppendMenu(hMenu, MF_STRING, ID_FILTERBY, szItem);

					}
				}
			}
			break;
			*/

		case WM_NOTIFY:
			{
				LPNMHDR lpNotify = (LPNMHDR)lParam;

				switch(lpNotify->code)
				{
					//slow click a file entry to edit
					case LVN_BEGINLABELEDIT:
						{
							HWND			hListViewWnd	= lpNotify->hwndFrom;
							HWND			hEditControl	= ListView_GetEditControl(hListViewWnd);
							NMLVDISPINFO *	pdi = (NMLVDISPINFO *) lParam; 

							if(!HeaderEntries[m_ColumnIDArray[m_iLastClickedSubitem-1]].bEditable)
							{
								SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
								return TRUE;
							}

							ListView_GetSubItemRect(hListViewWnd, pdi->item.iItem, m_iLastClickedSubitem, LVIR_LABEL, &m_editRect);
							SetWindowLongPtr(hEditControl, GWLP_USERDATA, (DWORD_PTR)this);
							m_origEditWndProc = (WNDPROC)SetWindowLongPtr(hEditControl, GWLP_WNDPROC, (LONG_PTR)EditWndProc);

							IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(pdi->item.iItem);
							if(pIPE)
							{
								TCHAR szFieldData[512];
								pIPE->GetTextRepresentation(m_ColumnIDArray[m_iLastClickedSubitem-1], szFieldData, 512);
								SetWindowText(hEditControl, szFieldData);
							}
						}
						break;

						//finished entry edit
					case LVN_ENDLABELEDIT:
						{
							NMLVDISPINFO * pdi = (NMLVDISPINFO *) lParam;

							if(pdi->item.pszText)
							{
								IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(pdi->item.iItem);
								if(pIPE)
								{
									if(pIPE->SetField(m_ColumnIDArray[m_iLastClickedSubitem-1], (void *)pdi->item.pszText))
									{
										if(pIPE == tuniacApp.m_PlaylistManager.GetActivePlaylist()->GetActiveEntry())
										{	
											tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
										}

										//todo bits: per column tag writing?
										//tuniacApp.m_MediaLibrary.WriteFileTags(pIPE, m_ColumnIDArray[m_iLastClickedSubitem-1], pdi->item.pszText);
										if(m_ColumnIDArray[m_iLastClickedSubitem-1] != FIELD_PLAYCOUNT || m_ColumnIDArray[m_iLastClickedSubitem-1] != FIELD_URL)
											tuniacApp.m_MediaLibrary.WriteFileTags(pIPE);

										//update title bar etc
										tuniacApp.UpdateTitles();
									}
								}
							}
						}
						break;

						//column click (sort column)
					case LVN_COLUMNCLICK:
						{
							LPNMLISTVIEW pnmv = (LPNMLISTVIEW) lParam;

							bool bNotPlayOrder = false;
							if(pnmv->iSubItem == 0)
								bNotPlayOrder = true;

							else if(m_ColumnIDArray[pnmv->iSubItem-1] != FIELD_PLAYORDER)
								bNotPlayOrder = true;

							if(bNotPlayOrder && m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANSORT)
							{
								if(pnmv->iSubItem > 0)
									m_pPlaylist->Sort(m_ColumnIDArray[pnmv->iSubItem-1]);
								else
									m_pPlaylist->Sort(FIELD_URL);
								Update();
							}
						}
						break;
				
					case LVN_KEYDOWN:
						{
							HWND			hListViewWnd	= lpNotify->hwndFrom;
							LPNMLVKEYDOWN	pnkd			= (LPNMLVKEYDOWN) lParam;

							switch(pnkd->wVKey)
							{
								//del key press(del item from playlist)
								case VK_DELETE:
									{
										if (m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANDELETE)
										{
											IndexArray	m_DeleteArray;

											HWND hListViewWnd = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);
											int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
											if (iPos == -1)
												break;
											while (iPos != -1)
											{
												m_DeleteArray.AddTail((unsigned long &)iPos);
												iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED);
											}
											ListView_SetItemState(hListViewWnd, -1, 0, LVIS_SELECTED);
											m_pPlaylist->DeleteNormalFilteredIndexArray(m_DeleteArray);
											m_DeleteArray.RemoveAll();

											//if deleted from ML it may have removed the file from numerous playlists, refresh them all
											if (m_pPlaylist->GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY)
											{
												for (unsigned long x = 0; x < tuniacApp.m_PlaylistManager.GetNumPlaylists(); x++)
												{
													IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
													if (pPlaylist)
													{
														//song tags may have changed, reapply filter
														if (pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
															((IPlaylistEX *)pPlaylist)->ApplyFilter();
													}
												}
											}
											else
											{
												m_pPlaylist->ApplyFilter();
											}

											if ((m_pPlaylist->GetActiveEntry() == NULL) && (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist()))
											{
												CCoreAudio::Instance()->Reset();
											}
										}
									}
									break;

									//select all with control + a
								case 0x41: // 'A' key
									{
										if (GetKeyState(VK_CONTROL))
											ListView_SetItemState(hListViewWnd, -1, LVIS_SELECTED ,LVIS_SELECTED);
									}
									break;

									//arrow up
								case VK_UP:
									{
										if(tuniacApp.m_Preferences.GetShowAlbumArt() && tuniacApp.m_Preferences.GetArtOnSelection())
										{
											int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
											if(iPos == -1)
												break;

											IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos-1);
											if(pIPE)
												tuniacApp.GetArt((LPTSTR)pIPE->GetField(FIELD_URL));
										}
									}
									break;

									//arrow down
								case VK_DOWN:
									{
										if(tuniacApp.m_Preferences.GetShowAlbumArt() && tuniacApp.m_Preferences.GetArtOnSelection())
										{
											int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
											if(iPos == -1)
												break;

											IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos+1);
											if(pIPE)
												tuniacApp.GetArt((LPTSTR)pIPE->GetField(FIELD_URL));
										}
									}
									break;

								//catch media keys
								case VK_MEDIA_PLAY_PAUSE:
									{
										if (tuniacApp.m_LogWindow)
										{
											if (tuniacApp.m_LogWindow->GetLogOn())
											{
												tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistSourceView"), TEXT("VK_MEDIA_PLAY_PAUSE event"));
											}
										}
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
									}
									break;

								case VK_MEDIA_STOP:
									{
										if (tuniacApp.m_LogWindow)
										{
											if (tuniacApp.m_LogWindow->GetLogOn())
											{
												tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistSourceView"), TEXT("VK_MEDIA_STOP event"));
											}
										}
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_STOP, 0), 0);
									}
									break;

								case VK_MEDIA_NEXT_TRACK:
									{
										if (tuniacApp.m_LogWindow)
										{
											if (tuniacApp.m_LogWindow->GetLogOn())
											{
												tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistSourceView"), TEXT("VK_MEDIA_NEXT_TRACK event"));
											}
										}
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
									}
									break;

								case VK_MEDIA_PREV_TRACK:
									{
										if (tuniacApp.m_LogWindow)
										{
											if (tuniacApp.m_LogWindow->GetLogOn())
											{
												tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistSourceView"), TEXT("VK_MEDIA_PREV_TRACK event"));
											}
										}
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
									}
									break;

								/* these are handled in the OS almost always, so lets not double up
								case VK_VOLUME_UP:
									{
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEUP, 0), 0);
									}
									break;

								case VK_VOLUME_DOWN:
									{
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEDOWN, 0), 0);
									}
									break;

								case VK_VOLUME_MUTE:
									{
										SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_VOLUMEMUTE, 0), 0);
									}
									break;
									*/
								default:
									return FALSE;
									break;
							}
						}
						break;

						//drag items in playlist
					case LVN_BEGINDRAG:
						{
							if(lpNotify->idFrom == IDC_PLAYLIST_LIST)
							{
								//correct window found
								m_Drag = true;	
								SetRect(&m_draggedItemRect, 0, 0, 0, 0);

								//capture the mouse
								SetCapture(m_PlaylistSourceWnd);
							}
						}
						break;

						//get info for display such as playorder and left play image location
					case LVN_GETDISPINFO:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *) lParam;
							if(pDispInfo->item.mask & LVIF_IMAGE)
							{
								pDispInfo->item.iImage = 0;
								if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == m_pPlaylist)
								{
									if(m_pPlaylist->GetActiveNormalFilteredIndex() == pDispInfo->item.iItem)
									{
										pDispInfo->item.iImage = 1;
									}
								}
								if(m_pPlaylist)
								{
									if(m_pPlaylist->GetEntryAtNormalFilteredIndex(pDispInfo->item.iItem))
									{
										if(m_pPlaylist->GetEntryAtNormalFilteredIndex(pDispInfo->item.iItem)->GetField(FIELD_AVAILABILITY))
										{
											pDispInfo->item.iImage = 2;
										}
									}
								}
							}

							if(pDispInfo->item.mask & LVIF_TEXT)
							{
								if(pDispInfo->item.iSubItem > 0)
								{
									if(m_ColumnIDArray[pDispInfo->item.iSubItem-1] == FIELD_PLAYORDER)
									{
										if(m_pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
										{
											unsigned ulPlayOrder = ((IPlaylistEX *)m_pPlaylist)->GetPlayOrder(pDispInfo->item.iItem);

											if (ulPlayOrder != INVALID_PLAYLIST_INDEX)
												StringCchPrintf(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, TEXT("%u"), ulPlayOrder);
											else
												StringCchPrintf(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, TEXT(""));
										}
										else
										{
											StringCchPrintf(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, TEXT(""));
										}
									}
									else
									{
										if(m_pPlaylist)
										{
											IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(pDispInfo->item.iItem);
											if(pIPE)
											{
												pIPE->GetTextRepresentation(	m_ColumnIDArray[pDispInfo->item.iSubItem-1],
																			pDispInfo->item.pszText,
																			pDispInfo->item.cchTextMax);
											}
										}
									}
								}
							}
						}
						break;

					case NM_CUSTOMDRAW:
						{
							HWND hHeader =	ListView_GetHeader(GetDlgItem(hDlg, IDC_PLAYLIST_LIST));

							if(lpNotify->idFrom == IDC_PLAYLIST_LIST)
							{
								NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)( lParam );
								HWND			lListWnd = lpNotify->hwndFrom;

								if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
								{
									SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
								}
								else if( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
								{
									if ((pLVCD->nmcd.dwItemSpec % 2) == 0)
									{
										pLVCD->clrTextBk = DARKCOL;
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NEWFONT);
									}

									SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_DODEFAULT);
								}
							}
							else if(lpNotify->hwndFrom == hHeader)
							{
								LPNMCUSTOMDRAW lpCD = (LPNMCUSTOMDRAW) lParam;
								HWND hFilter = GetDlgItem(hDlg, IDC_PLAYLISTSOURCE_FILTERCOMBO);

								if(lpCD->dwDrawStage == CDDS_PREPAINT)
								{
									SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
									break;
								}
								else if(lpCD->dwDrawStage == CDDS_POSTPAINT)
								{
									RECT r;
									GetClientRect(hFilter, &r);
									RedrawWindow(hFilter, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_VALIDATE | RDW_UPDATENOW);
								}
								else
								{

									if(lpCD->dwDrawStage == CDDS_ITEMPREPAINT)
									{
										if(m_ShowingHeaderFilter && lpCD->dwItemSpec == m_iLastClickedSubitem)
										{
											SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_SKIPDEFAULT);
											SetWindowPos(hFilter, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
											RECT r;
											GetClientRect(hFilter, &r);
											RedrawWindow(hFilter, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_VALIDATE | RDW_UPDATENOW);
										}
									}
									else
									{
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_DODEFAULT);
									}

								}
							}
						}
						break;

						//click an item in playlist
					case NM_CLICK:
						{
							LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;
							m_iLastClickedItem = lpnmitem->iItem;
							// we need to remember this somewhere so that when the inplace editing begins we can edit the correct subitem
							m_iLastClickedSubitem = lpnmitem->iSubItem;
							if(tuniacApp.m_Preferences.GetShowAlbumArt() && tuniacApp.m_Preferences.GetArtOnSelection())
							{
								IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(lpnmitem->iItem);
								if(pIPE)
									tuniacApp.GetArt((LPTSTR)pIPE->GetField(FIELD_URL));
							}
						}
						break;

						//doubleclick item in playlist
					//case NM_DBLCLK:
					case LVN_ITEMACTIVATE:
						{
							LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;

							if(lpnmitem->iItem != -1)
							{
								tuniacApp.m_PlaylistManager.SetActivePlaylist(m_ulActivePlaylistIndex);
								tuniacApp.PlayEntry(m_pPlaylist->GetEntryAtNormalFilteredIndex(lpnmitem->iItem), true, false);
							}
						}
						break;

						//right click item in playlist
					case NM_RCLICK:
						{
							LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;
							m_iLastClickedItem			= lpnmitem->iItem;
							m_iLastClickedSubitem		= lpnmitem->iSubItem;

							HWND		hListHeader		= ListView_GetHeader(GetDlgItem(hDlg, IDC_PLAYLIST_LIST));

							POINT pt, cpt;
							GetCursorPos(&pt);
							GetCursorPos(&cpt);
							ScreenToClient(hListHeader, &cpt);
							RECT rc;
							Header_GetItemRect(hListHeader, 0, &rc);

							if(cpt.y > rc.bottom) // check user didn't right-click the coloumn headers
							{
								// check user clicked a valid item + didn't right-click the icon column(right click item menu)
								if(m_iLastClickedItem >= 0 && m_iLastClickedSubitem > 0)
								{
									IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(m_iLastClickedItem);
									if(pIPE)
									{
										EnableMenuItem(m_ItemMenu, ID_FILTERBYFIELD, MF_BYCOMMAND | (HeaderEntries[m_ColumnIDArray[m_iLastClickedSubitem - 1]].bFilterable ? MF_ENABLED : MF_GRAYED));
										EnableMenuItem(m_ItemMenu, ID_QUEUEBYFIELD, MF_BYCOMMAND | (HeaderEntries[m_ColumnIDArray[m_iLastClickedSubitem - 1]].bFilterable ? MF_ENABLED : MF_GRAYED));
										CheckMenuItem(m_ItemMenu, ID_PAUSEHERE, tuniacApp.m_SoftPause.ulAt == pIPE->GetEntryID() ? MF_CHECKED : MF_UNCHECKED);
										EnableMenuItem(m_ItemMenu, ID_SHOWFILE, MF_BYCOMMAND | (!PathIsURL((LPTSTR)pIPE->GetField(FIELD_URL)) ? MF_ENABLED : MF_GRAYED));
									}
									EnableMenuItem(m_ItemMenu, ID_PLAYNEXT, MF_BYCOMMAND | (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist() ? MF_ENABLED : MF_GRAYED));
									EnableMenuItem(m_ItemMenu, ID_QUEUE, MF_BYCOMMAND | (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist() ? MF_ENABLED : MF_GRAYED));
									EnableMenuItem(m_ItemMenu, ID_PAUSEHERE, MF_BYCOMMAND | (m_pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist() ? MF_ENABLED : MF_GRAYED));
									TrackPopupMenu(m_ItemMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_PlaylistSourceWnd, NULL);
								}
							}
							else
							{
								HDHITTESTINFO hdHitTest;
								hdHitTest.pt = cpt;
								SendMessage(hListHeader, HDM_HITTEST, 0, (LPARAM)&hdHitTest);
								m_iLastClickedSubitem = hdHitTest.iItem;

								bool bCanFilter = false;
								if(m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANFILTER)
								{
									if(m_iLastClickedSubitem > 0)
									{
										if(m_iLastClickedSubitem <= m_ColumnIDArray.GetCount())
										{
											if(HeaderEntries[m_ColumnIDArray[m_iLastClickedSubitem-1]].bFilterable)
											{
												bCanFilter = true;
											}
										}
									}
								}
								EnableMenuItem(m_HeaderMenu, ID_FILTERBY, (bCanFilter ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
								TrackPopupMenu(m_HeaderMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_PlaylistSourceWnd, NULL);
							}
						}
						break;

					case LVN_ODSTATECHANGED:
					case LVN_ITEMCHANGED:
						{
							HWND hListViewWnd = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);
							int iSelCount = ListView_GetSelectedCount(hListViewWnd);

							//alt key modifier held while selecting songs to cause play selected
							if(GetKeyState(VK_MENU) & 0x8000)
							{
								//keep previous items selected
								if(!(GetKeyState(VK_CONTROL) & 0x8000 || GetKeyState(VK_SHIFT) & 0x8000))
								{
									LVITEM lvi;
									lvi.stateMask = LVIS_SELECTED;
									lvi.state = LVIS_SELECTED;

									for(unsigned long i = 0; i < tuniacApp.m_PlaySelected.GetCount(); i++) 
									{
										SendMessage(hListViewWnd, LVM_SETITEMSTATE, tuniacApp.m_PlaySelected[i], WPARAM(&lvi));
									}
								}

								//rebuild playselected
								tuniacApp.m_PlaySelected.RemoveAll();

								int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
								if(iPos == -1)
									break;
								while(iPos != -1)
								{
									tuniacApp.m_PlaySelected.AddTail((unsigned long &)iPos);
									iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED);
								}
							}
							else
							{
								if(tuniacApp.m_PlaySelected.GetCount() == 1 && ListView_GetSelectedCount(hListViewWnd) == 1)
								{
									if(ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED) != tuniacApp.m_PlaySelected[0])
										tuniacApp.m_PlaySelected.RemoveAll();
								}
								else if(tuniacApp.m_PlaySelected.GetCount() != iSelCount)
								{
									tuniacApp.m_PlaySelected.RemoveAll();
								}
							}
							tuniacApp.RebuildFutureMenu();
						}
						break;

				}
			}
			break;

		case WM_UPDATE:
			{
				if(m_pPlaylist)
				{
					unsigned long Count = m_pPlaylist->GetNumItems();
					unsigned long TotalTime = 0;
					TCHAR		szStatusText[1024];

					for(unsigned long x=0; x<Count; x++)
					{
						IPlaylistEntry * pIPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(x);

						if(pIPE)
						{
							if((int)pIPE->GetField(FIELD_PLAYBACKTIME) != LENGTH_UNKNOWN && (int)pIPE->GetField(FIELD_PLAYBACKTIME) != LENGTH_STREAM)
								TotalTime += (int)pIPE->GetField(FIELD_PLAYBACKTIME) / 1000;
						}
					}

					TCHAR Time[256];
					TCHAR tstr[256];

					unsigned long secs	= TotalTime % 60;
					TotalTime /= 60;

					unsigned long mins	= TotalTime % 60;
					TotalTime /= 60;

					unsigned long hours = TotalTime % 24;
					TotalTime /= 24;

					unsigned long days	= TotalTime % 7;
					TotalTime /= 7;

					unsigned long weeks = TotalTime % 52;
					TotalTime /= 52;

					unsigned long years = TotalTime;

					StringCchCopy(Time, 256, TEXT(""));

					if(years)
					{
						if(years > 1)
							StringCchPrintf(tstr, 256, TEXT("%u Years, "), years);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Year, "), years);

						StringCchCat(Time, 256, tstr);
					}

					if(weeks)
					{
						if(weeks > 1)
							StringCchPrintf(tstr, 256, TEXT("%u Weeks, "), weeks);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Week, "), weeks);

						StringCchCat(Time, 256, tstr);
					}

					if(days)
					{
						if(days>1)
							StringCchPrintf(tstr, 256, TEXT("%u Days, "), days);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Day, "), days);

						StringCchCat(Time, 256, tstr);
					}

					if(hours)
					{
						if(hours>1)
							StringCchPrintf(tstr, 256, TEXT("%u Hours, "), hours);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Hour, "), hours);

						StringCchCat(Time, 256, tstr);
					}

					if(mins)
					{
						if(mins>1)
							StringCchPrintf(tstr, 256, TEXT("%u Minutes, "), mins);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Minute, "), mins);

						StringCchCat(Time, 256, tstr);
					}

					if(secs)
					{
						if(secs>1)
							StringCchPrintf(tstr, 256, TEXT("%u Seconds."), secs);
						else
							StringCchPrintf(tstr, 256, TEXT("%u Second."), secs);

						StringCchCat(Time, 256, tstr);
					}


					StringCchPrintf(szStatusText, 1024, TEXT("%s - %u Items - %s"), m_pPlaylist->GetPlaylistName(), Count, Time);
					tuniacApp.SetStatusText(szStatusText);

					ListView_SetItemCountEx(GetDlgItem(hDlg, IDC_PLAYLIST_LIST), m_pPlaylist->GetNumItems(), LVSICF_NOSCROLL);
				}
				else
				{
					ListView_SetItemCountEx(GetDlgItem(hDlg, IDC_PLAYLIST_LIST), 0, LVSICF_NOSCROLL);
				}
			}
			break;

		case WM_TIMER:
			{
				if(wParam == DELAYEDAPPLYFILTER_TIMERID)
				{
					KillTimer(hDlg, DELAYEDPLAY_TIMERID);

					TCHAR tBuffer[256];
					GetDlgItemText(hDlg, IDC_PLAYLIST_FILTER, tBuffer, 256);

					if(StrCmpI(tBuffer, m_pPlaylist->GetTextFilter()))
					{
						m_pPlaylist->SetTextFilter(tBuffer);
						m_pPlaylist->ApplyFilter();

						if(wcsnlen_s(tBuffer, 256))
						{
							ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_SHOW);
							ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_SHOW);
						}
						else
						{
							ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_HIDE);
							ShowWindow(GetDlgItem(hDlg, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_HIDE);
						}

						if(tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex() == tuniacApp.m_PlaylistManager.GetActivePlaylistIndex() && tuniacApp.m_Preferences.GetFollowCurrentSongMode())
						{
							if(tuniacApp.m_SourceSelectorWindow)
							{
								tuniacApp.m_SourceSelectorWindow->ShowCurrentlyPlaying();
							}
						}
					}
					break;
				}

				if(wParam == DELAYEDREDRAW_TIMERID)
				{
					KillTimer(hDlg, DELAYEDREDRAW_TIMERID);
					if(m_ShowingHeaderFilter)
					{
						HWND hFilter = GetDlgItem(hDlg, IDC_PLAYLISTSOURCE_FILTERCOMBO);
						RECT r;
						GetClientRect(hFilter, &r);
						RedrawWindow(hFilter, &r, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
					}
					break;
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				//Added - Mark
				//check for dragging
				if(m_Drag)
				{
					POINT		pt;
					POINT		ptThisWndPt;
					HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);

					POINTSTOPOINT(pt,MAKEPOINTS(lParam));

					ptThisWndPt = pt;

					ClientToScreen(m_PlaylistSourceWnd, &pt);

					if(tuniacApp.m_SourceSelectorWindow->IsPointInPlaylistSelector(&pt))
					{
						IPlaylist * pSPlaylist = tuniacApp.m_SourceSelectorWindow->GetPlaylistFromPoint(&pt);
						if(pSPlaylist == NULL)
						{
							SetCursor(m_NewPlaylistCursor);
						}
						else if (!(pSPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANADD))
						{
							SetCursor(::LoadCursor(NULL, IDC_NO));
						}
						else
						{
							SetCursor(m_AddCursor);
						}

						if(!IsRectEmpty(&m_draggedItemRect))
						{
							HDC dc = GetDC(hListViewWnd);
							DrawFocusRect(dc, &m_draggedItemRect);
							ReleaseDC(hListViewWnd, dc);

							SetRectEmpty(&m_draggedItemRect);
						}
					}
					else //if(WindowFromPoint(ptThisWndPt) == tuniacApp.getMainWindow())
					{
						if(m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANMOVE)
						{
							LVHITTESTINFO	HitTest;

							HitTest.flags		= 0;
							HitTest.pt			= pt;
							HitTest.iItem		= 0;
							HitTest.iSubItem	= 0;

							ScreenToClient(hListViewWnd, &HitTest.pt);
							if(ListView_HitTest(hListViewWnd, &HitTest) == -1)
							{
								if(HitTest.flags & LVHT_ABOVE)
								{
									SetCursor(::LoadCursor(NULL, IDC_NO));
								}
								else if(HitTest.flags & LVHT_BELOW)
								{
									SetCursor(::LoadCursor(NULL, IDC_NO));
								}
								else if(HitTest.flags & LVHT_NOWHERE)
								{
									// nothing - draw insert icon at bottom of list
									HDC dc = GetDC(hListViewWnd);

									DrawFocusRect(dc, &m_draggedItemRect);

									ListView_GetItemRect(hListViewWnd, ListView_GetItemCount(hListViewWnd)-1, &m_draggedItemRect, LVIR_BOUNDS);
									m_draggedItemRect.top = m_draggedItemRect.bottom - 2;

									DrawFocusRect(dc, &m_draggedItemRect);
									ReleaseDC(hListViewWnd, dc);

									SetCursor(::LoadCursor(NULL, IDC_ARROW));
								}
							}
							else
							{
								if(HitTest.flags & LVHT_ONITEM)
								{
									// draw insert icon before HitTest.iItem
									HDC dc = GetDC(hListViewWnd);

									DrawFocusRect(dc, &m_draggedItemRect);

									ListView_GetItemRect(hListViewWnd, HitTest.iItem, &m_draggedItemRect, LVIR_BOUNDS);
									m_draggedItemRect.bottom = m_draggedItemRect.top+2;

									DrawFocusRect(dc, &m_draggedItemRect);
									ReleaseDC(hListViewWnd, dc);
								}

								SetCursor(::LoadCursor(NULL, IDC_ARROW));
							}
						}
						else
						{
							// TODO: can't reorder this playlist!
							SetCursor(::LoadCursor(NULL, IDC_NO));
						}
					}
				}
			}
			break;

		case WM_LBUTTONUP:
			{
				//Added - Mark
				if(m_Drag)
				{
					POINT		pt;
					POINT		ptThisWndPt;
					HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_PLAYLIST_LIST);

					POINTSTOPOINT(pt, MAKEPOINTS(lParam));

					ptThisWndPt = pt;

					ClientToScreen(m_PlaylistSourceWnd, &pt);

					ReleaseCapture();
					m_Drag = false;

					if(tuniacApp.m_SourceSelectorWindow->IsPointInPlaylistSelector(&pt))
					{
						IPlaylistEntry	*	pPE = NULL;
						EntryArray			entryArray;

						int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
						if(iPos == -1)
							break;
						while(iPos != -1)
						{
							pPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos);

							if(pPE)
							{
								entryArray.AddTail(pPE);
							}

							iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
						}

						IPlaylist * pPlaylist = tuniacApp.m_SourceSelectorWindow->GetPlaylistFromPoint(&pt);

						if(pPlaylist)
						{
							if(pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANADD)
								((IPlaylistEX *)pPlaylist)->AddEntryArray(entryArray);
						}
						else
						{
							tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(TEXT("New Dropped Playlist"), entryArray);
							tuniacApp.m_SourceSelectorWindow->UpdateList();
						}

					}
					else if(ChildWindowFromPoint(hDlg, ptThisWndPt) == hListViewWnd)
					{
						if(m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANMOVE)
						{
							LVHITTESTINFO	HitTest;
							IndexArray		m_MoveArray;

							HitTest.pt = pt;

							ScreenToClient(hListViewWnd, &HitTest.pt);

							int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
							if(iPos == -1)
								break;
							while(iPos != -1)
							{
								m_MoveArray.AddTail((unsigned long &)iPos);
								iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED);
							}

							ListView_SetItemState(hListViewWnd, -1, 0, LVIS_SELECTED);

							int ItemHit = ListView_HitTest(hListViewWnd, &HitTest);
							if(ItemHit == -1)
							{
								if(HitTest.flags & LVHT_ABOVE)
								{
									//POSSIBLY
									// get the top index and move the items there
									m_pPlaylist->MoveNormalFilteredIndexArray(ListView_GetTopIndex(hListViewWnd), m_MoveArray);
								}
								else if(HitTest.flags & LVHT_BELOW)
								{
									//POSSIBLY
									// move items to the bottom index currently displayed
								}
								else if(HitTest.flags & LVHT_NOWHERE)
								{
									// move items to end of list
									m_pPlaylist->MoveNormalFilteredIndexArray(m_pPlaylist->GetNumItems(), m_MoveArray);
								}
							}
							else
							{
								//move items to ItemHit
								m_pPlaylist->MoveNormalFilteredIndexArray(ItemHit, m_MoveArray);
							}
						}
					}

					SetRectEmpty(&m_draggedItemRect);
					SetCursor(::LoadCursor(NULL, IDC_ARROW));
					tuniacApp.RebuildFutureMenu();
					Update();
				}
			}
			break;

			case WM_SHOWACTIVEITEM:
				{
					HWND		hListView  = GetDlgItem(hDlg, IDC_PLAYLIST_LIST);
					unsigned long ulActiveIndex = m_pPlaylist->GetActiveNormalFilteredIndex();
					ListView_EnsureVisible(hListView, ulActiveIndex, FALSE);
					ListView_SetSelectionMark(hListView, ulActiveIndex);
					Update();
				}
				break;

			case WM_SETFOCUSFILTER:
				{


						//SetForegroundWindow(m_PlaylistSourceWnd);
						//BringWindowToTop(m_PlaylistSourceWnd);
						//SetActiveWindow(m_PlaylistSourceWnd);

						PostMessage(m_PlaylistSourceWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_FILTER), TRUE);
				}
				break;


		default:
			return FALSE;
			break;
	}

	return TRUE;
}


bool CPlaylistSourceView::SetPlaylistSource(unsigned long ulPlaylistIndex)
{
	IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(ulPlaylistIndex);
	if(pPlaylist)
	{
		if(pPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
		{
			m_pPlaylist = (IPlaylistEX *)pPlaylist;
			m_ulActivePlaylistIndex = ulPlaylistIndex;
			if(m_pPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANFILTER)
			{
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_FILTER), SW_SHOWNOACTIVATE);
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_SELECTFILTERBYFIELD), SW_SHOWNOACTIVATE);

				SetDlgItemText(m_PlaylistSourceWnd, IDC_PLAYLIST_FILTER, m_pPlaylist->GetTextFilter());
				if(wcsnlen_s(m_pPlaylist->GetTextFilter(), 128) < 1)
				{
					ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_HIDE);
					ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_HIDE);
				}
				else
				{
					ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_SHOWNOACTIVATE);
					ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_SHOWNOACTIVATE);
				}

				unsigned long ulFilterByField = m_pPlaylist->GetTextFilterField();
				int iMenuPos = 0;
				if(ulFilterByField == FIELD_ARTIST)
					iMenuPos = 2;
				else if(ulFilterByField == FIELD_ALBUM)
					iMenuPos = 3;
				else if(ulFilterByField == FIELD_TITLE)
					iMenuPos = 4;
				else if(ulFilterByField == FIELD_GENRE)
					iMenuPos = 5;
				else if(ulFilterByField == FIELD_YEAR)
					iMenuPos = 6;
				else if(ulFilterByField == FIELD_COMMENT)
					iMenuPos = 7;
				else if (ulFilterByField == FIELD_ALBUMARTIST)
					iMenuPos = 8;
				else if (ulFilterByField == FIELD_COMPOSER)
					iMenuPos = 9;
				else if(ulFilterByField == FIELD_URL)
					iMenuPos = 11;
				else if(ulFilterByField == FIELD_FILENAME)
					iMenuPos = 12;
				else if(ulFilterByField == FIELD_FILEEXTENSION)
					iMenuPos = 13;

				CheckMenuRadioItem(m_FilterByFieldMenu, 0, 13, iMenuPos, MF_BYPOSITION);

				CheckMenuItem(m_FilterByFieldMenu, 15, MF_BYPOSITION | (m_pPlaylist->GetTextFilterReversed() ? MF_CHECKED : MF_UNCHECKED));
			}
			else
			{
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_CLEARFILTER), SW_HIDE);
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_MAKEPLAYLIST), SW_HIDE);
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_FILTER), SW_HIDE);
				ShowWindow(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_SOURCE_SELECTFILTERBYFIELD), SW_HIDE);
			}
			tuniacApp.RebuildFutureMenu();
			Redraw();
			Update();
		}
	}
	return true;
}

bool CPlaylistSourceView::Update(void)
{
	PostMessage(m_PlaylistSourceWnd, WM_UPDATE, 0, 0);
	return true;
}

bool	CPlaylistSourceView::ShowCurrentItem(void)
{
	PostMessage(m_PlaylistSourceWnd, WM_SHOWACTIVEITEM, 0, 0);
	return true;
}

void CPlaylistSourceView::Redraw(void)
{
	RedrawWindow(m_PlaylistSourceWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void	CPlaylistSourceView::SetFocusFilter(void)
{
	PostMessage(m_PlaylistSourceWnd, WM_SETFOCUSFILTER, 0, 0);
}

void	CPlaylistSourceView::ClearTextFilter(void)
{
	SetDlgItemText(m_PlaylistSourceWnd, IDC_PLAYLIST_FILTER, TEXT(""));
}

bool CPlaylistSourceView::UpdateColumns(void)
{
	HWND		hListView  = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);
	LVCOLUMN	lvC;

	HWND Header = ListView_GetHeader(hListView);
	int NumCols = Header_GetItemCount(Header);
	if(NumCols)
	{
		for(int x=0; x<NumCols; x++)
		{
			ListView_DeleteColumn(hListView, 0);
		}
	}
	
	ZeroMemory(&lvC, sizeof(LVCOLUMN));

	lvC.mask	= LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;

	lvC.pszText = TEXT("");
	lvC.cx		= 20;
	lvC.fmt		= 0;
	ListView_InsertColumn(hListView, 0, &lvC);

	for(unsigned long t=0; t<m_ColumnIDArray.GetCount(); t++)
	{
		lvC.pszText		= HeaderEntries[m_ColumnIDArray[t]].szHeaderText;
		lvC.cx			= HeaderEntries[m_ColumnIDArray[t]].iHeaderWidth;
		lvC.fmt			= HeaderEntries[m_ColumnIDArray[t]].ulFlags;
		lvC.iSubItem	= t;

		ListView_InsertColumn(hListView, t+1, &lvC);
	}

	Update();

	return true;
}

//Added Mark 7th October
bool CPlaylistSourceView::EditTrackInfo(void)
{
	//get the list of files here
	//TODO spin into seperate function
							
	//Add to object
	//m_TagEditor.AddListToEdit(//pointer to EntryArray);

	//display the dialog


	if(m_iLastClickedItem < 0)
		return false;

	IPlaylistEntry	*	pPE = NULL;
	EntryArray			entryArray;

	HWND		hListViewWnd	= GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);
	int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
	if(iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED) != -1)
	{
		while(iPos != -1)
		{
			pPE = m_pPlaylist->GetEntryAtNormalFilteredIndex(iPos);
			if(pPE)
			{
				entryArray.AddTail(pPE);
			}
			iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED) ;
		}
		m_TagEditor.AddListToEdit(&entryArray);
	}

	return	m_TagEditor.ShowEditor(m_PlaylistSourceWnd);
}

void	CPlaylistSourceView::DeselectItem(unsigned long ulIndex)
{
		ListView_SetItemState(GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST), ulIndex, 0, LVIS_SELECTED);
}

bool	CPlaylistSourceView::GetSelectedIndexes(IndexArray & indexArray)
{
	indexArray.RemoveAll();
	HWND hListViewWnd = GetDlgItem(m_PlaylistSourceWnd, IDC_PLAYLIST_LIST);
	int iPos = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED);
	if(iPos == -1)
		return false;

	while(iPos != -1)
	{
		indexArray.AddTail((unsigned long &)iPos);
		iPos = ListView_GetNextItem(hListViewWnd, iPos, LVNI_SELECTED);
	}
	return true;
}