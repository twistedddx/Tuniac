#pragma once
#include "iinfomanager.h"
#include ".\OptimFROG\OptimFROG.h"

class COFRInfoManager :
	public IInfoManager
{
protected:

public:
	COFRInfoManager(void);
	~COFRInfoManager(void);

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
