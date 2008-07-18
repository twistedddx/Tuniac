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

#define TIMES	8
#define DISPLAYSAMPLES	512

static float	Samples[TIMES][DISPLAYSAMPLES];
static int		Rotator = 0;


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
}

CTuniacVisual::~CTuniacVisual(void)
{
}

void	CTuniacVisual::Destroy(void)
{
	delete this;
}

LPTSTR	CTuniacVisual::GetPluginName(void)
{
	return TEXT("Tuniac Visualizer");
}

unsigned long CTuniacVisual::GetFlags(void)
{
	return 0;
}

bool	CTuniacVisual::SetHelper(ITuniacVisHelper *pHelper)
{
	this->pHelper = pHelper;
	return true;
}

bool	CTuniacVisual::Attach(HDC hDC)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match

	m_LastWidth	 = 0;
	m_LastHeight = 0;

	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion   = 1 ; 
	pfd.dwFlags    =	PFD_DOUBLEBUFFER |
						PFD_SUPPORT_OPENGL |
						PFD_DRAW_TO_WINDOW |
						PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24 ;
	pfd.cDepthBits = 32 ;
	pfd.iLayerType = PFD_MAIN_PLANE ;


	m_glDC = hDC;

	if (!(PixelFormat=ChoosePixelFormat(m_glDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		return false;								// Return FALSE
	}

	if (!(m_glRC=wglCreateContext(m_glDC)))				// Are We Able To Get A Rendering Context?
	{
		return false;								// Return FALSE
	}

	if(!wglMakeCurrent(m_glDC, m_glRC))					// Try To Activate The Rendering Context
	{
		return false;								// Return FALSE
	}

	setVSync(1);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear (GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);			// Enable Blending
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_POLYGON_SMOOTH);

	return true;
}

bool	CTuniacVisual::Detach()
{
	if (m_glRC)												// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))						// Are We Able To Release The DC And RC Contexts?
		{
		}

		if (!wglDeleteContext(m_glRC))						// Are We Able To Delete The RC?
		{
		}

		m_glRC=NULL;										// Set RC To NULL
	}

	m_glDC = NULL;

	return true;
}

bool	CTuniacVisual::Render(int w, int h)
{
	// just to make sure!
	if(!wglMakeCurrent(m_glDC, m_glRC))					// Try To Activate The Rendering Context
	{
		return false;									// Return FALSE
	}

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

		glClearColor(0.9f, 0.92f, 0.96f, 0.2f);
		glClear (GL_COLOR_BUFFER_BIT);

	}

	if(pHelper->GetVisData(Samples[Rotator], DISPLAYSAMPLES))
	{
		glClearColor(0.9f, 0.92f, 0.96f, 0.2f);
		glClear (GL_COLOR_BUFFER_BIT);
		glLoadIdentity();					// Reset The Modelview Matrix

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


		for(int time=0; time<TIMES; time++)
		{
			float halfheight = ((float)m_LastHeight / 2.0f);
			float multiplier = (float)m_LastWidth / (float)(DISPLAYSAMPLES/2.0f);

			int thisindex = ((Rotator + 1) + time) % TIMES;


			float alpha = 0.01 + (((float)time / (float)TIMES) / 3.0f);

			glColor4f(0,0,0, alpha);
			glBegin(GL_QUADS);
			{
				for(unsigned int samp=0; samp<DISPLAYSAMPLES-2; samp++)
				{
					glVertex2f(samp*multiplier,		halfheight);
					glVertex2f(samp*multiplier,		halfheight 	- abs(Samples[thisindex][(samp*2)]		* halfheight) );

					glVertex2f((samp+1)*multiplier,	halfheight 	- abs(Samples[thisindex][(samp*2)+2]		* halfheight));
					glVertex2f((samp+1)*multiplier,	halfheight);
				}
			}
			glEnd();

			glColor4f(0,0,0, alpha);
			glBegin(GL_QUADS);
			{
				for(unsigned int samp=0; samp<DISPLAYSAMPLES-2; samp++)
				{
					glVertex2f(samp*multiplier,		halfheight);
					glVertex2f(samp*multiplier,		halfheight + abs(Samples[thisindex][(samp*2)+1]		* halfheight));

					glVertex2f((samp+1)*multiplier,	halfheight + abs(Samples[thisindex][(samp*2)+3]		* halfheight));
					glVertex2f((samp+1)*multiplier,	halfheight);
				}
			}
			glEnd();
		}



		Rotator++;
		Rotator %= TIMES;

		glFinish();

		SwapBuffers(m_glDC);

	}


	return true;
}

bool	CTuniacVisual::About(HWND hWndParent)
{
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

/*
			float halfheight = ((float)m_LastHeight / 2.0f);
			float multiplier = (float)m_LastWidth / (float)(DISPLAYSAMPLES/2.0f);

			glColor4f(0,0,0, 0.5f);
			glBegin(GL_QUADS);
			{
				for(unsigned int samp=0; samp<NumSamples-2; samp++)
				{
					glVertex2f(samp*multiplier,		halfheight);
					glVertex2f(samp*multiplier,		halfheight 	- abs(AudioData[(samp*2)]		* halfheight) );

					glVertex2f((samp+1)*multiplier,	halfheight 	- abs(AudioData[(samp*2)+2]		* halfheight));
					glVertex2f((samp+1)*multiplier,	halfheight);
				}
			}
			glEnd();

			glColor4f(0,0,0, 0.5f);
			glBegin(GL_QUADS);
			{
				for(unsigned int samp=0; samp<NumSamples-2; samp++)
				{
					glVertex2f(samp*multiplier,		halfheight);
					glVertex2f(samp*multiplier,		halfheight + abs(AudioData[(samp*2)+1]		* halfheight));

					glVertex2f((samp+1)*multiplier,	halfheight + abs(AudioData[(samp*2)+3]		* halfheight));
					glVertex2f((samp+1)*multiplier,	halfheight);
				}
			}
			glEnd();
*/