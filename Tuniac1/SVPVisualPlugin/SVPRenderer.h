#pragma once

#pragma pack(16)

#include "ituniacvisplugin.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "SoniqueVis.h"
#include "Array.h"
#include "CriticalSection.h"

class SVPRenderer :
	public ITuniacVisPlugin
{
protected:

	CCriticalSection					m_RenderLock;

	HDC									m_glDC;		// Private GDI Device Context
	HGLRC								m_glRC;		// Permanent Rendering Context

	int									m_LastWidth;
	int									m_LastHeight;
	int									iVisRes;

	VisData								vd;
	unsigned long					*	m_textureData;
	float							*	visdata;
	unsigned long						ulOldNumChannels;

	ITuniacVisHelper				*	m_pHelper;

	Array<LPTSTR, 4>					m_VisFilenameArray;
	SoniqueVisExternal				*	m_TheVisual;
	int									m_SelectedVisual;

	RECT								m_NextVisRect;
	RECT								m_PrevVisRect;

	unsigned long						m_LastMove;

    BITMAP								m_ArrowBM;
	HBITMAP								m_hArrow;



public:
	SVPRenderer(void);
	~SVPRenderer(void);

	bool AddFolderOfSVP(LPTSTR	szFolder);

	bool RenderVisual(void);
	bool SetActiveVisual(int vis);

public:
	void	Destroy(void);

	LPTSTR	GetPluginName(void);
	unsigned long GetFlags(void);

	bool	SetHelper(ITuniacVisHelper *pHelper);
	
	bool	Attach(HDC hDC);
	bool	Detach();

	bool	Render(int w, int h);

	bool	About(HWND hWndParent);
	bool	Configure(HWND hWndParent);

	bool	Notify(unsigned long Notification);
	bool	MouseFunction(unsigned long function, int x, int y);
};
