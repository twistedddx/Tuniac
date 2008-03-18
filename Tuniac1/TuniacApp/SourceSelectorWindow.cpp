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

#include "resource.h"
#include "sourceselectorwindow.h"

#include "DoubleBuffer.h"

#define LIGHTCOL				RGB(255,255,255)
#define DARKCOL					RGB(237,243,254)

#define SPLITMINSIZE			125
#define SPLITMAXSIZE			300

#define WM_UPDATELIST			(WM_APP+34)
#define WM_RENAMEITEM			(WM_APP+78)


CSourceSelectorWindow::CSourceSelectorWindow(void) :
	m_hSourceWnd(NULL),
	m_bTrackingDrag(false),
	m_bSourceDrag(false),
	m_bSourceAltDrag(false),
	m_AltDragDest(-1),
	m_pVisibleView(NULL),
	m_ulVisiblePlaylistIndex(-1)
{
	m_ulSeparatorX = tuniacApp.m_Preferences.GetSourceViewDividerX();

	ISourceView	* t;

	t = m_PlaylistSourceView = new CPlaylistSourceView();
	m_SourceViewArray.AddTail(t);

	t = m_AudioCDSourceView = new CAudioCDSourceView();
	m_SourceViewArray.AddTail(t);

//	t = m_RadioSourceView = new CRadioSourceView();
//	m_SourceViewArray.AddTail(t);

	m_hMenu = GetSubMenu(LoadMenu(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDR_SOURCESELECT_MENU)), 0);
	m_hDragMenu = GetSubMenu(LoadMenu(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDR_SOURCESELECT_DRAGMENU)), 0);
}

CSourceSelectorWindow::~CSourceSelectorWindow(void)
{
	tuniacApp.m_Preferences.SetSourceViewDividerX(m_ulSeparatorX);
}

void			CSourceSelectorWindow::Destroy(void)
{
	delete this;
}

LPTSTR			CSourceSelectorWindow::GetName(void)
{
	return TEXT("Source Selector");
}

GUID			CSourceSelectorWindow::GetPluginID(void)
{
	// {035A43D3-9AF0-4fd0-BDBA-BBA93EB3976E}
	static const GUID SOURCESELECTORGUID = { 0x35a43d3, 0x9af0, 0x4fd0, { 0xbd, 0xba, 0xbb, 0xa9, 0x3e, 0xb3, 0x97, 0x6e } };

	return SOURCESELECTORGUID;
}

unsigned long	CSourceSelectorWindow::GetFlags(void)
{
	return  0;
}

bool			CSourceSelectorWindow::CreatePluginWindow(HWND hParent, HINSTANCE hInst)
{
	m_hSourceWnd = CreateDialogParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_SOURCESELECTOR), hParent, (DLGPROC)WndProcStub, (DWORD_PTR)this);

	if(m_hSourceWnd == NULL)
		return false;

	UpdateList();

	return true;
}

bool			CSourceSelectorWindow::DestroyPluginWindow(void)
{
	if(m_hSourceWnd)
	{
		DestroyWindow(m_hSourceWnd);
		return true;
	}

	return false;
}

bool			CSourceSelectorWindow::Show(void)
{
	if(m_hSourceWnd)
	{
		ShowWindow(m_hSourceWnd, SW_SHOW);
		return true;
	}

	return false;
}

bool			CSourceSelectorWindow::Hide(void)
{
	if(m_hSourceWnd)
	{
		ShowWindow(m_hSourceWnd, SW_HIDE);
		return true;
	}

	return false;
}

bool			CSourceSelectorWindow::SetPos(int x, int y, int w, int h)
{
	if(m_hSourceWnd)
	{
		SetWindowPos(m_hSourceWnd, NULL, x, y, w, h, SWP_NOZORDER);
		return true;
	}

	return false;
}


