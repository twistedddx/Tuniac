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

#include "stdafx.h"
#include "tuniacvisual.h"
#include "resource.h"

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

	fHalfHeight = 0.0f;
	fShift = 0.0f;
	fClearRate = 0.8f;
	fShiftRate = 0.001f;
	
	bFillStrip = 0;

	Samples = NULL;
	m_glRC = NULL;
	m_glDC = NULL;
}

CTuniacVisual::~CTuniacVisual(void)
{
}


LRESULT CALLBACK CTuniacVisual::WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CTuniacVisual * pCTuniacVisual = (CTuniacVisual *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pCTuniacVisual->WndProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK CTuniacVisual::WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTuniacVisual * pCTuniacVisual = (CTuniacVisual *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
		pCTuniacVisual = (CTuniacVisual *)lParam;

		HWND		hParent = GetParent(hDlg);
		RECT		rcDlg, rcParent;

		GetWindowRect(hDlg, &rcDlg);
		GetWindowRect(hParent, &rcParent);

		int iWidth = rcDlg.right - rcDlg.left;
		int iHeight = rcDlg.bottom - rcDlg.top;

		int x = ((rcParent.right - rcParent.left) - iWidth) / 2 + rcParent.left;
		int y = ((rcParent.bottom - rcParent.top) - iHeight) / 2 + rcParent.top;

		int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x + iWidth  > iScreenWidth)  x = iScreenWidth - iWidth;
		if (y + iHeight > iScreenHeight) y = iScreenHeight - iHeight;

		MoveWindow(hDlg, x, y, iWidth, iHeight, FALSE);

		SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_FILLSTRIP, BM_SETCHECK, pCTuniacVisual->bFillStrip ? BST_CHECKED : BST_UNCHECKED, 0);

		TCHAR		tstr[32];

		StringCchPrintf(tstr, 32, TEXT("Clear rate modifier %1.1f"), pCTuniacVisual->fClearRate);
		SetDlgItemText(hDlg, IDC_TUNIACVISPREF_CLEARRATE_TEXT, tstr);
		SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_CLEARRATE_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(1, 10));
		SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_CLEARRATE_SLIDER, TBM_SETPOS, TRUE, pCTuniacVisual->fClearRate * 10.0f);

		StringCchPrintf(tstr, 32, TEXT("Line width modifier %1.3f"), pCTuniacVisual->fShiftRate);
		SetDlgItemText(hDlg, IDC_TUNIACVISPREF_SHIFTRATE_TEXT, tstr);
		SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_SHIFTRATE_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, 10));
		SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_SHIFTRATE_SLIDER, TBM_SETPOS, TRUE, pCTuniacVisual->fShiftRate * 1000.0f);
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_TUNIACVISPREF_FILLSTRIP:
			{
				pCTuniacVisual->bFillStrip = true;

				int State = SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_FILLSTRIP, BM_GETCHECK, 0, 0);
				pCTuniacVisual->bFillStrip = State == BST_UNCHECKED ? FALSE : TRUE;
				pCTuniacVisual->m_pHelper->SetVisualPref(TEXT("TuniacVisual"), TEXT("FillStrip"), REG_DWORD, (LPBYTE)&pCTuniacVisual->bFillStrip, sizeof(int));
			}
			break;

			case IDOK:
			case IDCANCEL:
			{
				EndDialog(hDlg, wParam);
				return TRUE;
			}
			break;
		}

	case WM_HSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			TCHAR		tstr[42];

			pCTuniacVisual->fClearRate = SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_CLEARRATE_SLIDER, TBM_GETPOS, 0, 0) / 10.0f;
			StringCchPrintf(tstr, 42, TEXT("Clear rate modifier %1.1f"), pCTuniacVisual->fClearRate);
			SetDlgItemText(hDlg, IDC_TUNIACVISPREF_CLEARRATE_TEXT, tstr);

			pCTuniacVisual->fShiftRate = SendDlgItemMessage(hDlg, IDC_TUNIACVISPREF_SHIFTRATE_SLIDER, TBM_GETPOS, 0, 0) / 1000.0f;
			StringCchPrintf(tstr, 42, TEXT("Line width modifier %1.3f"), pCTuniacVisual->fShiftRate);
			SetDlgItemText(hDlg, IDC_TUNIACVISPREF_SHIFTRATE_TEXT, tstr);
			pCTuniacVisual->fShift = ((float)m_LastWidth*pCTuniacVisual->fShiftRate) + 1.0f;

			pCTuniacVisual->m_pHelper->SetVisualPref(TEXT("TuniacVisual"), TEXT("ClearRate"), REG_DWORD, (LPBYTE)&pCTuniacVisual->fClearRate, sizeof(float));
			pCTuniacVisual->m_pHelper->SetVisualPref(TEXT("TuniacVisual"), TEXT("ShiftRate"), REG_DWORD, (LPBYTE)&pCTuniacVisual->fShiftRate, sizeof(float));

		}
		break;
		}
	}
	break;

	default:
		return FALSE;

	}
	return TRUE;
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
	return PLUGINFLAGS_CONFIG | PLUGINFLAGS_ABOUT;
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

	//glClearColor(0.9f, 0.92f, 0.96f, 0.2f);
	glClear (GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	glEnable(GL_POLYGON_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Samples = (float *)VirtualAlloc(NULL, DISPLAYSAMPLES * sizeof(float), MEM_COMMIT, PAGE_READWRITE);
	Samples = (float*)_aligned_malloc(DISPLAYSAMPLES * sizeof(float), 16);

	bFillStrip = 0;
	fClearRate = 0.8f;
	fShift = 0.0001f;

	DWORD	lpRegType = REG_DWORD;
	DWORD	dwRegSize = sizeof(int);
	m_pHelper->GetVisualPref(TEXT("TuniacVisual"), TEXT("FillStrip"), &lpRegType, (LPBYTE)&bFillStrip, &dwRegSize);

	dwRegSize = sizeof(float);
	m_pHelper->GetVisualPref(TEXT("TuniacVisual"), TEXT("ShiftRate"), &lpRegType, (LPBYTE)&fShiftRate, &dwRegSize);
	m_pHelper->GetVisualPref(TEXT("TuniacVisual"), TEXT("ClearRate"), &lpRegType, (LPBYTE)&fClearRate, &dwRegSize);

	glFinish();
	SwapBuffers(m_glDC);


	return true;
}

bool	CTuniacVisual::Detach()
{
	//flush
	glFinish();
	SwapBuffers(m_glDC);

	if (m_glRC)
	{
		wglMakeCurrent(NULL, NULL);
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
	if (wglGetCurrentContext())
	{
		if ((m_LastWidth != w) || (m_LastHeight != h))
		{
			m_LastWidth = w;
			m_LastHeight = h;
			fHalfHeight = ((float)m_LastHeight * 0.5f);
			fShift = ((float)m_LastWidth*fShiftRate) + 1.0f;

			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0.0, (float)w, (float)h, 0.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		//glClear (GL_COLOR_BUFFER_BIT);

		//matt gray
		glBegin(GL_QUAD_STRIP);
		{
			glColor4f(0.9f, 0.92f, 0.96f, fClearRate);
			glVertex2f(0, 0);

			glColor4f(0.8f, 0.82f, 0.86f, fClearRate);
			glVertex2f(0, m_LastHeight);

			glColor4f(0.8f, 0.82f, 0.86f, fClearRate);
			glVertex2f(m_LastWidth, 0);

			glColor4f(0.4f, 0.42f, 0.46f, fClearRate);
			glVertex2f(m_LastWidth, m_LastHeight);
		}
		glEnd();

		// draw background grid
		glColor4f(0, 0, 0, 0.1f);
		glBegin(GL_LINES);
		{
			for (unsigned long x = 0; x < m_LastWidth; x += m_LastWidth / 10)
			{
				glVertex2i(x, 0);
				glVertex2i(x, m_LastHeight);
			}

			for (unsigned long y = 0; y < m_LastHeight; y += m_LastHeight / 10)
			{
				glVertex2i(0, y);
				glVertex2i(m_LastWidth, y);
			}
		}
		glEnd();

		unsigned long ulSamples = m_pHelper->GetVisData(Samples, DISPLAYSAMPLES);
		if (ulSamples)
		{
			float fSeparation = m_LastWidth / (float)ulSamples;

			glColor4f(0, 0, 0, 1);
			glBegin(GL_TRIANGLE_STRIP);
			{
				//left channel
				unsigned long ulLeftSample = 0;
				for (unsigned long ulPoint = 0; ulPoint < ulSamples/2; ulPoint++)
				{
					if(bFillStrip)
						glVertex2f((float)ulPoint*fSeparation, fHalfHeight);
	
					glVertex2f((float)ulPoint*fSeparation, fHalfHeight + Samples[ulLeftSample] * fHalfHeight);

					if (!bFillStrip)
						glVertex2f(((float)ulPoint*fSeparation)+fShift, fHalfHeight + (Samples[ulLeftSample] * fHalfHeight));

					ulLeftSample += 2;
				}
			}
			glEnd();

			glColor4f(0, 0, 0, 1);
			glBegin(GL_TRIANGLE_STRIP);
			{
				//right channel
				unsigned long ulRightSample = 1;
				for (unsigned long ulPoint = ulSamples / 2; ulPoint < ulSamples; ulPoint++)
				{
					if (bFillStrip)
						glVertex2f((float)ulPoint*fSeparation, fHalfHeight);

					glVertex2f((float)ulPoint*fSeparation, fHalfHeight + (Samples[ulRightSample] * fHalfHeight));

					if (!bFillStrip)
						glVertex2f(((float)ulPoint*fSeparation) + fShift, fHalfHeight + (Samples[ulRightSample] * fHalfHeight));

					ulRightSample += 2;
				}
			}
			glEnd();
		}

		glFinish();
		SwapBuffers(m_glDC);
	}

	return true;
}

bool	CTuniacVisual::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Tuniac included visual.\nTony Million 2009\nBrett Hoyle 2016"), GetPluginName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool	CTuniacVisual::Configure(HWND hWndParent)
{
	DialogBoxParam(GetModuleHandle(L"tuniacapp.exe"), MAKEINTRESOURCE(IDD_TUNIACVISPREFWINDOW), hWndParent, (DLGPROC)WndProcStub, (DWORD_PTR)this);
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