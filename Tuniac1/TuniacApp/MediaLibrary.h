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
};