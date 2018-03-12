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

#pragma once

#include "Array.h"
#include "IPlaylist.h"
#include "MediaLibrary.h"
#include "TuniacApp.h"

class CHistory
{
protected:
	HMENU						m_hHistoryMenu;
	typedef struct
	{
		unsigned long		m_HistoryItemPlaylistID;
		unsigned long		m_HistoryItemEntryID;
	} HistoryEntry;

	Array<HistoryEntry, 10>				m_History;

public:
	CHistory(void);
	~CHistory(void);
	
	bool			Initialize();
	bool			Shutdown();

	void			RebuildMenuBase(void);
	void			PopupMenu(int x, int y);

	void			RebuildHistoryMenu(void);

	bool			PlayHistoryIndex(unsigned long ulIndex);

	unsigned long	GetCount(void);

	void			InitAddHistoryItem(unsigned long ulPlaylistID, unsigned long ulEntryID);
	void			AddHistoryItem(unsigned long ulPlaylistID, unsigned long ulEntryID);
	
	unsigned long	GetPlaylistIDAtIndex(unsigned long ulIndex);
	unsigned long	GetEntryIDAtIndex(unsigned long ulIndex);

	void			RemovePlaylistID(unsigned long ulPlaylistID);
	void			RemoveEntryID(unsigned long ulEntryID);

	void			Clear(void);
};
