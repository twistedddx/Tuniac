#pragma once
#include "iinfomanager.h"
#include "All.h"
#include "MACLib.h"

class CAPEInfoManager :
	public IInfoManager
{
protected:

	IAPEDecompress* MACDecompressor;

public:
	CAPEInfoManager(void);
	~CAPEInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
};
