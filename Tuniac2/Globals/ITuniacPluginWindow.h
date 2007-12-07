#pragma once

#include "ITuniacPlugin.h"

// {6C208856-B547-48f2-82F5-8AC6EA59ACD1}
static const GUID GUID_TUNIACPLUGINWINDOW = { 0x6c208856, 0xb547, 0x48f2, { 0x82, 0xf5, 0x8a, 0xc6, 0xea, 0x59, 0xac, 0xd1 } };

class ITuniacPluginWindow : public ITuniacPlugin
{
public:
	virtual bool			CreatePluginWindow(HWND hParent, HINSTANCE hInst) = 0;
	virtual bool			DestroyPluginWindow(void) = 0;

	virtual bool			Show(void) = 0;
	virtual bool			Hide(void) = 0;

	virtual bool			SetPos(int x, int y, int w, int h) = 0;
};