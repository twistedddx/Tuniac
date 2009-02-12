#pragma once

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

	unsigned long						m_LastWidth;
	unsigned long						m_LastHeight;
	int									iVisRes;

	Array<LPTSTR, 4>					m_VisFilenameArray;
	unsigned long						m_SelectedVisual;

	SoniqueVisExternal *				m_TheVisual;
	ITuniacVisHelper		*			m_pHelper;

	GLuint								m_textureID;
	GLubyte				*				m_textureData;

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
