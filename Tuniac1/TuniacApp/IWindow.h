#pragma once

class IWindow
{
public:
	virtual void			Destroy(void) = 0;

	virtual LPTSTR			GetName(void) = 0;
	virtual GUID			GetPluginID(void) = 0;		// use guidgen to make a custom guid to return

	virtual unsigned long	GetFlags(void) = 0;

	virtual bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst) = 0;
	virtual bool			DestroyPluginWindow(void) = 0;

	virtual bool			Show(void) = 0;
	virtual bool			Hide(void) = 0;

	virtual bool			SetPos(int x, int y, int w, int h) = 0;
};
