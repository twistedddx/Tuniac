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
	GetMediaDBLocation(m_DBFilename);

	try
	{
		sqlite3x::sqlite3_connection con(m_DBFilename);
		CreateDatabaseSchema(con);
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
	unsigned __int64 retval = m_vIDList.size();
	return retval;
}

bool CMediaManager::PopulateMediaItemFromAccessor(String filename, MediaItem & mediaItem)
{
	bool			bWorked = false;

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

	if(pAccessor->ReadMetaData(mediaItem))
	{
		// yay
		bWorked = true;
	}

	if(pAccessor)
		pAccessor->Destroy();
	
	return bWorked;
}

bool CMediaManager::GetAlbums(StringArray & albumList)
{
	albumList.clear();

	try
	{
		String sGetAlbumSQL = TEXT("SELECT DISTINCT Album FROM MediaLibrary;");

		sqlite3x::sqlite3_connection con(m_DBFilename);
		sqlite3x::sqlite3_command albumcmd(con, sGetAlbumSQL);

		sqlite3x::sqlite3_cursor cursor = albumcmd.executecursor();

		while(cursor.step())
		{
			//existscmd.
			String temp = cursor.getstring(0);

			albumList.push_back(temp);
		}

		cursor.close();
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
	artistList.clear();

	try
	{
		String sGetAlbumSQL = TEXT("SELECT DISTINCT Artist FROM MediaLibrary;");

		sqlite3x::sqlite3_connection con(m_DBFilename);
		sqlite3x::sqlite3_command albumcmd(con, sGetAlbumSQL);

		sqlite3x::sqlite3_cursor cursor = albumcmd.executecursor();

		while(cursor.step())
		{
			//existscmd.
			String temp = cursor.getstring(0);

			artistList.push_back(temp);
		}

		cursor.close();
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
	m_vIDList.clear();

	try
	{
		// TODO: replace this code with something a lot more efficient.....
		// SQLite doesn't like LIMIT commands, so it may be a case of 
		String sGetRangeSQL = TEXT("SELECT ID FROM MediaLibrary");

		sqlite3x::sqlite3_connection con(m_DBFilename);
		sqlite3x::sqlite3_command rangecmd(con, sGetRangeSQL);

		sqlite3x::sqlite3_cursor cursor = rangecmd.executecursor();

		while(cursor.step())
		{
			m_vIDList.push_back(cursor.getint64(0));
		}

		cursor.close();
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool CMediaManager::GetRange(unsigned long ulStart, unsigned long ulCount, MediaItemArray & itemList)
{
	itemList.clear();

	try
	{
		// TODO: replace this code with something a lot more efficient.....
		// SQLite doesn't like LIMIT commands, so it may be a case of 


		String sGetRangeSQL = TEXT("SELECT * FROM MediaLibrary WHERE ID IN (");

		for(int x=0; x<ulCount; x++)
		{
			sGetRangeSQL += TEXT("?");
			if(x<ulCount-1)
			sGetRangeSQL += TEXT(",");
		}
		sGetRangeSQL += TEXT(")");

		sqlite3x::sqlite3_connection con(m_DBFilename);
		sqlite3x::sqlite3_command rangecmd(con, sGetRangeSQL);


		for(int x=0; x<ulCount; x++)
		{
			rangecmd.bind(x+1, (long long)m_vIDList[ulStart+x]);
		}

		sqlite3x::sqlite3_cursor cursor = rangecmd.executecursor();

		while(cursor.step())
		{
			//existscmd.
			MediaItem tempItem = {0};


			ReaderMediaItem(cursor, tempItem);



			itemList.push_back(tempItem);
		}

		cursor.close();
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
		(DateAdded,	FileOnline, Filename,	FileSize,	FileModifiedTime,	Title,	Artist,	DiscTitle,	Album,	AlbumArtist,	Composer,	Year,	Genre,	Comment,	Track,	MaxTrack,	Disc,	MaxDisc,	Rating,	BPM,	PlaybackTime,	PlaybackTimeAccuracy,	SampleRate, Channels,	Bitrate,	FirstPlayed,	LastPlayed,		PlayCount,	ReplayGainTrack,	ReplayPeakTrack,	ReplayGainAlbum,	ReplayPeakAlbum)\
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

bool CMediaManager::AddFileArray(StringArray filenameArray)
{
	if(filenameArray.size())
	{
		sqlite3x::sqlite3_connection con(m_DBFilename);

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
	try
	{
		String sDeleteEntrySQL = TEXT("DELETE FROM MediaLibrary WHERE ID = ?;");

		sqlite3x::sqlite3_connection con(m_DBFilename);
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
	try
	{
		String sDeleteEntrySQL = TEXT("DELETE FROM MediaLibrary WHERE Filename = ?;");

		sqlite3x::sqlite3_connection con(m_DBFilename);
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
bool CMediaManager::ReaderMediaItem(sqlite3x::sqlite3_cursor & cursor, MediaItem & item)
{
	try
	{
		item.ulID					= cursor.getint64(0);

		item.dateAdded				= cursor.getint64(2);
		item.fileOnline				= cursor.getint64(3);

		item.filename				= cursor.getstring16(4);
		item.ullFilesize			= cursor.getint64(5);
		item.ullFileModifiedTime	= cursor.getint64(6);

		item.title					= cursor.getstring16(7);
		item.artist					= cursor.getstring16(8);

		item.album					= cursor.getstring16(10);
		item.albumartist			= cursor.getstring16(11);
		item.composer				= cursor.getstring16(12);
		item.ulYear					= cursor.getint64(13);
		item.genre					= cursor.getstring16(14);
		item.comment				= cursor.getstring16(15);

		item.ulTrack				= cursor.getint64(16);
		item.ulMaxTrack				= cursor.getint64(17);

		item.ulDisk					= cursor.getint64(18);
		item.ulMaxDisk				= cursor.getint64(19);

		item.ulRating				= cursor.getint64(20);
		item.ulBPM					= cursor.getint64(21);

		item.ulPlayTimeMS			= cursor.getint64(22);
		item.ulPlaybackTimeAccuracy	= cursor.getint64(23);

		item.ulSampleRate			= cursor.getint64(24);
		item.ulChannelCount			= cursor.getint64(25);
		item.ulBitRate				= cursor.getint64(26);

		item.ullFirstPlayed 		= cursor.getint64(27);
		item.ullLastPlayed			= cursor.getint64(28);
		item.ullPlayCount			= cursor.getint64(29);

		item.fReplayGainTrack		= cursor.getdouble(30);
		item.fReplayPeakTrack		= cursor.getdouble(31);

		item.fReplayGainAlbum		= cursor.getdouble(32);
		item.fReplayPeakAlbum		= cursor.getdouble(33);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void CMediaManager::CreateDatabaseSchema(sqlite3x::sqlite3_connection & con)
{
	bool hasMediaLibrary	= con.executeint("SELECT COUNT(*) FROM SQLITE_MASTER WHERE TYPE = 'table' AND NAME = 'MediaLibrary'") == 1;
	bool hasPlaylist		= con.executeint("SELECT COUNT(*) FROM SQLITE_MASTER WHERE TYPE = 'table' AND NAME = 'Playlist'") == 1;
	bool hasPlaylistTracks	= con.executeint("SELECT COUNT(*) FROM SQLITE_MASTER WHERE TYPE = 'table' AND NAME = 'PlaylistTracks'") == 1;

	if (!hasMediaLibrary)
	{
		// all dates in TNG are stored in time_t format (either 32 r 64 bits it makes no difference)
		String sql = TEXT("										\
			CREATE TABLE MediaLibrary (							\
				ID						INTEGER PRIMARY KEY,	\
				DirtyFlag				INTEGER,				\
																\
				DateAdded				INTEGER, 				\
				FileOnline				INTEGER, 				\
				Filename				TEXT, 					\
				FileSize				TEXT, 					\
				FileModifiedTime		INTEGER, 				\
																\
				Title					TEXT, 					\
				Artist					TEXT, 					\
				DiscTitle				TEXT, 					\
				Album					TEXT, 					\
				AlbumArtist				TEXT, 					\
				Composer				TEXT, 					\
				Year					INTEGER, 				\
				Genre					TEXT, 					\
				Comment					TEXT, 					\
																\
				Track					TEXT, 					\
				MaxTrack				TEXT, 					\
				Disc					TEXT, 					\
				MaxDisc					TEXT, 					\
																\
				Rating					INTEGER, 				\
				BPM						INTEGER, 				\
				PlaybackTime			INTEGER, 				\
				PlaybackTimeAccuracy	INTEGER DEFAULT 0, 		\
				SampleRate				INTEGER, 				\
				Channels				INTEGER, 				\
				Bitrate					INTEGER, 				\
																\
				FirstPlayed				INTEGER DEFAULT 0, 		\
				LastPlayed				INTEGER DEFAULT 0, 		\
				PlayCount				INTEGER DEFAULT 0, 		\
																\
				ReplayGainTrack			REAL DEFAULT 0, 		\
				ReplayPeakTrack			REAL DEFAULT 0, 		\
				ReplayGainAlbum			REAL DEFAULT 0, 		\
				ReplayPeakAlbum			REAL DEFAULT 0, 		\
																\
				EncoderDelay			INTEGER DEFAULT 0,		\
				EncoderPadding			INTEGER DEFAULT 0,		\
																\
				InfoRead				INTEGER DEFAULT 0 		\
			)													\
		");

		con.executenonquery(sql);

	}

	if (!hasPlaylist)
	{
		String sql = TEXT("										\
			CREATE TABLE Playlist (								\
				ID						INTEGER PRIMARY KEY,	\
				Name					TEXT,					\
				CreateDate				INTEGER 				\
			)													\
		");

		con.executenonquery(sql);
	}

	if (!hasPlaylistTracks)
	{
		String sql = TEXT("									\
			CREATE TABLE PlaylistTracks (					\
				PlaylistEntryID			INTEGERPRIMARY KEY,	\
				PlaylistID				INTEGER,			\
				TrackID					INTEGER,			\
				OrderNum				INTEGER				\
			)												\
		");

		con.executenonquery(sql);
	}

}
