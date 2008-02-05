#pragma once

#include "Array.h"
#include "IPlaylist.h"
#include "MediaLibrary.h"
#include "TuniacApp.h"

class CHistory
{
protected:
	HMENU						m_hMenu;
	Array<IPlaylistEntry *, 3>	m_History;

public:
	CHistory(void);
	~CHistory(void);
	
	bool		Initialize();
	bool		Shutdown();

	void		AddItem(IPlaylistEntry * pEntry);
	
	bool		RemoveItem(IPlaylistEntry * pEntry);
	void		Clear(void);
	
	void		PopupMenu(int x, int y);
	bool		PlayHistoryItem(unsigned long ulIndex);
	IPlaylistEntry * GetHistoryItem(unsigned long ulIndex);

	unsigned long	GetCount(void);

};
