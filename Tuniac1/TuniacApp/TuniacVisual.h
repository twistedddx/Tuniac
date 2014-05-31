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
#include "ituniacvisplugin.h"

#include <gl/gl.h>
#include <gl/glu.h>

#define TIMES	8
#define DISPLAYSAMPLES	512

class CTuniacVisual :
	public ITuniacVisPlugin
{
protected:
	ITuniacVisHelper	*	m_pHelper;

	HDC									m_glDC;		// Private GDI Device Context
	HGLRC								m_glRC;		// Permanent Rendering Context

	float							*	Samples;

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
