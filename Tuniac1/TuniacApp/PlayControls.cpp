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
#include "playcontrols.h"

#include "DoubleBuffer.h"

#include "resource.h"

//#define BUTTON_SIZE		48
//#define SMALLER_BUTTON_SIZE		38

#define NEW_BUTTON_WIDTH	32
#define NEW_BUTTON_HEIGHT	24
#define NEW_BUTTON_SEP		4

#define LEFTSIDE	175


CPlayControls::CPlayControls(void)
{
}

CPlayControls::~CPlayControls(void)
{
}


bool CPlayControls::Create(HWND hWndParent)
{
	WNDCLASSEX	wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProcStub;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= tuniacApp.getMainInstance();
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= TEXT("TUNIACCONTROLWINDOW");
	wcex.hIconSm		= NULL;

	if(!RegisterClassEx(&wcex))
		return(false);

	m_hWnd = CreateWindowEx(0,
							TEXT("TUNIACCONTROLWINDOW"), 
							TEXT("TUNIACCONTROLWINDOW"), 
							WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
							0, 
							0, 
							100, 
							100, 
							hWndParent, 
							NULL, 
							tuniacApp.getMainInstance(), 
							this);


	if(!m_hWnd)
	{
		return(false);
	}

	return true;
}

bool CPlayControls::Destroy(void)
{
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
	return true;
}

bool CPlayControls::Move(int x, int y, int w, int h)
{
	if(m_hWnd)
	{
		MoveWindow(m_hWnd, x,y,w,h, TRUE);
		RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return(true);
	}

	return(false);
}

bool CPlayControls::UpdateVolume(void)
{
	SendMessage(m_hVolumeWnd, TBM_SETPOS, TRUE, (LPARAM)CCoreAudio::Instance()->GetVolumePercent());

	return true;
}

bool CPlayControls::UpdateState(void)
{
	RedrawWindow(m_hPlayWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_INTERNALPAINT);	
	return true;
}

bool CPlayControls::SysColorChange(WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_hWnd, WM_SYSCOLORCHANGE, wParam, lParam);
	return true;
}


