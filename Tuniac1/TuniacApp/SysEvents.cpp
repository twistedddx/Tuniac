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

	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ovi);
	m_IsNT = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT;
	m_WasPlaying = false;

	m_WorkstationLocked = false;
	m_ScreensaverActive = false;

	SetTimer(tuniacApp.getMainWindow(), SYSEVENTS_TIMERID, 5000, NULL);
	return true;
}

bool			CSysEvents::Shutdown(void)
{
	KillTimer(tuniacApp.getMainWindow(), SYSEVENTS_TIMERID);
	return true;
}

void			CSysEvents::CheckSystemState()
{
	bool bLocked = false;
	if(m_IsNT){
		bLocked = IsWorkstationLocked();
	}

	bool bSaver = IsScreensaverActive();

	if((tuniacApp.m_Preferences.GetPauseOnLock() && bLocked && !m_WorkstationLocked)
		|| (tuniacApp.m_Preferences.GetPauseOnScreensave() && bSaver && !m_ScreensaverActive))
	{
		if(tuniacApp.m_CoreAudio.GetState() == STATE_PLAYING)
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