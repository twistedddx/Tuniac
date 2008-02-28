#include "stdafx.h"
#include ".\mediamanager.h"

//sprintf(SQLString, "UPDATE MediaLibrary SET LastPlayed = DATETIME('NOW') WHERE EntryID = %d;", SongID);

// get length of media library SELECT SUM(SongLength) FROM MediaLibrary;

// select a bunch of ids
//SELECT * FROM table WHERE id IN (1,4,5,7);
//
// select * from medialibrary where title like '%stormy%' Order By Artist, Album, Track;

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
		m_DBCon.open(szURL);

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
			// Year
			// Genre
			// TrackNumber
			// MaxTracks
			// Disc Number
			// Max Disc
			// Comment
			// Rating

			// Playback Time
			// Sample Rate
			// Channels
			// Bitrate

			// Last Played
			// Playcount

			String DBColumns[] =
			{
				"EntryID			 INTEGER PRIMARY KEY",
				"DateAdded			 DATETIME",

				"Filename			 TEXT",
				"Filesize			 INT",
				"FileModifiedTime	 DATETIME",
				"Filehash			 BLOB",

				"Title				 TEXT",
				"Artist				 TEXT",
				"DiscTitle			 TEXT",
				"Composer			 TEXT",
				"Album				 TEXT",
				"Year				 INT",
				"Genre				 TEXT",
				"Track				 INT",

				"MaxTrack			 INT",
				"Disc				 INT",
				"MaxDisc			 INT",
				"Comment			 TEXT",
				"Rating				 INT",

				"PlaybackTime		 INT",
				"SampleRate			 INT",
				"Channels			 INT",
				"Bitrate			 INT",

				"FirstPlayed		 DATETIME",
				"LastPlayed			 DATETIME",
				"Playcount			 INT",
				""
			};


			m_DBCon.executenonquery("PRAGMA page_size=4096;");

			String DBCreateBase = "CREATE TABLE MediaLibrary (";
			
			int x=0;
			while(DBColumns[x].length())
			{
				DBCreateBase += DBColumns[x];

				if(DBColumns[x+1].length())
					DBCreateBase += ", ";

				x++;
			}
			DBCreateBase += ");";

			m_DBCon.executenonquery(DBCreateBase);
		}

		//return true;
	}
	catch(exception &ex) 
	{
		String debugstring = ex.what();
		OutputDebugString(debugstring);
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
	String alreadyExis tsSQL = "SELECT count(*) FROM MediaLibrary WHERE Filename = ?;";

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
					if(pHandler->CanHandle(filename, &t1, &t2))
					{
						break;
					}
				}
			}

			if(!pHandler)
				return false;

			ITuniacInfoAccessor * pAccessor = pHandler->CreateAccessor(filename);
			if(!pAccessor)
				return false;


			// extract generic info from the file (creation time/size)
			HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				FILETIME		ft;
				GetFileTime(hFile, NULL, NULL, &ft);
				FileTimeToSystemTime(&ft, &stFileModifiedTime);

				dwFilesize = GetFileSize(hFile, NULL);

				CloseHandle(hFile);
			}


			String insertFilenameSQL = "insert into MediaLibrary (DateAdded,					Filename,	Filesize,	FileModifiedTime,	Artist, Title,	Album,	Year,	Genre,	Track,	PlaybackTime,	SampleRate,	Channels,	Bitrate) \
														  values (DATETIME('now','localtime'),	?,			?,			?,					?,		?,		?,		?,		?,		?,		?,				?,			?,			?);";
			String fileModifiedTime;

			g_tuniacApp.getHelper().FormatSystemTime(fileModifiedTime, stFileModifiedTime);

			sqlite3_command cmd(m_DBCon, insertFilenameSQL);

			String artist, title, album, year, genre, track;
			String playbacktime, samplerate, channels, bitrate;

			pAccessor->GetField(Artist, artist);
			pAccessor->GetField(Title,	title);
			pAccessor->GetField(Album,	album);
			pAccessor->GetField(Year,	year);
			pAccessor->GetField(Genre,	genre);
			pAccessor->GetField(Track,	track);

			pAccessor->GetField(PlaybackTime,	playbacktime);
			pAccessor->GetField(SampleRate,		samplerate);
			pAccessor->GetField(Channels,		channels); 
			pAccessor->GetField(Bitrate,		bitrate);
			
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
		OutputDebugString(debugstring+"\n");
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
