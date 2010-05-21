#include "StdAfx.h"
#include <emmintrin.h>
#include "svprenderer.h"
#include "resource.h"

LRESULT CALLBACK SVPRenderer::WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	SVPRenderer * pSVPRenderer = (SVPRenderer *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pSVPRenderer->WndProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK SVPRenderer::WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	SVPRenderer * pSVPRenderer = (SVPRenderer *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);

    switch (uMsg) 
    { 
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pSVPRenderer = (SVPRenderer *)lParam;

				SendDlgItemMessage(hDlg, IDC_SVPPREF_USEOPENGL, BM_SETCHECK, pSVPRenderer->iUseOpenGL ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_SVPPREF_USEPBO, BM_SETCHECK, pSVPRenderer->iUsePBO ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_SVPPREF_USEMAPPEDPBO, BM_SETCHECK, pSVPRenderer->iUseMappedPBO ? BST_CHECKED : BST_UNCHECKED, 0);
				
				SendDlgItemMessage(hDlg, IDC_SVPPREF_USEGDI, BM_SETCHECK, pSVPRenderer->iUseOpenGL ? BST_UNCHECKED : BST_CHECKED, 0);

				SendDlgItemMessage(hDlg, IDC_SVPPREF_ALLOWNONPOWEROF2, BM_SETCHECK, pSVPRenderer->iAllowNonPowerOf2 ? BST_CHECKED : BST_UNCHECKED, 0);


				if(!pSVPRenderer->iUseOpenGL || !pSVPRenderer->bPBOSupport)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEPBO), false);
					EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), false);
				}

				if(!pSVPRenderer->iUsePBO)
					EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), false);

				if(!pSVPRenderer->bNonPowerOf2Support)
					EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_ALLOWNONPOWEROF2), false);

				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("128*128"));
				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("256*256"));
				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("512*512"));
				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("1024*1024"));
				if(pSVPRenderer->iAllowNonPowerOf2)
				{
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("-------"));
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("240*240"));
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("420*420"));
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("640*640"));
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("800*800"));
					SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("Max res"));
				}

				int curres = 0;
				if(pSVPRenderer->iVisMaxRes == 256)
					curres = 1;
				if(pSVPRenderer->iVisMaxRes == 512)
					curres = 2;
				if(pSVPRenderer->iVisMaxRes == 1024)
					curres = 3;
				if(pSVPRenderer->iAllowNonPowerOf2)
				{
					if(pSVPRenderer->iVisMaxRes == 240)
						curres = 5;
					if(pSVPRenderer->iVisMaxRes == 420)
						curres = 6;
					if(pSVPRenderer->iVisMaxRes == 640)
						curres = 7;
					if(pSVPRenderer->iVisMaxRes == 800)
						curres = 8;
					if(pSVPRenderer->iVisMaxRes == 2048)
						curres = 9;
				}
				SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_SETCURSEL, curres, 0);
			}
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
				case IDC_SVPPREF_USEOPENGL:
					{
						pSVPRenderer->iUseOpenGL = true;

						if(pSVPRenderer->bPBOSupport)
						{
							EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEPBO), true);
							EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), pSVPRenderer->iUsePBO ? true : false);
						}
						else
						{
							EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEPBO), false);
							EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), false);
						}

						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("UseOpenGL"), REG_DWORD, (LPBYTE)&pSVPRenderer->iUseOpenGL, sizeof(int));
					}
					break;

				case IDC_SVPPREF_USEGDI:
					{
						pSVPRenderer->iUseOpenGL = false;
						EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEPBO), false);
						EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), false);
						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("UseOpenGL"), REG_DWORD, (LPBYTE)&pSVPRenderer->iUseOpenGL, sizeof(int));
					}
					break;

				case IDC_SVPPREF_USEPBO:
					{
						int State = SendDlgItemMessage(hDlg, IDC_SVPPREF_USEPBO, BM_GETCHECK, 0, 0);
						pSVPRenderer->iUsePBO = State == BST_UNCHECKED ? FALSE : TRUE;
						EnableWindow(GetDlgItem(hDlg, IDC_SVPPREF_USEMAPPEDPBO), pSVPRenderer->iUsePBO ? TRUE : FALSE);
						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("UsePBO"), REG_DWORD, (LPBYTE)&pSVPRenderer->iUsePBO, sizeof(int));
					}
					break;

				case IDC_SVPPREF_USEMAPPEDPBO:
					{
						int State = SendDlgItemMessage(hDlg, IDC_SVPPREF_USEMAPPEDPBO, BM_GETCHECK, 0, 0);
						pSVPRenderer->iUseMappedPBO = State == BST_UNCHECKED ? FALSE : TRUE;
						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("UseMappedPBO"), REG_DWORD, (LPBYTE)&pSVPRenderer->iUseMappedPBO, sizeof(int));
					}
					break;

				case IDC_SVPPREF_ALLOWNONPOWEROF2:
					{
						int State = SendDlgItemMessage(hDlg, IDC_SVPPREF_ALLOWNONPOWEROF2, BM_GETCHECK, 0, 0);
						pSVPRenderer->iAllowNonPowerOf2 = State == BST_UNCHECKED ? FALSE : TRUE;

						int curres = SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_GETCURSEL, 0, 0);

						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_RESETCONTENT, 0, 0);
						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("128*128"));
						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("256*256"));
						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("512*512"));
						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("1024*1024"));
						if(pSVPRenderer->iAllowNonPowerOf2)
						{
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("-------"));
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("240*240"));
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("420*420"));
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("640*640"));
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("800*800"));
							SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_ADDSTRING, 0, (LPARAM)TEXT("Max(not recommended)"));
						}
						else if(curres > 2)
						{
							curres = 0;
							pSVPRenderer->iVisMaxRes = 128;
							pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("VisMaxRes"), REG_DWORD, (LPBYTE)&pSVPRenderer->iVisMaxRes, sizeof(int));
						}

						SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_SETCURSEL, curres, 0);

						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("AllowNonPowerOf2"), REG_DWORD, (LPBYTE)&pSVPRenderer->iAllowNonPowerOf2, sizeof(int));
					}
					break;

				case IDC_SVPPREF_MAXVISRES:
					{
						int curres = SendDlgItemMessage(hDlg, IDC_SVPPREF_MAXVISRES, CB_GETCURSEL, 0, 0);
						if(curres == 0)
							pSVPRenderer->iVisMaxRes = 128;
						if(curres == 1)
							pSVPRenderer->iVisMaxRes = 256;
						if(curres == 2)
							pSVPRenderer->iVisMaxRes = 512;
						if(curres == 3)
							pSVPRenderer->iVisMaxRes = 1024;
						if(curres == 5)
							pSVPRenderer->iVisMaxRes = 240;
						if(curres == 6)
							pSVPRenderer->iVisMaxRes = 420;
						if(curres == 7)
							pSVPRenderer->iVisMaxRes = 640;
						if(curres == 8)
							pSVPRenderer->iVisMaxRes = 800;
						if(curres == 9)
							pSVPRenderer->iVisMaxRes = 2048;

						pSVPRenderer->m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("VisMaxRes"), REG_DWORD, (LPBYTE)&pSVPRenderer->iVisMaxRes, sizeof(int));
					}
					break;

                case IDOK: 
					{
						EndDialog(hDlg, wParam); 
						return TRUE;
					}
					break;
            }

		default:
			return FALSE;
			break;

    } 
    return TRUE; 
} 