LRESULT CALLBACK			CSourceSelectorWindow::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
				for(unsigned long x=0; x<m_SourceViewArray.GetCount(); x++)
				{
					m_SourceViewArray[x]->CreateSourceView(hDlg);
					m_SourceViewArray[x]->ShowSourceView(false);
				}

				ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_SOURCESELECTOR), LVS_EX_FULLROWSELECT);

				LV_COLUMN lvC;

				// Create columns.
				lvC.mask	= LVCF_WIDTH | LVCF_TEXT;
				lvC.cx		= 200;

				lvC.pszText	= TEXT("Source");
				ListView_InsertColumn(GetDlgItem(hDlg, IDC_SOURCESELECTOR), 0, &lvC);

				HIMAGELIST hList  = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 3, 1); 

				// 0 : unknown playlist
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_STANDARD)); 

				// 1: Library
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_MEDIALIBRARY));

				// 2: Standard Playlist
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_STANDARD)); 

				// 3: smart playlist
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_STANDARD)); 

				// 4: CD playlist
				ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_CD)); 

				// 5: Radio!
				//ImageList_AddIcon(hList, tuniacApp.m_Skin.GetIcon(THEMEICON_PLAYLIST_RADIO)); 

				ListView_SetImageList(GetDlgItem(hDlg, IDC_SOURCESELECTOR), hList, LVSIL_SMALL);

				ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex);
			}
			break;

		case WM_DESTROY:
			{
				for(unsigned long x=0; x<m_SourceViewArray.GetCount(); x++)
				{
					m_SourceViewArray[x]->ShowSourceView(false);
					m_SourceViewArray[x]->DestroySourceView();
				}
			}
			break;

		case WM_TIMER:
			{
				if(wParam == 0)
				{
					ReleaseCapture();
					m_bTrackingDrag = false;
					m_bTrackingMoved = false;
				}
				KillTimer(hDlg, wParam);
			}
			break;

		case WM_SETCURSOR:
			{
				POINT pt;
				RECT		rcWindowRect;
				RECT		r;

				GetClientRect(hDlg, &rcWindowRect);
				SetRect(&r, (m_ulSeparatorX), 0, (m_ulSeparatorX+SEPERATOR_WIDTH), rcWindowRect.bottom);

				GetCursorPos(&pt);

				ScreenToClient(hDlg, &pt);

				if(PtInRect(&r, pt))
				{
					SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				}
				else
				{
					return FALSE;
				}
			}
			break;

		case WM_LBUTTONDOWN:
			{
				POINT pt;
				RECT		rcWindowRect;
				RECT		r;

				POINTSTOPOINT(pt, MAKEPOINTS(lParam));

				GetClientRect(hDlg, &rcWindowRect);
				SetRect(&r, (m_ulSeparatorX), 0, (m_ulSeparatorX+SEPERATOR_WIDTH), rcWindowRect.bottom);

				if(PtInRect(&r, pt))
				{
					m_iDragOffset = m_ulSeparatorX - pt.x;

					m_bTrackingDrag = true;
					SetCapture(hDlg);
				}
			}
			break;

		case WM_LBUTTONUP:
			{
				if(m_bTrackingDrag)
				{
					SetTimer(m_hSourceWnd, 0, m_bTrackingMoved ? 0 : GetDoubleClickTime(), NULL);
				}
				else if(m_bSourceDrag && !m_bSourceAltDrag)
				{
					POINT		pt;
					POINT		ptThisWndPt;
					HWND		hListViewWnd	= GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR);

					POINTSTOPOINT(pt, MAKEPOINTS(lParam));

					ptThisWndPt = pt;

					ClientToScreen(m_hSourceWnd, &pt);

					ReleaseCapture();
					m_bSourceDrag = false;

					if(tuniacApp.m_SourceSelectorWindow->IsPointInPlaylistSelector(&pt))
					{
						IPlaylist * pIP = GetPlaylistFromPoint(&pt);
						if(pIP == NULL || (pIP != NULL && pIP->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST) && GetVisiblePlaylist()->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST)
						{
							LVHITTESTINFO	HitTest;
							HitTest.pt		= pt;
							ScreenToClient(hListViewWnd, &HitTest.pt);
							unsigned long				iItemHit = ListView_HitTest(hListViewWnd, &HitTest);
							
							if(iItemHit > 0 && GetVisiblePlaylistIndex() < iItemHit)
								iItemHit--;

							tuniacApp.m_PlaylistManager.MoveStandardPlaylist(GetVisiblePlaylistIndex(), iItemHit);
							UpdateList();
							ShowPlaylistAtIndex(iItemHit);
						}
					}
				}
			}
			break;

		case WM_LBUTTONDBLCLK:
			{
				if(m_bTrackingDrag)
				{
					ReleaseCapture();
					m_bTrackingDrag = false;

					RECT		rcWindowRect;
					GetClientRect(hDlg, &rcWindowRect);

					if(m_ulSeparatorX == SEPERATOR_WIDTH - 1)
						m_ulSeparatorX = m_ulSeparatorXOld;
					else
					{
						m_ulSeparatorXOld = m_ulSeparatorX;
						m_ulSeparatorX = SEPERATOR_WIDTH - 1;
					}
					SendMessage(hDlg, WM_SIZE, 0, MAKELPARAM(rcWindowRect.right, rcWindowRect.bottom));
				}
			}
			break;

		case WM_RBUTTONUP:
			{
				if(m_bSourceDrag && m_bSourceAltDrag)
				{
					POINT		pt;
					POINT		ptThisWndPt;
					HWND		hListViewWnd	= GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR);

					POINTSTOPOINT(pt, MAKEPOINTS(lParam));

					ptThisWndPt = pt;

					ClientToScreen(m_hSourceWnd, &pt);

					ReleaseCapture();
					m_bSourceDrag = false;
					m_bSourceAltDrag = false;

					if(tuniacApp.m_SourceSelectorWindow->IsPointInPlaylistSelector(&pt))
					{
						IPlaylist * pIP = GetPlaylistFromPoint(&pt);
						if(pIP == NULL || (pIP != NULL && pIP->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST) && GetVisiblePlaylist()->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST)
						{
							LVHITTESTINFO	HitTest;
							HitTest.pt		= pt;
							ScreenToClient(hListViewWnd, &HitTest.pt);
							int				iItemHit = ListView_HitTest(hListViewWnd, &HitTest);
							
							if(iItemHit != -1 && iItemHit != GetVisiblePlaylistIndex())
							{
								m_AltDragDest = iItemHit;
								POINT pt;
								GetCursorPos(&pt);
								TrackPopupMenu(m_hDragMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hSourceWnd, NULL);
							}
						}
					}
					ListView_SetItemState(hListViewWnd, -1, 0, LVIS_DROPHILITED);
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				if(m_bTrackingDrag)
				{
					m_bTrackingMoved = true;
					if(wParam & MK_LBUTTON)
					{
						POINT pt;
						RECT		rcWindowRect;

						POINTSTOPOINT(pt, MAKEPOINTS(lParam));

						GetClientRect(hDlg, &rcWindowRect);

						m_ulSeparatorX = min(max(pt.x+m_iDragOffset, SPLITMINSIZE), SPLITMAXSIZE);

						SendMessage(hDlg, WM_SIZE, 0, MAKELPARAM(rcWindowRect.right, rcWindowRect.bottom));
					}
					else
					{
						KillTimer(hDlg, 0);
						SendMessage(hDlg, WM_TIMER, 0, 0);

					}
				}
				else if(m_bSourceDrag)
				{
					POINT		pt;
					POINT		ptThisWndPt;
					HWND		hListViewWnd	= GetDlgItem(hDlg, IDC_SOURCESELECTOR);

					POINTSTOPOINT(pt, MAKEPOINTS(lParam));

					ptThisWndPt = pt;

					ClientToScreen(m_hSourceWnd, &pt);

					if(IsPointInPlaylistSelector(&pt))
					{

						LVHITTESTINFO	HitTest;

						HitTest.flags		= 0;
						HitTest.pt			= pt;
						HitTest.iItem		= 0;
						HitTest.iSubItem	= 0;

						ScreenToClient(hListViewWnd, &HitTest.pt);
						if(ListView_HitTest(hListViewWnd, &HitTest) == -1)
						{
							if(HitTest.flags & LVHT_ABOVE || HitTest.flags & LVHT_BELOW)
							{
								SetCursor(::LoadCursor(NULL, IDC_NO));
							}
							else if(HitTest.flags & LVHT_NOWHERE)
							{
								// nothing - draw insert icon at bottom of list
								HDC dc = GetDC(hListViewWnd);

								DrawFocusRect(dc, &m_SourceDraggedRect);

								ListView_GetItemRect(hListViewWnd, ListView_GetItemCount(hListViewWnd)-1, &m_SourceDraggedRect, LVIR_BOUNDS);
								m_SourceDraggedRect.top = m_SourceDraggedRect.bottom - 2;

								DrawFocusRect(dc, &m_SourceDraggedRect);
								ReleaseDC(hListViewWnd, dc);

								SetCursor(::LoadCursor(NULL, IDC_ARROW));
							}
						}
						else
						{
							if(HitTest.flags & LVHT_ONITEM)
							{
								IPlaylist * pIP = GetPlaylistFromPoint(&pt);
								if(pIP != NULL && pIP->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST && GetVisiblePlaylist()->GetPlaylistType() == PLAYLIST_TYPE_STANDARDPLAYLIST)
								{
									if(!m_bSourceAltDrag)
									{
										if(HitTest.iItem == GetVisiblePlaylistIndex())
										{
											SetCursor(::LoadCursor(NULL, IDC_NO));
										}
										else
										{
											// draw insert icon before HitTest.iItem
											HDC dc = GetDC(hListViewWnd);

											DrawFocusRect(dc, &m_SourceDraggedRect);

											ListView_GetItemRect(hListViewWnd, HitTest.iItem, &m_SourceDraggedRect, LVIR_BOUNDS);
											m_SourceDraggedRect.bottom = m_SourceDraggedRect.top+2;

											DrawFocusRect(dc, &m_SourceDraggedRect);
											ReleaseDC(hListViewWnd, dc);
										}
									}
									else
									{
										if(pIP == NULL || HitTest.iItem == GetVisiblePlaylistIndex())
										{
											ListView_SetItemState(hListViewWnd, -1, 0, LVIS_DROPHILITED);
											SetCursor(::LoadCursor(NULL, IDC_NO));
										}
										else
										{
											ListView_SetItemState(hListViewWnd, -1, 0, LVIS_DROPHILITED);
											ListView_SetItemState(hListViewWnd, HitTest.iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
										}
									}
								}
								else
								{
									SetCursor(::LoadCursor(NULL, IDC_NO));
								}
							}

							SetCursor(::LoadCursor(NULL, IDC_ARROW));
						}
					}
					else
					{
						SetCursor(::LoadCursor(NULL, IDC_NO));
					}

				}
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT		ps;
				RECT			r;
				CDoubleBuffer	doubleBuffer;

				GetClientRect(hDlg, &r);

				r.right		= m_ulSeparatorX+10;
				r.bottom	= 30;

				HDC		hDC = BeginPaint(hDlg, &ps);
				doubleBuffer.Begin(hDC, &r);

				FillRect(hDC, &r, GetSysColorBrush(COLOR_BTNFACE));

				r.left += 15;
				SetBkMode(hDC, TRANSPARENT);
				SelectObject(hDC, tuniacApp.GetTuniacFont(FONT_SIZE_LARGE));
				DrawText(hDC, TEXT("Source"), -1, &r, DT_SINGLELINE | DT_VCENTER);

				doubleBuffer.End(hDC);
				EndPaint(hDlg, &ps);

			}
			break;

		case WM_SIZE:
			{
				WORD Width = LOWORD(lParam);
				WORD Height = HIWORD(lParam);

				MoveWindow(	GetDlgItem(hDlg, IDC_SOURCESELECTOR),	
							2,
							28, 
							m_ulSeparatorX-2,
							Height - 64, 
							TRUE);

				MoveWindow(	GetDlgItem(hDlg, IDC_SOURCE_ADDPLAYLIST),
							0,
							Height - 29,
							(m_ulSeparatorX/2)-2,
							25,
							TRUE);

				MoveWindow(	GetDlgItem(hDlg, IDC_SOURCE_DELPLAYLIST),
							(m_ulSeparatorX/2)+2,
							Height - 29,
							(m_ulSeparatorX/2)-2,
							25,
							TRUE);

				for(unsigned long x=0; x<m_SourceViewArray.GetCount(); x++)
				{
					m_SourceViewArray[x]->MoveSourceView(m_ulSeparatorX+SEPERATOR_WIDTH,	0, Width-(m_ulSeparatorX+SEPERATOR_WIDTH),		Height);
				}

				RECT r = {0,0, m_ulSeparatorX+10, Height};

				RedrawWindow(hDlg, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_VALIDATE | RDW_UPDATENOW);

				ListView_SetColumnWidth(GetDlgItem(hDlg, IDC_SOURCESELECTOR), 0, LVSCW_AUTOSIZE_USEHEADER);
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{

					case IDC_SOURCE_ADDPLAYLIST:
						{
							TCHAR str[60];

							wsprintf(str, TEXT("Untitled Playlist"));

							if(tuniacApp.m_PlaylistManager.CreateNewStandardPlaylist(str))
							{
								// TODO: insert rename code here
								unsigned long lastlist = tuniacApp.m_PlaylistManager.GetNumPlaylists() -1;

								UpdateList();

								PostMessage(hDlg, WM_RENAMEITEM, 0, lastlist);
							}

						}
						break;

					case IDC_SOURCE_DELPLAYLIST:
						{
							if(m_ulVisiblePlaylistIndex == 0)
								break;

							if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(m_ulVisiblePlaylistIndex))
							{
								CCoreAudio::Instance()->Reset();
							}

							if(tuniacApp.m_PlaylistManager.DeletePlaylistAtIndex(m_ulVisiblePlaylistIndex))
							{
								if(m_ulVisiblePlaylistIndex >= tuniacApp.m_PlaylistManager.GetNumPlaylists())
									ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex-1);
								else
									ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex);

								UpdateList();
							}
						}
						break;

					case ID_RENAME:
						{
							int iSel = ListView_GetSelectionMark(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR));
							if(iSel >= 0)
							{
								if(tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(iSel)->GetFlags() & PLAYLIST_FLAGS_CANRENAME)
									ListView_EditLabel(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), iSel);
							}
						}
						break;

					case ID_REMOVE:
						{
							int iSel = ListView_GetSelectionMark(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR));
							if(iSel < 0) break;
							if(tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(iSel)->GetPlaylistType() == PLAYLIST_TYPE_MEDIALIBRARY) break;

							if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(iSel))
							{
								CCoreAudio::Instance()->Reset();
							}

							if(tuniacApp.m_PlaylistManager.DeletePlaylistAtIndex(iSel))
							{
								if(m_ulVisiblePlaylistIndex >= tuniacApp.m_PlaylistManager.GetNumPlaylists())
									ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex-1);
								else
									ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex);

								UpdateList();
							}
						}
						break;

					case ID_PLAYPLAYLIST:
						{
							int iSel = ListView_GetSelectionMark(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR));
							if(iSel < 0) break;

							CCoreAudio::Instance()->Reset();
							ShowPlaylistAtIndex(iSel);
							tuniacApp.m_PlaylistManager.SetActivePlaylist(m_ulVisiblePlaylistIndex);
							if(tuniacApp.m_PlaylistManager.GetActivePlaylist()->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
							{
								IPlaylistEX * pPlaylist = (IPlaylistEX *)tuniacApp.m_PlaylistManager.GetActivePlaylist();
								pPlaylist->SetActiveFilteredIndex(0);
							}
							SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
						}
						break;

						//remove duplicate entries from a playlist
					case ID_REMOVEDUPLICATES:
						{
							int iSel = ListView_GetSelectionMark(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR));
							if(iSel < 0) break;

							ShowPlaylistAtIndex(iSel);
							if(GetVisiblePlaylist()->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
							{
								IPlaylistEX * pPlaylist = (IPlaylistEX *)GetVisiblePlaylist();
								
								IPlaylistEntry * pActiveEntry = NULL;
								if(pPlaylist == tuniacApp.m_PlaylistManager.GetActivePlaylist())
									pActiveEntry = tuniacApp.m_PlaylistManager.GetActivePlaylist()->GetActiveItem();

								IPlaylistEntry * pEntry1;
								IPlaylistEntry * pEntry2;
								IndexArray deleteArray;

								for(unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
								{
									pEntry1 = pPlaylist->GetItemAtNormalFilteredIndex(i);
									for(unsigned long j = 0; j < i; j++)
									{
										pEntry2 = pPlaylist->GetItemAtNormalFilteredIndex(j);
										if(StrCmpI((LPTSTR)pEntry1->GetField(FIELD_URL), (LPTSTR)pEntry2->GetField(FIELD_URL)) == 0)
										{
											if(pEntry1 == pActiveEntry)
												deleteArray.AddTail((int &)j);
											else
												deleteArray.AddTail((int &)i);
											break;
										}
									}
								}

								pPlaylist->DeleteItemArray(deleteArray);

								if(pActiveEntry != NULL)
								{
									for(unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
									{
										if(pPlaylist->GetItemAtNormalFilteredIndex(i) == pActiveEntry)
										{
											pPlaylist->SetActiveFilteredIndex(i);
											break;
										}
									}
								}
							}

						}
						break;

					case ID_EXPORT:
						{
							int iSel = ListView_GetSelectionMark(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR));
							if(iSel < 0) break;

							ShowPlaylistAtIndex(iSel);
							if(GetVisiblePlaylist()->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
							{
								IPlaylistEX * pPlaylist = (IPlaylistEX *)GetVisiblePlaylist();
								IPlaylistEntry * pEntry;
								EntryArray exportArray;
								for(unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
								{
									pEntry = pPlaylist->GetItemAtNormalFilteredIndex(i);
									exportArray.AddTail(pEntry);
								}
								tuniacApp.m_MediaLibrary.m_ImportExport.Export(exportArray, NULL);
							}
						}
						break;

					case ID_COPYTO:
						{
							IPlaylist * pSource = GetVisiblePlaylist();
							IPlaylist * pDest = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(m_AltDragDest);
							if(pSource->GetFlags() & PLAYLIST_FLAGS_EXTENDED && pDest->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
							{
								IPlaylistEntry *	pPE = NULL;
								EntryArray			EA;
								for(unsigned long i = 0; i < ((IPlaylistEX *)pSource)->GetNumItems(); i++)
								{
									pPE = ((IPlaylistEX *)pSource)->GetItemAtNormalFilteredIndex(i);
									EA.AddTail(pPE);
								}
								((IPlaylistEX *)pDest)->AddEntryArray(EA);
							}
						}
						break;

					case ID_MERGEINTO:
						{
							SendMessage(m_hSourceWnd, WM_COMMAND, MAKELONG(ID_COPYTO, 0), 0);
							SendMessage(m_hSourceWnd, WM_COMMAND, MAKELONG(ID_REMOVE, 0), 0);
						}
						break;

					case ID_REMOVEFROM:
						{
							IPlaylist * pSource = GetVisiblePlaylist();
							IPlaylist * pDest = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(m_AltDragDest);
							if(pSource->GetFlags() & PLAYLIST_FLAGS_EXTENDED && pDest->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
							{
								IndexArray IA;
								for(unsigned long i = 0; i < ((IPlaylistEX *)pSource)->GetNumItems(); i++)
								{
									for(unsigned long j = 0; j < ((IPlaylistEX *)pDest)->GetNumItems(); j++)
									{
										if(((IPlaylistEX *)pDest)->GetItemAtNormalFilteredIndex(j)->GetEntryID() == ((IPlaylistEX *)pSource)->GetItemAtNormalFilteredIndex(i)->GetEntryID())
										{
											IA.AddTail((int &)j);
											continue;
										}
									}
								}
								((IPlaylistEX *)pDest)->DeleteItemArray(IA);
							}
						}
						break;
					
				}

			}
			break;

		case WM_NOTIFY:
			{
				UINT idCtrl = wParam;
				LPNMHDR lpNotify = (LPNMHDR)lParam;

				switch(lpNotify->code)
				{

					case LVN_KEYDOWN:
						{
							HWND			hListViewWnd	= lpNotify->hwndFrom;
							LPNMLVKEYDOWN	pnkd			= (LPNMLVKEYDOWN) lParam;

							switch(pnkd->wVKey)
							{
								case VK_DELETE:
									{
										if(m_ulVisiblePlaylistIndex == 0)
											break;

										if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(m_ulVisiblePlaylistIndex))
										{
											CCoreAudio::Instance()->Reset();
										}

										if(tuniacApp.m_PlaylistManager.DeletePlaylistAtIndex(m_ulVisiblePlaylistIndex))
										{
											if(m_ulVisiblePlaylistIndex >= tuniacApp.m_PlaylistManager.GetNumPlaylists())
												ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex-1);
											else
												ShowPlaylistAtIndex(m_ulVisiblePlaylistIndex);

											UpdateList();
										}
									}
									break;

								default:
									return FALSE;
									break;
							}
						}
						break;

					case LVN_GETDISPINFO:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *) lParam;
							if(pDispInfo->item.iItem != -1)
							{
								int x = pDispInfo->item.iItem;

								IPlaylist * pList = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);

								if(pList)
								{
									pDispInfo->item.pszText = pList->GetPlaylistName();
									pDispInfo->item.iImage	= pList->GetPlaylistType();
								}

							}
						}
						break;

					case LVN_BEGINLABELEDIT:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *)lParam;

							if(pDispInfo->item.iItem != -1)
							{
								int x = pDispInfo->item.iItem;

								IPlaylist * pList = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
								if(pList)
								{
									if(pList->GetFlags() & PLAYLIST_FLAGS_CANRENAME)
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
									else
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
								}
								else
								{
									SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
								}
							}
						}
						break;

					case LVN_ENDLABELEDIT:
						{
							NMLVDISPINFO * pDispInfo = (NMLVDISPINFO *)lParam;

							if(pDispInfo->item.iItem != -1)
							{
								int x = pDispInfo->item.iItem;

								IPlaylist * pList = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
								if(pList)
								{
									if(pList->GetFlags() & PLAYLIST_FLAGS_CANRENAME)
									{
										pList->SetPlaylistName(pDispInfo->item.pszText);
									}
								}

								m_PlaylistSourceView->Redraw();

							}
						}
						break;

					case NM_CLICK:
					case LVN_ITEMACTIVATE:
						{
							LPNMITEMACTIVATE lpnmitem	= (LPNMITEMACTIVATE)lParam;

							if(lpnmitem->iItem != -1)
							{
								ShowPlaylistAtIndex(lpnmitem->iItem);
							}
							else
							{
								ListView_SetItemState(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), m_ulVisiblePlaylistIndex, LVIS_SELECTED, LVIS_SELECTED);
							}
						}
						break;

					case NM_RCLICK:
						{
							POINT pt;
							GetCursorPos(&pt);
							IPlaylist * pSPlaylist = GetPlaylistFromPoint(&pt);
							if (pSPlaylist != NULL) 
							{
								EnableMenuItem(m_hMenu, ID_RENAME, MF_BYCOMMAND | (pSPlaylist->GetFlags() & PLAYLIST_FLAGS_CANRENAME ? MF_ENABLED : MF_GRAYED));
								EnableMenuItem(m_hMenu, ID_REMOVE, MF_BYCOMMAND | (pSPlaylist->GetPlaylistType() != PLAYLIST_TYPE_MEDIALIBRARY ? MF_ENABLED : MF_GRAYED));
								EnableMenuItem(m_hMenu, ID_REMOVEDUPLICATES, MF_BYCOMMAND | (pSPlaylist->GetFlags() & PLAYLISTEX_FLAGS_CANDELETE && pSPlaylist->GetPlaylistType() != PLAYLIST_TYPE_MEDIALIBRARY ? MF_ENABLED : MF_GRAYED));
								EnableMenuItem(m_hMenu, ID_EXPORT, MF_BYCOMMAND | (pSPlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED ? MF_ENABLED : MF_GRAYED));
								
								TrackPopupMenu(m_hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hSourceWnd, NULL);
							}
						}
						break;

					case LVN_BEGINDRAG:
					case LVN_BEGINRDRAG:
						{
							if(lpNotify->idFrom == IDC_SOURCESELECTOR)
							{
								HWND hListViewWnd = GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR);
								LVHITTESTINFO HitTest;
								
								GetCursorPos(&HitTest.pt);
								ScreenToClient(hListViewWnd, &HitTest.pt);
								int iItemHit = ListView_HitTest(hListViewWnd, &HitTest);
								
								if(iItemHit != -1)
								{
									ShowPlaylistAtIndex(iItemHit);

									m_bSourceDrag = true;
									if(lpNotify->code == LVN_BEGINRDRAG)
										m_bSourceAltDrag = true;

									SetRect(&m_SourceDraggedRect, 0, 0, 0, 0);

									//capture the mouse
									SetCapture(m_hSourceWnd);
								}

							}
						}
						break;

				}
			}
			break;

		case WM_UPDATELIST:
			{
				unsigned long itemCount = tuniacApp.m_PlaylistManager.GetNumPlaylists();
				ListView_SetItemCountEx(GetDlgItem(hDlg, IDC_SOURCESELECTOR), itemCount, LVSICF_NOSCROLL);
				ListView_SetColumnWidth(GetDlgItem(hDlg, IDC_SOURCESELECTOR), 0, LVSCW_AUTOSIZE_USEHEADER);
			}
			break;

		case WM_RENAMEITEM:
			{
				if(tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(lParam)->GetFlags() & PLAYLIST_FLAGS_CANRENAME)
					ListView_EditLabel(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), lParam);
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

