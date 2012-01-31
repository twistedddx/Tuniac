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

// MediaLibrary.cpp: implementation of the CMediaLibrary class.
// This is not a viewable playlist LibraryPlaylist is simply based on this
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MediaLibrary.h"
#include "resource.h"

// only increment this when a change becomes incompatable with older versions!
#define TUNIAC_MEDIALIBRARY_VERSION		MAKELONG(0, 5)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LRESULT crapProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)tuniacApp.m_Skin.GetIcon(THEMEICON_WINDOW));
				SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)tuniacApp.m_Skin.GetIcon(THEMEICON_WINDOW_SMALL));

				ShowWindow(hDlg, SW_SHOW);
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}

CMediaLibrary::CMediaLibrary() :
	m_hAddingWindow(NULL),
	ulEntryID(0)
{
}

CMediaLibrary::~CMediaLibrary()
{
}

unsigned long CMediaLibrary::GetCount(void)
{
	return(m_MediaLibrary.GetCount());
}

CMediaLibraryPlaylistEntry * CMediaLibrary::GetEntryByIndex(unsigned long ulIndex)
{
	if(ulIndex > GetCount())
	{
		return NULL;
	}

	return m_MediaLibrary[ulIndex];
}

CMediaLibraryPlaylistEntry * CMediaLibrary::GetEntryByEntryID(unsigned long ulEntryID)
{
	for(unsigned long index = 0; index < GetCount(); index++)
	{
		if(m_MediaLibrary[index]->GetEntryID() == ulEntryID)
		{
			return(m_MediaLibrary[index]);
		}
	}

	return NULL;
}

CMediaLibraryPlaylistEntry * CMediaLibrary::GetEntryByURL(LPTSTR szURL)
{
	for(unsigned long index = 0; index < GetCount(); index++)
	{
		if(StrCmpI(m_MediaLibrary[index]->GetLibraryEntry()->szURL, szURL) == 0)
		{
			return(m_MediaLibrary[index]);
		}
	}
	return(NULL);
}

unsigned long CMediaLibrary::GetEntryIDByIndex(unsigned long ulIndex)
{
	if(ulIndex > GetCount())
	{
		return NULL;
	}

	return m_MediaLibrary[ulIndex]->GetEntryID();
}

unsigned long CMediaLibrary::GetEntryIDByItem(CMediaLibraryPlaylistEntry * pIPE)
{
	for(unsigned long index = 0; index < GetCount(); index++)
	{
		if(m_MediaLibrary[index] == pIPE)
		{
			return(m_MediaLibrary[index]->GetEntryID());
		}
	}
	return(NULL);
}

unsigned long CMediaLibrary::GetEntryIDByURL(LPTSTR szURL)
{
	for(unsigned long index = 0; index < GetCount(); index++)
	{
		if(StrCmpI(m_MediaLibrary[index]->GetLibraryEntry()->szURL, szURL) == 0)
		{
			return(m_MediaLibrary[index]->GetEntryID());
		}
	}
	return(NULL);
}

bool CMediaLibrary::BeginAdd(unsigned long ulNumItems)
{
	//TODO: handle unknown number of files: BEGIN_ADD_UNKNOWNNUMBER
	if(m_hAddingWindow != NULL)
		return false;

	m_hAddingWindow = CreateDialog(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_ADDINGFILES), tuniacApp.getMainWindow(), (DLGPROC)crapProc);

	m_ulAddingCountFiles = 0;
	m_ulAddingCountDirs = 0;

	if(m_hAddingWindow)
	{
		ShowWindow(m_hAddingWindow, SW_SHOW);
		EnableWindow(tuniacApp.getMainWindow(), FALSE);

		SendDlgItemMessage(m_hAddingWindow, IDC_ADDINGFILES_PROGRESS, PBM_SETRANGE, 0, MAKELONG(0, ulNumItems));
	}

	return true;
}