ULONG_PTR m_gdiplusToken = 0;

extern HANDLE	hInst;

__m64 Get_m64(__int64 n)
{
    union __m64__m64
    {
        __m64 m;
        __int64 i;
    } mi;

    mi.i = n;
    return mi.m;
}

#if defined (_M_IX86)

void ChangeBrightnessC_MMX(
    unsigned long* pSource, 
    unsigned long* pDest, 
    int nNumberOfPixels, 
    int nChange)
{
    if ( nChange > 255 )
        nChange = 255;
    else if ( nChange < -255 )
        nChange = -255;

    BYTE b = (BYTE) abs(nChange);

    // make 64 bits value with b in each byte
    __int64 c = b;

    for ( int i = 1; i <= 7; i++ )
    {
        c = c << 8;
        c |= b;
    }

    // 2 pixels are processed in one loop
    int nNumberOfLoops = nNumberOfPixels / 2;

    __m64* pIn = (__m64*) pSource;          // input pointer
    __m64* pOut = (__m64*) pDest;           // output pointer

    __m64 tmp;                              // work variable


    _mm_empty();                            // emms

    __m64 nChange64 = Get_m64(c);

    if ( nChange > 0 )
    {
        for ( int i = 0; i < nNumberOfLoops; i++ )
        {
            tmp = _mm_adds_pu8(*pIn, nChange64); // Unsigned addition 
                                                 // with saturation.
                                                 // tmp = *pIn + nChange64
                                                 // for each byte

            *pOut = tmp;

            pIn++;                               // next 2 pixels
            pOut++;
        }
    }
    else
    {
        for ( int i = 0; i < nNumberOfLoops; i++ )
        {
            tmp = _mm_subs_pu8(*pIn, nChange64); // Unsigned subtraction 
                                                 // with saturation.
                                                 // tmp = *pIn - nChange64
                                                 // for each byte

            *pOut = tmp;

            pIn++;                               // next 2 pixels
            pOut++;
        }
    }

    _mm_empty();                            // emms
}

