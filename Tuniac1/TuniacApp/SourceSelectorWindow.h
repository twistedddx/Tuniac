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

#pragma once
#include "iwindow.h"
#include "ISourceView.h"

#include "PlaylistSourceView.h"
#include "AudioCDSourceView.h"
//#include "PlayQueueSourceView.h"
//#include "RadioSourceView.h"

#define SEPERATOR_WIDTH		5

#define INVALID_PLAYLIST_VALUE	0xffffffff

class CSourceSelectorWindow :
	public IWindow
{
protected:
	HWND						m_hSourceWnd;
	HMENU						m_hMenu;
	HMENU						m_hDragMenu;

	unsigned long				m_ulSeparatorX;
	unsigned long				m_ulAlbumArtX;
	unsigned long				m_ulSeparatorXOld;

	bool						m_bTrackingDrag;
	bool						m_bTrackingMoved;
	int							m_iDragOffset;

	bool						m_bSourceDrag;
	bool						m_bSourceAltDrag;
	RECT						m_SourceDraggedRect;
	int							m_AltDragDest;

	ISourceView	*				m_pVisibleView;
	unsigned long				m_ulVisiblePlaylistIndex;

	Array<ISourceView	*, 3>	m_SourceViewArray;

	LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CPlaylistSourceView		*	m_PlaylistSourceView;
	CAudioCDSourceView		*	m_AudioCDSourceView;
	//CRadioSourceView		*	m_RadioSourceView;

public:
	CSourceSelectorWindow(void);
	~CSourceSelectorWindow(void);

	ISourceView*    GetVisibleView() {return m_pVisibleView;}
	IPlaylist	*	GetVisiblePlaylist();
	unsigned long	GetVisiblePlaylistIndex();

public:

	bool			Refresh();

	bool			UpdateList();
	bool			UpdateView();

	IPlaylist	 *	GetPlaylistFromPoint(LPPOINT lpPt);
	bool			IsPointInPlaylistSelector(LPPOINT lpPt);

	bool			ShowActiveViewViewOptions(HWND hParentWnd);
	bool			ShowPlaylistAtIndex(unsigned long ulIndex);

public:
	void			Destroy(void);

	LPTSTR			GetName(void);
	GUID			GetPluginID(void);

	unsigned long	GetFlags(void);

	bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst);
	bool			DestroyPluginWindow(void);

	bool			Show(void);
	bool			Hide(void);

	void			ToggleAlbumArt(bool bEnabled);

	bool			SetPos(int x, int y, int w, int h);

	//Added Mark 7th October
	void			ShowCurrentlyPlaying(void);
};