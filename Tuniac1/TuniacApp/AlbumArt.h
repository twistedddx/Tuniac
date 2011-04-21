#pragma once

#include "jpeglib.h"

class CAlbumArt
{
protected:
	CCriticalSection		m_ArtLock;

	LPVOID					m_pBitmapData;
	unsigned long			m_ulBitmapWidth;
	unsigned long			m_ulBitmapHeight;

	TCHAR					szCurrentArtSource[512];

public:
	CAlbumArt(void);
	~CAlbumArt(void);

	LPTSTR	GetCurrentArtSource(void);
	void	SetCurrentArtSource(LPTSTR szNewArtSource);

	bool	SetSource(LPVOID pCompressedData, unsigned long ulDataLength, LPTSTR szMimeType);
	bool	SetSource(LPTSTR szFilename);

	bool	Draw(HDC hDC, long x, long y, long lWidth, long lHeight);
};