LRESULT CALLBACK			CSourceSelectorWindow::WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CSourceSelectorWindow * pSSW = (CSourceSelectorWindow *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	return(pSSW->WndProc(hDlg, message, wParam, lParam));

}

bool			CSourceSelectorWindow::UpdateList()
{
	SendMessage(m_hSourceWnd, WM_UPDATELIST, 0, 0);
	return true;
}

bool			CSourceSelectorWindow::UpdateView()
{
	if(m_pVisibleView)
	{
		m_pVisibleView->Update();
		return true;
	}

	return false;
}

bool			CSourceSelectorWindow::IsPointInPlaylistSelector(LPPOINT lpPt)
{
	POINT			pt;

	pt.x = lpPt->x;
	pt.y = lpPt->y;

	ScreenToClient(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), &pt);

	if(::ChildWindowFromPoint(m_hSourceWnd, pt) == GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR))
	{
		return true;
	}

	return false;
}

IPlaylist * CSourceSelectorWindow::GetPlaylistFromPoint(LPPOINT lpPoint)
{
	POINT			pt;
	LVHITTESTINFO	hitTest;

	pt.x = lpPoint->x;
	pt.y = lpPoint->y;

	ScreenToClient(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), &pt);

	hitTest.pt.x = pt.x;
	hitTest.pt.y = pt.y;

	int x = ListView_HitTest(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), &hitTest);

	if(x == -1)
		return NULL;

	IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x);
	return pPlaylist;
}

