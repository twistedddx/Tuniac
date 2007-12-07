#pragma once
#include "iwindow.h"
#include "ISourceView.h"

#include "PlaylistSourceView.h"
#include "AudioCDSourceView.h"
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

	bool			UpdateList();
	bool			UpdateView();

	IPlaylist	 *	GetPlaylistFromPoint(LPPOINT lpPt);
	bool			IsPointInPlaylistSelector(LPPOINT lpPt);

	bool			ShowActiveViewViewOptions(HWND hParentWnd);
	bool			ShowPlaylistAtIndex(unsigned long index);

public:
	void			Destroy(void);

	LPTSTR			GetName(void);
	GUID			GetPluginID(void);

	unsigned long	GetFlags(void);

	bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst);
	bool			DestroyPluginWindow(void);

	bool			Show(void);
	bool			Hide(void);

	bool			SetPos(int x, int y, int w, int h);

	//Added Mark 7th October
	void ShowCurrentlyPlaying(void);
};