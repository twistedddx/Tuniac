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
#include "audiocdplaylist.h"


CAudioCDPlaylist::CAudioCDPlaylist(char cDriveLetter)
{
	TCHAR cdtitle[32];
	m_DriveLetter = cDriveLetter;

	StringCchPrintf(cdtitle, 32, TEXT("%c:\\ Unknown CD"), m_DriveLetter);
	StringCchCopy(m_AlbumTitle, 256, cdtitle);

	GetCDInfo();

	m_ActiveItem = 0;
}

CAudioCDPlaylist::~CAudioCDPlaylist(void)
{
	while(m_TrackList.GetCount())
	{
		CMediaLibraryPlaylistEntry * tt = (CMediaLibraryPlaylistEntry *)m_TrackList[0];

		m_TrackList.RemoveAt(0);

		delete tt;
	}
}

bool CAudioCDPlaylist::GetCDInfo(void)
{
	if(ReadTOC())
	{
		for(unsigned long x=m_TOC.FirstTrack; x<m_TOC.LastTrack+1; x++)
		{
			if((m_TOC.TrackData[x].Control & 0x4) == 0)
			{
				LibraryEntry	libEnt = {0};

				libEnt.dwTrack[0]			= (unsigned short)x;
				libEnt.dwTrack[1]			= m_TOC.LastTrack;
			
				unsigned long h,m,s,f;

				h = m_TOC.TrackData[x].Address[0] - m_TOC.TrackData[x-1].Address[0];
				m = m_TOC.TrackData[x].Address[1] - m_TOC.TrackData[x-1].Address[1];
				s = m_TOC.TrackData[x].Address[2] - m_TOC.TrackData[x-1].Address[2];
				f = m_TOC.TrackData[x].Address[3] - m_TOC.TrackData[x-1].Address[3];

				libEnt.ulPlaybackTime = ((h*60*60) + (m*60) + s) * 1000;


				StringCchPrintf(libEnt.szURL, 260,		TEXT("AUDIOCD:%c:%u"),	m_DriveLetter,		x);
				StringCchPrintf(libEnt.szTitle, 128,		TEXT("Track %02u"),		x);
				StringCchCopy(libEnt.szArtist, 128,	TEXT("Unknown Artist"));

				IPlaylistEntry * tt = new CMediaLibraryPlaylistEntry(&libEnt);
				m_TrackList.AddTail(tt);
			}
		}

		if(m_TrackList.GetCount())
			return true;

	}
	return false;
}

bool CAudioCDPlaylist::ReadTOC(void)
{
	HANDLE hDrive;
	TCHAR buf[32];
	bool retVal = true;
	DWORD dwReturned;
  
	StringCchPrintf(buf, 32, TEXT("\\\\.\\%c:"), m_DriveLetter );
	hDrive = CreateFile(	buf, 
							GENERIC_READ, 
							FILE_SHARE_READ, 
							NULL,
							OPEN_EXISTING, 
							0, 
							NULL );

	if( hDrive == INVALID_HANDLE_VALUE )
		return false;

	dwReturned = 0;
	ZeroMemory( &m_TOC, sizeof(CDROM_TOC) );
	if ( !DeviceIoControl(	hDrive,
							IOCTL_CDROM_READ_TOC,
							NULL,
							0,
							&m_TOC,
							sizeof(CDROM_TOC),
							&dwReturned,
							NULL ) )
		retVal = false;
  
	CloseHandle( hDrive );

	return retVal;
}

unsigned long		CAudioCDPlaylist::GetNumItems(void)
{
	return m_TrackList.GetCount();
}

unsigned long		CAudioCDPlaylist::GetFlags(void)
{
	return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_DONTCROSSFADE;
}

unsigned long		CAudioCDPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_CD;
}

bool				CAudioCDPlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	if(szPlaylistName)
	{
		StringCchCopy(m_AlbumTitle, 256, szPlaylistName);
		return true;
	}

	return false;
}

LPTSTR				CAudioCDPlaylist::GetPlaylistName(void)
{
	return m_AlbumTitle;
}

unsigned long		CAudioCDPlaylist::Previous(void)
{
	if(m_ActiveItem > 0)
		return (m_ActiveItem - 1);

	return INVALID_PLAYLIST_INDEX;
}

unsigned long		CAudioCDPlaylist::Next(void)
{
	if(m_ActiveItem < (GetNumItems()-1))
		return m_ActiveItem + 1;

	return INVALID_PLAYLIST_INDEX;
}

unsigned long		CAudioCDPlaylist::GetNextFilteredIndexForFilteredIndex(unsigned long ulNormalFilteredIndex)
{
	if(ulNormalFilteredIndex < (GetNumItems()-1))
	{
		return ulNormalFilteredIndex + 1;
	}

	return INVALID_PLAYLIST_INDEX;
}

IPlaylistEntry	*	CAudioCDPlaylist::GetActiveEntry(void)
{
	return m_TrackList[m_ActiveItem];
}

IPlaylistEntry *	CAudioCDPlaylist::GetEntryAtIndex(unsigned long Index)
{
	return m_TrackList[Index];
}

unsigned long		CAudioCDPlaylist::GetActiveFilteredIndex(void)
{
	return m_ActiveItem;
}

bool				CAudioCDPlaylist::SetActiveFilteredIndex(unsigned long ulIndex)
{
	m_ActiveItem = ulIndex;
	return true;
}

unsigned long		CAudioCDPlaylist::GetActiveNormalFilteredIndex()
{
	return GetActiveFilteredIndex();
}

bool				CAudioCDPlaylist::SetActiveNormalFilteredIndex(unsigned long ulIndex)
{
	SetActiveFilteredIndex(ulIndex);
	return true;
}

IPlaylistEntry *	CAudioCDPlaylist::GetEntryAtFilteredIndex(unsigned long ulFilteredIndex)
{
	if(ulFilteredIndex != INVALID_PLAYLIST_INDEX)
	{
		return m_TrackList[ulFilteredIndex];
	}

	return NULL;
}

IPlaylistEntry *	CAudioCDPlaylist::GetEntryAtNormalFilteredIndex(unsigned long ulFilteredIndex)
{
	return GetEntryAtFilteredIndex(ulFilteredIndex);
}

unsigned long		CAudioCDPlaylist::GetFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{

	for(unsigned long x=0; x < m_TrackList.GetCount(); x++)
	{
		if(m_TrackList[x] == pIPE)
			return x;
	}

	return INVALID_PLAYLIST_INDEX;
}

unsigned long		CAudioCDPlaylist::GetNormalFilteredIndexforEntry(IPlaylistEntry	* pIPE)
{
	return GetFilteredIndexforEntry(pIPE);
}