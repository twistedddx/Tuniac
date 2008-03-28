#include "stdafx.h"
#include "MediaManager.h"

#include <sqlite3x.hpp>

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
	String szDBName;
	GetMediaDBLocation(szDBName);

	try
	{
		String insertFilenameSQL = TEXT("insert into MediaLibrary (DateAdded, Filename) values (DATETIME('now','localtime'), ?)");

		sqlite3x::sqlite3_connection con(szDBName);
		sqlite3x::sqlite3_command cmd(con, insertFilenameSQL);

		cmd.bind(1, filename);

		cmd.executenonquery();
	}
	catch(...)
	{
		return false;
	}

	return true;
}
