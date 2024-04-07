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
	Copyright (C) 2003-2014 Brett Hoyle
*/

#include "stdafx.h"
#include "micplaylist.h"
#include "resource.h"


CMicPlaylist::CMicPlaylist(char cMicInput) :
m_IsEnabled(0),
m_ActiveItem(0)
{
	StringCchCopy(m_PlaylistName, 256, TEXT("Mic Input"));
}

CMicPlaylist::~CMicPlaylist(void)
{
	while(m_TrackList.GetCount())
	{
		CMediaLibraryPlaylistEntry * tt = (CMediaLibraryPlaylistEntry *)m_TrackList[0];

		m_TrackList.RemoveAt(0);

		delete tt;
	}
}

bool CMicPlaylist::IsEnabled(void)
{
	return m_IsEnabled;
}

void CMicPlaylist::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		GetMicInfo();
	}
	else
	{

		if(GetPlaylistID() == tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylist()->GetPlaylistID())
			tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(0);

		if (GetPlaylistID() == tuniacApp.m_PlaylistManager.GetActivePlaylistID())
		{
			tuniacApp.m_PlaylistManager.SetActivePlaylistByIndex(0);
			tuniacApp.m_PlaylistManager.m_LibraryPlaylist.RebuildPlaylist();
			tuniacApp.m_PlaylistManager.m_LibraryPlaylist.ApplyFilter();
			SendMessage(tuniacApp.getMainWindow(), WM_COMMAND, MAKELONG(ID_PLAYBACK_NEXT, 0), 0);
		}
		while (m_TrackList.GetCount())
		{
			CMediaLibraryPlaylistEntry* tt = (CMediaLibraryPlaylistEntry*)m_TrackList[0];

			m_TrackList.RemoveAt(0);

			delete tt;
		}

	}
	m_IsEnabled = bEnabled;

	tuniacApp.m_SourceSelectorWindow->UpdateList();
	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
}

bool CMicPlaylist::GetMicInfo(void)
{
	IInfoManager* pPlugin = tuniacApp.m_MediaLibrary.GetInfoManagerForFilename(L"MICINPUT:GetDevices");
	if (pPlugin)
	{
		unsigned long ulMic = 0;
		while(true)
		{
			LibraryEntry  libraryEntry;
			ZeroMemory(&libraryEntry, sizeof(LibraryEntry));

			StringCchPrintf(libraryEntry.szURL, 260, TEXT("MICINPUT:%d"), ulMic);


			if (pPlugin->CanHandle(libraryEntry.szURL) == false)
				break;

			pPlugin->GetInfo(&libraryEntry);

			IPlaylistEntry* pIPE = new CMediaLibraryPlaylistEntry(&libraryEntry);
			m_TrackList.AddTail(pIPE);
			ulMic++;
		}
		if (m_TrackList.GetCount())
			return true;
	}
	return false;
}

unsigned long		CMicPlaylist::GetNumItems(void)
{
	return m_TrackList.GetCount();
}

unsigned long		CMicPlaylist::GetFlags(void)
{
	return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_DONTCROSSFADE;
}

unsigned long		CMicPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_MIC;
}

void				CMicPlaylist::SetPlaylistID(unsigned long ulPlaylistID)
{
	m_ulPlaylistID = ulPlaylistID;
}

unsigned long		CMicPlaylist::GetPlaylistID(void)
{
	return m_ulPlaylistID;
}

bool				CMicPlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	if(szPlaylistName)
	{
		StringCchCopy(m_PlaylistName, 256, szPlaylistName);
		return true;
	}

	return false;
}

LPTSTR				CMicPlaylist::GetPlaylistName(void)
{
	return m_PlaylistName;
}

unsigned long		CMicPlaylist::Previous(void)
{
	if(m_ActiveItem > 0)
		return (m_ActiveItem - 1);

	return INVALID_PLAYLIST_INDEX;
}

unsigned long		CMicPlaylist::Next(void)
{
	if(m_ActiveItem < (GetNumItems()-1))
		return m_ActiveItem + 1;

	return INVALID_PLAYLIST_INDEX;
}

bool		CMicPlaylist::CheckFilteredIndex(unsigned long ulIndex)
{
	//invalid
	if (ulIndex == INVALID_PLAYLIST_INDEX)
			return false;

	//too low
	if (ulIndex < 0)
		return false;

	//too high, m_NormalIndexArray/m_RandomIndexArray should be the same length
	if (ulIndex >= m_TrackList.GetCount())
		return false;

	return true;
}

unsigned long		CMicPlaylist::GetNextFilteredIndexForFilteredIndex(unsigned long ulNormalFilteredIndex)
{
	if(ulNormalFilteredIndex < (GetNumItems()-1))
	{
		return ulNormalFilteredIndex + 1;
	}

	return INVALID_PLAYLIST_INDEX;
}

IPlaylistEntry	*	CMicPlaylist::GetActiveEntry(void)
{
	return m_TrackList[m_ActiveItem];
}

unsigned long		CMicPlaylist::GetActiveFilteredIndex(void)
{
	return m_ActiveItem;
}

bool				CMicPlaylist::SetActiveFilteredIndex(unsigned long ulIndex)
{
	m_ActiveItem = ulIndex;
	return true;
}

unsigned long		CMicPlaylist::GetActiveNormalFilteredIndex()
{
	return GetActiveFilteredIndex();
}

bool				CMicPlaylist::SetActiveNormalFilteredIndex(unsigned long ulIndex)
{
	SetActiveFilteredIndex(ulIndex);
	return true;
}

IPlaylistEntry *	CMicPlaylist::GetEntryAtFilteredIndex(unsigned long ulFilteredIndex)
{
	if(ulFilteredIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_TrackList[ulFilteredIndex];
	}

	return NULL;
}

IPlaylistEntry *	CMicPlaylist::GetEntryAtNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	return GetEntryAtFilteredIndex(ulFilteredIndex);
}

unsigned long		CMicPlaylist::GetFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{

	for(unsigned long x=0; x < m_TrackList.GetCount(); x++)
	{
		if(m_TrackList[x] == pIPE)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

unsigned long		CMicPlaylist::GetNormalFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{
	return GetFilteredIndexforEntry(pIPE);
}