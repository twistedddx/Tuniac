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

#pragma once

#define THEMEICON_WINDOW					0
#define THEMEICON_WINDOW_SMALL				1
#define THEMEICON_ITEM_NORMAL				2
#define THEMEICON_ITEM_ACTIVE				3
#define THEMEICON_ITEM_UNAVAILABLE			4
#define THEMEICON_PLAYLIST_MEDIALIBRARY		5
#define THEMEICON_PLAYLIST_CD				6
#define THEMEICON_PLAYLIST_STANDARD			7
//#define THEMEICON_PLAYLIST_RADIO			11

#define THEMEICON_MAXICON					8


class CTuniacSkin
{
protected:
	TCHAR				m_BasePath[512];

	//bool				LoadTheme(LPTSTR szThemeName);

public:
	CTuniacSkin(void);
	~CTuniacSkin(void);

	bool				Initialize(void);
	bool				Shutdown(void);

	HICON				GetIcon(unsigned long ulIcon);

};
