#pragma once
#include "iinfomanager.h"
#include <stdio.h>
#include <mmsystem.h>

class CWAVInfoManager :
	public IInfoManager
{
protected:

public:
	CWAVInfoManager(void);
	~CWAVInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
	unsigned long	GetNumberOfAlbumArts(LPTSTR		szFilename);
	bool			GetAlbumArt(LPTSTR				szFilename, 
								unsigned long		ulImageIndex,
								LPVOID			*	pImageData,
								unsigned long	*	ulImageDataSize,
								LPTSTR				szMimeType,
								unsigned long	*	ulArtType);

	bool			FreeAlbumArt(LPVOID				pImageData);
};
