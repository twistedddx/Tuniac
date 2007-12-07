#pragma once
#include "iinfomanager.h"
#include "vorbis\vorbisfile.h"

class COGGInfoManager :
	public IInfoManager
{
protected:
  vorbis_info *pInfo;
  vorbis_comment *tInfo;
  OggVorbis_File oggFile;
  FILE			*	f;

public:
	COGGInfoManager(void);
	~COGGInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
};
