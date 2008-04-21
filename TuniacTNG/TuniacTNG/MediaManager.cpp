#include "stdafx.h"
#include "MediaManager.h"

#include "TuniacHelper.h"

CMediaManager::CMediaManager(void)
{
}

CMediaManager::~CMediaManager(void)
{
}

bool CMediaManager::Initialize(void)
{
	String szDBName;
	GetMediaDBLocation(szDBName);
	try
	{
		sqlite3x::sqlite3_connection con(szDBName);

		int count = con.executeint("select count(*) from sqlite_master;");

		if(count == 0)
		{
			// THERE IS NO MEDIA LIBRARY :(

			String DBColumns[] =
			{
				TEXT("EntryID				 INTEGER PRIMARY KEY"),
				TEXT("DirtyFlag				 INT"),

				TEXT("DateAdded				 DATETIME"),
				TEXT("FileUnavailable		 INT"),

				TEXT("Filename				 TEXT"),
				TEXT("Filesize				 INT"),
				TEXT("FileModifiedTime		 DATETIME"),

				TEXT("Title					 TEXT"),
				TEXT("Artist				 TEXT"),
				TEXT("DiscTitle				 TEXT"),
				TEXT("Album					 TEXT"),
				TEXT("AlbumArtist			 TEXT"),
				TEXT("Composer				 TEXT"),
				TEXT("Year					 INT"),
				TEXT("Genre					 TEXT"),
				TEXT("Comment				 TEXT"),

				TEXT("Track					 INT"),
				TEXT("MaxTrack				 INT"),

				TEXT("Disc					 INT"),
				TEXT("MaxDisc				 INT"),

				TEXT("Rating				 INT"),
				TEXT("BPM					 INT"),

				TEXT("PlaybackTime			 INT"),
				TEXT("PlaybackTimeAccuracy	 INT"),

				TEXT("SampleRate			 INT"),
				TEXT("Channels				 INT"),
				TEXT("Bitrate				 INT"),

				TEXT("FirstPlayed			 DATETIME"),
				TEXT("LastPlayed			 DATETIME"),
				TEXT("Playcount				 INT"),

				TEXT("ReplayGainTrack		 INT"),
				TEXT("ReplayPeakTrack		 REAL"),
				TEXT("ReplayGainAlbum		 INT"),
				TEXT("ReplayPeakalbum		 REAL"),

				TEXT("EncoderDelay			 INT"),
				TEXT("EncoderPadding		 INT"),

				TEXT("")
			};

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

			con.executenonquery(DBCreateBase);
		}
	}
	catch(std::exception &ex)
	{
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}

	String runFolder;
	CTuniacHelper::Instance()->GetTuniacRunFolder(runFolder);

	StringArray		dllFileArray;
	CTuniacHelper::Instance()->GetAllFilesInFolderWithExtension(runFolder, TEXT(".DLL"), dllFileArray);

	while(dllFileArray.size())
	{
		InfoHandler		ih;

		ih.hDLL = LoadLibrary(dllFileArray.back());

		if(ih.hDLL)
		{
			CreateInfoHandlerFunc pFunc = (CreateInfoHandlerFunc)GetProcAddress(ih.hDLL, "CreateInfoHandler");

			if(pFunc)
			{
				ih.pInfoHandler = pFunc();
				if(ih.pInfoHandler)
				{
					m_vInfoHandlers.push_back(ih);
				}
				else
				{
					FreeLibrary(ih.hDLL);
				}
			}
			else
			{
				FreeLibrary(ih.hDLL);
			}
		}

		dllFileArray.pop_back();
	}
	return true;
}

bool CMediaManager::Shutdown(void)
{
	return true;
}

