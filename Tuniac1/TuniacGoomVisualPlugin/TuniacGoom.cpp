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

#include "stdafx.h"
#include "resource.h"
#include "TuniacGoom.h"

ULONG_PTR m_gdiplusToken = 0;

extern HANDLE	hInst;

TuniacGoom::TuniacGoom(void)
{
	m_LastWidth	 = 0;
	m_LastHeight = 0;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_textureData = NULL;
	visdata = NULL;

	goom = NULL;

	m_hDC = NULL;
	m_glRC = NULL;
}

TuniacGoom::~TuniacGoom(void)
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


LRESULT CALLBACK TuniacGoom::WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	TuniacGoom * pTuniacGoom = (TuniacGoom *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pTuniacGoom->WndProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK TuniacGoom::WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TuniacGoom* pTuniacGoom = (TuniacGoom*)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
		pTuniacGoom = (TuniacGoom*)lParam;

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
		if (x + iWidth > iScreenWidth)  x = iScreenWidth - iWidth;
		if (y + iHeight > iScreenHeight) y = iScreenHeight - iHeight;

		MoveWindow(hDlg, x, y, iWidth, iHeight, FALSE);

		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEOPENGL, BM_SETCHECK, pTuniacGoom->iUseOpenGL ? BST_CHECKED : BST_UNCHECKED, 0);
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEPBO, BM_SETCHECK, pTuniacGoom->iUsePBO ? BST_CHECKED : BST_UNCHECKED, 0);
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO, BM_SETCHECK, pTuniacGoom->iUseMappedPBO ? BST_CHECKED : BST_UNCHECKED, 0);

		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEGDI, BM_SETCHECK, pTuniacGoom->iUseOpenGL ? BST_UNCHECKED : BST_CHECKED, 0);

		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_ALLOWNONPOWEROF2, BM_SETCHECK, pTuniacGoom->iAllowNonPowerOf2 ? BST_CHECKED : BST_UNCHECKED, 0);


		if (!pTuniacGoom->iUseOpenGL || !pTuniacGoom->bPBOSupport)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEPBO), false);
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), false);
		}

		if (!pTuniacGoom->iUsePBO)
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), false);

		if (!pTuniacGoom->bNonPowerOf2Support)
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_ALLOWNONPOWEROF2), false);

		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("128*128"));
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("256*256"));
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("512*512"));
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("1024*1024"));
		if (pTuniacGoom->iAllowNonPowerOf2)
		{
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("-------"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("240*240"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("420*420"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("640*640"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("800*800"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("Max res"));
		}

		int curres = 0;
		if (pTuniacGoom->iVisMaxRes == 256)
			curres = 1;
		if (pTuniacGoom->iVisMaxRes == 512)
			curres = 2;
		if (pTuniacGoom->iVisMaxRes == 1024)
			curres = 3;
		if (pTuniacGoom->iAllowNonPowerOf2)
		{
			if (pTuniacGoom->iVisMaxRes == 240)
				curres = 5;
			if (pTuniacGoom->iVisMaxRes == 420)
				curres = 6;
			if (pTuniacGoom->iVisMaxRes == 640)
				curres = 7;
			if (pTuniacGoom->iVisMaxRes == 800)
				curres = 8;
			if (pTuniacGoom->iVisMaxRes == 2048)
				curres = 9;
		}
		SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_SETCURSEL, curres, 0);
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_TUNIACGOOMPREF_USEOPENGL:
		{
			pTuniacGoom->iUseOpenGL = true;

			if (pTuniacGoom->bPBOSupport)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEPBO), true);
				EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), pTuniacGoom->iUsePBO ? true : false);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEPBO), false);
				EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), false);
			}

			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("UseOpenGL"), REG_DWORD, (LPBYTE)&pTuniacGoom->iUseOpenGL, sizeof(int));
		}
		break;

		case IDC_TUNIACGOOMPREF_USEGDI:
		{
			pTuniacGoom->iUseOpenGL = false;
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEPBO), false);
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), false);
			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("UseOpenGL"), REG_DWORD, (LPBYTE)&pTuniacGoom->iUseOpenGL, sizeof(int));
		}
		break;

		case IDC_TUNIACGOOMPREF_USEPBO:
		{
			int State = SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEPBO, BM_GETCHECK, 0, 0);
			pTuniacGoom->iUsePBO = State == BST_UNCHECKED ? FALSE : TRUE;
			EnableWindow(GetDlgItem(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO), pTuniacGoom->iUsePBO ? TRUE : FALSE);
			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("UsePBO"), REG_DWORD, (LPBYTE)&pTuniacGoom->iUsePBO, sizeof(int));
		}
		break;

		case IDC_TUNIACGOOMPREF_USEMAPPEDPBO:
		{
			int State = SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_USEMAPPEDPBO, BM_GETCHECK, 0, 0);
			pTuniacGoom->iUseMappedPBO = State == BST_UNCHECKED ? FALSE : TRUE;
			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("UseMappedPBO"), REG_DWORD, (LPBYTE)&pTuniacGoom->iUseMappedPBO, sizeof(int));
		}
		break;

		case IDC_TUNIACGOOMPREF_ALLOWNONPOWEROF2:
		{
			int State = SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_ALLOWNONPOWEROF2, BM_GETCHECK, 0, 0);
			pTuniacGoom->iAllowNonPowerOf2 = State == BST_UNCHECKED ? FALSE : TRUE;

			int curres = SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_GETCURSEL, 0, 0);

			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("128*128"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("256*256"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("512*512"));
			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("1024*1024"));
			if (pTuniacGoom->iAllowNonPowerOf2)
			{
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("-------"));
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("240*240"));
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("420*420"));
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("640*640"));
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("800*800"));
				SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("Max(not recommended)"));
			}
			else if (curres > 2)
			{
				curres = 0;
				pTuniacGoom->iVisMaxRes = 128;
				pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("VisMaxRes"), REG_DWORD, (LPBYTE)&pTuniacGoom->iVisMaxRes, sizeof(int));
			}

			SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_SETCURSEL, curres, 0);

			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("AllowNonPowerOf2"), REG_DWORD, (LPBYTE)&pTuniacGoom->iAllowNonPowerOf2, sizeof(int));
		}
		break;

		case IDC_TUNIACGOOMPREF_MAXVISRES:
		{
			int curres = SendDlgItemMessage(hDlg, IDC_TUNIACGOOMPREF_MAXVISRES, CB_GETCURSEL, 0, 0);
			if (curres == 0)
				pTuniacGoom->iVisMaxRes = 128;
			if (curres == 1)
				pTuniacGoom->iVisMaxRes = 256;
			if (curres == 2)
				pTuniacGoom->iVisMaxRes = 512;
			if (curres == 3)
				pTuniacGoom->iVisMaxRes = 1024;
			if (curres == 5)
				pTuniacGoom->iVisMaxRes = 240;
			if (curres == 6)
				pTuniacGoom->iVisMaxRes = 420;
			if (curres == 7)
				pTuniacGoom->iVisMaxRes = 640;
			if (curres == 8)
				pTuniacGoom->iVisMaxRes = 800;
			if (curres == 9)
				pTuniacGoom->iVisMaxRes = 2048;

			pTuniacGoom->m_pHelper->SetVisualPref(TEXT("TuniacGoom"), TEXT("VisMaxRes"), REG_DWORD, (LPBYTE)&pTuniacGoom->iVisMaxRes, sizeof(int));
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

	default:
		return FALSE;

	}
	return TRUE;
}

