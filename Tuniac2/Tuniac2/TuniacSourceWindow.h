#pragma once
#include "ituniacpluginwindow.h"

class CTuniacSourceWindow :
	public ITuniacPluginWindow
{
protected:
	HWND			m_hWnd;

public:
	CTuniacSourceWindow(void);
	~CTuniacSourceWindow(void);

public:
	void			Destroy(void);

	bool			GetName(String & nameString);
	GUID			GetPluginID(void);		// use guidgen to make a custom guid to return

	unsigned long	GetFlags(void);

	void			SetHelper(ITuniacPluginHelper * pHelper);

public:
	bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst);
	bool			DestroyPluginWindow(void);

	bool			Show(void);
	bool			Hide(void);

	bool			SetPos(int x, int y, int w, int h);
};
