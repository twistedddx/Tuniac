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

#include "StdAfx.h"
#include "TuniacSkin.h"
#include "resource.h"

typedef struct
{
	LPTSTR			szFilename;
	unsigned long	ulRes;
	unsigned long	ulSize;
	HICON			hHandle;
} ThemeIcon;

static ThemeIcon ThemeIconSet[THEMEICON_MAXICON] =
{
	{
		TEXT("window.ico"), //0
		IDI_TUNIAC,
		32,
		NULL
	},
	{
		TEXT("window.ico"), //1
		IDI_TUNIAC,
		16,
		NULL
	},
	{
		TEXT("item-normal.ico"), //2
		IDI_THEMEICON_ITEM_NORMAL,
		16,
		NULL
	},
	{
		TEXT("item-active.ico"), //3
		IDI_THEMEICON_ITEM_ACTIVE,
		16,
		NULL
	},
	{
		TEXT("item-unavailable.ico"), //4
		IDI_THEMEICON_ITEM_UNAVAILABLE,
		16,
		NULL
	},
	{
		TEXT("playlist-medialibrary.ico"), //5
		IDI_THEMEICON_PLAYLIST_MEDIALIBRARY,
		32,
		NULL
	},
		{
		TEXT("playlist-radio.ico"), //6
		IDI_THEMEICON_PLAYLIST_MIC,
		32,
		NULL
	},
	{
		TEXT("playlist-cd.ico"), //7
		IDI_THEMEICON_PLAYLIST_CD,
		32,
		NULL
	},
	{
		TEXT("playlist-standard.ico"), //8
		IDI_THEMEICON_PLAYLIST_STANDARD,
		32,
		NULL
	},
	{
		TEXT("playlist-medialibrary-active.ico"), //9
		IDI_THEMEICON_PLAYLIST_MEDIALIBRARY_ACTIVE,
		32,
		NULL
	},
	{
		TEXT("playlist-radio-active.ico"), //10
		IDI_THEMEICON_PLAYLIST_MIC_ACTIVE,
		32,
		NULL
	},
	{
		TEXT("playlist-cd-active.ico"), //11
		IDI_THEMEICON_PLAYLIST_CD_ACTIVE,
		32,
		NULL
	},
	{
		TEXT("playlist-standard-active.ico"), //12
		IDI_THEMEICON_PLAYLIST_STANDARD_ACTIVE,
		32,
		NULL
	}


/*
	{
		TEXT("playlist-radio.ico"),
		IDI_THEMEICON_PLAYLIST_RADIO,
		32,
		NULL
	}
*/
};



CTuniacSkin::CTuniacSkin(void)
{
	m_dwMutedAccentColor = RGB(236, 243, 254);
}

CTuniacSkin::~CTuniacSkin(void)
{
}

bool	CTuniacSkin::Initialize(void)
{
	for(int i = 0; i < THEMEICON_MAXICON; i++)
	{
		ThemeIconSet[i].hHandle = (HICON)LoadImage(tuniacApp.getMainInstance(), MAKEINTRESOURCE(ThemeIconSet[i].ulRes), IMAGE_ICON, ThemeIconSet[i].ulSize, ThemeIconSet[i].ulSize, LR_DEFAULTCOLOR);
	}

	//LoadTheme(tuniacApp.m_Preferences.GetTheme());

	return true;
}

bool	CTuniacSkin::Shutdown(void)
{
	return true;
}
/*
bool			CTuniacSkin::LoadTheme(LPTSTR szThemeName)
{
	if(szThemeName == NULL)
		return false;

	GetModuleFileName(NULL, m_BasePath, 512);
	PathRemoveFileSpec(m_BasePath);
	PathAddBackslash(m_BasePath);
	StringCchCat(m_BasePath, TEXT("themes"));
	PathAddBackslash(m_BasePath);
	StringCchCat(m_BasePath, szThemeName);
	PathAddBackslash(m_BasePath);

	TCHAR szFilename[512];
	for(int i = 0; i < THEMEICON_MAXICON; i++)
	{
		StringCchPrintf(szFilename, 512, TEXT("%s%s"), m_BasePath, ThemeIconSet[i].szFilename);
		ThemeIconSet[i].hHandle = (HICON)LoadImage(	tuniacApp.getMainInstance(), 
													szFilename, 
													IMAGE_ICON, 
													0, 
													0, 
													LR_LOADFROMFILE | LR_DEFAULTCOLOR);

		if(ThemeIconSet[i].hHandle == NULL)
			ThemeIconSet[i].hHandle = (HICON)LoadImage(tuniacApp.getMainInstance(), MAKEINTRESOURCE(ThemeIconSet[i].ulRes), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	}
	return true;
}
*/
HICON	CTuniacSkin::GetIcon(unsigned long ulIcon)
{
	if(ulIcon >= THEMEICON_MAXICON)
		return NULL;
	return ThemeIconSet[ulIcon].hHandle;
}

DWORD	CTuniacSkin::GetAccentColor(void)
{
	return GetSysColor(COLOR_GRADIENTACTIVECAPTION);
}

void	CTuniacSkin::SetMutedAccentColor(DWORD dwRGB)
{
	int iMutedR = 255 - ((255 - GetRValue(dwRGB)) * 0.3);
	int iMutedG = 255 - ((255 - GetGValue(dwRGB)) * 0.3);
	int iMutedB = 255 - ((255 - GetBValue(dwRGB)) * 0.3);

	m_dwMutedAccentColor = RGB(iMutedR, iMutedG, iMutedB);
}


DWORD	CTuniacSkin::GetMutedAccentColor(void)
{
	return m_dwMutedAccentColor;
}