#pragma once

#pragma pack(16)

#include "ituniacvisplugin.h"

#include "kiss_fft.h"
#include "GL\glew.h"
#include "glfont2.h"
using namespace glfont;

#include <gdiplus.h>
using namespace Gdiplus;

#include "SoniqueVis.h"
#include "Array.h"
#include "CriticalSection.h"


class SVPRenderer :
	public ITuniacVisPlugin
{
protected:
	CCriticalSection					m_RenderLock;

	HWND								m_hSVPPrefDlg;
	HDC									m_hDC;

	HGLRC								m_glRC;
	unsigned int						pboIds[1];

	GLvoid							*	ioMem;

	GLFont								myfont;
	char							*	visName;

	int									m_LastWidth;
	int									m_LastHeight;
	int									iVisResHeight;
	int									iVisResWidth;
	bool								bResChange;
	unsigned long					*	m_textureData;
	int									iTextureSize;

	VisData								vd;
	float							*	visdata;

	unsigned long						ulNumChannels;
	unsigned long						ulOldNumChannels;
	unsigned long						ulSamples;

	Array<LPTSTR, 4>					m_VisFilenameArray;
	SoniqueVisExternal				*	m_TheVisual;
	int									m_SelectedVisual;

	RECT								m_NextVisRect;
	RECT								m_PrevVisRect;

	unsigned long						m_LastMove;
	int									m_iElaspedTime;

    BITMAP								m_ArrowBM;
	HBITMAP								m_hArrow;

	kiss_fft_cfg						kiss_cfg;
	kiss_fft_cpx					*	in_freq_data;
	kiss_fft_cpx					*	out_freq_data;

	int									iUseOpenGL;
	int									iVisMaxRes;
	int									iUsePBO;
	int									iUseMappedPBO;
	int									iAllowNonPowerOf2;

	bool								bPBOSupport;
	bool								bNonPowerOf2Support;

	ITuniacVisHelper				*	m_pHelper;

	static LRESULT CALLBACK WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	bool	Detach(void);

	bool	Render(int w, int h);

	bool	About(HWND hWndParent);
	bool	Configure(HWND hWndParent);

	bool	Notify(unsigned long Notification);
	bool	MouseFunction(unsigned long function, int x, int y);

	bool	InitOpenGL(void);
	void	ShutdownOpenGL(void);

};