bool CMediaLibrary::EndAdd(void)
{
	if(m_hAddingWindow)
	{
		EnableWindow(tuniacApp.getMainWindow(), TRUE);
		DestroyWindow(m_hAddingWindow);
		m_hAddingWindow = NULL;
	}

	tuniacApp.m_PlaylistManager.m_LibraryPlaylist.RebuildPlaylist();
	tuniacApp.m_PlaylistManager.m_LibraryPlaylist.ApplyFilter();

	IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();
	if(pPlaylist)
	{
		if(pPlaylist->GetActiveFilteredIndex() == INVALID_PLAYLIST_INDEX)
			pPlaylist->SetActiveNormalFilteredIndex(0);
	}


	tuniacApp.m_SourceSelectorWindow->UpdateView();

	return true;
}

void CMediaLibrary::AddingFilesIncrement(bool bDir)
{
	if(m_hAddingWindow == NULL)
		return;

	if(bDir)
		m_ulAddingCountDirs++;
	else
		m_ulAddingCountFiles++;

	if(m_ulAddingCountFiles % 10 != 0)
		return;

	TCHAR szCount[256];
	_snwprintf(szCount, 256, TEXT("%d files in %d folders"), m_ulAddingCountFiles, m_ulAddingCountDirs);
	SendDlgItemMessage(m_hAddingWindow, IDC_ADDINGFILES_COUNT, WM_SETTEXT, 0, (WPARAM)szCount);
}