#else

void ChangeBrightnessC_MMX(
    unsigned long* pSource, 
    unsigned long* pDest, 
    int nNumberOfPixels, 
    int nChange)
{
	for(int rep = 0; rep < nNumberOfPixels; rep++)
	{
		int x = (int)*pSource;

		x-=nChange;

		if(x<0)
			x=0;

		*pDest = x;

		pSource++;
	}
}

#endif

SVPRenderer::SVPRenderer(void)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_textureData = NULL;
	visdata = NULL;

	m_hDC = NULL;
	m_glRC = NULL;

	hgdiDC = NULL;
	hVisBMP = NULL;
	hOldVisBMP = NULL;

	kiss_cfg = kiss_fftr_alloc(512,0,NULL,NULL);
	freq_data = (kiss_fft_cpx*)KISS_FFT_MALLOC((512)*sizeof(kiss_fft_cpx));
}

SVPRenderer::~SVPRenderer(void)
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

LPTSTR	SVPRenderer::GetPluginName(void)
{
	return TEXT("Sonique SVP Visual Plugin");
}

unsigned long SVPRenderer::GetFlags(void)
{
	return PLUGINFLAGS_CONFIG | PLUGINFLAGS_ABOUT;
}

bool	SVPRenderer::SetHelper(ITuniacVisHelper *pHelper)
{
	m_pHelper = pHelper;
	return true;
}

bool SVPRenderer::AddFolderOfSVP(LPTSTR	szFolder)
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;

	TCHAR	szFilename[1024];

	StrCpyN(szFilename, szFolder, 1024);
	PathAddBackslash(szFilename);
	StrCat(szFilename, TEXT("*.*"));

	hFind = FindFirstFile( szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR temp[1024];

			StrCpy(temp, szFolder);
			PathAddBackslash(temp);
			StrCat(temp, w32fd.cFileName);

			if(GetFileAttributes(temp) & FILE_ATTRIBUTE_DIRECTORY)
			{
				AddFolderOfSVP(temp);	
			}
			else
			{
				LPTSTR ext = PathFindExtension(temp);

				if(!StrCmpI(ext, TEXT(".SVP")))
				{
					LPTSTR string = (LPTSTR)malloc(2048 * sizeof(TCHAR));

					StrCpy(string, temp);

					m_VisFilenameArray.AddTail(string);
				}
			}
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}

	return true;
}

