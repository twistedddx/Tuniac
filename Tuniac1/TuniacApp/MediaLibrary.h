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

#pragma once

#include "LibraryEntry.h"
#include "IInfoManager.h"
#include "IPlaylist.h"
#include "MediaLibraryPlaylistEntry.h"
#include "PlayQueue.h"
#include "ImportExportManager.h"

#define BEGIN_ADD_UNKNOWNNUMBER		(-1)

class CMediaLibrary  
{
protected:
typedef struct
{
	HINSTANCE			hDLL;
	IInfoManager *		pInfoManager;
} InfoManagerEntry;

	Array<InfoManagerEntry, 3>				m_InfoManagerArray;

	Array<CMediaLibraryPlaylistEntry *, 200>	m_MediaLibrary;

	HWND									m_hAddingWindow;

	unsigned long							m_ulAddingCountFiles;
	unsigned long							m_ulAddingCountDirs;

	bool AddFileToLibrary(LPTSTR			szURL);
	bool AddStreamToLibrary(LPTSTR			szURL);
	bool AddDirectoryToLibrary(LPTSTR		szDirectory);

	void AddingFilesIncrement(bool bDir);
public:
	CPlayQueue				m_Queue;
	CImportExportManager	m_ImportExport;

	CMediaLibrary();
	virtual ~CMediaLibrary();

	bool					Initialize(void);
	bool					Shutdown(bool bSave = true);

	bool					SaveMediaLibrary(void);
	bool					LoadMediaLibrary(void);

	unsigned long			GetCount(void);

	bool					BeginAdd(unsigned long ulNumItems);
	bool					EndAdd(void);
	bool					AddItem(LPTSTR szItemToAdd);
	bool					RemoveItem(IPlaylistEntry *			pEntry);

	bool					UpdateMLIndex(unsigned long	ulMLIndex);

	CMediaLibraryPlaylistEntry *	GetItemByIndex(unsigned long	ulIndex);
	CMediaLibraryPlaylistEntry *	GetItemByID(unsigned long		ulID);
	CMediaLibraryPlaylistEntry *	GetItemByURL(LPTSTR		szURL);

	IInfoManager	*		GetInfoManagerForFilename(LPTSTR szItemToAdd);
};