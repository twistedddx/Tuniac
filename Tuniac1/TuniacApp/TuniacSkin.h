#pragma once

#define THEMEICON_WINDOW					0
#define THEMEICON_WINDOW_SMALL				1
#define THEMEICON_CONTROL_PLAY				2
#define THEMEICON_CONTROL_PAUSE				3
#define THEMEICON_CONTROL_PREV				4
#define THEMEICON_CONTROL_NEXT				5
#define THEMEICON_ITEM_NORMAL				6
#define THEMEICON_ITEM_ACTIVE				7
#define THEMEICON_PLAYLIST_MEDIALIBRARY		8
#define THEMEICON_PLAYLIST_CD				9
#define THEMEICON_PLAYLIST_STANDARD			10
//#define THEMEICON_PLAYLIST_RADIO			11

#define THEMEICON_MAXICON					11


class CTuniacSkin
{
protected:
	TCHAR				m_BasePath[512];

	bool				LoadTheme(LPTSTR szThemeName);

public:
	CTuniacSkin(void);
	~CTuniacSkin(void);

	bool				Initialize(void);
	bool				Shutdown(void);

	HICON				GetIcon(unsigned long ulIcon);

};
