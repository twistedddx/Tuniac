#pragma once

class CAlbumArt
{
protected:
	CCriticalSection		m_ArtLock;

	LPVOID					m_pBitmapData;
	long					m_ulBitmapWidth;
	long					m_ulBitmapHeight;
	unsigned long			m_ulBytesPerPixel;
	unsigned long			m_ulComponents;

	//bool	LoadJpegData(

public:
	CAlbumArt(void);
	~CAlbumArt(void);

	bool SetSource(LPVOID pData, unsigned long ulDataLength, LPTSTR szMimeType);
	bool SetSource(LPTSTR szFilename);

	bool Draw(HDC hDC, long x, long y, long lWidth, long lHeight);
};
