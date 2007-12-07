#pragma once
#include "iinfomanager.h"

class Cm4aInfoManager : public IInfoManager
{
public:
	Cm4aInfoManager(void);
	~Cm4aInfoManager(void);

	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);

};