bool	SVPRenderer::Attach(HDC hDC)
{

	m_glRC				= NULL;
	m_hDC				= hDC;

	bPBOSupport			= false;
	bNonPowerOf2Support	= false;

	iUseOpenGL			= 1;
	iUsePBO				= 0;
	iUseMappedPBO		= 0;
	iVisMaxRes			= 128;
	iAllowNonPowerOf2	= 0;

	m_LastWidth			= -1;
	m_LastHeight		= -1;
	m_SelectedVisual	= 0;
	m_TheVisual			= NULL;
	iVisResHeight		= 128;
	iVisResWidth		= 128;

	bResChange			= true;

	DWORD				lpRegType = REG_DWORD;
	DWORD				iRegSize = sizeof(int);

	m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("VisMaxRes"), &lpRegType, (LPBYTE)&iVisMaxRes, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("AllowNonPowerOf2"), &lpRegType, (LPBYTE)&iAllowNonPowerOf2, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("UseOpenGL"), &lpRegType, (LPBYTE)&iUseOpenGL, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("UsePBO"), &lpRegType, (LPBYTE)&iUsePBO, &iRegSize);
	m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("UseMappedPBO"), &lpRegType, (LPBYTE)&iUseMappedPBO, &iRegSize);

	if(iUseOpenGL)
	{
		if(!InitOpenGL())
			return false;
	}

	GLenum err = glewInit();
	if (GLEW_OK != err)
		return false;

	if(glewIsSupported("GL_VERSION_2_1"))
		bPBOSupport = true;
	else
		iUsePBO = 0;

	if(glewIsSupported("GL_VERSION_2_0"))
		bNonPowerOf2Support = true;
	else
		iAllowNonPowerOf2 = 0;

	if(iUseOpenGL && iUsePBO)
	{
		glGenBuffers(1, pboIds);
	}

	ulOldNumChannels = (unsigned long)m_pHelper->GetVariable(Variable_NumChannels);

	visdata = (float*)_aligned_malloc(512 * ulOldNumChannels * sizeof(float), 16);

	m_LastMove = GetTickCount();

	TCHAR szVisualsPath[2048];
	GetModuleFileName((HMODULE)hInst, szVisualsPath, 512);
	PathRemoveFileSpec(szVisualsPath);
	PathAddBackslash(szVisualsPath);
	StrCat(szVisualsPath, TEXT("vis"));
	PathAddBackslash(szVisualsPath);

	AddFolderOfSVP(szVisualsPath);

	if(m_VisFilenameArray.GetCount())
	{
		lpRegType = REG_DWORD;
		iRegSize = sizeof(int);
		m_pHelper->GetVisualPref(TEXT("SVPRenderer"), TEXT("CurrentVis"), &lpRegType, (LPBYTE)&m_SelectedVisual, &iRegSize);
		if(m_SelectedVisual >= m_VisFilenameArray.GetCount())
			m_SelectedVisual = 0;

		SetActiveVisual(m_SelectedVisual);
	}

	return true;
}

bool	SVPRenderer::Detach()
{
	if(iUseOpenGL)
		ShutdownOpenGL();
	else
		ShutdownGDI();

	m_hDC = NULL;
	if(m_textureData)
	{
		free(m_textureData);
		m_textureData = NULL;
	}

	if(visdata)
	{
		_aligned_free(visdata);
		visdata = NULL;
	}

	return true;
}

void	SVPRenderer::Destroy(void)
{
	m_hDC = NULL;
	if(visdata)
	{
		_aligned_free(visdata);
		visdata = NULL;
	}

	kiss_fft_free(kiss_cfg);
	free(freq_data);
	
	delete this;
}

bool SVPRenderer::RenderVisual(void)
{
	if(m_TheVisual)
	{
		ulNumChannels = (unsigned long)m_pHelper->GetVariable(Variable_NumChannels);
		if(ulNumChannels == INVALID || ulNumChannels == 0)
			return false;

		if(ulNumChannels != ulOldNumChannels)
		{
			if(visdata)
			{
				_aligned_free(visdata);
			}

			ulOldNumChannels = ulNumChannels;
			visdata = (float*)_aligned_malloc(512 * ulNumChannels * sizeof(float), 16);
		}

		vd.MillSec	= (unsigned long)m_pHelper->GetVariable(Variable_PositionMS);

		m_pHelper->GetVisData(visdata, 512 * ulNumChannels);

		if(m_TheVisual->NeedsWaveform())
		{
			int sample = 0;
			for(int x=0; x<512; x++)
			{
				if(ulNumChannels == 1)
				{
					vd.Waveform[0][x] = vd.Waveform[1][x] = (visdata[sample]*64.0f);
				}
				else
				{
					vd.Waveform[0][x] = (visdata[sample]*64.0f);
					vd.Waveform[1][x] = (visdata[sample+1]*64.0f);
				}
				sample+=ulNumChannels;
			}
		}

		if(m_TheVisual->NeedsSpectrum())
		{
			long tempbuffer = 0;
			if(ulNumChannels == 1)
			{
				kiss_fftr(kiss_cfg, visdata, freq_data);
				for(int p = 0; p < 256; p++)
				{
					//tempbuffer = (freq_data[p].r * 4.0f);
					tempbuffer = min(255,freq_data[p].r*4.0f);
					if(tempbuffer < 0)
						tempbuffer = -tempbuffer;
					vd.Spectrum[1][p] = vd.Spectrum[0][p] = tempbuffer;
				}
			}
			else
			{
				int sample = 0;
				for(int x=0; x<512; x++)
				{
					fSamples[0][x] = visdata[sample];
					fSamples[1][x] = visdata[sample+1];
					sample+=ulNumChannels;
				}
				
				kiss_fftr(kiss_cfg, fSamples[0], freq_data);
				for(int p = 0; p < 256; p++)
				{
					//tempbuffer = (freq_data[p].r * 4.0f);
					tempbuffer = min(255,freq_data[p].r*4.0f);
					if(tempbuffer < 0)
						tempbuffer = -tempbuffer;
					vd.Spectrum[0][p] = tempbuffer;
				}

				kiss_fftr(kiss_cfg, fSamples[1], freq_data);
				for(int p = 0; p < 256; p++)
				{
					//tempbuffer = (freq_data[p].r * 4.0f);
					tempbuffer = min(255,freq_data[p].r*4.0f);
					if(tempbuffer < 0)
						tempbuffer = -tempbuffer;
					vd.Spectrum[1][p] = tempbuffer;
				}
			}
		}

		if(m_TheVisual->NeedsVisFX())
			ChangeBrightnessC_MMX(m_textureData, m_textureData, iVisResWidth*iVisResHeight, -24);

		return m_TheVisual->Render(m_textureData, iVisResWidth, iVisResHeight, iVisResWidth, &vd);
	}

	return true;
}

