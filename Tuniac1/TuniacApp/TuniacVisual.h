#pragma once
#include "ituniacvisplugin.h"

#include <gl/gl.h>
#include <gl/glu.h>

class CTuniacVisual :
	public ITuniacVisPlugin
{
protected:
	ITuniacVisHelper	*	pHelper;

	HDC									m_glDC;		// Private GDI Device Context
	HGLRC								m_glRC;		// Permanent Rendering Context

	unsigned long						m_LastWidth;
	unsigned long						m_LastHeight;


public:
	CTuniacVisual(void);
	~CTuniacVisual(void);

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
