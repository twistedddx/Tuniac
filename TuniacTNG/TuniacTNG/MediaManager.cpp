#include "stdafx.h"
#include "MediaManager.h"

#include <sqlite3x.hpp>

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
				TEXT("Filehash				 BLOB"),

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
	catch(...)
	{
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
	catch(...)
	{
	}

	return retval;
}


bool CMediaManager::AddFile(String filename)
{
	bool bret = false;

	String szDBName;
	GetMediaDBLocation(szDBName);

	try
	{
		String alreadyExistsSQL = TEXT("SELECT count(*) FROM MediaLibrary WHERE Filename = ?;");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command existscmd(con, alreadyExistsSQL);

		existscmd.bind(1, filename);

		if(existscmd.executeint() > 0)
			return true;
	}
	catch(...)
	{
		return false;
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

	try
	{
		String insertFilenameSQL = TEXT("insert into MediaLibrary (DateAdded, Filename) values (DATETIME('now','localtime'), ?)");


		MediaItem	newItem;
		PopulateMediaItemFromAccessor(newItem, pAccessor);


		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command cmd(con, insertFilenameSQL);

		cmd.bind(1, filename);

		cmd.executenonquery();

		bret = true;
	}
	catch(...)
	{
		bret = false;
	}

	if(pAccessor)
		pAccessor->Destroy();

	return bret;
}


bool CMediaManager::PopulateMediaItemFromAccessor(MediaItem & mediaItem, IInfoAccessor * pAccessor)
{
	TCHAR			temp[1024];
	__int64			tempint;


	//mediaItem.filename			= filename;
	//mediaItem.ulFilesize		= 1024;

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



	pAccessor->GetIntField(Track, &tempint);
	mediaItem.ulTrack			= tempint;

	pAccessor->GetIntField(MaxTrack, &tempint);
	mediaItem.ulMaxTrack		= tempint;



	pAccessor->GetIntField(Disc, &tempint);
	mediaItem.ulDisk			= tempint;
	pAccessor->GetIntField(MaxDisc, &tempint);
	mediaItem.ulMaxDisk			= tempint;


	tempint = 0;
	mediaItem.ulRating			= tempint;
	mediaItem.ulBPM				= tempint;

	
	pAccessor->GetIntField(PlaybackTime, &tempint);
	mediaItem.ulPlayTimeMS		= tempint;

	pAccessor->GetIntField(SampleRate, &tempint);
	mediaItem.ulSampleRate		= tempint;

	pAccessor->GetIntField(Channels, &tempint);
	mediaItem.ulChannelCount	= tempint;

	pAccessor->GetIntField(Bitrate, &tempint);
	mediaItem.ulBitRate			= tempint;
}