bool CMediaManager::GetMediaDBLocation(String & strPath)
{
	TCHAR szSpecialPath[MAX_PATH];

	SHGetFolderPath(NULL,
					CSIDL_MYMUSIC,
					NULL, 
					0, 
					szSpecialPath);

	strPath = szSpecialPath;

	if(strPath.Right(1) != TEXT("\\"))
		strPath+=TEXT("\\");
		
	strPath+=TEXT("TuniacMediaLibrary.db");
	
	return true;
}

unsigned __int64 CMediaManager::GetNumEntries(void)
{
	unsigned __int64 retval = 0;
	String szDBName;
	GetMediaDBLocation(szDBName);
	try
	{
		sqlite3x::sqlite3_connection con(szDBName);

		retval = con.executeint64("SELECT count(*) from MediaLibrary");

	}
	catch(std::exception &ex)
	{
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
	}

	return retval;
}

bool CMediaManager::PopulateMediaItemFromAccessor(String filename, MediaItem & mediaItem)
{
	TCHAR			temp[1024];
	__int64			tempint;

	mediaItem.filename = filename;


	// first lets see if we have an infohandler capable of using this
	IInfoHandler		* pHandler = NULL;

	for(unsigned int x=0; x<m_vInfoHandlers.size(); x++)
	{
		unsigned long Ability, Merit;
		if(m_vInfoHandlers[x].pInfoHandler->CanHandle((wchar_t *)filename.c_str(), &Ability, &Merit))
		{
			pHandler = m_vInfoHandlers[x].pInfoHandler;
			break;
		}
	}
	
	// dont bother if we can't get the info for it!
	if(pHandler == NULL)
		return false;

	IInfoAccessor * pAccessor = pHandler->CreateAccessor((wchar_t *)filename.c_str());
	if(!pAccessor)
		return false;

	pAccessor->GetTextField(Title, temp, 1024);
	mediaItem.title				= temp;	

	pAccessor->GetTextField(Artist, temp, 1024);
	mediaItem.artist			= temp;	

	pAccessor->GetTextField(Album, temp, 1024);
	mediaItem.album				= temp;	

	pAccessor->GetTextField(Composer, temp, 1024);
	mediaItem.composer			= temp;	

	pAccessor->GetTextField(Genre, temp, 1024);
	mediaItem.genre				= temp;	

	pAccessor->GetTextField(Comment, temp, 1024);
	mediaItem.comment			= temp;	

	pAccessor->GetIntField(Year,			&mediaItem.ulYear);

	pAccessor->GetIntField(Track,			&mediaItem.ulTrack);
	pAccessor->GetIntField(MaxTrack,		&mediaItem.ulMaxTrack);

	pAccessor->GetIntField(Disc,			&mediaItem.ulDisk);
	pAccessor->GetIntField(MaxDisc,			&mediaItem.ulMaxDisk);

	tempint = 0;
	mediaItem.ulRating			= tempint;
	mediaItem.ulBPM				= tempint;

	pAccessor->GetIntField(PlaybackTime,	&mediaItem.ulPlayTimeMS);

	pAccessor->GetIntField(SampleRate,		&mediaItem.ulSampleRate);
	pAccessor->GetIntField(Channels,		&mediaItem.ulChannelCount);
	pAccessor->GetIntField(Bitrate,			&mediaItem.ulBitRate);

	if(pAccessor)
		pAccessor->Destroy();

	// extract generic info from the file (creation time/size)
	HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME		ft;
		GetFileTime(hFile, NULL, NULL, &ft);
		FileTimeToSystemTime(&ft, &mediaItem.fileModifiedTime);
		mediaItem.ulFilesize = GetFileSize(hFile, NULL);

		CloseHandle(hFile);
	}
	
	return true;
}

bool CMediaManager::GetAlbums(StringArray & albumList)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	albumList.clear();

	try
	{
		String sGetAlbumSQL = TEXT("SELECT DISTINCT Album FROM MediaLibrary;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command albumcmd(con, sGetAlbumSQL);

		sqlite3x::sqlite3_reader reader = albumcmd.executereader();

		while(reader.read())
		{
			//existscmd.
			String temp = reader.getstring(0);

			albumList.push_back(temp);
		}

		reader.close();
	}
	catch(std::exception &ex) 
	{
		//LOGDEBUG("Exception Occured: " << ex.what());
	}

	return true;
}

