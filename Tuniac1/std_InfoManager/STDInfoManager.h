#pragma once
#include "iinfomanager.h"

class CSTDInfoManager :
	public IInfoManager
{

public:
	CSTDInfoManager(void);
	~CSTDInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
};
