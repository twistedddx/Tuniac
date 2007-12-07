#pragma once
#include "iplaylist.h"

class CRadioTunerPlaylist :
	public IPlaylist,
	public IPlaylistEntry
{
protected:


	TCHAR				m_StationName[256];

	TCHAR				m_CurrentSong[256];

	Array<LPTSTR, 3>	m_URLList;

public:
	CRadioTunerPlaylist(void);
	~CRadioTunerPlaylist(void);

	void	ResetURLS(void);
	void	AddURL(LPTSTR szURL);

public:
	unsigned long		GetFlags(void);
	unsigned long		GetPlaylistType(void);

	bool				SetPlaylistName(LPTSTR szPlaylistName);
	LPTSTR				GetPlaylistName(void);

	bool				Previous(void);
	bool				Next(void);
	bool				CheckNext(void);

	IPlaylistEntry	*	GetActiveItem(void);

public:	
	unsigned long	GetEntryID(void);

	void *	GetField(unsigned long ulFieldID);
	bool	SetField(unsigned long ulFieldID, void * pNewData);

	bool	GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars);
};
