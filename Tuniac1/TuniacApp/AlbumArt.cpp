#include "stdafx.h"

#include "../jpegdecoder/jpegdecoder.h"

#include "AlbumArt.h"


CAlbumArt::CAlbumArt(void) :
	m_pBitmapData(NULL)
{
	m_ulBitmapWidth		= 100;
	m_ulBitmapHeight	= 100;
	m_ulBytesPerPixel	= 4;

	m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * m_ulBytesPerPixel);
	ZeroMemory(m_pBitmapData, m_ulBitmapWidth * m_ulBitmapHeight * m_ulBytesPerPixel);

}

CAlbumArt::~CAlbumArt(void)
{
	if(m_pBitmapData)
	{
		free(m_pBitmapData);
		m_pBitmapData = NULL;
	}

}

bool CAlbumArt::SetSource(LPVOID pData, unsigned long ulDataLength, LPTSTR szMimeType)
{
	CAutoLock lock(&m_ArtLock);
	return false;
}

bool CAlbumArt::SetSource(LPTSTR szFilename)
{
	CAutoLock lock(&m_ArtLock);

	jpeg_decoder_file_stream input_stream;
	if (input_stream.open(szFilename))
	{
		return (false);
	}


	jpeg_decoder d(&input_stream, false);
	if(d.get_error_code() != 0)
	{
		// Always be sure to delete the input stream object _after_
		// the decoder is deleted. Reason: the decoder object calls the input
		// stream's detach() method.
		return (false);
	}

	if(d.begin())
		return false;

	m_ulBitmapWidth		= d.get_width();
	m_ulBitmapHeight	= d.get_height();
	m_ulBytesPerPixel	= d.get_bytes_per_pixel();
	m_ulComponents		= d.get_num_components();

	if(m_pBitmapData)
	{
		free(m_pBitmapData);
		m_pBitmapData = NULL;
	}

	m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * m_ulBytesPerPixel);
	if(!m_pBitmapData)
		return false;

	unsigned char * pData = (unsigned char *)m_pBitmapData;
	while(true)
	{
		unsigned char * Pscan_line_ofs;
		uint scan_line_len;

		int src_bpp = d.get_bytes_per_pixel();


		if(d.decode((void**)&Pscan_line_ofs, &scan_line_len))
			break;

//		CopyMemory(pData, Pscan_line_ofs, m_ulBitmapWidth * m_ulBytesPerPixel);

		for(int x=0; x<scan_line_len; x+= src_bpp)
		{
			pData[x+0]	= Pscan_line_ofs[x+2];
			pData[x+1]	= Pscan_line_ofs[x+1];
			pData[x+2]	= Pscan_line_ofs[x+0];
			pData[x+3]	= Pscan_line_ofs[x+3];
		}

		pData += m_ulBitmapWidth * m_ulBytesPerPixel;
		int x=0;
	}

	input_stream.close();

	return true;
}

bool CAlbumArt::Draw(HDC hDC, long x, long y, long lWidth, long lHeight)
{
	CAutoLock lock(&m_ArtLock);

	BITMAPINFO		bmi = {0,};

	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= m_ulBitmapWidth;
	bmi.bmiHeader.biHeight		= -m_ulBitmapHeight;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biSizeImage	= 0;

	SetStretchBltMode(hDC, COLORONCOLOR);
	int iRet = StretchDIBits(	hDC,
								x,
								y, 
								lWidth,
								lHeight,
								0,
								0,
								m_ulBitmapWidth,
								m_ulBitmapHeight,
								m_pBitmapData,
								&bmi,
								DIB_RGB_COLORS,
								SRCCOPY);



	if (iRet == GDI_ERROR)
	{
		return false;
	}

	return true;
}
