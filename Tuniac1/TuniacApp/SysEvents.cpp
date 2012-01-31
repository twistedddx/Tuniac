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

#include "stdafx.h"
#include "resource.h"
#include "SysEvents.h"

#include "CoreAudio.h"

CSysEvents::CSysEvents(void)
{
}

CSysEvents::~CSysEvents(void)
{
}

bool			CSysEvents::Initialize(void)
{

	//OSVERSIONINFO ovi = { 0 };
	//ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	//GetVersionEx(&ovi);
	//m_IsNT = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT;
	m_WasPlaying = false;

	m_WorkstationLocked = false;
	m_ScreensaverActive = false;


	return true;
}

bool			CSysEvents::Shutdown(void)
{
	KillTimer(tuniacApp.getMainWindow(), SYSEVENTS_TIMERID);
	return true;
}

void			CSysEvents::CheckSystemState()
{
	//bool bLocked = false;
	//if(m_IsNT){
	bool bLocked = IsWorkstationLocked();
	//}
	bool bSaver = IsScreensaverActive();

	if((tuniacApp.m_Preferences.GetPauseOnLock() && bLocked && !m_WorkstationLocked)
		|| (tuniacApp.m_Preferences.GetPauseOnScreensave() && bSaver && !m_ScreensaverActive))
	{
		if(CCoreAudio::Instance()->GetState() == STATE_PLAYING)
		{
			SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_PAUSE, 0), 0);
			m_WasPlaying = true;
		}
	}
	else if ((tuniacApp.m_Preferences.GetPauseOnLock() || tuniacApp.m_Preferences.GetPauseOnScreensave())
			&& (bLocked != m_WorkstationLocked || bSaver != m_ScreensaverActive)
			&& (m_WorkstationLocked || m_ScreensaverActive))
	{
		if(m_WasPlaying)
		{
			SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_PLAY, 0), 0);
			m_WasPlaying = false;
		}
	}
	m_WorkstationLocked = bLocked;
	m_ScreensaverActive = bSaver;
}

bool			CSysEvents::IsWorkstationLocked(void)
{
	TCHAR szBuf[256];
	bool bLocked = false;
	HDESK hd = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);

	if (hd == NULL)
	{
		bLocked = true;
	}
	else
	{
		GetUserObjectInformation(hd, UOI_NAME, szBuf, sizeof(szBuf), NULL);
		CloseDesktop(hd);
		if (StrCmp(szBuf, TEXT("Winlogon")) == 0)
		{
			bLocked = true;
		}
	}
	return bLocked;
}

bool			CSysEvents::IsScreensaverActive(void)
{
	BOOL bActive;
	SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bActive, 0);
	if(bActive == TRUE)
		return true;
	
	return false;
}