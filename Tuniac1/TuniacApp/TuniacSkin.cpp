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
		TEXT("window.ico"),
		IDI_TUNIAC,
		32,
		NULL
	},
	{
		TEXT("window.ico"),
		IDI_TUNIAC,
		16,
		NULL
	},
	{
		TEXT("item-normal.ico"),
		IDI_THEMEICON_ITEM_NORMAL,
		16,
		NULL
	},
	{
		TEXT("item-active.ico"),
		IDI_THEMEICON_ITEM_ACTIVE,
		16,
		NULL
	},
	{
		TEXT("playlist-medialibrary.ico"),
		IDI_THEMEICON_PLAYLIST_MEDIALIBRARY,
		32,
		NULL
	},
	{
		TEXT("playlist-cd.ico"),
		IDI_THEMEICON_PLAYLIST_CD,
		32,
		NULL
	},
	{
		TEXT("playlist-standard.ico"),
		IDI_THEMEICON_PLAYLIST_STANDARD,
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
}

CTuniacSkin::~CTuniacSkin(void)
{
}

bool			CTuniacSkin::Initialize(void)
{
	for(int i = 0; i < THEMEICON_MAXICON; i++)
	{
		ThemeIconSet[i].hHandle = (HICON)LoadImage(tuniacApp.getMainInstance(), MAKEINTRESOURCE(ThemeIconSet[i].ulRes), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	}

	//LoadTheme(tuniacApp.m_Preferences.GetTheme());

	return true;
}

bool			CTuniacSkin::Shutdown(void)
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
	StrCatN(m_BasePath, TEXT("themes"), 512);
	PathAddBackslash(m_BasePath);
	StrCatN(m_BasePath, szThemeName, 512);
	PathAddBackslash(m_BasePath);

	TCHAR szFilename[512];
	for(int i = 0; i < THEMEICON_MAXICON; i++)
	{
		wnsprintf(szFilename, 512, TEXT("%s%s"), m_BasePath, ThemeIconSet[i].szFilename);
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
HICON			CTuniacSkin::GetIcon(unsigned long ulIcon)
{
	if(ulIcon >= THEMEICON_MAXICON)
		return NULL;
	return ThemeIconSet[ulIcon].hHandle;
}