bool	SVPRenderer::Render(int w, int h)
{
	CAutoLock m(&m_RenderLock);

	if((m_LastWidth != w) || (m_LastHeight != h) || bResChange)
	{
		if(w == 0 || h == 0)
			return false;

		if(iAllowNonPowerOf2)
		{
			iVisResHeight = min(h, iVisMaxRes);
			iVisResWidth = min(w, iVisMaxRes);
		}
		else
		{

			if(h >= iVisMaxRes || w >= iVisMaxRes)
				iVisResHeight = iVisResWidth = iVisMaxRes;
			else
			{
				int iVisTempRes = max(h, w);

				if(iVisTempRes >= iVisMaxRes)
					iVisResHeight = iVisResWidth = iVisMaxRes;
				else if(iVisTempRes <= 128)
					iVisResHeight = iVisResWidth = 128;
				else if(iVisTempRes <= 256)
					iVisResHeight = iVisResWidth = 256;
				else if(iVisTempRes <= 512)
					iVisResHeight = iVisResWidth = 512;
				else
					iVisResHeight = iVisResWidth = 1024;
			}
		}

		iTextureSize = iVisResWidth*iVisResHeight*sizeof(unsigned long);

		if(m_textureData)
		{
			free(m_textureData);
			m_textureData = NULL;
		}

		m_textureData = (unsigned long*)malloc(iTextureSize);

		SetRect(&m_NextVisRect, 
				w-(64+16),
				h-(64+16),
				w-(64+16)+64,
				h-(64+16)+64);

		SetRect(&m_PrevVisRect, 
				(16),
				h-(64+16),
				(16)+64,
				h-(64+16)+64);

		if(iUseOpenGL)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, (float)w, (float)h, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glViewport(0,0,w,h);

			//bind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			glTexImage2D( GL_TEXTURE_2D, 
								0,
								GL_RGB8,
								iVisResWidth,
								iVisResHeight,
								0,
								GL_BGRA,
								GL_UNSIGNED_INT_8_8_8_8_REV,
								0);
		}
		else
		{
			if(hOldVisBMP)
				SelectObject(hgdiDC, hOldVisBMP);

			if(hVisBMP)
				DeleteObject(hVisBMP);

			if(hgdiDC)
				DeleteDC(hgdiDC);

			bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
			bi.bmiHeader.biWidth=iVisResWidth;
			bi.bmiHeader.biHeight=-iVisResHeight;
			bi.bmiHeader.biPlanes=1;
			bi.bmiHeader.biBitCount=32;
			hVisBMP = CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void**)&m_textureData, 0, 0);
			hgdiDC=CreateCompatibleDC(0);
			hOldVisBMP = (HBITMAP)SelectObject(hgdiDC, hVisBMP);
		}

		bResChange = false;

		m_LastWidth		= w;
		m_LastHeight	= h;
	}

	RenderVisual();

	if(iUseOpenGL)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		
		//update texture
		if(iUsePBO)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, iTextureSize, 0, GL_STREAM_DRAW);
			if(iUseMappedPBO)
			{
				ioMem = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
				memcpy( ioMem, m_textureData, iTextureSize );
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

		glBegin (GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, h);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(w, h);	// Top Right Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(w, 0.0f);	// Top Left Of The Texture and Quad
		glEnd ();

		//arrows
		m_iElaspedTime = GetTickCount() - m_LastMove;
		if(m_iElaspedTime < 4000)
		{
			glBindTexture(GL_TEXTURE_2D, 1);
			if(m_iElaspedTime < 2000)
			{
				//clear arrow
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				//fade arrow
				int val = m_iElaspedTime - 2000;
				float scale = (2000.0f - (float)val) / 2000.0f;
				glColor4f(1.0f, 1.0f, 1.0f, scale);
			}
			
			//left
			glBegin (GL_QUADS);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(m_NextVisRect.left,	m_NextVisRect.top);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(m_NextVisRect.left,	m_NextVisRect.bottom);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(m_NextVisRect.right,	m_NextVisRect.bottom);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(m_NextVisRect.right,	m_NextVisRect.top);
			glEnd();

			//right
			glBegin (GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(m_PrevVisRect.left,	m_PrevVisRect.top);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(m_PrevVisRect.left,	m_PrevVisRect.bottom);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(m_PrevVisRect.right,	m_PrevVisRect.bottom);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(m_PrevVisRect.right,	m_PrevVisRect.top);
			glEnd();
		}
		
		SwapBuffers(m_hDC);
	}
	else
	{
		StretchDIBits(m_hDC,
			//destination
			0, 0, w, h,
			//source
			0, 0, iVisResWidth, iVisResHeight,
			//image
			hgdiDC, &bi ,DIB_RGB_COLORS, SRCCOPY);

		//BitBlt(m_hDC, 0, 0, iVisResWidth, iVisResHeight, hgdiDC, 0,0, SRCCOPY);
		UpdateWindow(WindowFromDC(m_hDC));
	}

	return true;
}