bool CMediaLibrary::AddItem(LPTSTR szItemToAdd)
{
	SendDlgItemMessage(m_hAddingWindow, IDC_ADDINGFILES_PROGRESS, PBM_STEPIT, 0, 0);
	if(m_hAddingWindow)
	{
		MSG	msg;

		if(PeekMessage(&msg, NULL, 0 , 0, PM_REMOVE))
		{
			if(!IsDialogMessage(m_hAddingWindow, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	if(PathIsURL(szItemToAdd))
	{
		return AddStreamToLibrary(szItemToAdd);
	}
	else
	{
		if(GetFileAttributes(szItemToAdd) & FILE_ATTRIBUTE_DIRECTORY)
		{
			return AddDirectoryToLibrary(szItemToAdd);
		}
		else
		{
			return AddFileToLibrary(szItemToAdd);
		}
	}
	return false;
}

bool CMediaLibrary::AddStreamToLibrary(LPTSTR szURL)
{
	if(GetEntryByURL(szURL))
		return true;

	LibraryEntry  libraryEntry;

	ZeroMemory(&libraryEntry, sizeof(LibraryEntry));

	// fill in media library specific stuff
	while(GetEntryByEntryID(ulEntryID))
	{
		ulEntryID++;
	}
	// we need to set the streampath here plus a nice name
	StrCpy(libraryEntry.szArtist, szURL);
    StrCpy(libraryEntry.szURL, szURL);
	GetLocalTime(&libraryEntry.stDateAdded);

	libraryEntry.dwRating = 0;
	libraryEntry.dwPlayCount = 0;
	libraryEntry.dwKind = ENTRY_KIND_URL;

	CMediaLibraryPlaylistEntry * pIPE = new CMediaLibraryPlaylistEntry(&libraryEntry);
	pIPE->SetEntryID(ulEntryID);
	m_MediaLibrary.AddTail(pIPE);

	//create or add to Streams playlist
	IPlaylistEntry	*	pPE = NULL;
	EntryArray playlistEntries;
	pPE = pIPE;
	if(pPE)
	{
		playlistEntries.AddTail(pPE);
	}


	bool bStreamPlaylist = false;
	unsigned long ulPlaylistCount = tuniacApp.m_PlaylistManager.GetNumPlaylists();
	for(unsigned long i = 0; i < ulPlaylistCount; i++)
	{
		if(StrCmpI(tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(i)->GetPlaylistName(), L"Streams") == 0)
		{
			IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(i);
			if(pPlaylist)
			{
				((IPlaylistEX *)pPlaylist)->AddEntryArray(playlistEntries);
				bStreamPlaylist=true;
			}
		}

	}
	if(!bStreamPlaylist)
	{
		tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(L"Streams", playlistEntries);
	}
	tuniacApp.m_SourceSelectorWindow->UpdateList();

	ulEntryID++;

	return true;
}

bool CMediaLibrary::AddFileToLibrary(LPTSTR szURL)
{
	if(GetEntryByURL(szURL))
		return false;

	//if its a playlist file import it
	if(m_ImportExport.Import(szURL))
		return true;

	//decoders are more vital than infomanagers
	for(unsigned long i=0; i < CCoreAudio::Instance()->GetNumPlugins(); i++)
	{
		IAudioSourceSupplier * pPlugin = CCoreAudio::Instance()->GetPluginAtIndex(i);
		if(pPlugin->CanHandle(szURL))
		{
			LibraryEntry  libraryEntry;

			ZeroMemory(&libraryEntry, sizeof(LibraryEntry));

			// fill in media library specific stuff
			while(GetEntryByEntryID(ulEntryID))
			{
				ulEntryID++;
			}

			// we need to set the filename here, because its the one bit of information the InfoManager needs to work with
			StrCpy(libraryEntry.szURL, szURL);


			// extract generic info from the file (creation time/size)
			HANDLE hFile = CreateFile(szURL, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
				return false;

			FILETIME ft;
			GetFileTime(hFile, &ft, NULL, NULL);
			FileTimeToSystemTime(&ft, &libraryEntry.stFileCreationDate);

			if(tuniacApp.m_Preferences.GetDateAddedToFileCreationTime())
			{
				FileTimeToSystemTime(&ft, &libraryEntry.stDateAdded);
			}
			else
			{
				GetLocalTime(&libraryEntry.stDateAdded);
			}

			libraryEntry.dwFilesize = GetFileSize(hFile, NULL);
			CloseHandle(hFile);

			libraryEntry.dwRating = 0;
			libraryEntry.dwPlayCount = 0;
			libraryEntry.dwKind = ENTRY_KIND_FILE;

			// let the info manager get the format specific stuff here!
			for(unsigned long plugin=0; plugin<m_InfoManagerArray.GetCount(); plugin++)
			{
				if(m_InfoManagerArray[plugin].pInfoManager->CanHandle(szURL))
				{
					if(m_InfoManagerArray[plugin].pInfoManager->GetInfo(&libraryEntry))
						break;
				}
			}

			//set filename as title if tag readers didn't find one
			if(libraryEntry.szTitle[0] ==  TEXT('\0'))
			{
				TCHAR	szFileTitle[128];
				GetFileTitle(szURL, szFileTitle, 128);
				StrCpy(libraryEntry.szTitle, szFileTitle);
			}


			CMediaLibraryPlaylistEntry * pIPE = new CMediaLibraryPlaylistEntry(&libraryEntry);

			pIPE->SetEntryID(ulEntryID);
			m_MediaLibrary.AddTail(pIPE);

			AddingFilesIncrement(false);

			ulEntryID++;

			return true;
		}
	}
	return false;
}

bool CMediaLibrary::AddDirectoryToLibrary(LPTSTR szDirectory)
{
	WIN32_FIND_DATA w32fd; 
	HANDLE			hFind; 

	TCHAR path[1024];

	StrCpy(path, szDirectory);
	PathAddBackslash(path);
	StrCat(path, TEXT("*.*"));

	hFind = FindFirstFile( path, &w32fd); 

	if(INVALID_HANDLE_VALUE == hFind) 
	{
		return false;
	} 

	do
	{
		if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
			continue;

		TCHAR temp[1024];

		StrCpy(temp, szDirectory);
		PathAddBackslash(temp);
		StrCat(temp, w32fd.cFileName);

		AddItem(temp);

	} while(FindNextFile( hFind, &w32fd));

	FindClose(hFind);
	
	AddingFilesIncrement(true);

	return true;
}

bool			CMediaLibrary::RemoveEntry(IPlaylistEntry * pIPE)
{
	if(pIPE == NULL)
		return false;

	tuniacApp.m_Queue.RemoveEntryID(pIPE->GetEntryID());
	tuniacApp.m_History.RemoveEntryID(pIPE->GetEntryID());
	for(unsigned long x=0; x<m_MediaLibrary.GetCount(); x++)
	{
		if(m_MediaLibrary[x] == pIPE)
		{
			delete m_MediaLibrary[x];
			m_MediaLibrary.RemoveAt(x);
			return true;
		}
	}
	return false;
}

bool			CMediaLibrary::RemoveEntryID(unsigned long ulEntryID)
{
	tuniacApp.m_Queue.RemoveEntryID(ulEntryID);
	tuniacApp.m_History.RemoveEntryID(ulEntryID);
	for(unsigned long x=0; x<m_MediaLibrary.GetCount(); x++)
	{
		if(m_MediaLibrary[x]->GetEntryID()== ulEntryID)
		{
			delete m_MediaLibrary[x];
			m_MediaLibrary.RemoveAt(x);
			return true;
		}
	}
	return false;
}

bool CMediaLibrary::Initialize()
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szURL[MAX_PATH];

	GetModuleFileName(NULL, szURL, MAX_PATH);
	PathRemoveFileSpec(szURL);
	PathAddBackslash(szURL);
	StrCat(szURL, TEXT("*.dll"));

	hFind = FindFirstFile( szURL, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR temp[512];

			InfoManagerEntry	IME;

			GetModuleFileName(NULL, temp, 512);
			PathRemoveFileSpec(temp);
			PathAddBackslash(temp);
			StrCat(temp, w32fd.cFileName);

			IME.hDLL = LoadLibrary(temp);
			if(IME.hDLL)
			{
				CreateInfoManagerPluginFunc pCIMPF;

				pCIMPF = (CreateInfoManagerPluginFunc)GetProcAddress(IME.hDLL, "CreateInfoManagerPlugin");
				if(pCIMPF)
				{
					IME.pInfoManager = pCIMPF();

					if(IME.pInfoManager)
					{
						m_InfoManagerArray.AddTail(IME);
					}
					else
					{
						FreeLibrary(IME.hDLL);
					}
				}
				else
				{
					FreeLibrary(IME.hDLL);
				}
			}

		} while(FindNextFile( hFind, &w32fd));

		FindClose(hFind); 
	}

	if(!m_InfoManagerArray.GetCount())
	{
		MessageBox(NULL, TEXT("Warning: There are no InfoManager plugins available.\nYou will not be able to see or edit extra information on new items in the Media Library."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
	}

	m_ImportExport.Initialize();

	LoadMediaLibrary();

	return true;
}

bool CMediaLibrary::Shutdown(bool bSave)
{
	if(bSave)
		SaveMediaLibrary();

	m_ImportExport.Shutdown();

	while(m_InfoManagerArray.GetCount())
	{
		m_InfoManagerArray[0].pInfoManager->Destroy();
		FreeLibrary(m_InfoManagerArray[0].hDLL);
		m_InfoManagerArray.RemoveAt(0);
	}

	return true;
}

bool CMediaLibrary::UpdateMLIndex(unsigned long ulMLIndex)
{
	if(ulMLIndex > GetCount())
		return false;

	CMediaLibraryPlaylistEntry *	pIPE = GetEntryByIndex(ulMLIndex);
	LPTSTR szURL = (LPTSTR)pIPE->GetField(FIELD_URL);

	//stream
	if(PathIsURL(szURL))
		return true;

	//decoders are more vital than infomanagers
	for(unsigned long i=0; i < CCoreAudio::Instance()->GetNumPlugins(); i++)
	{
		IAudioSourceSupplier * pPlugin = CCoreAudio::Instance()->GetPluginAtIndex(i);
		if(pPlugin->CanHandle(szURL))
		{
			LibraryEntry  libraryEntry;

			ZeroMemory(&libraryEntry, sizeof(LibraryEntry));

			// we need to set the filename here, because its the one bit of information the InfoManager needs to work with
			StrCpy(libraryEntry.szURL, szURL);

			// extract generic info from the file (creation time/size)
			HANDLE hFile = CreateFile(szURL, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
				return false;

			FILETIME ft;
			GetFileTime(hFile, &ft, NULL, NULL);
			FileTimeToSystemTime(&ft, &libraryEntry.stFileCreationDate);
			libraryEntry.dwFilesize = GetFileSize(hFile, NULL);
			CloseHandle(hFile);

			// let the info manager get the format specific stuff here!
			for(unsigned long plugin=0; plugin<m_InfoManagerArray.GetCount(); plugin++)
			{
				if(m_InfoManagerArray[plugin].pInfoManager->CanHandle(szURL))
				{
					if(m_InfoManagerArray[plugin].pInfoManager->GetInfo(&libraryEntry))
						break;
				}
			}

			//set filename as title if tag readers didn't find one
			if(libraryEntry.szTitle[0] ==  TEXT('\0'))
			{
				TCHAR	szFileTitle[128];
				GetFileTitle(szURL, szFileTitle, 128);
				StrCpy(libraryEntry.szTitle, szFileTitle);
			}

			unsigned long ulEntryID = pIPE->GetEntryID();
			unsigned long ulPlayCount = (unsigned long)pIPE->GetField(FIELD_PLAYCOUNT);

			CMediaLibraryPlaylistEntry * pIPE = new CMediaLibraryPlaylistEntry(&libraryEntry);

			pIPE->SetEntryID(ulEntryID);
			pIPE->SetField(FIELD_PLAYCOUNT, (void *)(ulPlayCount+1));
			m_MediaLibrary.RemoveAt(ulMLIndex);
			m_MediaLibrary.InsertBefore(ulMLIndex, pIPE);

			return true;
		}
	}
	return false;
}

//todo bits: per column tag writing?
//bool CMediaLibrary::WriteFileTags(LPTSTR szURL, unsigned long ulFieldID, void * pNewData)
bool CMediaLibrary::WriteFileTags(IPlaylistEntry * pIPE)
{
	LibraryEntry * libEnt = GetEntryByEntryID(pIPE->GetEntryID())->GetLibraryEntry();
	// let the info manager get the format specific stuff here!
	for(unsigned long plugin=0; plugin<m_InfoManagerArray.GetCount(); plugin++)
	{
		if(m_InfoManagerArray[plugin].pInfoManager->CanHandle(libEnt->szURL))
		{
			if(m_InfoManagerArray[plugin].pInfoManager->SetInfo(libEnt))
				return true;;
		}
	}
	return false;
}

typedef struct
{
	unsigned long Version;
	unsigned long NumEntries;
	unsigned long PauseAt;
} MLDiskHeader;

bool CMediaLibrary::LoadMediaLibrary(void)
{
	TCHAR				szURL[MAX_PATH];
	HANDLE				hLibraryFile;
	MLDiskHeader		MLDH;
	unsigned long		BytesRead;
	bool				bOK = true;



	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szURL ) ) )
	{
		PathAppend( szURL, TEXT("\\Tuniac\\TuniacMediaLibrary.dat") );
	}
	else
	{
		//cant get appdata path
		return false;
	}

	hLibraryFile = CreateFile(	szURL,
							GENERIC_READ,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_FLAG_SEQUENTIAL_SCAN,
							NULL);

	if(hLibraryFile == INVALID_HANDLE_VALUE)
	{
		// there is no media library!
		return false;
	}

	ReadFile(hLibraryFile, &MLDH, sizeof(MLDH), &BytesRead, NULL);
	if(BytesRead != sizeof(MLDH))
	{
		MessageBox(NULL, TEXT("MediaLibrary is corrupt, resetting library."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else if(MLDH.Version != TUNIAC_MEDIALIBRARY_VERSION)
	{
		MessageBox(NULL, TEXT("MediaLibrary is saved in an incompatable version, resetting library."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{
		tuniacApp.m_SoftPause.ulAt = MLDH.PauseAt;
		
		LibraryEntry	MLE;
		unsigned long	TempID;

		for(unsigned long x = 0; x < MLDH.NumEntries; x++)
		{
			ReadFile(hLibraryFile, &TempID, sizeof(unsigned long), &BytesRead, NULL);
			if(BytesRead != sizeof(unsigned long))
			{
				MessageBox(NULL, TEXT("MediaLibrary is corrupt, resetting library."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
				m_MediaLibrary.RemoveAll();
				bOK = false;
				break;
			}

			ReadFile(hLibraryFile, &MLE, sizeof(LibraryEntry), &BytesRead, NULL);
			if(BytesRead != sizeof(LibraryEntry))
			{
				MessageBox(NULL, TEXT("MediaLibrary is corrupt, resetting library."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
				m_MediaLibrary.RemoveAll();
				bOK = false;
				break;
			}

			CMediaLibraryPlaylistEntry * pIPE = new CMediaLibraryPlaylistEntry(&MLE);
			pIPE->SetEntryID(TempID);
			m_MediaLibrary.AddTail(pIPE);			
		}

		if(bOK)
		{
			unsigned long ulQueueSize;
			ReadFile(hLibraryFile, &ulQueueSize, sizeof(unsigned long), &BytesRead, NULL);
			if(BytesRead != sizeof(unsigned long))
			{
				MessageBox(NULL, TEXT("Queue is corrupt, resetting queue."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
			}
			else
			{
				for(unsigned long x = 0; x < ulQueueSize; x++)
				{
					ReadFile(hLibraryFile, &TempID, sizeof(unsigned long), &BytesRead, NULL);
					if(BytesRead != sizeof(unsigned long))
					{
						MessageBox(NULL, TEXT("Queue is corrupt, resetting queue."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
						tuniacApp.m_Queue.Clear();
						bOK = false;
						break;
					}
					tuniacApp.m_Queue.Append(TempID);
				}
			}

			unsigned long ulHistorySize;
			ReadFile(hLibraryFile, &ulHistorySize, sizeof(unsigned long), &BytesRead, NULL);
			if(BytesRead != sizeof(unsigned long))
			{
				MessageBox(NULL, TEXT("History list is corrupt, resetting list."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
			}
			else
			{
				for(unsigned long x = 0; x < ulHistorySize; x++)
				{
					ReadFile(hLibraryFile, &TempID, sizeof(unsigned long), &BytesRead, NULL);
					if(BytesRead != sizeof(unsigned long))
					{
						MessageBox(NULL, TEXT("History list is corrupt, resetting list."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
						tuniacApp.m_History.Clear();
						bOK = false;
						break;
					}

					tuniacApp.m_History.AddEntryID(TempID);
				}
			}
		}

		if(tuniacApp.m_Queue.GetCount() == 0 && tuniacApp.m_Preferences.GetRepeatMode() == RepeatAllQueued)
			tuniacApp.m_Preferences.SetRepeatMode(RepeatAll);

	}

	CloseHandle(hLibraryFile);
	return bOK;
}

static int SavingProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)tuniacApp.m_Skin.GetIcon(THEMEICON_WINDOW));
				SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)tuniacApp.m_Skin.GetIcon(THEMEICON_WINDOW_SMALL));

				ShowWindow(hDlg, SW_SHOW);
			}
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

bool CMediaLibrary::SaveMediaLibrary(void)
{
	TCHAR				szURL[MAX_PATH];
	HANDLE				hLibraryFile;
	MLDiskHeader		MLDH;
	unsigned long		BytesWritten;
	bool				bOK = true;

	HWND hSaveWnd = CreateDialog(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_SAVINGLIBRARY), NULL, (DLGPROC)SavingProc);
	if(hSaveWnd)
	{
		MSG	msg;

		while(PeekMessage(&msg, NULL, 0 , 0, PM_REMOVE))
		{
			if(!IsDialogMessage(hSaveWnd, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	SendDlgItemMessage(hSaveWnd, IDC_SAVINGLIBRARY_PROGRESS, PBM_SETRANGE32, 0, (int)(m_MediaLibrary.GetCount() / 100) + 1);
	SendDlgItemMessage(hSaveWnd, IDC_SAVINGLIBRARY_PROGRESS, PBM_SETSTEP, 1, 0);

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szURL ) ) )
	{
		PathAppend( szURL, TEXT("\\Tuniac\\TuniacMediaLibrary.dat") );
	}
	else{
		//cant get appdata path
		return false;
	}

	hLibraryFile = CreateFile(	szURL,
							GENERIC_WRITE, 
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_FLAG_SEQUENTIAL_SCAN,
							NULL);

	if(hLibraryFile == INVALID_HANDLE_VALUE)
	{
		// there is no media library!
		return false;
	}
	MLDH.Version			= TUNIAC_MEDIALIBRARY_VERSION;
	MLDH.NumEntries			= m_MediaLibrary.GetCount();
	MLDH.PauseAt			= tuniacApp.m_SoftPause.ulAt;

	WriteFile(hLibraryFile, &MLDH, sizeof(MLDH), &BytesWritten, NULL);
	if(BytesWritten != sizeof(MLDH))
	{
		MessageBox(NULL, TEXT("Error saving MediaLibrary header information."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{
		for(unsigned long x = 0; x < m_MediaLibrary.GetCount(); x++)
		{
			if(x % 100 == 0)
				SendDlgItemMessage(hSaveWnd, IDC_SAVINGLIBRARY_PROGRESS, PBM_STEPIT, 0, 0);
			unsigned long RealID = tuniacApp.m_PlaylistManager.m_LibraryPlaylist.GetEntryIDAtRealIndex(x);
			
			WriteFile(hLibraryFile, &RealID, sizeof(unsigned long), &BytesWritten, NULL);
			if(BytesWritten != sizeof(unsigned long))
			{
				TCHAR tstr[256];
				_snwprintf(tstr, 256, TEXT("Error saving playlist entry %d."), x);
				MessageBox(NULL, tstr, TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
				break;
			}


			WriteFile(hLibraryFile, GetEntryByEntryID(RealID)->GetLibraryEntry(), sizeof(LibraryEntry), &BytesWritten, NULL);

			if(BytesWritten != sizeof(LibraryEntry))
			{
				TCHAR tstr[256];
				_snwprintf(tstr, 256, TEXT("Error saving playlist entry %d."), x);
				MessageBox(NULL, tstr, TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
				break;
			}
		}

		if(bOK)
		{
			unsigned long ulQueueSize = tuniacApp.m_Queue.GetCount();
			WriteFile(hLibraryFile, &ulQueueSize, sizeof(unsigned long), &BytesWritten, NULL);
			if(BytesWritten != sizeof(unsigned long))
			{
				MessageBox(NULL, TEXT("Error saving play queue."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
			}
			else
			{
				for(unsigned long x = 0; x < ulQueueSize; x++)
				{
					unsigned long ulEntryID = tuniacApp.m_Queue.GetEntryIDAtIndex(x);
					if(ulEntryID == NULL)
						continue;
					WriteFile(hLibraryFile, &ulEntryID, sizeof(unsigned long), &BytesWritten, NULL);

					if(BytesWritten != sizeof(unsigned long))
					{
						MessageBox(NULL, TEXT("Error saving play queue."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
						bOK = false;
						break;
					}
				}
			}
		}
		if(bOK)
		{
			unsigned long ulHistorySize = tuniacApp.m_History.GetCount();
			WriteFile(hLibraryFile, &ulHistorySize, sizeof(unsigned long), &BytesWritten, NULL);
			if(BytesWritten != sizeof(unsigned long))
			{
				MessageBox(NULL, TEXT("Error saving history list."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
			}
			else
			{

				for(int x = ulHistorySize - 1; x >= 0; x--)
				{
					unsigned long ulEntryID = tuniacApp.m_History.GetHistoryEntryID(x);
					if(ulEntryID == INVALID_PLAYLIST_INDEX)
						continue;
					WriteFile(hLibraryFile, &ulEntryID, sizeof(unsigned long), &BytesWritten, NULL);

					if(BytesWritten != sizeof(unsigned long))
					{
						MessageBox(NULL, TEXT("Error saving history list."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
						bOK = false;
						break;
					}
				}

			}
		}
	}

	CloseHandle(hLibraryFile);

	DestroyWindow(hSaveWnd);

	return bOK;
}

IInfoManager	*		CMediaLibrary::GetInfoManagerForFilename(LPTSTR szItemToAdd)
{
	// let the info manager get the format specific stuff here!
	for(unsigned long plugin=0; plugin<m_InfoManagerArray.GetCount(); plugin++)
	{
		if(m_InfoManagerArray[plugin].pInfoManager->CanHandle(szItemToAdd))
		{
			return m_InfoManagerArray[plugin].pInfoManager;
		}
	}

	return NULL;
}
