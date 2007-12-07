#pragma once
#include "iinfomanager.h"

class CDUMMYInfoManager :
	public IInfoManager
{

public:
	CDUMMYInfoManager(void);
	~CDUMMYInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
};