bool SVPRenderer::SetActiveVisual(int visindex)
{
	CAutoLock m(&m_RenderLock);

	if(m_TheVisual)
	{
		TCHAR	szOldVisFilename[2048];
		char settingsdir[2048];
		StrCpy(szOldVisFilename, m_VisFilenameArray[m_SelectedVisual]);
		StrCat(szOldVisFilename, TEXT(".ini"));
		WideCharToMultiByte(CP_ACP, 0, szOldVisFilename, 512, settingsdir, 512, NULL, NULL);

		SoniqueVisExternal * t = m_TheVisual;
		m_TheVisual = NULL;
		t->SaveSettings(settingsdir);
		t->Shutdown();
		delete t;
	}

	m_SelectedVisual = visindex;

	//freshen up vd. serial experiment lain will crash without
	vd.MillSec = 0;
	for(int i=0; i<2; i++)
	{
		for(int x = 0; x < 512; x++)
		{
			vd.Waveform[i][x] = 0;
		}
	}
	for(int i=0; i<2; i++)
	{
		for(int x = 0; x < 256; x++)
		{
			vd.Spectrum[i][x] = 0;
		}
	}

	TCHAR oldFolder[2048];
	GetCurrentDirectory(2048, oldFolder);

	TCHAR	szVisFilename[2048];
	StrCpy(szVisFilename, m_VisFilenameArray[visindex]);

	TCHAR	szPath[2048];
	StrCpy(szPath, szVisFilename);
	PathRemoveFileSpec(szPath);
	SetCurrentDirectory(szPath);

	SoniqueVisExternal * newVis = new SoniqueVisExternal();
	if(newVis->LoadFromExternalDLL(szVisFilename))
	{
		m_TheVisual = newVis;
		m_TheVisual->SetQueryHelper(m_pHelper);
	}
	else
	{
		delete newVis;
	}

	SetCurrentDirectory(oldFolder);

	m_pHelper->SetVisualPref(TEXT("SVPRenderer"), TEXT("CurrentVis"), REG_DWORD, (LPBYTE)&m_SelectedVisual, sizeof(int));

	return true;
}