void	TuniacGoom::Destroy(void)
{
	Detach();

	delete this;
}

bool TuniacGoom::RenderVisual(void)
{
	CAutoLock m(&m_RenderLock);

	if (m_pHelper)
	{
		ulNumChannels = (unsigned long)m_pHelper->GetVariable(Variable_NumChannels);
		if (ulNumChannels == INVALID || ulNumChannels == 0)
			return false;

		if (ulNumChannels != ulOldNumChannels)
		{
			if (visdata)
			{
				_aligned_free(visdata);
			}

			ulOldNumChannels = ulNumChannels;
			visdata = (float*)_aligned_malloc(VISDATA_SIZE * ulNumChannels * sizeof(float), 16);
		}

		m_pHelper->GetVisData(visdata, VISDATA_SIZE * ulNumChannels);

		int sample = 0;
		for (int x = 0; x < VISDATA_SIZE; x++)
		{
			if (ulNumChannels == 1)
			{
				vd[0][x] = vd[1][x] = (visdata[sample] * 32767.0f);
			}
			else
			{
				vd[0][x] = (visdata[sample] * 32767.0f);
				vd[1][x] = (visdata[sample + 1] * 32767.0f);
			}

			sample += ulNumChannels;
		}

		if (!goom_set_screenbuffer(goom, (void*)m_textureData))
			return false;

		return goom_update(goom, vd, 0, 0.0f, 0, 0);
	}

	return true;
}

