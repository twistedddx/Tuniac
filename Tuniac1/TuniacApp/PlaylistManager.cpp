#include "stdafx.h"
#include ".\playlistmanager.h"

// only increment this when a change becomes incompatable with older versions!
#define TUNIAC_PLAYLISTLIBRARY_VERSION		MAKELONG(0, 2)



bool DriveInMask(ULONG uMask, char Letter)
{
	unsigned long x = 1;
	unsigned long letterindex = toupper(Letter) - 'A';

	for(int t=0; t<letterindex; t++)
	{
		x <<= 1;
	}

	if(uMask & x)
		return true;

	return false;
}

CPlaylistManager::CPlaylistManager(void) :
	m_ActivePlaylist(NULL)
{
}

CPlaylistManager::~CPlaylistManager(void)
{
}

bool CPlaylistManager::Initialize(void)
{
	unsigned long DrivesMask = GetLogicalDrives();

	for(char c = 'A'; c<='Z'; c++)
	{
		if(DriveInMask(DrivesMask, c))
		{
			TCHAR tstr[10];
			wsprintf(tstr, TEXT("%c:\\"), c);

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

	SetActivePlaylist(0);
	m_LibraryPlaylist.RebuildPlaylist();
	LoadPlaylistLibrary();
	return true;
}

bool CPlaylistManager::Shutdown(bool bSave)
{
	if(bSave)
		SavePlaylistLibrary();

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

bool			CPlaylistManager::LoadPlaylistLibrary(void)		
{
	PLDiskHeader		PLDH;
	TCHAR				szFilename[MAX_PATH];
	HANDLE				hFile;
	unsigned long		ulBytesRead;
	bool				bOK = true;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szFilename ) ) )
	{
		PathAppend( szFilename, TEXT("\\Tuniac\\Playlists.dat") );


		hFile = CreateFile(	szFilename,
						GENERIC_READ, 
						0,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN,
						NULL);

		if(hFile == INVALID_HANDLE_VALUE)
			//no saved playlists
			return false;
	}
	else{
		//cant get appdata path
	}

	ReadFile(hFile, &PLDH, sizeof(PLDH), &ulBytesRead, NULL);

	if(ulBytesRead != sizeof(PLDH))
	{
		MessageBox(NULL, TEXT("Playlist Library is corrupt, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
	}
	else if(PLDH.Version != TUNIAC_PLAYLISTLIBRARY_VERSION)
	{
		MessageBox(NULL, TEXT("Playlist Library is saved in an incompatable version, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{

		for(unsigned long iPlaylist = 0; iPlaylist < PLDH.NumEntries; iPlaylist++)
		{
			PLDiskSubHeader SubHeader;
			EntryArray		myEntryArray;
			unsigned long	ulEntryID;

			CStandardPlaylist * pPlaylist = new CStandardPlaylist;

			ReadFile(hFile, &SubHeader, sizeof(PLDiskSubHeader), &ulBytesRead, NULL);
			if(ulBytesRead != sizeof(PLDiskSubHeader))
			{
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
					MessageBox(NULL, TEXT("Playlist Library is corrupt, resetting playlists."), TEXT("Startup Error"), MB_OK | MB_ICONWARNING);
					bOK = false;
					break;
				}

				IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.GetItemByID(ulEntryID);
				if(pIPE)
				{
					myEntryArray.AddTail(pIPE);
				}
			}
			if(!bOK)
			{
				delete pPlaylist;
				m_StandardPlaylists.RemoveAll();
				break;
			}

			pPlaylist->AddEntryArray(myEntryArray);
			
			pPlaylist->SetTextFilterField(SubHeader.FilterField);
			pPlaylist->SetTextFilterReversed(SubHeader.FilterReverse);
			SubHeader.Filter[127] = L'\0';
			pPlaylist->SetTextFilter(SubHeader.Filter);

			m_StandardPlaylists.AddTail(pPlaylist);
			if(PLDH.ActivePlaylist - 1 == iPlaylist)
			{
				SetActivePlaylist(GetNumPlaylists() - 1);
				pPlaylist->SetActiveIndex(PLDH.ActiveIndex);
				if(pPlaylist->GetActiveIndex() != INVALID_PLAYLIST_INDEX)
					tuniacApp.m_CoreAudio.SetSource(pPlaylist->GetActiveItem());
			}
		}

		if(PLDH.ActivePlaylist == 0)
		{
			SetActivePlaylist(0);
			m_LibraryPlaylist.SetActiveIndex(PLDH.ActiveIndex);
			if(m_LibraryPlaylist.GetActiveIndex() != INVALID_PLAYLIST_INDEX && (unsigned long)m_LibraryPlaylist.GetActiveItem()->GetField(FIELD_KIND) != ENTRY_KIND_URL)
				tuniacApp.m_CoreAudio.SetSource(m_LibraryPlaylist.GetActiveItem());
			
		}
	}

	CloseHandle(hFile);

	m_LibraryPlaylist.SetTextFilterField(PLDH.LibraryFilterField);
	m_LibraryPlaylist.SetTextFilterReversed(PLDH.LibraryFilterReverse);
	PLDH.LibraryFilter[127] = L'\0';
	m_LibraryPlaylist.SetTextFilter(PLDH.LibraryFilter);
	
	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
	return bOK;
}

bool			CPlaylistManager::SavePlaylistLibrary(void)
{
	PLDiskHeader		PLDH;
	TCHAR				szFilename[MAX_PATH];
	HANDLE				hFile;
	unsigned long		ulBytesWritten;
	bool				bOK = true;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szFilename ) ) )
	{
		PathAppend( szFilename, TEXT("\\Tuniac\\Playlists.dat") );

		hFile = CreateFile(	szFilename,
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
	}
	else
	{
		//cant get appdata path
	}

	PLDH.Version		= TUNIAC_PLAYLISTLIBRARY_VERSION;
	PLDH.NumEntries		= m_StandardPlaylists.GetCount();
	PLDH.LibraryFilterField = m_LibraryPlaylist.GetTextFilterField();
	PLDH.LibraryFilterReverse = m_LibraryPlaylist.GetTextFilterReversed();
	StrCpyN(PLDH.LibraryFilter, m_LibraryPlaylist.GetTextFilter(), 128);
	if(m_ActivePlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
	{
		if(GetActivePlaylistIndex() == 0)
		{
			PLDH.ActivePlaylist = 0;
			PLDH.ActiveIndex = m_LibraryPlaylist.GetActiveIndex();
		}
		else
		{
			PLDH.ActivePlaylist	= GetActivePlaylistIndex() - (GetNumPlaylists() - m_StandardPlaylists.GetCount() - 1);
			PLDH.ActiveIndex = ((IPlaylistEX *)m_ActivePlaylist)->GetActiveIndex();
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
		MessageBox(NULL, TEXT("Error saving Playlist Library header information."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
		bOK = false;
	}
	else
	{

		for(unsigned long playlist = 0; playlist < m_StandardPlaylists.GetCount(); playlist++)
		{
			PLDiskSubHeader SubHeader;
			ZeroMemory(&SubHeader, sizeof(PLDiskSubHeader));

			StrCpyN(SubHeader.Name, m_StandardPlaylists[playlist]->GetPlaylistName(), 128);
			StrCpyN(SubHeader.Filter, m_StandardPlaylists[playlist]->GetTextFilter(), 128);
			SubHeader.FilterField = m_StandardPlaylists[playlist]->GetTextFilterField();
			SubHeader.FilterReverse = m_StandardPlaylists[playlist]->GetTextFilterReversed();
			SubHeader.NumItems = m_StandardPlaylists[playlist]->GetRealCount();

			WriteFile(hFile, &SubHeader, sizeof(PLDiskSubHeader), &ulBytesWritten, NULL);
			if(ulBytesWritten != sizeof(PLDiskSubHeader))
			{
				MessageBox(NULL, TEXT("Error saving Playlist Library."), TEXT("Save Error"), MB_OK | MB_ICONWARNING);
				bOK = false;
				break;
			}

			for(unsigned long x = 0; x < SubHeader.NumItems; x++)
			{
				unsigned long ulEntryID = m_StandardPlaylists[playlist]->GetIDAtRealIndex(x);
				WriteFile(hFile, &ulEntryID, sizeof(unsigned long), &ulBytesWritten, NULL);
				if(ulBytesWritten != sizeof(unsigned long))
				{
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
	return bOK;

}

unsigned long	CPlaylistManager::GetNumPlaylists(void)
{
	return 1 + m_CDPlaylists.GetCount() + m_StandardPlaylists.GetCount();
}

IPlaylist *		CPlaylistManager::GetPlaylistAtIndex(int iIndex)
{
	if(iIndex == 0)
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
	iIndex -=1;

	if(iIndex < m_CDPlaylists.GetCount())
	{
		// standard playlist;
		return m_CDPlaylists[iIndex];
	}
	iIndex -= m_CDPlaylists.GetCount();

	if(iIndex < m_StandardPlaylists.GetCount())
	{
		// standard playlist;
		return m_StandardPlaylists[iIndex];
	}

	return NULL;
}

IPlaylist * CPlaylistManager::GetActivePlaylist(void)
{
	return m_ActivePlaylist;
}

int	CPlaylistManager::GetActivePlaylistIndex(void)
{
	for(int x=0; x<GetNumPlaylists(); x++)
	{
		if(GetActivePlaylist() == GetPlaylistAtIndex(x))
			return x;
	}

	return -1;
}

bool CPlaylistManager::SetActiveByEntry(IPlaylistEntry * pEntry)
{
	if(m_ActivePlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
	{
		IPlaylistEX * pPlaylist = (IPlaylistEX *)m_ActivePlaylist;
		for (unsigned long i = 0; i < pPlaylist->GetNumItems(); i++)
		{
			if(pPlaylist->GetItemAtIndex(i) == pEntry)
			{
				pPlaylist->SetActiveIndex(i);
				return true;
			}
		}
	}
	tuniacApp.m_PlaylistManager.SetActivePlaylist(0);
	IPlaylistEX * pPlaylistEX = (IPlaylistEX *)GetPlaylistAtIndex(0);
	for (unsigned long i = 0; i < pPlaylistEX->GetNumItems(); i++)
	{
		if(pPlaylistEX->GetItemAtIndex(i) == pEntry)
		{
			pPlaylistEX->SetActiveIndex(i);
			return true;
		}
	}

	//we failed to find our song (its filtered out?)
	return false;
}

bool CPlaylistManager::SetActivePlaylist(int iPlaylistNumber)
{
	int iOldIndex = GetActivePlaylistIndex();
	if(iPlaylistNumber == iOldIndex)
		return true;

	if(m_ActivePlaylist != NULL && m_ActivePlaylist->GetFlags() & PLAYLIST_FLAGS_EXTENDED)
	{
		((IPlaylistEX *)m_ActivePlaylist)->SetActiveIndex(INVALID_PLAYLIST_INDEX);
	}

	if(iPlaylistNumber == 0)
	{
		// medialibrary
		m_ActivePlaylist = &m_LibraryPlaylist;
/*
		return true;
	}
	iPlaylistNumber -= 1;

	if(iPlaylistNumber == 0)
	{
		// medialibrary
		m_ActivePlaylist = &m_RadioPlaylist;
		return true;
*/
	}
	iPlaylistNumber -= 1;

	if(iPlaylistNumber < m_CDPlaylists.GetCount())
	{
		// cd playlist;
		m_ActivePlaylist = m_CDPlaylists[iPlaylistNumber];
		//return true;
	}
	iPlaylistNumber -= m_CDPlaylists.GetCount();

	if(iPlaylistNumber < m_StandardPlaylists.GetCount())
	{
		// standard playlist;
		m_ActivePlaylist = m_StandardPlaylists[iPlaylistNumber];
		//return true;
	}
	iPlaylistNumber -= m_StandardPlaylists.GetCount();


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

	PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
	return true;
}

bool CPlaylistManager::DeletePlaylistAtIndex(unsigned long iPlaylistNumber)
{
	if(iPlaylistNumber == 0)
		return false;

	iPlaylistNumber-=1;

	if(iPlaylistNumber < m_CDPlaylists.GetCount())
	{	// cd playlist playlist
		
		// maybe we can eject here?

		SetActivePlaylist(0);
		return true;
	}
	iPlaylistNumber -= m_CDPlaylists.GetCount();

	if(iPlaylistNumber < m_StandardPlaylists.GetCount())
	{	// standard playlist
		if(m_StandardPlaylists[iPlaylistNumber] == GetActivePlaylist())
		{
			SetActivePlaylist(0);
		}
		delete m_StandardPlaylists[iPlaylistNumber];
		m_StandardPlaylists.RemoveAt(iPlaylistNumber);
	}
	iPlaylistNumber -= m_StandardPlaylists.GetCount();

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
										wsprintf(tstr, TEXT("%c:\\"), c);

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
										wsprintf(tstr, TEXT("%c:\\"), c);

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

	if(t->GetNumCDTracks())
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

			unsigned long VisItem = tuniacApp.m_SourceSelectorWindow->GetVisiblePlaylistIndex();
			if(VisItem > (1 + x))
			{
				tuniacApp.m_SourceSelectorWindow->ShowPlaylistAtIndex(VisItem-1);
			}

			tuniacApp.m_SourceSelectorWindow->UpdateList();

			delete t;

			PostMessage(tuniacApp.getMainWindow(), WM_APP, NOTIFY_PLAYLISTSCHANGED, 0);
			return true;
		}
	}

	return false;
}