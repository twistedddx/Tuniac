#include "stdafx.h"
#include ".\mediamanager.h"

//sprintf(SQLString, "UPDATE MediaLibrary SET LastPlayed = DATETIME('NOW') WHERE EntryID = %d;", SongID);

// get length of media library SELECT SUM(SongLength) FROM MediaLibrary;

// select a bunch of ids
//SELECT * FROM table WHERE id IN (1,4,5,7);
//
// select * from medialibrary where title like '%stormy%' Order By Artist, Album, Track;

/*
[16:44] <mefisto> the way I'd store playlists would be something like:  
create table 'Playlists' ('id' INT PRIMARY KEY AUTO_INCREMENT, 'name' VARCHAR(50));  
create table Playlist_Entries('id' INT PRIMARY KEY AUTO_INCREMENT, 'playlist_id' INT, 'order' INT, 'song_id' INT);
*/

CMediaManager::CMediaManager(void)
{
}

CMediaManager::~CMediaManager(void)
{
}

bool CMediaManager::Initialize(void)
{
	String szURL;

	g_tuniacApp.getHelper().GetTuniacLibraryFilename(szURL);

	try
	{
		m_DBCon.open(szURL.c_str());

		int count = m_DBCon.executeint("select count(*) from sqlite_master;");
		if(count == 0)
		{
			// new database initialize it

			// remember this:
			// If you try to insert a NULL into an INTEGER PRIMARY KEY column, the column will actually be filled with a integer that is one greater than the largest key already in the table.

			// Entry ID							// PRIMARY KEY
			// Date Added
			// DirtyFlag						- BOOL
			// FileUnavailable Flag				- BOOL

			// Filename
			// Filesize
			// File last modified time
			// File Hash/CRC

			// Title
			// Artist
			// Album
			// Album Artist
			// Year
			// Genre
			// TrackNumber
			// MaxTracks
			// Disc Number
			// Max Disc
			// Comment
			// Rating

			// Playback Time
			// Playback Time Accuracy

			// Sample Rate
			// Channels
			// Bitrate

			// BPM

			// Last Played
			// Playcount

			String DBColumns[] =
			{
				TEXT("EntryID				 INTEGER PRIMARY KEY"),
				TEXT("DateAdded				 DATETIME"),
				TEXT("DirtyFlag				 INT"),
				TEXT("FileUnavailable		 INT"),

				TEXT("Filename				 TEXT"),
				TEXT("Filesize				 INT"),
				TEXT("FileModifiedTime		 DATETIME"),
				TEXT("Filehash				 BLOB"),

				TEXT("Title					 TEXT"),
				TEXT("Artist				 TEXT"),
				TEXT("DiscTitle				 TEXT"),
				TEXT("Composer				 TEXT"),
				TEXT("Album					 TEXT"),
				TEXT("AlbumArtist			 TEXT"),
				TEXT("Year					 INT"),
				TEXT("Genre					 TEXT"),

				TEXT("Track					 INT"),
				TEXT("MaxTrack				 INT"),

				TEXT("Disc					 INT"),
				TEXT("MaxDisc				 INT"),

				TEXT("Comment				 TEXT"),
				TEXT("Rating				 INT"),

				TEXT("PlaybackTime			 INT"),
				TEXT("PlaybackTimeAccuracy	 INT"),

				TEXT("SampleRate			 INT"),
				TEXT("Channels				 INT"),
				TEXT("Bitrate				 INT"),

				TEXT("BPM					 INT"),

				TEXT("FirstPlayed			 DATETIME"),
				TEXT("LastPlayed			 DATETIME"),
				TEXT("Playcount				 INT"),
				TEXT("")
			};


			m_DBCon.executenonquery("PRAGMA page_size=4096;");

			String DBCreateBase = TEXT("CREATE TABLE MediaLibrary (");
			
			int x=0;
			while(DBColumns[x].length())
			{
				DBCreateBase += DBColumns[x];

				if(DBColumns[x+1].length())
					DBCreateBase += TEXT(", ");

				x++;
			}
			DBCreateBase += TEXT(");");

			m_DBCon.executenonquery(DBCreateBase);
		}

		//return true;
	}
	catch(exception &ex) 
	{
//		String debugstring = ex.what();
//		OutputDebugString(debugstring.c_str());
		return false;
	}

	return true;
}

bool CMediaManager::Shutdown(void)
{
	m_DBCon.close();

	return true;
}