LRESULT CALLBACK CPlayControls::PrevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPlayControls * pPC = (CPlayControls *)(ULONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(message)
	{
		case WM_RBUTTONDOWN:
			{
				POINT p;
				GetCursorPos(&p);
				tuniacApp.m_History.PopupMenu(p.x, p.y);
			}
			break;

		default:
			return CallWindowProc(pPC->m_origPrevWndProc, hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK CPlayControls::NextWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPlayControls * pPC = (CPlayControls *)(ULONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(message)
	{
		case WM_RBUTTONDOWN:
			{
				POINT p;
				GetCursorPos(&p);
				TrackPopupMenu(tuniacApp.GetFutureMenu(), TPM_RIGHTBUTTON, p.x, p.y, 0, tuniacApp.getMainWindow(), NULL);
			}
			break;

		default:
			return CallWindowProc(pPC->m_origNextWndProc, hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK CPlayControls::WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPlayControls * pPC;

	if(message == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pPC = (CPlayControls *)lpCS->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (ULONG_PTR)pPC);
	}
	else
	{
		pPC = (CPlayControls *)(ULONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pPC->WndProc(hWnd, message, wParam, lParam));
}

LRESULT CALLBACK CPlayControls::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			{
				m_PreviousBitmap	=	CreateMappedBitmap(NULL, (IDB_PREVIOUS),		0, NULL, 0);
				m_NextBitmap		=	CreateMappedBitmap(NULL, (IDB_NEXT),			0, NULL, 0);
				m_PlayBitmap		=	CreateMappedBitmap(NULL, (IDB_PLAY),			0, NULL, 0);
				m_PauseBitmap		=	CreateMappedBitmap(NULL, (IDB_PAUSE),		0, NULL, 0);

				m_hPrevWnd = CreateWindow(	L"BUTTON",
											L"Prev",
											WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
											0,
											0,
											NEW_BUTTON_WIDTH,
											NEW_BUTTON_HEIGHT,
											hWnd,
											(HMENU)0,
											tuniacApp.getMainInstance(), 
											NULL);
				SetWindowLongPtr(m_hPrevWnd, GWLP_USERDATA, (DWORD_PTR)this);
				m_origPrevWndProc = (WNDPROC)SetWindowLongPtr(m_hPrevWnd, GWLP_WNDPROC, (LONG_PTR)PrevWndProc);




				m_hPlayWnd = CreateWindow(	L"BUTTON",
											L"Play",
											WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
											0,
											0,
											NEW_BUTTON_WIDTH,
											NEW_BUTTON_HEIGHT,
											hWnd,
											(HMENU)1,
											tuniacApp.getMainInstance(), 
											NULL);



				m_hNextWnd = CreateWindow(	L"BUTTON",
											L"Next",
											WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
											0,
											0,
											NEW_BUTTON_WIDTH,
											NEW_BUTTON_HEIGHT,
											hWnd,
											(HMENU)2,
											tuniacApp.getMainInstance(), 
											NULL);
				SetWindowLongPtr(m_hNextWnd, GWLP_USERDATA, (DWORD_PTR)this);
				m_origNextWndProc = (WNDPROC)SetWindowLongPtr(m_hNextWnd, GWLP_WNDPROC, (LONG_PTR)NextWndProc);


				m_hVolumeWnd = CreateWindow(	TRACKBAR_CLASS,
												L"",
												WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS | TBS_BOTH,
												0,
												0,
												48,
												48,
												hWnd,
												(HMENU)3,
												tuniacApp.getMainInstance(), 
												NULL);

				SendMessage(m_hVolumeWnd, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 100));
				UpdateVolume();

				SetTimer(hWnd, 0, 250, NULL);

			}
			break;

		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

				HDC		hDC = lpDrawItem->hDC;

				CDoubleBuffer doubleBuffer;
				doubleBuffer.Begin(hDC, &lpDrawItem->rcItem);

				SelectObject(hDC, GetSysColorBrush(COLOR_BTNFACE));
				SetDCPenColor(hDC, GetSysColor(COLOR_3DSHADOW));
				SelectObject(hDC, GetStockObject(DC_PEN));

				Rectangle(	hDC, 
							0, 
							0,
							lpDrawItem->rcItem.right,
							lpDrawItem->rcItem.bottom);


				HDC mDC = CreateCompatibleDC(hDC);

				if(mDC)
				{
					if(lpDrawItem->hwndItem == m_hPrevWnd)
						SelectObject(mDC, m_PreviousBitmap);

					if(lpDrawItem->hwndItem == m_hPlayWnd)
					{
						if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
							SelectObject(mDC, m_PauseBitmap);
						else
							SelectObject(mDC, m_PlayBitmap);

					}

					if(lpDrawItem->hwndItem == m_hNextWnd)
						SelectObject(mDC, m_NextBitmap);

					if(lpDrawItem->itemState & ODS_SELECTED)
						BitBlt(hDC, 2,2, 29,21, mDC, 1, 1, SRCCOPY);
					else
						BitBlt(hDC, 1,1, 30,22, mDC, 1, 1, SRCCOPY);

					DeleteDC(mDC);
				}

				doubleBuffer.End(hDC);

				return TRUE; 
			}
			break;

		case WM_TIMER:
			{
				RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			}
			break;

		case WM_SIZE:
			{
				RECT r;
				GetClientRect(hWnd, &r);

				MoveWindow(m_hPrevWnd, NEW_BUTTON_SEP,									5,	NEW_BUTTON_WIDTH,	NEW_BUTTON_HEIGHT,	FALSE);
				MoveWindow(m_hPlayWnd, NEW_BUTTON_WIDTH			+ (NEW_BUTTON_SEP * 2),	5,	NEW_BUTTON_WIDTH,	NEW_BUTTON_HEIGHT,	FALSE);
				MoveWindow(m_hNextWnd, (NEW_BUTTON_WIDTH * 2)	+ (NEW_BUTTON_SEP * 3),	5,	NEW_BUTTON_WIDTH,	NEW_BUTTON_HEIGHT,	FALSE);

				SetRect(&m_SeekRect,	(NEW_BUTTON_WIDTH * 3)	+ (NEW_BUTTON_SEP * 4), 
										r.bottom - 22, 
										r.right  - 32, 
										r.bottom - 7);

				InflateRect(&m_SeekRect, -50, 0);

				MoveWindow(	m_hVolumeWnd, 
							NEW_BUTTON_SEP, 
							NEW_BUTTON_HEIGHT + (NEW_BUTTON_SEP * 2), 
							(NEW_BUTTON_WIDTH * 3) + (NEW_BUTTON_SEP * 2), 
							r.bottom - NEW_BUTTON_HEIGHT - (NEW_BUTTON_SEP * 2) - 4, 
							TRUE);

			}
			break;

		case WM_COMMAND:
			{
				int wmId, wmEvent;
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);

				switch(wmId)
				{
					case 0:
						{
							if(GetKeyState(VK_CONTROL) & 0x8000)
								SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS_BYHISTORY, 0), 0);
							else
								SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(ID_PLAYBACK_PREVIOUS, 0), 0);
						}
						break;

					case 1:
						{
							SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAYPAUSE, 0), 0);
						}
						break;

					case 2:
						{
							SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
						}
						break;
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				POINT		pt;
				POINTSTOPOINT(pt,MAKEPOINTS(lParam));

				RECT SeekRect;
				CopyRect(&SeekRect, &m_SeekRect);

				InflateRect(&SeekRect, -3, -3);
				
				if(PtInRect(&SeekRect, pt))
				{
					if(wParam & MK_LBUTTON)
					{
						if(CCoreAudio::Instance()->GetLength() != LENGTH_UNKNOWN)
						{
							float Width = SeekRect.right - SeekRect.left;
							float Offset = pt.x - SeekRect.left;

							float Percent = Offset / Width;

							float Seekpos = CCoreAudio::Instance()->GetLength() * Percent;

							CCoreAudio::Instance()->SetPosition(Seekpos);
							tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SEEK_MANUAL, NULL, NULL);
						}
					}
					//else
					//{
					//	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
					//}
				}
			}
			break;

		case WM_LBUTTONDOWN:
			{
				POINT		pt;

				POINTSTOPOINT(pt,MAKEPOINTS(lParam));

				RECT SeekRect;
				CopyRect(&SeekRect, &m_SeekRect);
				InflateRect(&SeekRect, -3, -3);
				
				if(PtInRect(&SeekRect, pt))
				{
					if(CCoreAudio::Instance()->GetLength() != LENGTH_UNKNOWN)
					{
						float Width = SeekRect.right - SeekRect.left;
						float Offset = pt.x - SeekRect.left;

						float Percent = Offset / Width;

						float Seekpos = CCoreAudio::Instance()->GetLength() * Percent;

						CCoreAudio::Instance()->SetPosition(Seekpos);
						tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SEEK_MANUAL, NULL, NULL);
					}
				}
			}
			break;

			case WM_LBUTTONUP:
			{
				POINT		pt;

				POINTSTOPOINT(pt,MAKEPOINTS(lParam));

				RECT r;
				GetClientRect(hWnd, &r);

				RECT CurrentSongRect;

				SetRect(&CurrentSongRect,	r.left + 160, 
											r.top, 
											r.right, 
											r.bottom - 20);
				
				if(PtInRect(&CurrentSongRect, pt))
				{
					if(tuniacApp.m_SourceSelectorWindow)
					{
						tuniacApp.m_SourceSelectorWindow->ShowCurrentlyPlaying();
					}
				}
			}
			break;

		case WM_HSCROLL:
			{
				int iVol = SendMessage(m_hVolumeWnd, TBM_GETPOS, 0, 0);
				CCoreAudio::Instance()->SetVolumePercent(iVol);
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				RECT		r;

				GetClientRect(hWnd, &r);

				HDC		hDC = BeginPaint(hWnd, &ps);

				CDoubleBuffer doubleBuffer;
				doubleBuffer.Begin(hDC, &r);

				FillRect(hDC, &r, GetSysColorBrush(COLOR_3DFACE));

				r.left-=2;
				r.right+=2;

				r.top = r.bottom - 2;
				DrawEdge(hDC, &r, EDGE_ETCHED, BF_RECT);

				r.top = 0;
				r.bottom = 1;
				DrawEdge(hDC, &r, EDGE_ETCHED, BF_RECT);

				GetClientRect(hWnd, &r);
				RECT SeekRect;
				CopyRect(&SeekRect, &m_SeekRect);

				SelectObject(hDC, GetSysColorBrush(COLOR_BTNFACE));
				SetDCPenColor(hDC, GetSysColor(COLOR_3DSHADOW));
				SelectObject(hDC, GetStockObject(DC_PEN));

				Rectangle(	hDC, 
							SeekRect.left, 
							SeekRect.top,
							SeekRect.right,
							SeekRect.bottom);

				InflateRect(&SeekRect, -3, -3);

				float Width = SeekRect.right - SeekRect.left;

				unsigned long ulPosition = CCoreAudio::Instance()->GetPosition() / 1000;
				unsigned long ulSongLength = CCoreAudio::Instance()->GetLength();
				if(ulSongLength != LENGTH_UNKNOWN)
					ulSongLength = ulSongLength / 1000;
				else
					ulSongLength = 0;

				float fProgress = 0.0f;
				if(ulSongLength != 0)
					fProgress = (float)ulPosition / (float)ulSongLength;

				SeekRect.right = ((Width) * fProgress) + SeekRect.left;
				FillRect(hDC, &SeekRect, (HBRUSH)GetSysColorBrush(COLOR_3DSHADOW));

				RECT TimeRect;

				SelectObject(hDC, tuniacApp.GetTuniacFont(FONT_SIZE_TINY));
				SetBkMode(hDC, TRANSPARENT);

				TCHAR tstr[256];

				_snwprintf(tstr, 256, TEXT("%01d:%02d:%02d"), (ulPosition / 60) / 60, (ulPosition / 60) % 60, ulPosition % 60);

				SetRect(&TimeRect, 
						SeekRect.left - 48, 
						SeekRect.top, 
						m_SeekRect.left, 
						SeekRect.bottom);

				DrawText(	hDC, 
							tstr, 
							-1, 
							&TimeRect, 
							DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

				if(ulSongLength != LENGTH_UNKNOWN)
				{
					_snwprintf(tstr, 256, TEXT("%01d:%02d:%02d"), (ulSongLength / 60) / 60, (ulSongLength / 60) % 60, ulSongLength % 60);

					SetRect(&TimeRect, 
							m_SeekRect.right +2, 
							SeekRect.top, 
							m_SeekRect.right + 48, 
							SeekRect.bottom);

					DrawText(	hDC, 
								tstr, 
								-1, 
								&TimeRect, 
								DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				}


				RECT	SongTitleRect;
				SetRect(	&SongTitleRect, 
							m_SeekRect.left, 
							4, 
							m_SeekRect.right, 
							4+18);

				RECT	ArtistTitleRect;
				SetRect(	&ArtistTitleRect, 
							m_SeekRect.left, 
							20, 
							m_SeekRect.right, 
							38);

				IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();

				if(pPlaylist)
				{
					IPlaylistEntry * pIPE = pPlaylist->GetActiveEntry();

					if(pIPE)
					{
						LPTSTR  szTitle		= (LPTSTR)pIPE->GetField(FIELD_TITLE);
						LPTSTR	szArtist	= (LPTSTR)pIPE->GetField(FIELD_ARTIST);

						if(szTitle == NULL)
							szTitle = TEXT("No Song Loaded");

						if(szArtist == NULL)
							szArtist = TEXT("");

						SelectObject(hDC, tuniacApp.GetTuniacFont(FONT_SIZE_SMALL_MEDIUM));
						SetBkMode(hDC, TRANSPARENT);
						DrawText(	hDC, 
									szTitle,
									-1, 
									&SongTitleRect, 
									DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX | DT_WORD_ELLIPSIS);

						SelectObject(hDC, tuniacApp.GetTuniacFont(FONT_SIZE_SMALL_MEDIUM));
						SetBkMode(hDC, TRANSPARENT);
						DrawText(	hDC, 
									szArtist,
									-1, 
									&ArtistTitleRect, 
									DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
					}
				}


				doubleBuffer.End(hDC);

				EndPaint(hWnd, &ps);

			}
			break;

		case WM_SYSCOLORCHANGE:
			{
				DeleteObject(m_PreviousBitmap);
				m_PreviousBitmap	=	CreateMappedBitmap(NULL, (IDB_PREVIOUS),		0, NULL, 0);

				DeleteObject(m_NextBitmap);
				m_NextBitmap		=	CreateMappedBitmap(NULL, (IDB_NEXT),			0, NULL, 0);

				DeleteObject(m_PlayBitmap);
				m_PlayBitmap		=	CreateMappedBitmap(NULL, (IDB_PLAY),			0, NULL, 0);

				DeleteObject(m_PauseBitmap);
				m_PauseBitmap		=	CreateMappedBitmap(NULL, (IDB_PAUSE),			0, NULL, 0);

				SendMessage(m_hPrevWnd,		message, wParam, lParam);
				SendMessage(m_hPlayWnd,		message, wParam, lParam);
				SendMessage(m_hNextWnd,		message, wParam, lParam);
				SendMessage(m_hVolumeWnd,	message, wParam, lParam);
			}
			break;

		default:
			return(DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}

	return(0);
}
