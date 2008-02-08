#include "stdafx.h"
#include "tuniacvisual.h"

#define TIMES	8

#define DISPLAYSAMPLES	4096
#define VISUALSIZE		DISPLAYSAMPLES*TIMES

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
	}

	glClearColor(0.9f, 0.92f, 0.96f, 0.2f);
	glClear (GL_COLOR_BUFFER_BIT);
	glLoadIdentity();					// Reset The Modelview Matrix


	// draw background grid
	glColor4f(0,0,0, 0.2f);
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

	static float AudioData[VISUALSIZE];

	unsigned long NumSamples = min(m_LastWidth, DISPLAYSAMPLES);

	unsigned long Channels = (unsigned long)pHelper->GetVariable(Variable_NumChannels);
	if(pHelper->GetVisData(AudioData, NumSamples * Channels))
	{
		if(Channels == 2)
		{
			float halfheight = ((float)m_LastHeight / 2.0f);


			
			glColor4f(0,0,0, 1.0f / (float)TIMES);
			for(int start=0; start<TIMES; start++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for(unsigned int samp=start; samp<NumSamples; samp+=TIMES)
					{
						glVertex2f(samp,	halfheight);
						glVertex2f(samp,	halfheight + (AudioData[samp*2]		* halfheight));


						glVertex2f(samp,	halfheight);
						glVertex2f(samp,	halfheight - (AudioData[(samp*2)+1]	* halfheight));
					}
				}
				glEnd();
			}
		}
	}

	SwapBuffers(m_glDC);

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
