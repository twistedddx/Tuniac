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
#include "array.h"
#include "isourceview.h"
#include "TagEditor.h"

class CPlaylistSourceView :
	public ISourceView
{
protected:

	typedef struct
	{
		TCHAR	szText[64];
		int		nCount;
	} FieldDataSet;
	typedef Array<FieldDataSet, 3> FieldDataArray;

	HWND				m_PlaylistSourceWnd;

	static WNDPROC		m_ViewOptionsListViewOldProc;

	IPlaylistEX		*	m_pPlaylist;
	unsigned long		m_ulActivePlaylistIndex;

	HCURSOR				m_AddCursor;
	HCURSOR				m_NewPlaylistCursor;

	HMENU				m_ItemMenu;
	HMENU				m_HeaderMenu;
	HMENU				m_FilterByFieldMenu;

	RECT				m_draggedItemRect;
	bool				m_Drag;
	bool				m_ShowingHeaderFilter;

	unsigned long		m_iLastClickedItem;
	unsigned long		m_iLastClickedSubitem;
	WNDPROC				m_origEditWndProc;
	RECT				m_editRect;

	CTagEditor			m_TagEditor;


	Array<unsigned long, 3>		m_ColumnIDArray;			// the index here is actually a FIELD_ID from iplaylist.h to say which columns should be displayed.

	LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK ViewOptionsWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ViewOptionsWndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ViewOptionsListViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK EditWndProc(HWND, UINT, WPARAM, LPARAM);

public:
	CPlaylistSourceView(void);
	~CPlaylistSourceView(void);

	bool SetPlaylistSource(unsigned long ulPlaylist);
	void Redraw(void);
	bool UpdateColumns(void);

public:
	bool	ShowSourceViewOptions(HWND hWndParent);

	bool	CreateSourceView(HWND hWndParent);
	bool	DestroySourceView(void);

	bool	ShowSourceView(bool bShow);
	bool	MoveSourceView(int x, int y, int w, int h);

	bool	Update(void);
	bool	ShowCurrentItem(void);

	void	ClearTextFilter(void);
	void	SetFocusFilter(void);
	bool	EditTrackInfo(void);

	void	DeselectItem(unsigned long ulIndex);
	bool	GetSelectedIndexes(IndexArray & indexArray);
	bool	SelectedAll(IndexArray & indexArray);


};
