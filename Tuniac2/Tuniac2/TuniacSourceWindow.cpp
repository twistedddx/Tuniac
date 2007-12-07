#include "stdafx.h"
#include ".\tuniacsourcewindow.h"

CTuniacSourceWindow::CTuniacSourceWindow(void)
{
}

CTuniacSourceWindow::~CTuniacSourceWindow(void)
{
}

void			CTuniacSourceWindow::Destroy(void)
{
	delete this;
}

bool			CTuniacSourceWindow::GetName(String & nameString)
{
	nameString = TEXT("Source Selector");
	return true;
}

GUID			CTuniacSourceWindow::GetPluginID(void)
{
	// {3C1300A6-EC61-484d-9692-AC807A9F2A74}
	static const GUID GUID_SOURCESELECTOR = { 0x3c1300a6, 0xec61, 0x484d, { 0x96, 0x92, 0xac, 0x80, 0x7a, 0x9f, 0x2a, 0x74 } };

	return GUID_SOURCESELECTOR;
}		// use guidgen to make a custom guid to return

unsigned long	CTuniacSourceWindow::GetFlags(void)
{
	return 0;
}

void			CTuniacSourceWindow::SetHelper(ITuniacPluginHelper * pHelper)
{
}

bool			CTuniacSourceWindow::CreatePluginWindow(HWND hParent, HINSTANCE hInst)
{
	return false;
}

bool			CTuniacSourceWindow::DestroyPluginWindow(void)
{
	return false;
}

bool			CTuniacSourceWindow::Show(void)
{
	return false;
}

bool	CTuniacSourceWindow::Hide(void)
{
	return false;
}

bool			CTuniacSourceWindow::SetPos(int x, int y, int w, int h)
{
	return false;
}