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

//the playlist manager handles all tuniac's playlists, whther they be playlist 0 the LibraryPlaylist.cpp or low numbered AudioCDPlaylist.cpp(when valid) or the StandardPlaylist.cpp


#include "stdafx.h"
#include "playlistmanager.h"


// only increment this when a change becomes incompatable with older versions!
#define TUNIAC_PLAYLISTLIBRARY_VERSION		MAKELONG(0, 6)

//past
#define TUNIAC_PLAYLISTLIBRARY_VERSION05		MAKELONG(0, 5)
#define TUNIAC_PLAYLISTLIBRARY_VERSION04		MAKELONG(0, 4)

bool DriveInMask(ULONG uMask, char Letter)
{
	unsigned long x = 1;
	unsigned long letterindex = toupper(Letter) - 'A';

	for(unsigned long t=0; t<letterindex; t++)
	{
		x <<= 1;
	}

	if(uMask & x)
		return true;

	return false;
}

CPlaylistManager::CPlaylistManager(void)
{
}

CPlaylistManager::~CPlaylistManager(void)
{
}

bool CPlaylistManager::Initialize(LPTSTR szLibraryFolder)
{
	unsigned long DrivesMask = GetLogicalDrives();

	for(char c = 'A'; c<='Z'; c++)
	{
		if(DriveInMask(DrivesMask, c))
		{
			TCHAR tstr[10];
			StringCchPrintf(tstr, 10, TEXT("%c:\\"), c);

			if(GetDriveType(tstr) == DRIVE_CDROM)
			{
				AddCDWithDriveLetter(c);
			}
		}
	}

	m_hThread = CreateThread(	NULL,
								16384,
								PMThreadStub,
								this,
								0,
								&m_dwThreadID);

	if(!m_hThread)
	{
		return false;
	}

	m_ulActivePlaylistIndex = INVALID_PLAYLIST_INDEX;

	m_LibraryPlaylist.RebuildPlaylist();
	LoadPlaylistLibrary(szLibraryFolder);

	if(m_ActivePlaylist == NULL)
		SetActivePlaylist(0);

	return true;
}

