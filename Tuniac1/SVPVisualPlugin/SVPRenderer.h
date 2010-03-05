#pragma once

#pragma pack(16)

#include "ituniacvisplugin.h"

#include <gl/gl.h>
#include <gl/glu.h>

#include "SoniqueVis.h"
#include "Array.h"
#include "CriticalSection.h"
#include "kiss_fftr.h"

class SVPRenderer :
	public ITuniacVisPlugin
{
protected:

	CCriticalSection					m_RenderLock;

	bool								bOpenGL;

	HDC									m_hDC;

	HGLRC								m_glRC;

	HDC									m_gdiDC;
	HBITMAP								visBMP;
	HBITMAP								hBitmap;

	int									m_LastWidth;
	int									m_LastHeight;
	int									iVisResHeight;
	int									iVisResWidth;
	int									iVisMaxRes;
	int									iAllowNonPowerOf2;
	bool								bResChange;
	unsigned long					*	m_textureData;

	VisData								vd;
	float							*	visdata;
	float								fSamples[2][512];

	unsigned long						ulNumChannels;
	unsigned long						ulOldNumChannels;
	unsigned long						ulSamples;

	ITuniacVisHelper				*	m_pHelper;

	Array<LPTSTR, 4>					m_VisFilenameArray;
	SoniqueVisExternal				*	m_TheVisual;
	int									m_SelectedVisual;

	RECT								m_NextVisRect;
	RECT								m_PrevVisRect;

	unsigned long						m_LastMove;

	int									m_iElaspedTime;

    BITMAP								m_ArrowBM;
	HBITMAP								m_hArrow;

	kiss_fftr_cfg						kiss_cfg;
	kiss_fft_cpx					*	freq_data;

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
