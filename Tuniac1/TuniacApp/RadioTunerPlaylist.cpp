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
#include ".\radiotunerplaylist.h"

CRadioTunerPlaylist::CRadioTunerPlaylist(void)
{
}

CRadioTunerPlaylist::~CRadioTunerPlaylist(void)
{
}

void	CRadioTunerPlaylist::ResetURLS(void)
{
	m_URLList.RemoveAll();

	StrCpyN(m_CurrentSong,		TEXT("Track Info Unavailable"), 256);
}

void	CRadioTunerPlaylist::AddURL(LPTSTR szURL)
{
	m_URLList.AddTail(szURL);
}

unsigned long		CRadioTunerPlaylist::GetFlags(void)
{
	return 0;
}

unsigned long		CRadioTunerPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_RADIO;
}

bool				CRadioTunerPlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	return false;
}

LPTSTR				CRadioTunerPlaylist::GetPlaylistName(void)
{
	return TEXT("Radio Tuner");
}

bool				CRadioTunerPlaylist::Previous(void)
{
	return true;
}

bool				CRadioTunerPlaylist::Next(void)
{
	return true;
}

bool				CRadioTunerPlaylist::CheckNext(void)
{
	return false;
}

IPlaylistEntry	*	CRadioTunerPlaylist::GetActiveItem(void)
{
	return static_cast<IPlaylistEntry *>(this);
}

unsigned long	CRadioTunerPlaylist::GetEntryID(void)
{
	return -1;
}

void *	CRadioTunerPlaylist::GetField(unsigned long ulFieldID)
{
	switch(ulFieldID)
	{
		case FIELD_FILENAME:
		case FIELD_URL:
			{
				if(m_URLList.GetCount() == 0)
					return NULL;

				int x = g_Rand.IRandom(0, m_URLList.GetCount()-1);
				return m_URLList[x];
			}
			break;

		case FIELD_TITLE:
			{
				return m_CurrentSong;
			}
			break;

		case FIELD_STATIONNAME:
			{
				return m_StationName;
			}
			break;
	}

	return NULL;
}

bool	CRadioTunerPlaylist::SetField(unsigned long ulFieldID, void * pNewData)
{
	switch(ulFieldID)
	{
		case FIELD_TITLE:
			{
				StrCpyN(m_CurrentSong, (LPTSTR)pNewData, 256);
			}
			break;
			
		case FIELD_STATIONNAME:
			{
				StrCpyN(m_StationName, (LPTSTR)pNewData, 256);
			}
			break;

		default:
			return false;
	}

	return true;
}

bool	CRadioTunerPlaylist::GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars)
{
	StrCpyN(szString, (LPTSTR)GetField(ulFieldID), ulNumChars);
	return true;
}