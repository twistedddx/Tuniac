#pragma once
#include "iinfomanager.h"
#include "bass.h"
#include "array.h"
#include <ctime>

class CBASSInfoManager :
	public IInfoManager
{
protected:

	Array<InfoDevice, 3>				m_DeviceArray;
	unsigned long						m_ActiveDevice;

public:
	CBASSInfoManager(void);
	~CBASSInfoManager(void);

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