bool CPlaylistManager::Shutdown(LPTSTR szLibraryFolder, bool bSave)
{
	if(bSave)
		SavePlaylistLibrary(szLibraryFolder);

	if(m_hThread)
	{
		int Count = 10;
		while(!PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0) && Count--)
			Sleep(10);

		if(WaitForSingleObject(m_hThread, 10000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	return true;
}

typedef struct
{
	unsigned long	Version;
	unsigned long	NumEntries;
	TCHAR			Name[128];
	int				ActivePlaylist;
	int				ActiveIndex;
	TCHAR			LibraryFilter[128];
	unsigned long	LibraryFilterField;
	bool			LibraryFilterReverse;
} PLDiskHeader;

typedef struct
{
	unsigned long	NumItems;
	TCHAR			Name[128];
	TCHAR			Filter[128];
	unsigned long	FilterField;
	bool			FilterReverse;
} PLDiskSubHeader;

bool			CPlaylistManager::LoadPlaylistLibrary(LPTSTR szLibraryFolder)
{
	PLDiskHeader		PLDH;
	HANDLE				hFile;
	unsigned long		ulBytesRead;
	bool				bOK = true;
	TCHAR				szPlaylistPath[MAX_PATH];
	
	if (szLibraryFolder[0] == TEXT('\0'))
	{
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szLibraryFolder)))
		{
			PathAppend(szLibraryFolder, TEXT("\\Tuniac"));
		}
		else
		{
			return false;
		}
	}

	StringCbCopy(szPlaylistPath, MAX_PATH, szLibraryFolder);
	PathAppend(szPlaylistPath, TEXT("\\Playlists.dat"));

	hFile = CreateFile(szPlaylistPath,
					GENERIC_READ, 
					0,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_SEQUENTIAL_SCAN,
					NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		//no saved playlists
		return false;

	ReadFile(hFile, &PLDH, sizeof(PLDH), &ulBytesRead, NULL);

	if(ulBytesRead != sizeof(PLDH))
	{
		//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist Library is corrupt, resetting playlists."));
		MessageBox(NULL, TEXT("Playlist Library is corrupt, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else if (PLDH.Version != TUNIAC_PLAYLISTLIBRARY_VERSION && PLDH.Version != TUNIAC_PLAYLISTLIBRARY_VERSION05 && PLDH.Version != TUNIAC_PLAYLISTLIBRARY_VERSION04)
	{
		//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist Library is saved in an incompatable version, resetting playlists."));
		MessageBox(NULL, TEXT("Playlist Library is saved in an incompatable version, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{

		for(unsigned long ulPlaylist = 0; ulPlaylist < PLDH.NumEntries; ulPlaylist++)
		{
			PLDiskSubHeader SubHeader;
			EntryArray		myEntryArray;
			unsigned long	ulEntryID;

			CStandardPlaylist * pPlaylist = new CStandardPlaylist;

			ReadFile(hFile, &SubHeader, sizeof(PLDiskSubHeader), &ulBytesRead, NULL);
			if(ulBytesRead != sizeof(PLDiskSubHeader))
			{
				//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist Library is corrupt, resetting playlists."));
				MessageBox(NULL, TEXT("Playlist Library is corrupt, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
				delete pPlaylist;
				m_StandardPlaylists.RemoveAll();
				bOK = false;
				break;
			}
			
			pPlaylist->SetPlaylistName(SubHeader.Name);

			for(unsigned long x = 0; x < SubHeader.NumItems; x++)
			{
				ReadFile(hFile, &ulEntryID, sizeof(unsigned long), &ulBytesRead, NULL);
				if(ulBytesRead != sizeof(unsigned long))
				{
					//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist Library is corrupt, resetting playlists."));
					MessageBox(NULL, TEXT("Playlist Library is corrupt, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
					delete pPlaylist;
					m_StandardPlaylists.RemoveAll();
					bOK = false;
					break;
				}

				IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.GetEntryByEntryID(ulEntryID);
				if(pIPE)
					myEntryArray.AddTail(pIPE);
			}


			pPlaylist->AddEntryArray(myEntryArray);
			
			if (PLDH.Version == TUNIAC_PLAYLISTLIBRARY_VERSION05 && SubHeader.FilterField == 29)
				pPlaylist->SetTextFilterField(30);
			else if(PLDH.Version == TUNIAC_PLAYLISTLIBRARY_VERSION04 && SubHeader.FilterField == 28)
				pPlaylist->SetTextFilterField(30);
			else
				pPlaylist->SetTextFilterField(SubHeader.FilterField);
			pPlaylist->SetTextFilterReversed(SubHeader.FilterReverse);
			SubHeader.Filter[127] = L'\0';
			pPlaylist->SetTextFilter(SubHeader.Filter);

			m_StandardPlaylists.AddTail(pPlaylist);
			if(PLDH.ActivePlaylist - 1 == ulPlaylist)
			{
				SetActivePlaylist(GetNumPlaylists() - 1);
				pPlaylist->ApplyFilter();
				if(pPlaylist->CheckFilteredIndex(PLDH.ActiveIndex))
				{
					if(!tuniacApp.PlayEntry(pPlaylist->GetEntryAtNormalFilteredIndex(PLDH.ActiveIndex), false, false))
						pPlaylist->SetActiveNormalFilteredIndex(0);
				}
				else
				{
					if(!tuniacApp.PlayEntry(pPlaylist->GetEntryAtNormalFilteredIndex(0), false, false))
						pPlaylist->SetActiveNormalFilteredIndex(0);
				}
			}
			else
				pPlaylist->ApplyFilter();
		}

		m_LibraryPlaylist.SetPlaylistName(PLDH.Name);

		if (PLDH.Version == TUNIAC_PLAYLISTLIBRARY_VERSION05 && PLDH.LibraryFilterField == 29)
			m_LibraryPlaylist.SetTextFilterField(30);
		else if(PLDH.Version == TUNIAC_PLAYLISTLIBRARY_VERSION04 && PLDH.LibraryFilterField == 28)
			m_LibraryPlaylist.SetTextFilterField(30);
		else
			m_LibraryPlaylist.SetTextFilterField(PLDH.LibraryFilterField);

		m_LibraryPlaylist.SetTextFilterReversed(PLDH.LibraryFilterReverse);
		PLDH.LibraryFilter[127] = L'\0';
		m_LibraryPlaylist.SetTextFilter(PLDH.LibraryFilter);
		
		if(PLDH.ActivePlaylist == 0)
		{
			SetActivePlaylist(0);
			m_LibraryPlaylist.ApplyFilter();
			if(m_LibraryPlaylist.CheckFilteredIndex(PLDH.ActiveIndex))
			{
				if(!tuniacApp.PlayEntry(m_LibraryPlaylist.GetEntryAtNormalFilteredIndex(PLDH.ActiveIndex), false, false))
					m_LibraryPlaylist.SetActiveNormalFilteredIndex(0);
			}
			else
			{
				if(!tuniacApp.PlayEntry(m_LibraryPlaylist.GetEntryAtNormalFilteredIndex(0), false, false))
				m_LibraryPlaylist.SetActiveNormalFilteredIndex(0);
			}
			//m_LibraryPlaylist.RebuildPlaylistArrays();
		}
		else
			m_LibraryPlaylist.ApplyFilter();
	}

	CloseHandle(hFile);

	//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist load complete"));

	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
	return bOK;
}

bool			CPlaylistManager::SavePlaylistLibrary(LPTSTR szLibraryFolder)
{
	PLDiskHeader		PLDH;
	HANDLE				hFile;
	unsigned long		ulBytesWritten;
	bool				bOK = true;
	TCHAR				szPlaylistPath[MAX_PATH];

	if (szLibraryFolder[0] == TEXT('\0'))
	{
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szLibraryFolder)))
		{
			PathAppend(szLibraryFolder, TEXT("\\Tuniac"));
		}
		else
		{
			return false;
		}
	}
	CreateDirectory(szLibraryFolder, 0);

	StringCbCopy(szPlaylistPath, MAX_PATH, szLibraryFolder);
	PathAppend(szPlaylistPath, TEXT("\\Playlists.dat"));

	hFile = CreateFile(szPlaylistPath,
					GENERIC_WRITE, 
					0,
					NULL,
					CREATE_ALWAYS,
					FILE_FLAG_SEQUENTIAL_SCAN,
					NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		// there is no playlist library!
		return false;
	}

	PLDH.Version		= TUNIAC_PLAYLISTLIBRARY_VERSION;
	PLDH.NumEntries		= m_StandardPlaylists.GetCount();
	PLDH.LibraryFilterField = m_LibraryPlaylist.GetTextFilterField();
	PLDH.LibraryFilterReverse = m_LibraryPlaylist.GetTextFilterReversed();
	StringCchCopy(PLDH.LibraryFilter, 128, m_LibraryPlaylist.GetTextFilter());
	StringCchCopy(PLDH.Name, 128, m_LibraryPlaylist.GetPlaylistName());
	if(m_ActivePlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
	{
		if(GetActivePlaylistIndex() == 0)
		{
			PLDH.ActivePlaylist = 0;
			PLDH.ActiveIndex = m_LibraryPlaylist.GetActiveNormalFilteredIndex();
		}
		else
		{
			PLDH.ActivePlaylist	= GetActivePlaylistIndex() - (GetNumPlaylists() - m_StandardPlaylists.GetCount() - 1);
			PLDH.ActiveIndex = m_ActivePlaylist->GetActiveNormalFilteredIndex();
		}
	}
	else
	{
		PLDH.ActivePlaylist = -1;
		PLDH.ActiveIndex = INVALID_PLAYLIST_INDEX;
	}
	WriteFile(hFile, &PLDH, sizeof(PLDH), &ulBytesWritten, NULL);
	if(ulBytesWritten != sizeof(PLDH))
	{
		if (tuniacApp.m_LogWindow)
		{
			tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Error saving Playlist Library header information."));
		}
		MessageBox(NULL, TEXT("Error saving Playlist Library header information."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{

		for(unsigned long playlist = 0; playlist < m_StandardPlaylists.GetCount(); playlist++)
		{
			PLDiskSubHeader SubHeader;
			ZeroMemory(&SubHeader, sizeof(PLDiskSubHeader));

			StringCchCopy(SubHeader.Name, 128, m_StandardPlaylists[playlist]->GetPlaylistName());
			StringCchCopy(SubHeader.Filter, 128, m_StandardPlaylists[playlist]->GetTextFilter());
			SubHeader.FilterField = m_StandardPlaylists[playlist]->GetTextFilterField();
			SubHeader.FilterReverse = m_StandardPlaylists[playlist]->GetTextFilterReversed();
			SubHeader.NumItems = m_StandardPlaylists[playlist]->GetRealCount();

			WriteFile(hFile, &SubHeader, sizeof(PLDiskSubHeader), &ulBytesWritten, NULL);
			if(ulBytesWritten != sizeof(PLDiskSubHeader))
			{
				if (tuniacApp.m_LogWindow)
				{
					tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Error saving Playlist Library."));
				}
				MessageBox(NULL, TEXT("Error saving Playlist Library."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
				break;
			}

			for(unsigned long x = 0; x < SubHeader.NumItems; x++)
			{
				unsigned long ulEntryID = m_StandardPlaylists[playlist]->GetEntryIDAtRealIndex(x);
				WriteFile(hFile, &ulEntryID, sizeof(unsigned long), &ulBytesWritten, NULL);
				if(ulBytesWritten != sizeof(unsigned long))
				{
					if (tuniacApp.m_LogWindow)
					{
						tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Error saving Playlist Library."));
					}
					MessageBox(NULL, TEXT("Error saving Playlist Library."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
					bOK = false;
					break;
				}
			}

			if(!bOK)
				break;

		}
	}

	CloseHandle(hFile);

	//tuniacApp.m_LogWindow->LogMessage(TEXT("PlaylistManager"), TEXT("Playlist save complete"));

	return bOK;

}

unsigned long	CPlaylistManager::GetNumPlaylists(void)
{
	return 1 + m_CDPlaylists.GetCount() + m_StandardPlaylists.GetCount();
}

IPlaylist *		CPlaylistManager::GetPlaylistAtIndex(unsigned long ulIndex)
{
	if(m_ulActivePlaylistIndex == ulIndex)
		return m_ActivePlaylist;

	if(ulIndex == INVALID_PLAYLIST_INDEX)
		return NULL;

	if(ulIndex == 0)
	{
		return & m_LibraryPlaylist;
/*
	}
	iIndex -=1;

	if(iIndex == 0)
	{
		return & m_RadioPlaylist;
*/
	}
	ulIndex -=1;

	if(ulIndex < m_CDPlaylists.GetCount())
	{
		// standard playlist;
		return m_CDPlaylists[ulIndex];
	}
	ulIndex -= m_CDPlaylists.GetCount();

	if(ulIndex < m_StandardPlaylists.GetCount())
	{
		// standard playlist;
		return m_StandardPlaylists[ulIndex];
	}

	return NULL;
}

IPlaylist * CPlaylistManager::GetActivePlaylist(void)
{
	return m_ActivePlaylist;
}

unsigned long CPlaylistManager::GetActivePlaylistIndex(void)
{
	return m_ulActivePlaylistIndex;
}

bool CPlaylistManager::SetActiveByEntry(IPlaylistEntry * pIPE)
{
	bool bOk= false;
	for (unsigned long i = 0; i < m_ActivePlaylist->GetNumItems(); i++)
	{
		if(m_ActivePlaylist->GetEntryAtNormalFilteredIndex(i) == pIPE)
		{
			bOk = m_ActivePlaylist->SetActiveNormalFilteredIndex(i);
		}
	}
	//not found in active, try ML instead
	if(!bOk)
	{
		tuniacApp.m_PlaylistManager.SetActivePlaylist(0);
		IPlaylist * pPlaylist = GetPlaylistAtIndex(0);
		if(pPlaylist)
		{
			for (unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
			{
				if(pPlaylist->GetEntryAtNormalFilteredIndex(i) == pIPE)
				{
					bOk = pPlaylist->SetActiveNormalFilteredIndex(i);
				}
			}
		}
	}
	//we failed to find our song (its filtered out?)
	return bOk;
}

bool CPlaylistManager::SetActivePlaylist(unsigned long ulPlaylistNumber)
{
	if(ulPlaylistNumber == m_ulActivePlaylistIndex)
		return true;

	if(ulPlaylistNumber == INVALID_PLAYLIST_INDEX)
		return true;

	tuniacApp.m_Queue.Clear();

	m_ulActivePlaylistIndex = ulPlaylistNumber;

	if(m_ActivePlaylist != NULL)
		m_ActivePlaylist->SetActiveFilteredIndex(INVALID_PLAYLIST_INDEX);

	if(ulPlaylistNumber == 0)
	{
		// medialibrary
		m_ActivePlaylist = &m_LibraryPlaylist;
		//return true;
	}
/*

	iPlaylistNumber -= 1;

	if(iPlaylistNumber == 0)
	{
		// medialibrary
		m_ActivePlaylist = &m_RadioPlaylist;
		return true;
*/
	ulPlaylistNumber -= 1;

	if(ulPlaylistNumber < m_CDPlaylists.GetCount())
	{
		// cd playlist;
		m_ActivePlaylist = m_CDPlaylists[ulPlaylistNumber];
		//return true;
	}
	ulPlaylistNumber -= m_CDPlaylists.GetCount();

	if(ulPlaylistNumber < m_StandardPlaylists.GetCount())
	{
		// standard playlist;
		m_ActivePlaylist = m_StandardPlaylists[ulPlaylistNumber];
		//return true;
	}

	if(tuniacApp.m_SourceSelectorWindow)
		tuniacApp.m_SourceSelectorWindow->UpdateList();

	return true;
}

bool CPlaylistManager::CreateNewStandardPlaylist(LPTSTR szName)
{
	CStandardPlaylist * pPlaylist = new CStandardPlaylist;
	if(pPlaylist)
	{
		pPlaylist->SetPlaylistName(szName);
		m_StandardPlaylists.AddTail(pPlaylist);
		PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
		return true;
	}

	return false;
}

bool CPlaylistManager::CreateNewStandardPlaylistWithIDs(LPTSTR szName, EntryArray & newEntries)
{
	CStandardPlaylist * pPlaylist = new CStandardPlaylist;

	if(pPlaylist)
	{
		pPlaylist->AddEntryArray(newEntries);
		pPlaylist->SetPlaylistName(szName);
		m_StandardPlaylists.AddTail(pPlaylist);
		PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
		return true;
	}

	return false;
}

bool CPlaylistManager::MoveStandardPlaylist(unsigned long ulIndex, unsigned long ulNewIndex)
{

	if(ulIndex == ulNewIndex)
		return false;

	if(1 + m_CDPlaylists.GetCount() > ulIndex)
		return false;

	unsigned long ulStdIndex = ulIndex - 1 - m_CDPlaylists.GetCount();
	if(ulStdIndex > m_StandardPlaylists.GetCount())
		return false;

	unsigned long ulStdNewIndex = ulNewIndex - 1 - m_CDPlaylists.GetCount();

	CStandardPlaylist * pSP = m_StandardPlaylists[ulStdIndex];
	m_StandardPlaylists.RemoveAt(ulStdIndex);
	
	if(ulStdNewIndex > m_StandardPlaylists.GetCount())
	{
		m_StandardPlaylists.AddTail(pSP);
	}
	else
	{
		if(ulStdNewIndex > ulNewIndex)
			m_StandardPlaylists.InsertBefore(ulStdNewIndex - 1, pSP);
		else
			m_StandardPlaylists.InsertBefore(ulStdNewIndex, pSP);
	}

	//moving active
	if(ulIndex == m_ulActivePlaylistIndex)
		m_ulActivePlaylistIndex = ulNewIndex;
	//old index is after the active and new index is before
	else if(ulIndex > m_ulActivePlaylistIndex && ulNewIndex <= m_ulActivePlaylistIndex)
		m_ulActivePlaylistIndex++;
	//old index is before the active and new is after
	else if(ulIndex < m_ulActivePlaylistIndex && ulNewIndex > m_ulActivePlaylistIndex)
		m_ulActivePlaylistIndex--;

	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
	return true;
}

bool CPlaylistManager::DeletePlaylistAtIndex(unsigned long ulPlaylistNumber)
{
	if(ulPlaylistNumber == 0)
		return false;

	if(ulPlaylistNumber == m_ulActivePlaylistIndex)
		SetActivePlaylist(0);

	else if(ulPlaylistNumber < m_ulActivePlaylistIndex)
		m_ulActivePlaylistIndex = INVALID_PLAYLIST_INDEX;

	ulPlaylistNumber-=1;

	if(ulPlaylistNumber < m_CDPlaylists.GetCount())
	{	// cd playlist playlist
		// maybe we can eject here?
		return true;
	}
	ulPlaylistNumber -= m_CDPlaylists.GetCount();

	if(ulPlaylistNumber < m_StandardPlaylists.GetCount())
	{	// standard playlist
		delete m_StandardPlaylists[ulPlaylistNumber];
		m_StandardPlaylists.RemoveAt(ulPlaylistNumber);
	}

	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
	return true;
}

unsigned long CPlaylistManager::PMThreadStub(void * in)
{
	CPlaylistManager * pPM = (CPlaylistManager *)in;

	return(pPM->PMThreadProc());
}

unsigned long CPlaylistManager::PMThreadProc(void)
{
	MSG					msg;
	bool				Done = false;

	HWND				hMessageWnd = NULL;
	WNDCLASSEX			wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProcStub;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= tuniacApp.getMainInstance();
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= TEXT("TUNIACDEVICENOTIFY");
	wcex.hIconSm		= NULL;

	if(!RegisterClassEx(&wcex))
		return(false);

	hMessageWnd = CreateWindow(	TEXT("TUNIACDEVICENOTIFY"), 
							TEXT("TUNIACDEVICENOTIFY"), 
							0,
							0, 
							0, 
							0, 
							0, 
							NULL, 
							NULL, 
							tuniacApp.getMainInstance(), 
							this);


	if(!hMessageWnd)
	{
		return(false);
	}


	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hMessageWnd);

	return 0;
}

LRESULT CALLBACK CPlaylistManager::WndProcStub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPlaylistManager * pVW;

	if(message == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pVW = (CPlaylistManager *)lpCS->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pVW);
	}
	else
	{
		pVW = (CPlaylistManager *)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return(pVW->WndProc(hWnd, message, wParam, lParam));
}

LRESULT CALLBACK CPlaylistManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

		case WM_DEVICECHANGE:
			{
				switch(wParam)
				{
					case DBT_DEVICEARRIVAL:
						{
							PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

							if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
							{
								PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

								for(char c = 'A'; c < 'Z'; c++)
								{
									if(DriveInMask(lpdbv->dbcv_unitmask, c))
									{
										TCHAR tstr[10];
										StringCchPrintf(tstr, 10, TEXT("%c:\\"), c);

										// is this drive a CD?
										if(GetDriveType(tstr) == DRIVE_CDROM)
										{
											DeleteCDWithDriveLetter(c);
											if(AddCDWithDriveLetter(c))
											{
												unsigned long VisItem = tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex();
												if(VisItem > (2 + m_CDPlaylists.GetCount()))
												{
													tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(VisItem+1);
												}

												tuniacApp.m_SourceSelectorWindow->UpdateList();
											}
										}
	
										// not a CD try something else...

									}
								}
							}
						}
						break;

					case DBT_DEVICEREMOVECOMPLETE:
						{
							PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

							if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
							{
								PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

								for(char c = 'A'; c < 'Z'; c++)
								{
									if(DriveInMask(lpdbv->dbcv_unitmask, c))
									{
										TCHAR tstr[10];
										StringCchPrintf(tstr, 10, TEXT("%c:\\"), c);

										// is this drive a CD?
										if(GetDriveType(tstr) == DRIVE_CDROM)
										{
											DeleteCDWithDriveLetter(c);
										}

										// its not a CD try something else
									}
								}
							}
						}
						break;
				}
			}
			break;

		default:
			return(DefWindowProc(hWnd, message, wParam, lParam));
			break;
	}

	return(0);
}

bool			CPlaylistManager::AddCDWithDriveLetter(char cDriveLetter)
{
	// create a new playlist for the device here!
	CAudioCDPlaylist * t = new CAudioCDPlaylist(cDriveLetter);

	if(t->GetNumItems())
	{
		m_CDPlaylists.AddTail(t);
		PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
		return true;
	}

	delete t;
	return false;
}

bool			CPlaylistManager::DeleteCDWithDriveLetter(char cDriveLetter)
{
	// scan playlists for drive letter here and delete
	for(unsigned long x=0; x<m_CDPlaylists.GetCount(); x++)
	{
		if(m_CDPlaylists[x]->GetDriveLetter() == cDriveLetter)
		{
			CAudioCDPlaylist * t = m_CDPlaylists[x];

			m_CDPlaylists.RemoveAt(x);

			/*
			unsigned long VisItem = tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex();
			if(VisItem > (1 + x))
			{
				tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(VisItem-1);
				SetActivePlaylist(VisItem-1);

			}
			*/

			tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(0);
			SetActivePlaylist(0);
			m_LibraryPlaylist.RebuildPlaylist();
			m_LibraryPlaylist.ApplyFilter();

			tuniacApp.m_SourceSelectorWindow->UpdateList();

			delete t;

			PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
			return true;
		}
	}

	return false;
}