bool CMediaManager::ShowAddFolderSelector(HWND hWndParent)
{
	LPMALLOC lpMalloc;  // pointer to IMalloc

	if(::SHGetMalloc(&lpMalloc) == NOERROR)
	{
		TCHAR szBuffer[1024];

		BROWSEINFO browseInfo;
		LPITEMIDLIST lpItemIDList;

		browseInfo.hwndOwner		= hWndParent;
		browseInfo.pidlRoot			= NULL; 
		browseInfo.pszDisplayName	= NULL;
		browseInfo.lpszTitle		= TEXT("Select a directory...");   // passed in
		browseInfo.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_USENEWUI;   // also passed in
		browseInfo.lpfn				= NULL;      // not used
		browseInfo.lParam			= 0;      // not used   

		if((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
		{
			if(::SHGetPathFromIDList(lpItemIDList, szBuffer))
			{
				StringArray		array;
				// TODO: here we should build a fully recursed array of all the files we're going to add!
				g_tuniacApp.getHelper().GetFolderContents(szBuffer, array, true);

				// TODO: then add it!
				AddFileArray(array);
			}

			lpMalloc->Free(lpItemIDList);
		}

		lpMalloc->Release();

		return true;
	}

	return false;
}

bool CMediaManager::ShowAddFiles(HWND hWndParent)
{
#define OFNBUFFERSIZE		(32*1024)
	OPENFILENAME		ofn;
	TCHAR				szURLBuffer[OFNBUFFERSIZE];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ZeroMemory(szURLBuffer, OFNBUFFERSIZE * sizeof TCHAR);

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= hWndParent;
	ofn.hInstance			= g_tuniacApp.getHINSTANCE();
	ofn.lpstrFilter			= TEXT("All Files\0*.*\0");
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= szURLBuffer;
	ofn.nMaxFile			= OFNBUFFERSIZE;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= NULL;
	ofn.Flags				= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofn))
	{
		if(ofn.nFileOffset < lstrlen(szURLBuffer))
		{
			AddFile(szURLBuffer);
		}
		else
		{
			LPTSTR	szOFNName = &szURLBuffer[ofn.nFileOffset];
			vector<String> nameArray;

			String szFilePath = szURLBuffer;
			while( lstrlen(szOFNName) != 0 )
			{
				String filename = szFilePath;
				filename += TEXT("\\");
				filename += szOFNName;

				nameArray.push_back(filename);

				szOFNName = &szOFNName[lstrlen(szOFNName) + 1];
			}

			AddFileArray(nameArray);
		}

		return true;
	}

	return false;
}

bool CMediaManager::AddFile(String filename)
{
	String alreadyExistsSQL = TEXT("SELECT count(*) FROM MediaLibrary WHERE Filename = ?;");

	try
	{
		sqlite3_command existscmd(m_DBCon, alreadyExistsSQL);

		existscmd.bind(1, filename);

		if(existscmd.executeint() == 0)
		{
			SYSTEMTIME		stFileModifiedTime;
			unsigned long	dwFilesize;
			// file is not in the DB so add it!

			ITuniacInfoHandler	*	pHandler;
			unsigned long numHandlers = g_tuniacApp.getPluginManager().GetPluginCount(GUID_TUNIACINFOHANDLER);

			if(numHandlers == 0)
				return false;

			for(unsigned long x=0; x<numHandlers; x++)
			{
				pHandler = NULL;
				pHandler = (ITuniacInfoHandler *)g_tuniacApp.getPluginManager().GetPluginAtIndex(GUID_TUNIACINFOHANDLER, x);

				if(pHandler)
				{
					unsigned long t1, t2;
					if(pHandler->CanHandle((wchar_t*)filename.c_str(), &t1, &t2))
					{
						break;
					}
				}
			}

			if(!pHandler)
				return false;

			ITuniacInfoAccessor * pAccessor = pHandler->CreateAccessor((wchar_t*)filename.c_str());
			if(!pAccessor)
				return false;


			// extract generic info from the file (creation time/size)
			HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				FILETIME		ft;
				GetFileTime(hFile, NULL, NULL, &ft);
				FileTimeToSystemTime(&ft, &stFileModifiedTime);

				dwFilesize = GetFileSize(hFile, NULL);

				CloseHandle(hFile);
			}


			String insertFilenameSQL = TEXT("insert into MediaLibrary (DateAdded,					Filename,	Filesize,	FileModifiedTime,	Artist, Title,	Album,	Year,	Genre,	Track,	PlaybackTime,	SampleRate,	Channels,	Bitrate) \
														  values (DATETIME('now','localtime'),	?,			?,			?,					?,		?,		?,		?,		?,		?,		?,				?,			?,			?);");
			String fileModifiedTime;

			g_tuniacApp.getHelper().FormatSystemTime(fileModifiedTime, stFileModifiedTime);

			sqlite3_command cmd(m_DBCon, insertFilenameSQL);

			String artist, title, album, year, genre, track;
			__int64 playbacktime, samplerate, channels, bitrate;

			wchar_t		TempData[1024];

			pAccessor->GetTextField(Artist, TempData, 1024);
			artist = TempData;

			pAccessor->GetTextField(Title,	TempData, 1024);
			title = TempData;

			pAccessor->GetTextField(Album,	TempData, 1024);
			album = TempData;

			pAccessor->GetTextField(Year,	TempData, 1024);
			year = TempData;

			pAccessor->GetTextField(Genre,	TempData, 1024);
			genre = TempData;

			pAccessor->GetTextField(Track,	TempData, 1024);
			track = TempData;

			pAccessor->GetIntField(PlaybackTime,	&playbacktime);
			pAccessor->GetIntField(SampleRate,		&samplerate);
			pAccessor->GetIntField(Channels,		&channels); 
			pAccessor->GetIntField(Bitrate,			&bitrate);
			
			cmd.bind(1, filename);
			cmd.bind(2, (int)dwFilesize);
			cmd.bind(3, fileModifiedTime);
			cmd.bind(4, artist);
			cmd.bind(5, title);
			cmd.bind(6, album);
			cmd.bind(7, year);
			cmd.bind(8, genre);
			cmd.bind(9, track);
			cmd.bind(10, playbacktime);
			cmd.bind(11, samplerate);
			cmd.bind(12, channels);
			cmd.bind(13, bitrate);

			cmd.executenonquery();

			pAccessor->Destroy();

			return true;
		}
		else
		{
			return true;
		}
	}
	catch(exception &ex) 
	{
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}

	return false;
}

bool CMediaManager::AddFileArray(StringArray filenameArray)
{
	if(filenameArray.size())
	{
		sqlite3_transaction trans(m_DBCon);

		for(unsigned int i=0; i<filenameArray.size(); i++)
		{
			AddFile(filenameArray.at(i));
		}

		trans.commit();

		return true;
	}

	return false;
}