bool CSourceSelectorWindow::ShowActiveViewViewOptions(HWND hParentWnd)
{
	if(m_pVisibleView)
	{
		return m_pVisibleView->ShowSourceViewOptions(hParentWnd);
	}

	return false;
}

bool CSourceSelectorWindow::ShowPlaylistAtIndex(unsigned long index)
{
	IPlaylist * pList = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(index);
	if(pList)
	{
		switch(pList->GetPlaylistType())
		{
			case PLAYLIST_TYPE_MEDIALIBRARY:
			case PLAYLIST_TYPE_STANDARDPLAYLIST:
			case PLAYLIST_TYPE_SMARTPLAYLIST:
				{
					m_PlaylistSourceView->SetPlaylistSource(index);
					m_pVisibleView = m_PlaylistSourceView;
				}
				break;


			case PLAYLIST_TYPE_CD:
				{
					m_AudioCDSourceView->SetPlaylistSource(index);
					m_pVisibleView = m_AudioCDSourceView;
				}
				break;
/*
			case PLAYLIST_TYPE_RADIO:
				{
					m_RadioSourceView->SetPlaylistSource(index);
					m_pVisibleView = m_RadioSourceView;
				}
				break;
*/
			default:
				{
					m_pVisibleView = NULL;
				}
				break;
		}

		for(unsigned long x=0; x<m_SourceViewArray.GetCount(); x++)
		{
			if(m_pVisibleView != m_SourceViewArray[x])
				m_SourceViewArray[x]->ShowSourceView(false);
			else
				m_SourceViewArray[x]->ShowSourceView(true);
		}

		m_pVisibleView->Update();
		m_ulVisiblePlaylistIndex = index;
		ListView_SetItemState(GetDlgItem(m_hSourceWnd, IDC_SOURCESELECTOR), m_ulVisiblePlaylistIndex, LVIS_SELECTED, LVIS_SELECTED);
	}

	return true;
}

IPlaylist	*	CSourceSelectorWindow::GetVisiblePlaylist()
{
	return tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(m_ulVisiblePlaylistIndex);
}

unsigned long	CSourceSelectorWindow::GetVisiblePlaylistIndex()
{
	return m_ulVisiblePlaylistIndex;
}

void CSourceSelectorWindow::ShowCurrentlyPlaying(void)
{
	for(unsigned long x=0; x<tuniacApp.m_PlaylistManager.GetNumPlaylists(); x++)
	{
		if(tuniacApp.m_PlaylistManager.GetActivePlaylist() == tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(x))
		{
			ShowPlaylistAtIndex(x);
			m_pVisibleView->ShowCurrentItem();
			break;
		}
	}
}