bool CMediaManager::GetArtists(StringArray & artistList)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	artistList.clear();

	try
	{
		String sGetAlbumSQL = TEXT("SELECT DISTINCT Artist FROM MediaLibrary;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command albumcmd(con, sGetAlbumSQL);

		sqlite3x::sqlite3_reader reader = albumcmd.executereader();

		while(reader.read())
		{
			//existscmd.
			String temp = reader.getstring(0);

			artistList.push_back(temp);
		}

		reader.close();
	}
	catch(std::exception &ex) 
	{
		//LOGDEBUG("Exception Occured: " << ex.what());
	}

	return true;
}

bool CMediaManager::GetRange(unsigned long ulStart, unsigned long ulCount, MediaItemList & itemList)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	itemList.clear();

	try
	{
		String sGetRangeSQL = TEXT("SELECT * FROM MediaLibrary LIMIT ?, ?;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command rangecmd(con, sGetRangeSQL);

		rangecmd.bind(1, (int)ulStart);
		rangecmd.bind(2, (int)ulCount);

		sqlite3x::sqlite3_reader reader = rangecmd.executereader();

		while(reader.read())
		{
			//existscmd.
			MediaItem tempItem = {0};

			tempItem.ulItemID		= reader.getint64(0);

			tempItem.filename		= reader.getstring16(4);
			tempItem.ulFilesize		= reader.getint64(5);

			tempItem.title			= reader.getstring16(7);
			tempItem.artist			= reader.getstring16(8);

			tempItem.album			= reader.getstring16(10);

			tempItem.ulYear			= reader.getint64(13);
			tempItem.genre			= reader.getstring16(14);
			tempItem.comment		= reader.getstring16(15);

			tempItem.ulTrack		= reader.getint64(16);
			tempItem.ulMaxTrack		= reader.getint64(17);

			tempItem.ulDisk			= reader.getint64(18);
			tempItem.ulMaxDisk		= reader.getint64(19);

			tempItem.ulPlayTimeMS	= reader.getint64(22);

			tempItem.ulSampleRate	= reader.getint64(24);
			tempItem.ulChannelCount	= reader.getint64(25);
			tempItem.ulBitRate		= reader.getint64(26);

			itemList.push_back(tempItem);
		}

		reader.close();
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool CMediaManager::InsertItemToMediaLibraryUsingConnection(sqlite3x::sqlite3_connection & con, MediaItem & newItem)
{
	try
	{
		String insertFilenameSQL = TEXT("INSERT INTO  MediaLibrary (DateAdded,						Filename,	Filesize,	FileModifiedTime,	Title,	Artist,	Composer,	Album,	Year,	Genre,	Comment,	Track,	MaxTrack,	Disc,	MaxDisc,	PlaybackTime,	SampleRate, Channels,	Bitrate) \
															VALUES (datetime('now','localtime'),	?,			?,			?,					?,		?,		?,			?,		?,		?,		?,			?,		?,			?,		?,			?,				?,			?,			?)");

		sqlite3x::sqlite3_command cmd(con, insertFilenameSQL);

		int bind=1;
		cmd.bind(bind++,	newItem.filename);
		cmd.bind(bind++,	(long long)newItem.ulFilesize);

		String modifiedtime;
		CTuniacHelper::Instance()->FormatSystemTime(modifiedtime, newItem.fileModifiedTime);
		cmd.bind(bind++,	modifiedtime);

		cmd.bind(bind++,	newItem.title);
		cmd.bind(bind++,	newItem.artist);
		cmd.bind(bind++,	newItem.composer);
		cmd.bind(bind++,	newItem.album);
		cmd.bind(bind++,	newItem.ulYear);
		cmd.bind(bind++,	newItem.genre);
		cmd.bind(bind++,	newItem.comment);
		cmd.bind(bind++,	newItem.ulTrack);
		cmd.bind(bind++,	newItem.ulMaxTrack);
		cmd.bind(bind++,	newItem.ulDisk);
		cmd.bind(bind++,	newItem.ulMaxDisk);
		cmd.bind(bind++,	newItem.ulPlayTimeMS);
		cmd.bind(bind++,	newItem.ulSampleRate);
		cmd.bind(bind++,	newItem.ulChannelCount);
		cmd.bind(bind++,	newItem.ulBitRate);

		cmd.executenonquery();

		return true;
	}
	catch(std::exception &ex)
	{
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}
}

bool CMediaManager::IsFileInLibrary(sqlite3x::sqlite3_connection & con, String filename)
{
	try
	{
		String alreadyExistsSQL = TEXT("SELECT count(*) FROM MediaLibrary WHERE Filename = ?;");

		sqlite3x::sqlite3_command existscmd(con, alreadyExistsSQL);
		existscmd.bind(1, filename);
		if(existscmd.executeint() > 0)
			return true;
	}
	catch(std::exception &ex)
	{
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}

	return false;
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
				StringArray		tempArray;

				// TODO: here we should build a fully recursed array of all the files we're going to add!
				CTuniacHelper::Instance()->GetFolderContents(szBuffer, tempArray, true);

				// TODO: then add it!
				AddFileArray(tempArray);
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
	ofn.hInstance			= (HINSTANCE)CTuniacHelper::Instance();
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
			StringArray		nameArray;

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
	String szDBName;
	GetMediaDBLocation(szDBName);


	sqlite3x::sqlite3_connection con(szDBName);
	if(IsFileInLibrary(con, filename))
		return true;


	MediaItem	newItem = {0,};
	if(!PopulateMediaItemFromAccessor(filename, newItem))
		return false;

	if(InsertItemToMediaLibraryUsingConnection(con, newItem))
		return true;

	return false;
}

bool CMediaManager::AddFileArray(StringArray filenameArray)
{
	if(filenameArray.size())
	{
		String szDBName;
		GetMediaDBLocation(szDBName);

		sqlite3x::sqlite3_connection con(szDBName);

		for(unsigned int i=0; i<filenameArray.size(); i++)
		{
			if(IsFileInLibrary(con, filenameArray[i]))
			{
				filenameArray.erase(filenameArray.begin()+i);
				i--;
			}
		}

		sqlite3x::sqlite3_transaction trans(con);
		for(unsigned int i=0; i<filenameArray.size(); i++)
		{
			MediaItem	newItem = {0,};
			if(PopulateMediaItemFromAccessor(filenameArray[i], newItem))
			{
				InsertItemToMediaLibraryUsingConnection(con, newItem);
			}
		}
		trans.commit();

		return true;
	}

	return false;
}

bool	CMediaManager::DeleteByID(__int64 ullID)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	try
	{
		String sDeleteEntrySQL = TEXT("DELETE FROM MediaLibrary WHERE EntryID = ?;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command deletecmd(con, sDeleteEntrySQL);

		deletecmd.bind(1, ullID);

		deletecmd.executenonquery();
	}
	catch(std::exception &ex) 
	{
		//LOGDEBUG("Exception Occured: " << ex.what());
		return false;
	}

	return true;
}

bool	CMediaManager::DeleteByFilename(CStdString filename)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	try
	{
		String sDeleteEntrySQL = TEXT("DELETE FROM MediaLibrary WHERE Filename = ?;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command deletecmd(con, sDeleteEntrySQL);

		deletecmd.bind(1, filename);

		deletecmd.executenonquery();
	}
	catch(std::exception &ex) 
	{
		//LOGDEBUG("Exception Occured: " << ex.what());
		return false;
	}

	return true;
}