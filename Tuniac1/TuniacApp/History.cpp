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
#include "History.h"

CHistory::CHistory(void)
{
}

CHistory::~CHistory(void)
{
}

bool		CHistory::Initialize()
{
	m_hMenu = CreatePopupMenu();
	return true;
}

bool		CHistory::Shutdown()
{
	Clear();
	return true;
}

void		CHistory::AddEntryID(unsigned long ulEntryID)
{
	if(m_History.GetCount() > 0 && m_History[0] == ulEntryID)
		return;

	if(m_History.GetCount() > 1 && m_History[1] == ulEntryID)
	{
		m_History.RemoveAt(1);
		DeleteMenu(m_hMenu, 1, MF_BYPOSITION);
	}

	IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.GetEntryByEntryID(ulEntryID);

	TCHAR szDetail[112];
	TCHAR szTime[16];
	TCHAR szItem[128];

	tuniacApp.FormatSongInfo(szDetail, 112, pIPE, tuniacApp.m_Preferences.GetListFormatString(), false);
	tuniacApp.EscapeMenuItemString(szDetail, szItem, 112);

	tuniacApp.FormatSongInfo(szTime, 16, pIPE, TEXT("\t[@I]"), false);
	StrCatN(szItem, szTime, 128);

	InsertMenu(m_hMenu, 0, MF_BYPOSITION, HISTORYMENU_BASE, szItem);

	while(GetMenuItemCount(m_hMenu) > tuniacApp.m_Preferences.GetHistoryListSize())
		DeleteMenu(m_hMenu, tuniacApp.m_Preferences.GetHistoryListSize(), MF_BYPOSITION);

	m_History.AddHead(ulEntryID);
	while(m_History.GetCount() > tuniacApp.m_Preferences.GetHistoryListSize())
		m_History.RemoveTail();

	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID;

	for(int i = 1; i < GetMenuItemCount(m_hMenu); i++)
	{
		GetMenuItemInfo(m_hMenu, i, TRUE, &mii);
		mii.wID = HISTORYMENU_BASE + i;
		SetMenuItemInfo(m_hMenu, i, TRUE, &mii);
	}
}

bool		CHistory::RemoveEntryID(unsigned long ulEntryID)
{
	for(unsigned long i = 0; i < m_History.GetCount(); i++)
	{
		if(m_History[i] == ulEntryID)
		{
			m_History.RemoveAt(i);
			DeleteMenu(m_hMenu, i, MF_BYPOSITION);
			return true;
		}
	}
	return false;
}

void		CHistory::Clear(void)
{
	while(GetMenuItemCount(m_hMenu) > 0)
		DeleteMenu(m_hMenu, 0, MF_BYPOSITION);
	m_History.RemoveAll();
}

void		CHistory::PopupMenu(int x, int y)
{
	if(GetMenuItemCount(m_hMenu) < 1)
		return;

	TrackPopupMenu(m_hMenu, TPM_RIGHTBUTTON, x, y, 0, tuniacApp.getMainWindow(), NULL);
}

bool		CHistory::PlayHistoryIndex(unsigned long ulIndex)
{
	if(ulIndex > m_History.GetCount())
		return false;

	//store before delete
	unsigned long ulEntryID = m_History[ulIndex];

	if(ulIndex > 0)
	{
		for(unsigned long i = 0; i < ulIndex; i++)
		{
			RemoveEntryID(m_History[i]);
		}
	}

	return tuniacApp.PlayEntryID(ulEntryID, true, true, true);
}

unsigned long CHistory::GetHistoryEntryID(unsigned long ulIndex)
{
	if(ulIndex > m_History.GetCount())
		return INVALID_PLAYLIST_INDEX;

	return m_History[ulIndex];
}

unsigned long	CHistory::GetCount(void)
{
	return m_History.GetCount();
}