LPTSTR	TuniacGoom::GetPluginName(void)
{
	return TEXT("Goom");
}

unsigned long TuniacGoom::GetFlags(void)
{
	return PLUGINFLAGS_CONFIG | PLUGINFLAGS_ABOUT;
}

bool	TuniacGoom::SetHelper(ITuniacVisHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

bool	TuniacGoom::Attach(HDC hDC)
{
	CAutoLock m(&m_RenderLock);

	m_LastWidth	 = -1;
	m_LastHeight = -1;

	m_glRC = NULL;
	m_hDC = hDC;

	bPBOSupport = false;
	bNonPowerOf2Support = false;

	iUseOpenGL = 1;
	iUsePBO = 0;
	iUseMappedPBO = 0;
	iVisMaxRes = 640;
	iAllowNonPowerOf2 = 0;

	m_LastWidth = -1;
	m_LastHeight = -1;

	iVisResHeight = 512;
	iVisResWidth = 512;

	bResChange = true;

	DWORD				lpRegType = REG_DWORD;
	DWORD				iRegSize = sizeof(int);

	m_pHelper->GetVisualPref(TEXT("TuniacGoom"), TEXT("VisMaxRes"), &lpRegType, (LPBYTE)&iVisMaxRes, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("TuniacGoom"), TEXT("AllowNonPowerOf2"), &lpRegType, (LPBYTE)&iAllowNonPowerOf2, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("TuniacGoom"), TEXT("UseOpenGL"), &lpRegType, (LPBYTE)&iUseOpenGL, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("TuniacGoom"), TEXT("UsePBO"), &lpRegType, (LPBYTE)&iUsePBO, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("TuniacGoom"), TEXT("UseMappedPBO"), &lpRegType, (LPBYTE)&iUseMappedPBO, &iRegSize);

	if (!InitOpenGL())
		return false;

	ulOldNumChannels = (unsigned long)m_pHelper->GetVariable(Variable_NumChannels);

	visdata = (float*)_aligned_malloc(VISDATA_SIZE * ulOldNumChannels * sizeof(float), 16);

	goom = goom_init((guint32)iVisResWidth, (guint32)iVisResHeight);

	//Render(iVisResWidth, iVisResHeight);

	return true;
}

bool	TuniacGoom::Detach()
{
	if (goom)
	{
		goom_close(goom);
		goom = NULL;
	}

	ShutdownOpenGL();

	m_hDC = NULL;
	if (m_textureData)
	{
		free(m_textureData);
		m_textureData = NULL;
	}

	return true;
}

bool	TuniacGoom::Render(int w, int h)
{
	CAutoLock m(&m_RenderLock);

	if (iUseOpenGL && !wglGetCurrentContext())
		return false;

	if ((m_LastWidth != w) || (m_LastHeight != h) || bResChange)
	{
		if (w == 0 || h == 0)
			return false;

		if (iAllowNonPowerOf2)
		{
			iVisResHeight = min(h, iVisMaxRes);
			iVisResWidth = min(w, iVisMaxRes);
		}
		else
		{

			if (h >= iVisMaxRes || w >= iVisMaxRes)
				iVisResHeight = iVisResWidth = iVisMaxRes;
			else
			{
				int iVisTempRes = max(h, w);

				if (iVisTempRes >= iVisMaxRes)
					iVisResHeight = iVisResWidth = iVisMaxRes;
				else if (iVisTempRes <= 128)
					iVisResHeight = iVisResWidth = 128;
				else if (iVisTempRes <= 256)
					iVisResHeight = iVisResWidth = 256;
				else if (iVisTempRes <= 512)
					iVisResHeight = iVisResWidth = 512;
				else
					iVisResHeight = iVisResWidth = 1024;
			}
		}

		iTextureSize = iVisResWidth * iVisResHeight * sizeof(unsigned long);

		if (m_textureData)
		{
			free(m_textureData);
			m_textureData = NULL;
		}

		m_textureData = (unsigned long*)malloc(iTextureSize);

		if (!goom_set_screenbuffer(goom, (void*)m_textureData))
			return false;

		goom_set_resolution(goom, iVisResWidth, iVisResHeight);


		if (iUseOpenGL)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, (float)w, (float)h, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glViewport(0, 0, w, h);

			//bind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB8,
				iVisResWidth,
				iVisResHeight,
				0,
				GL_BGRA,
				GL_UNSIGNED_INT_8_8_8_8_REV,
				0);
		}

		bResChange = false;

		m_LastWidth = w;
		m_LastHeight = h;
	}

	RenderVisual();

	if (iUseOpenGL)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//update texture
		if (iUsePBO)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, iTextureSize, 0, GL_STREAM_DRAW);
			if (iUseMappedPBO)
			{
				ioMem = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
				memcpy(ioMem, m_textureData, iTextureSize);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			}
			else
			{
				glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, iTextureSize, m_textureData);
			}

			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				0,
				0,
				iVisResWidth,
				iVisResHeight,
				GL_BGRA,
				GL_UNSIGNED_INT_8_8_8_8_REV,
				0);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		else
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				0,
				0,
				iVisResWidth,
				iVisResHeight,
				GL_BGRA,
				GL_UNSIGNED_INT_8_8_8_8_REV,
				m_textureData);
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f, h);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(w, h);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(w, 0.0f);
		glEnd();

		SwapBuffers(m_hDC);
	}
	else
	{
		BITMAPINFO		bmi = { 0, };

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = (long)iVisResWidth;
		bmi.bmiHeader.biHeight = -((long)iVisResHeight);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;

		SetStretchBltMode(m_hDC, HALFTONE);
		SetBrushOrgEx(m_hDC, 0, 0, NULL);
		StretchDIBits(m_hDC,
			0,
			0,
			w,
			h,
			0,
			0,
			iVisResWidth,
			iVisResHeight,
			m_textureData,
			&bmi,
			DIB_RGB_COLORS,
			SRCCOPY);


		//BitBlt(m_hDC, 0, 0, iVisResWidth, iVisResHeight, hgdiDC, 0,0, SRCCOPY);
		UpdateWindow(WindowFromDC(m_hDC));
	}

	return true;
}

