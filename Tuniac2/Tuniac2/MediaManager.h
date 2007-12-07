#pragma once

#include "sqlite3x.hpp"
using namespace sqlite3x;

class CMediaManager
{
protected:
	sqlite3_connection							m_DBCon;

public:
	CMediaManager(void);
	~CMediaManager(void);

	bool Initialize(void);
	bool Shutdown(void);

	bool ShowAddFolderSelector(HWND hWndParent);
	bool ShowAddFiles(HWND hWndParent);

	bool AddFile(String filename);
	bool AddFileArray(StringArray filenameArray);
};
