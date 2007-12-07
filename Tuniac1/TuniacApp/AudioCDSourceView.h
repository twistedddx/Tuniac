#pragma once
#include "isourceview.h"
#include "AudioCDPlaylist.h"

class CAudioCDSourceView :
	public ISourceView
{
protected:
	CAudioCDPlaylist	*	m_pCDPlaylist;
	HWND					m_AudioCDSourceWnd;
	unsigned long			m_ulActivePlaylistIndex;

	LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CAudioCDSourceView(void);
	~CAudioCDSourceView(void);

	bool SetPlaylistSource(unsigned long ulPlaylistIndex);

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
