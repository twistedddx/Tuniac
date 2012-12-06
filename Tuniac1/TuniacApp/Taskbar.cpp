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
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma once

#include "stdafx.h"
#include "resource.h"
#include "Taskbar.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Very Important:
//
//		CTaskBar should be rewritten to process a minimum of things, and not really have to handle the OnMinimize and OnMaximize stuff.
//
//		The start of this is now: I've started redoing it.
//
//			Basically it should handle Show/Hide tray icon, set the HMenu to popup (which should call to TuniacApp window not into the systray
//			Also being able to set the icon
//
//			ALL THE PROCESSING SHOULD HAPPEN IN THE TUNIACAPP MAIN WINDOW AND THE SYSTRAY STUFF SHOULD JUST BE THE BARE MINIMUM TO HANDLE THE SYSTRAY STUFF

CTaskbar::CTaskbar(void)
{
}

CTaskbar::~CTaskbar(void)
{
}

bool			CTaskbar::Initialize(HWND hTargetWindow, unsigned long AppMessage)
{
	if(tuniacApp.m_dwWinVer < 6) //XP and under
		m_TrayIconData.cbSize			= NOTIFYICONDATA_V3_SIZE;
	else //vista and over
		m_TrayIconData.cbSize			= sizeof(NOTIFYICONDATA);
	m_TrayIconData.hWnd				= hTargetWindow;
	m_TrayIconData.uID				= 1;
	m_TrayIconData.uCallbackMessage = AppMessage;
	m_TrayIconData.hIcon			= tuniacApp.m_Skin.GetIcon(THEMEICON_WINDOW_SMALL);
	m_TrayIconData.uVersion			= NOTIFYICON_VERSION;
	m_TrayIconData.uFlags			= NIF_ICON | NIF_MESSAGE | NIF_TIP;

	Shell_NotifyIcon(NIM_SETVERSION, &m_TrayIconData);
	return true;
}

bool			CTaskbar::Shutdown(void)
{
	Hide();
	return true;
}

void			CTaskbar::SetTitle(LPCWSTR szTitle)
{
	if(wcscmp(szTitle, m_TrayIconData.szInfo))
	{
		//fixed buffer overflow
		StringCchCopyNW(m_TrayIconData.szTip, sizeof(m_TrayIconData.szTip), szTitle, sizeof(m_TrayIconData.szTip));
		Shell_NotifyIcon(NIM_MODIFY, &m_TrayIconData);
	}
}

void			CTaskbar::SetIcon(HICON		hIcon)
{
	m_TrayIconData.hIcon			= hIcon;
	Shell_NotifyIcon(NIM_MODIFY, &m_TrayIconData);
}

void			CTaskbar::Show()
{
	Shell_NotifyIcon(NIM_SETVERSION,	&m_TrayIconData);
	Shell_NotifyIcon(NIM_ADD,			&m_TrayIconData);
	Shell_NotifyIcon(NIM_SETVERSION,	&m_TrayIconData);
}

void			CTaskbar::Hide()
{
	Shell_NotifyIcon(NIM_DELETE, &m_TrayIconData);
}
