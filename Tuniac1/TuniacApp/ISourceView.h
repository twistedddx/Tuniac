#pragma once

class ISourceView
{
public:
	virtual bool	ShowSourceViewOptions(HWND hWndParent)		= 0;
	virtual bool	Update(void)								= 0;

	virtual bool	CreateSourceView(HWND hWndParent)			= 0;
	virtual bool	DestroySourceView(void)						= 0;

	virtual bool	ShowSourceView(bool bShow)					= 0;		// if bShow == false then hide
	virtual bool	MoveSourceView(int x, int y, int w, int h)	= 0;

	//Added Mark 7th October
	virtual bool	EditTrackInfo()								= 0;		//Edits the info for each source
	virtual bool	ShowCurrentItem()							= 0;
};