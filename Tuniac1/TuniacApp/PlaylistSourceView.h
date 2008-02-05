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

	int					m_iLastClickedItem;
	int					m_iLastClickedSubitem;
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
	bool Redraw(void);
	bool UpdateColumns(void);

public:
	bool	ShowSourceViewOptions(HWND hWndParent);

	bool	CreateSourceView(HWND hWndParent);
	bool	DestroySourceView(void);

	bool	ShowSourceView(bool bShow);
	bool	MoveSourceView(int x, int y, int w, int h);

	bool	Update(void);

	bool	EditTrackInfo(void);
	bool	ShowCurrentItem(void);

	void	DeselectItem(unsigned long ulIndex);
	bool	GetSelectedIndexes(IndexArray & indexArray);
	bool	SelectedAll(IndexArray & indexArray);

	void	ClearTextFilter(void);

};
