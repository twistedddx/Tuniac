#pragma once
#include "iinfomanager.h"

#include <shnplay.h>

class CSHNInfoManager :
	public IInfoManager
{
protected:

public:
	CSHNInfoManager(void);
	~CSHNInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);
	unsigned long	GetNumberOfAlbumArts(void);
	bool			GetAlbumArt(unsigned long		ulImageIndex,
								LPVOID			*	pImageData,
								unsigned long	*	ulImageDataSize,
								LPTSTR				szMimeType,
								unsigned long	*	ulArtType);

	bool			FreeAlbumArt(LPVOID				pImageData);
};
