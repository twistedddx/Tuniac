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

#include "stdafx.h"
#include "tuniacvisual.h"

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void setVSync(int interval=1)
{
	const char *extensions = (char*)glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if( wglSwapIntervalEXT )
			wglSwapIntervalEXT(interval);
	}
}

CTuniacVisual::CTuniacVisual(void)
{
	m_LastWidth	 = 0;
	m_LastHeight = 0;

	m_glRC = NULL;
	m_glDC = NULL;
}

CTuniacVisual::~CTuniacVisual(void)
{
}

void	CTuniacVisual::Destroy(void)
{
	if(Samples)
	{
		//VirtualFree(Samples, 0, MEM_RELEASE);
		_aligned_free(Samples);
		Samples = NULL;
	}
	delete this;
}

LPTSTR	CTuniacVisual::GetPluginName(void)
{
	return TEXT("Tuniac Visualizer");
}

unsigned long CTuniacVisual::GetFlags(void)
{
	return PLUGINFLAGS_ABOUT;
}

bool	CTuniacVisual::SetHelper(ITuniacVisHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

bool	CTuniacVisual::Attach(HDC hDC)
{
	m_LastWidth	 = 0;
	m_LastHeight = 0;

	m_glRC = NULL;
	m_glDC = hDC;

	GLuint		PixelFormat;

	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion   = 1; 
	pfd.dwFlags    =	PFD_DOUBLEBUFFER |
						PFD_SUPPORT_OPENGL |
						PFD_DRAW_TO_WINDOW |
						PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24 ;
	pfd.cDepthBits = 32 ;
	pfd.iLayerType = PFD_MAIN_PLANE ;

	if (!(PixelFormat=ChoosePixelFormat(m_glDC,&pfd)))
		return false;

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))
		return false;

	if (!(m_glRC=wglCreateContext(m_glDC)))
		return false;

	if(!wglMakeCurrent(m_glDC, m_glRC))
		return false;

	setVSync(1);

	glClearColor(0.9f, 0.92f, 0.96f, 0.2f);
	glClear (GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_POLYGON_SMOOTH);

	//Samples = (float *)VirtualAlloc(NULL, DISPLAYSAMPLES * sizeof(float), MEM_COMMIT, PAGE_READWRITE);
	Samples = (float*)_aligned_malloc(DISPLAYSAMPLES * sizeof(float), 16);

	SwapBuffers(m_glDC);

	return true;
}

bool	CTuniacVisual::Detach()
{
	if (m_glRC)
	{
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(m_glRC);
		m_glRC=NULL;
	}

	m_glDC = NULL;

	if(Samples)
	{
		//VirtualFree(Samples, 0, MEM_RELEASE);
		_aligned_free(Samples);
		Samples = NULL;
	}

	return true;
}

bool	CTuniacVisual::Render(int w, int h)
{
	if((m_LastWidth != w) || (m_LastHeight != h))
	{
		m_LastWidth		= w;
		m_LastHeight	= h;

		glViewport(0,0,w,h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, (float)w, (float)h, 0.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	glClear (GL_COLOR_BUFFER_BIT);

	//matt gray
	glBegin(GL_QUAD_STRIP);
		glColor4f(0.9f, 0.92f, 0.96f, 0.5f);
		glVertex2f(0,			0);

		glColor4f(0.8f, 0.82f, 0.86f, 0.5f);
		glVertex2f(0,			m_LastHeight);
	
		glColor4f(0.8f, 0.82f, 0.86f, 0.5f);
		glVertex2f(m_LastWidth,	0);
	
		glColor4f(0.4f, 0.42f, 0.46f, 0.5f);
		glVertex2f(m_LastWidth,	m_LastHeight);
	glEnd();

	// draw background grid
	glColor4f(0,0,0, 0.1f);
	glBegin(GL_LINES);
	for(unsigned long x=0; x<m_LastWidth; x+=64)
	{
		glVertex2i(x, 0);
		glVertex2i(x, m_LastHeight);
	}

	for(unsigned long y=0; y<m_LastHeight; y+=64)
	{
		glVertex2i(0,			y);
		glVertex2i(m_LastWidth, y);
	}
	glEnd();

	if(m_pHelper->GetVisData(Samples, DISPLAYSAMPLES))
	{
		float halfheight = ((float)m_LastHeight / 2.0f);
		float multiplier = (float)m_LastWidth / (float)(DISPLAYSAMPLES/2.0f);

		glColor4f(0,0,0,1);
		glBegin(GL_QUADS);
		{
			for(unsigned int samp=0; samp<DISPLAYSAMPLES-3; samp++)
			{
				glVertex2f(samp*multiplier, halfheight);
				glVertex2f(samp*multiplier, halfheight - (Samples[samp] * halfheight ));

				glVertex2f((samp+1)*multiplier,	halfheight - (Samples[samp+1] * halfheight));
				glVertex2f((samp+1)*multiplier,	halfheight);
			}
		}
		glEnd();

		glColor4f(0,0,0,1);
		glBegin(GL_QUADS);
		{
			for(unsigned int samp=0; samp<DISPLAYSAMPLES-3; samp++)
			{
				glVertex2f(samp*multiplier, halfheight);
				glVertex2f(samp*multiplier, halfheight + (Samples[samp+2] * halfheight));

				glVertex2f((samp+1)*multiplier,	halfheight + (Samples[samp+3] * halfheight));
				glVertex2f((samp+1)*multiplier,	halfheight);
			}
		}
		glEnd();
	}

	//glFinish();
	SwapBuffers(m_glDC);
	return true;
}

bool	CTuniacVisual::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Tuniac included visual. Tony Million 2009"), GetPluginName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool	CTuniacVisual::Configure(HWND hWndParent)
{
	return true;
}

bool	CTuniacVisual::Notify(unsigned long Notification)
{
	return true;
}

bool	CTuniacVisual::MouseFunction(unsigned long function, int x, int y)
{
	return true;
}