#pragma once
#include "isourceview.h"
#include "RadioTunerPlaylist.h"

class CRadioSourceView :
	public ISourceView
{
protected:
	HWND					m_RadioSourceWnd;
	CRadioTunerPlaylist	*	m_pRadioPlaylist;
	unsigned long			m_ulActivePlaylistIndex;

	LRESULT CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CRadioSourceView(void);
public:
	~CRadioSourceView(void);

	bool SetPlaylistSource(unsigned long ulPlaylistIndex);
	bool ParseRadioList(void);

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