bool	SVPRenderer::MouseFunction(unsigned long function, int x, int y)
{
	if(function == VISUAL_MOUSEFUNCTION_DOWN)
	{
		POINT pt;

		pt.x = x;
		pt.y = y;

		if(m_VisFilenameArray.GetCount())
		{
			if(m_VisFilenameArray.GetCount() == 1)
				return true;

			m_LastMove = GetTickCount();

			if(m_TheVisual==NULL)
			{
				SetActiveVisual(0);
				return true;
			}

			if(PtInRect(&m_NextVisRect, pt))
			{
				if(m_SelectedVisual < m_VisFilenameArray.GetCount()-1)
				{
					SetActiveVisual(m_SelectedVisual+1);
				}
				else
				{
					SetActiveVisual(0);
				}
				return true;
			}
			else if(PtInRect(&m_PrevVisRect, pt))
			{
				if(m_SelectedVisual)
				{
					SetActiveVisual(m_SelectedVisual-1);
				}
				else
				{
					SetActiveVisual(m_VisFilenameArray.GetCount()-1);
				}
				return true;
			}

			// we need to scale the point to a 512x512 point then send it to the vis

			int visx = (int)(((float)x / (float)m_LastWidth) * iVisResWidth);
			int visy = (int)(((float)y / (float)m_LastHeight) * iVisResHeight);

			if(m_TheVisual)
				m_TheVisual->Clicked(visx,visy);
		}
	}

	return true;
}

bool	SVPRenderer::Configure(HWND hWndParent)
{
	CAutoLock m(&m_RenderLock);

	if(iUseOpenGL)
		ShutdownOpenGL();
	else
		ShutdownGDI();

	DialogBoxParam(GetModuleHandle(L"SVPVisualPlugin.dll"), MAKEINTRESOURCE(IDD_SVPPREFWINDOW), hWndParent, (DLGPROC)WndProcStub, (DWORD_PTR)this);

	if(iUseOpenGL)
	{
		InitOpenGL();

		GLenum err = glewInit();
		if (GLEW_OK != err)
			return false;

		if(iUsePBO)
		{
			glGenBuffers(1, pboIds);
		}
	}

	bResChange = true;

	return true;
}

bool	SVPRenderer::Notify(unsigned long Notification)
{
	return true;
}

bool	SVPRenderer::About(HWND hWndParent)
{
	MessageBox(hWndParent, TEXT("Sonique SVP renderer. Tony Million & Brett Hoyle 2010"), GetPluginName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool	SVPRenderer::InitOpenGL(void)
{
	m_hArrow = (HBITMAP)LoadImage((HINSTANCE)hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	::GetObject (m_hArrow, sizeof (m_ArrowBM), &m_ArrowBM);

	GLuint		PixelFormat;

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

	if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))
		return false;

	if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))
		return false;

	if (!(m_glRC=wglCreateContext(m_hDC)))
		return false;

	if(!wglMakeCurrent(m_hDC, m_glRC))
		return false;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glEnable (GL_TEXTURE_2D);

	//visual texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		GL_CLAMP);
	//for arrow blending
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//arrow texture
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		GL_CLAMP);
	glTexImage2D(	GL_TEXTURE_2D, 
					0, 
					GL_ALPHA,
					m_ArrowBM.bmWidth, 
					m_ArrowBM.bmHeight, 
					0,
					GL_ALPHA, 
					GL_UNSIGNED_BYTE, 
					m_ArrowBM.bmBits);

	SwapBuffers(m_hDC);

	return true;
}

void	SVPRenderer::ShutdownOpenGL(void)
{
	//flush
	SwapBuffers(m_hDC);

	if(iUsePBO)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, iTextureSize, 0, GL_STREAM_DRAW);
		glDeleteBuffers(1, pboIds);
	}

	//flush
	SwapBuffers(m_hDC);

	if (m_glRC)
	{
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(m_glRC);
		m_glRC=NULL;
	}
}

void	SVPRenderer::ShutdownGDI(void)
{
	if(hOldVisBMP)
		SelectObject(hgdiDC, hOldVisBMP);

	if(hVisBMP)
		DeleteObject(hVisBMP);

	if(hgdiDC)
		DeleteDC(hgdiDC);
}