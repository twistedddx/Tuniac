#pragma once

#include "png.h"
#include "jpeglib.h"

class CAlbumArt
{
protected:
	CCriticalSection		m_ArtLock;

	LPVOID					m_pBitmapData;
	unsigned long			m_ulBitmapWidth;
	unsigned long			m_ulBitmapHeight;

	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;

	char					szErrorMessage[JMSG_LENGTH_MAX];
	TCHAR					szCurrentArtSource[512];

	png_image				image;


public:
	CAlbumArt(void);
	~CAlbumArt(void);

	LPTSTR	GetCurrentArtSource(void);
	void	SetCurrentArtSource(LPTSTR szNewArtSource);

	bool	SetSource(LPVOID pCompressedData, unsigned long ulDataLength, LPTSTR szMimeType);
	bool	SetSource(LPTSTR szFilename);


	static void	errorExit(j_common_ptr cinfo);
	static jmp_buf	sSetjmpBuffer;

	void	SetJPEGErrorMessage(char * szError);


	bool	Draw(HDC hDC, long x, long y, long lWidth, long lHeight);
};
