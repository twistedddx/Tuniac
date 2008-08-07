#pragma once

#include <IInfoHandler.h>
#include <Singleton.h>
#include <sqlite3x.hpp>

#include <mediaitem.h>

class CMediaManager : public CSingleton<CMediaManager>
{
protected:
	typedef struct
	{
		IInfoHandler *	pInfoHandler;
		HINSTANCE		hDLL;
	} InfoHandler;

	std::vector<InfoHandler>		m_vInfoHandlers;
	std::vector<unsigned long long>	m_vIDList;

	String							m_DBFilename;

	bool PopulateMediaItemFromAccessor(String filename, MediaItem & pItem);
	bool InsertItemToMediaLibraryUsingConnection(sqlite3x::sqlite3_connection & con, MediaItem & pItem);
	bool IsFileInLibrary(sqlite3x::sqlite3_connection & con, String filename);

	bool ReaderMediaItem(sqlite3x::sqlite3_cursor & cursor, MediaItem & item);

	bool RebuildIDList(void);
	void CreateDatabaseSchema(sqlite3x::sqlite3_connection & con);

public:
	CMediaManager(void);
	~CMediaManager(void);

public:

	bool Initialize(void);
	bool Shutdown(void);

	bool GetMediaDBLocation(String & strPath);

	unsigned __int64 GetNumEntries(void);

	bool GetAlbums(StringArray & albumList);
	bool GetArtists(StringArray & artistList);
	bool GetRange(unsigned long ulStart, unsigned long ulCount, MediaItemArray & itemList);

	bool AddFileArray(StringArray filenameArray);

	bool DeleteByID(__int64 ullID);
	bool DeleteByFilename(String filename);

};
