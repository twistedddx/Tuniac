#pragma once
#include "iplaylist.h"

#include "ntddcdrm.h"
#include "devioctl.h"

class CAudioCDPlaylist :
	public IPlaylist
{
protected:
	TCHAR						m_AlbumTitle[256];
	char						m_DriveLetter;

	int							m_ActiveItem;
	
	EntryArray					m_TrackList;

	CDROM_TOC					m_TOC;

	bool GetCDInfo(void);
	bool ReadTOC(void);

public:
	CAudioCDPlaylist(char cDriveLetter);
	~CAudioCDPlaylist(void);

	char GetDriveLetter(void) { return m_DriveLetter; }

	unsigned long		GetNumCDTracks(void);
	IPlaylistEntry *	GetItemAtIndex(unsigned long Index);

	unsigned long		GetActiveIndex();
	bool				SetActiveIndex(int iIndex);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	int					GetNextIndex(void);
	bool				Next(void);

	IPlaylistEntry	*	GetActiveItem(void);
};