bool	TuniacGoom::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Tuniac Goom visual.\n\nUses Goom:\nCopyright (c)2000-2004, Jean-Christophe Hoelt <jeko@ios-software.com>\n\ngoom.lib from kodi/xbmc/visualization.goom"), GetPluginName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool	TuniacGoom::Configure(HWND hWndParent)
{
	CAutoLock m(&m_RenderLock);

	if (iUseOpenGL)
	{
		ShutdownOpenGL();
	}

	DialogBoxParam(GetModuleHandle(L"TuniacGoomVisualPlugin.dll"), MAKEINTRESOURCE(IDD_TUNIACGOOMPREFWINDOW), hWndParent, (DLGPROC)WndProcStub, (DWORD_PTR)this);

	if (iUseOpenGL)
		InitOpenGL();

	bResChange = true;

	return true;
}

bool	TuniacGoom::Notify(unsigned long Notification)
{
	return true;
}

bool	TuniacGoom::MouseFunction(unsigned long function, int x, int y)
{
	return true;
}


bool	TuniacGoom::InitOpenGL(void)
{
	CAutoLock m(&m_RenderLock);

	GLuint		PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER |
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
		return false;

	if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
		return false;

	if (!(m_glRC = wglCreateContext(m_hDC)))
		return false;

	if (!wglMakeCurrent(m_hDC, m_glRC))
		return false;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	//visual texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	SwapBuffers(m_hDC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		return false;


	//const char *version = (char*)glGetString( GL_VERSION );

	if (glewIsSupported("GL_VERSION_2_1"))
		bPBOSupport = true;
	else
		iUsePBO = 0;

	if (glewIsSupported("GL_VERSION_2_0"))
		bNonPowerOf2Support = true;
	else
		iAllowNonPowerOf2 = 0;

	if (iUsePBO)
	{
		glGenBuffers(1, pboIds);
	}

	return true;
}

void	TuniacGoom::ShutdownOpenGL(void)
{
	CAutoLock m(&m_RenderLock);

	//flush
	if (wglGetCurrentContext())
	{
		SwapBuffers(m_hDC);

		if (iUsePBO)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, iTextureSize, 0, GL_STREAM_DRAW);
			glDeleteBuffers(1, pboIds);
		}

		//flush
		SwapBuffers(m_hDC);

		if (m_glRC)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_glRC);
			m_glRC = NULL;
		}
	}
}