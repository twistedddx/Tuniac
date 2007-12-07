#include "stdafx.h"
#include ".\audiocdplaylist.h"


CAudioCDPlaylist::CAudioCDPlaylist(char cDriveLetter)
{
	TCHAR cdtitle[32];
	m_DriveLetter = cDriveLetter;

	wsprintf( cdtitle, TEXT("%c:\\ Unknown CD"), m_DriveLetter);
	StrCpyN(m_AlbumTitle, cdtitle, 254 );

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
				LibraryEntry	libEnt;

				libEnt.dwTrack[0]			= (unsigned short)x;
				libEnt.dwTrack[1]			= m_TOC.LastTrack;
			
				int h,m,s,f;

				h = m_TOC.TrackData[x].Address[0] - m_TOC.TrackData[x-1].Address[0];
				m = m_TOC.TrackData[x].Address[1] - m_TOC.TrackData[x-1].Address[1];
				s = m_TOC.TrackData[x].Address[2] - m_TOC.TrackData[x-1].Address[2];
				f = m_TOC.TrackData[x].Address[3] - m_TOC.TrackData[x-1].Address[3];

				libEnt.iPlaybackTime = ((h*60*60) + (m*60) + s) * 1000;


				wsprintf(libEnt.szURL,		TEXT("AUDIOCD:%c:%d"),	m_DriveLetter,		x);
				wsprintf(libEnt.szTitle,	TEXT("Track %02d"),		x);
				wsprintf(libEnt.szArtist,	TEXT("Unknown Artist"),		x+1);

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
  
	wsprintf( buf, TEXT("\\\\.\\%c:"), m_DriveLetter );
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

unsigned long		CAudioCDPlaylist::GetNumCDTracks(void)
{
	return m_TrackList.GetCount();
}

unsigned long		CAudioCDPlaylist::GetFlags(void)
{
	return PLAYLIST_FLAGS_CANRENAME;
}

unsigned long		CAudioCDPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_CD;
}

bool				CAudioCDPlaylist::SetPlaylistName(LPTSTR szPlaylistName)
{
	StrCpyN(m_AlbumTitle, szPlaylistName, 256);
	return true;
}

LPTSTR				CAudioCDPlaylist::GetPlaylistName(void)
{
	return m_AlbumTitle;
}

bool				CAudioCDPlaylist::Previous(void)
{
	if(m_ActiveItem > 0)
	{
		m_ActiveItem--;
		return true;
	}

	return false;
}

bool				CAudioCDPlaylist::CheckNext(void)
{
	if(m_ActiveItem < (GetNumCDTracks()-1))
	{
		return true;
	}

	return false;
}

bool				CAudioCDPlaylist::Next(void)
{
	if(m_ActiveItem < (GetNumCDTracks()-1))
	{
		m_ActiveItem++;
		return true;
	}

	return false;
}

IPlaylistEntry	*	CAudioCDPlaylist::GetActiveItem(void)
{
	return m_TrackList[m_ActiveItem];
}

IPlaylistEntry *	CAudioCDPlaylist::GetItemAtIndex(unsigned long Index)
{
	return m_TrackList[Index];
}

unsigned long		CAudioCDPlaylist::GetActiveIndex()
{
	return m_ActiveItem;
}

bool				CAudioCDPlaylist::SetActiveIndex(int iIndex)
{
	m_ActiveItem = iIndex;

	return true;
}