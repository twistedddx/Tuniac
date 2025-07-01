/*
	Copyright (C) 2025 Brett Hoyle

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

#pragma once

#pragma pack(16)

#include "ituniacvisplugin.h"
extern "C" {
#include "goom.h"
}

#include "GL\glew.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include "Array.h"
#include "CriticalSection.h"

#define VISDATA_SIZE 512

class TuniacGoom :
	public ITuniacVisPlugin
{
protected:
	CCriticalSection		m_RenderLock;

	ITuniacVisHelper	*	m_pHelper;

	PluginInfo			*	goom;

	HDC						m_hDC;
	HGLRC					m_glRC;

	unsigned int			pboIds[1];

	GLvoid				*	ioMem;

	long					m_LastWidth;
	long					m_LastHeight;

	int						iVisResHeight;
	int						iVisResWidth;
	bool					bResChange;
	unsigned long		*	m_textureData;
	int						iTextureSize;

	float				*	visdata;
	signed short			vd[2][VISDATA_SIZE];

	unsigned long			ulNumChannels;
	unsigned long			ulOldNumChannels;
	unsigned long			ulSamples;


	unsigned long			m_LastMove;
	int						m_iElaspedTime;

	int						iUseOpenGL;
	int						iVisMaxRes;
	int						iUsePBO;
	int						iUseMappedPBO;
	int						iAllowNonPowerOf2;
	
	bool					bPBOSupport;
	bool					bNonPowerOf2Support;

	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	TuniacGoom(void);
	~TuniacGoom(void);

	bool RenderVisual(void);

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

	bool	InitOpenGL(void);
	void	ShutdownOpenGL(void);
};
