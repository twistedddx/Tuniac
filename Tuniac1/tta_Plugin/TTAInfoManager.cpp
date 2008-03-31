#include "stdafx.h"
#include "ttainfomanager.h"

// actual class definition here

CTTAInfoManager::CTTAInfoManager(void)
{
}

CTTAInfoManager::~CTTAInfoManager(void)
{
}

void			CTTAInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CTTAInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CTTAInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".tta")
	};

	return exts[ulExtentionNum];
}

bool			CTTAInfoManager::CanHandle(LPTSTR szSource)
{
	for(unsigned int x=0; x<GetNumExtensions(); x++)
	{
		if(!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}

bool			CTTAInfoManager::GetInfo(LibraryEntry * libEnt)
{
	return true;
}

bool			CTTAInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CTTAInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			CTTAInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CTTAInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
