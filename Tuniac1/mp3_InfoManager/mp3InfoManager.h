#pragma once
#include "iinfomanager.h"

class CID3InfoManager :
	public IInfoManager
{
public:
	CID3InfoManager(void);
	~CID3InfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
};
