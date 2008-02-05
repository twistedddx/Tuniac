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

void		CHistory::AddItem(IPlaylistEntry * pEntry)
{
	if(pEntry == NULL)
		return;

	if(m_History.GetCount() > 0 && m_History[0] == pEntry)
		return;

	if(m_History.GetCount() > 1 && m_History[1] == pEntry)
	{
		m_History.RemoveAt(1);
		DeleteMenu(m_hMenu, 1, MF_BYPOSITION);
	}

	TCHAR szDetail[112];
	TCHAR szTime[16];
	TCHAR szItem[128];

	tuniacApp.FormatSongInfo(szDetail, 112, pEntry, tuniacApp.m_Preferences.GetListFormatString(), false);
	tuniacApp.EscapeMenuItemString(szDetail, szItem, 112);

	tuniacApp.FormatSongInfo(szTime, 16, pEntry, TEXT("\t[@I]"), false);
	StrCatN(szItem, szTime, 128);

	InsertMenu(m_hMenu, 0, MF_BYPOSITION, HISTORYMENU_BASE, szItem);

	while(GetMenuItemCount(m_hMenu) > tuniacApp.m_Preferences.GetHistoryListSize())
		DeleteMenu(m_hMenu, tuniacApp.m_Preferences.GetHistoryListSize(), MF_BYPOSITION);

	m_History.AddHead(pEntry);
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

bool		CHistory::RemoveItem(IPlaylistEntry * pEntry)
{
	if(pEntry == NULL)
		return false;

	for(unsigned long i = 0; i < m_History.GetCount(); i++)
	{
		if(m_History[i] == pEntry)
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

bool		CHistory::PlayHistoryItem(unsigned long ulIndex)
{
	if(ulIndex > m_History.GetCount())
		return false;

	IPlaylistEntry * pIPE = m_History[ulIndex];

	if(ulIndex > 0)
	{
		for(unsigned long i = 0; i < ulIndex; i++)
		{
			RemoveItem(m_History[i]);
		}
	}

	bool bOK = false;
	if(pIPE)
	{
		if(tuniacApp.m_CoreAudio.SetSource(pIPE))
		{
			bOK = true;
			//SetActiveByEntry will fail if the played song is filtered out
			//this in turn means the active song for infomational stuff never changed
			//leaving plugins/windowtitle/playcontrol bar all on last song
			tuniacApp.m_PlaylistManager.SetActiveByEntry(pIPE);
			tuniacApp.m_CoreAudio.Play();
			tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SONGCHANGE_MANUAL, NULL, NULL);
		}
		tuniacApp.m_SourceSelectorWindow->UpdateView();

	}
	return bOK;
}

IPlaylistEntry * CHistory::GetHistoryItem(unsigned long ulIndex)
{
	if(ulIndex > m_History.GetCount())
		return NULL;

	return m_History[ulIndex];
}

unsigned long	CHistory::GetCount(void)
{
	return m_History.GetCount();
}
