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
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma once

#include "LibraryEntry.h"
#include "IInfoManager.h"
#include "IPlaylist.h"
#include "MediaLibraryPlaylistEntry.h"
#include "ImportExportManager.h"

#define BEGIN_ADD_UNKNOWNNUMBER		(-1)

// only increment this when a change becomes incompatable with older versions!
#define TUNIAC_MEDIALIBRARY_VERSION		MAKELONG(0, 8)

//Past versions
#define TUNIAC_MEDIALIBRARY_VERSION07		MAKELONG(0, 7)
#define TUNIAC_MEDIALIBRARY_VERSION06		MAKELONG(0, 6)
#define TUNIAC_MEDIALIBRARY_VERSION05		MAKELONG(0, 5)

typedef struct
{
	unsigned long Version;
	unsigned long NumEntries;
	unsigned long PauseAt;
} MLDiskHeader;

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

	unsigned long							m_ulEntryID;
	unsigned long							m_bNotInitialML;

	bool AddFileToLibrary(LPTSTR			szURL);
	bool AddStreamToLibrary(LPTSTR			szURL);
	bool AddDirectoryToLibrary(LPTSTR		szDirectory);

	void AddingFilesIncrement(bool bDir);
public:
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
	bool					AddItem(LPTSTR szItemToAdd, bool bForceDuplicateCheck);
	bool					RemoveEntry(IPlaylistEntry *			pIPE);
	bool					RemoveEntryID(unsigned long ulEntryID);

	bool					UpdateMLIndex(unsigned long	ulMLIndex);
//todo bits: per column tag writing?
//	bool					WriteFileTags(LPTSTR szURL, unsigned long ulFieldID, void * pNewData);
	bool					WriteFileTags(IPlaylistEntry * pIPE);

	CMediaLibraryPlaylistEntry *	GetEntryByIndex(unsigned long	ulIndex);
	CMediaLibraryPlaylistEntry *	GetEntryByEntryID(unsigned long		ulEntryID);
	CMediaLibraryPlaylistEntry *	GetEntryByURL(LPTSTR		szURL);

	unsigned long			GetEntryIDByIndex(unsigned long	ulIndex);
	unsigned long			GetEntryIDByItem(CMediaLibraryPlaylistEntry *		pIPE);
	unsigned long			GetEntryIDByURL(LPTSTR		szURL);

	IInfoManager	*		GetInfoManagerForFilename(LPTSTR szItemToAdd);
};