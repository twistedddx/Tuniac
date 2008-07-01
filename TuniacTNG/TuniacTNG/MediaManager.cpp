#include "stdafx.h"
#include "MediaManager.h"

#include "TuniacHelper.h"

/* FILETIME utility */
const __int64 _onesec_in100ns = (__int64)10000000;


/* __int64 <--> FILETIME */
static __int64 wce_FILETIME2int64(FILETIME f)
{
	__int64 t;

	t = f.dwHighDateTime;
	t <<= 32;
	t |= f.dwLowDateTime;
	return t;
}

static FILETIME wce_int642FILETIME(__int64 t)
{
	FILETIME f;

	f.dwHighDateTime = (DWORD)((t >> 32) & 0x00000000FFFFFFFF);
	f.dwLowDateTime  = (DWORD)( t        & 0x00000000FFFFFFFF);
	return f;
}

static FILETIME wce_getFILETIMEFromYear(WORD year)
{
	SYSTEMTIME s={0};
	FILETIME f;

	s.wYear      = year;
	s.wMonth     = 1;
	s.wDayOfWeek = 1;
	s.wDay       = 1;

	SystemTimeToFileTime( &s, &f );
	return f;
}


/* FILETIME <--> time_t */
static time_t wce_FILETIME2time_t(const FILETIME* f)
{
	FILETIME f1601, f1970;
	__int64 t, offset;

	f1601 = wce_getFILETIMEFromYear(1601);
	f1970 = wce_getFILETIMEFromYear(1970);

	offset = wce_FILETIME2int64(f1970) - wce_FILETIME2int64(f1601);

	t = wce_FILETIME2int64(*f);

	t -= offset;
	return (time_t)(t / _onesec_in100ns);
}

static FILETIME wce_time_t2FILETIME(const time_t t)
{
	FILETIME f, f1970;
	__int64 time;

	f1970 = wce_getFILETIMEFromYear(1970);

	time = t;
	time *= _onesec_in100ns;
	time += wce_FILETIME2int64(f1970);

	f = wce_int642FILETIME(time);

	return f;
}



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
			// all dates in TNG are stored in time_t format (either 32 r 64 bits it makes no difference)

			// THERE IS NO MEDIA LIBRARY :(

			String DBColumns[] =
			{
				TEXT("EntryID				 INTEGER PRIMARY KEY"),
				TEXT("DirtyFlag				 INT"),

				TEXT("DateAdded				 INT"),
				TEXT("FileOnline			 INT"),

				TEXT("Filename				 TEXT"),
				TEXT("Filesize				 INT"),
				TEXT("FileModifiedTime		 INT"),

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

				TEXT("FirstPlayed			 INT"),
				TEXT("LastPlayed			 INT"),
				TEXT("Playcount				 INT"),

				TEXT("ReplayGainTrack		 REAL"),
				TEXT("ReplayPeakTrack		 REAL"),
				TEXT("ReplayGainAlbum		 REAL"),
				TEXT("ReplayPeakAlbum		 REAL"),

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

	RebuildIDList();

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

	// extract generic info from the file (creation time/size)
	HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME		ft;
		GetFileTime(hFile, NULL, NULL, &ft);
		mediaItem.ullFileModifiedTime = wce_FILETIME2time_t(&ft);

		LARGE_INTEGER		llFileSize;
		GetFileSizeEx(hFile, &llFileSize);
		mediaItem.ullFilesize = llFileSize.QuadPart;

		CloseHandle(hFile);
	}


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

	pAccessor->GetTextField(ReplayGainTrack, temp, 1024);
	mediaItem.fReplayGainTrack = _wtof(temp);

	pAccessor->GetTextField(ReplayPeakTrack, temp, 1024);
	mediaItem.fReplayPeakTrack = _wtof(temp);

	pAccessor->GetTextField(ReplayGainAlbum, temp, 1024);
	mediaItem.fReplayGainAlbum = _wtof(temp);

	pAccessor->GetTextField(ReplayPeakAlbum, temp, 1024);
	mediaItem.fReplayPeakAlbum = _wtof(temp);

	if(pAccessor)
		pAccessor->Destroy();
	
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
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
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
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
	}

	return true;
}

