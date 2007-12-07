#pragma once

#include "ITuniacPlugin.h"

#define MAXVISSAMPLES		4096

#define VISUAL_MOUSEFUNCTION_MOVE		0
#define VISUAL_MOUSEFUNCTION_DOWN		1
#define VISUAL_MOUSEFUNCTION_UP			2

// {6B473A84-73C9-485c-BC55-2B04BB8900E4}
static const GUID GUID_TUNIACPLUGINVISUAL = { 0x6b473a84, 0x73c9, 0x485c, { 0xbc, 0x55, 0x2b, 0x4, 0xbb, 0x89, 0x0, 0xe4 } };


class ITuniacPluginVisual : public ITuniacPlugin
{
public:
	virtual bool	Attach(HDC hDC)										= 0;
	virtual bool	Detach()											= 0;

	virtual bool	Render(int w, int h)								= 0;

	virtual bool	MouseFunction(unsigned long function, int x, int y)	= 0;
};