bool CMediaManager::RebuildIDList(void)
{
	String szDBName;
	GetMediaDBLocation(szDBName);

	m_vIDList.clear();

	try
	{
		// TODO: replace this code with something a lot more efficient.....
		// SQLite doesn't like LIMIT commands, so it may be a case of 
		String sGetRangeSQL = TEXT("SELECT EntryID FROM MediaLibrary");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command rangecmd(con, sGetRangeSQL);

		sqlite3x::sqlite3_reader reader = rangecmd.executereader();

		while(reader.read())
		{
			m_vIDList.push_back(reader.getint64(0));
		}

		reader.close();
	}
	catch(...)
	{
		return false;
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
		// TODO: replace this code with something a lot more efficient.....
		// SQLite doesn't like LIMIT commands, so it may be a case of 


		String sGetRangeSQL = TEXT("SELECT * FROM MediaLibrary WHERE EntryID IN (");

		for(int x=0; x<ulCount; x++)
		{
			sGetRangeSQL += TEXT("?");
			if(x<ulCount-1)
			sGetRangeSQL += TEXT(",");
		}
		sGetRangeSQL += TEXT(")");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command rangecmd(con, sGetRangeSQL);


		for(int x=0; x<ulCount; x++)
		{
			rangecmd.bind(x+1, (long long)m_vIDList[ulStart+x]);
		}

		sqlite3x::sqlite3_reader reader = rangecmd.executereader();

		while(reader.read())
		{
			//existscmd.
			MediaItem tempItem = {0};


			ReaderMediaItem(reader, tempItem);



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
		String insertFilenameSQL = TEXT("INSERT INTO  MediaLibrary \
		(DateAdded,	FileOnline, Filename,	Filesize,	FileModifiedTime,	Title,	Artist,	DiscTitle,	Album,	AlbumArtist,	Composer,	Year,	Genre,	Comment,	Track,	MaxTrack,	Disc,	MaxDisc,	Rating,	BPM,	PlaybackTime,	PlaybackTimeAccuracy,	SampleRate, Channels,	Bitrate,	FirstPlayed,	LastPlayed,		PlayCount,	ReplayGainTrack,	ReplayPeakTrack,	ReplayGainAlbum,	ReplayPeakalbum)\
		VALUES\
		(?,			1,			?,			?,			?,					?,		?,		?,			?,		?,				?,			?,		?,		?,			?,		?,			?,		?,			?,		?,		?,				0,						?,			?,			?,			0,				0,				0,			?,					?,					?,					?)");

		sqlite3x::sqlite3_command cmd(con, insertFilenameSQL);

		int bind=1;

		cmd.bind(bind++,	time(NULL));

		cmd.bind(bind++,	newItem.filename);
		cmd.bind(bind++,	(long long)newItem.ullFilesize);
		cmd.bind(bind++,	newItem.ullFileModifiedTime);

		cmd.bind(bind++,	newItem.title);
		cmd.bind(bind++,	newItem.artist);
		cmd.bind(bind++,	newItem.disktitle);

		cmd.bind(bind++,	newItem.album);
		cmd.bind(bind++,	newItem.albumartist);

		cmd.bind(bind++,	newItem.composer);

		cmd.bind(bind++,	newItem.ulYear);

		cmd.bind(bind++,	newItem.genre);
		cmd.bind(bind++,	newItem.comment);

		cmd.bind(bind++,	newItem.ulTrack);
		cmd.bind(bind++,	newItem.ulMaxTrack);
		cmd.bind(bind++,	newItem.ulDisk);
		cmd.bind(bind++,	newItem.ulMaxDisk);

		cmd.bind(bind++,	newItem.ulRating);
		cmd.bind(bind++,	newItem.ulBPM);

		cmd.bind(bind++,	newItem.ulPlayTimeMS);

		cmd.bind(bind++,	newItem.ulSampleRate);
		cmd.bind(bind++,	newItem.ulChannelCount);
		cmd.bind(bind++,	newItem.ulBitRate);

		cmd.bind(bind++,	newItem.fReplayGainTrack);
		cmd.bind(bind++,	newItem.fReplayPeakTrack);
		cmd.bind(bind++,	newItem.fReplayGainAlbum);
		cmd.bind(bind++,	newItem.fReplayPeakAlbum);

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
	{
		RebuildIDList();
		return true;
	}

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

		RebuildIDList();

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
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}

	RebuildIDList();


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
		String debugstring = ex.what();
		OutputDebugString(TEXT("Error: "));
		OutputDebugString((debugstring+TEXT("\n")).c_str());
		return false;
	}

	RebuildIDList();

	return true;
}

// THIS FUNCTION ONLY WORKS FOR SELECT * FROM MEDIALIBRARY!!!!1
// this is so I only have to update it in one place!!!
bool CMediaManager::ReaderMediaItem(sqlite3x::sqlite3_reader & reader, MediaItem & item)
{
	try
	{
		item.ulItemID				= reader.getint64(0);

		item.dateAdded				= reader.getint64(2);
		item.fileOnline				= reader.getint64(3);

		item.filename				= reader.getstring16(4);
		item.ullFilesize			= reader.getint64(5);
		item.ullFileModifiedTime	= reader.getint64(6);

		item.title					= reader.getstring16(7);
		item.artist					= reader.getstring16(8);

		item.album					= reader.getstring16(10);
		item.albumartist			= reader.getstring16(11);
		item.composer				= reader.getstring16(12);
		item.ulYear					= reader.getint64(13);
		item.genre					= reader.getstring16(14);
		item.comment				= reader.getstring16(15);

		item.ulTrack				= reader.getint64(16);
		item.ulMaxTrack				= reader.getint64(17);

		item.ulDisk					= reader.getint64(18);
		item.ulMaxDisk				= reader.getint64(19);

		item.ulRating				= reader.getint64(20);
		item.ulBPM					= reader.getint64(21);

		item.ulPlayTimeMS			= reader.getint64(22);
		item.ulPlaybackTimeAccuracy	= reader.getint64(23);

		item.ulSampleRate			= reader.getint64(24);
		item.ulChannelCount			= reader.getint64(25);
		item.ulBitRate				= reader.getint64(26);

		item.ullFirstPlayed 		= reader.getint64(27);
		item.ullLastPlayed			= reader.getint64(28);
		item.ullPlayCount			= reader.getint64(29);

		item.fReplayGainTrack		= reader.getdouble(30);
		item.fReplayPeakTrack		= reader.getdouble(31);

		item.fReplayGainAlbum		= reader.getdouble(32);
		item.fReplayPeakAlbum		= reader.getdouble(33);
	}
	catch (...)
	{
		return false;
	}

	return true;
}
