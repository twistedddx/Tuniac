/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2014 Brett Hoyle
*/

#include "stdafx.h"

#include <vector>
#include <iostream>
#include <fstream>


#include "AlbumArt.h"

jmp_buf	CAlbumArt::sSetjmpBuffer ;

void CAlbumArt::errorExit(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message) (cinfo, buffer);
	tuniacApp.m_AlbumArtPanel.SetJPEGErrorMessage(buffer);
	jpeg_abort(cinfo);
	longjmp(sSetjmpBuffer, 1) ;
}

CAlbumArt::CAlbumArt(void)
{
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = &CAlbumArt::errorExit;
	jpeg_create_decompress(&cinfo);
}

CAlbumArt::~CAlbumArt(void)
{
	if(m_pBitmapData)
	{
		free(m_pBitmapData);
		m_pBitmapData = NULL;
	}

	jpeg_destroy_decompress(&cinfo);
}

void CAlbumArt::SetCurrentArtSource(LPTSTR szNewArtSource)
{
	if(szNewArtSource)
	{
		if (tuniacApp.m_LogWindow)
		{
			TCHAR szMessage[512];
			StringCchPrintf(szMessage, 512, TEXT("Set source %s"), szNewArtSource);
			tuniacApp.m_LogWindow->LogMessage(TEXT("AlbumArt"), szMessage);
		}

		StringCchCopy(szCurrentArtSource, MAX_PATH, szNewArtSource);
	}
}

LPTSTR CAlbumArt::GetCurrentArtSource(void)
{
	return szCurrentArtSource;
}

void CAlbumArt::SetJPEGErrorMessage(char * szError)
{
	strcpy_s(szErrorMessage, szError);
}

bool CAlbumArt::SetSource(LPVOID pCompressedData, unsigned long ulDataLength, LPTSTR szMimeType)
{
	CAutoLock lock(&m_ArtLock);

	if(StrStrI(szMimeType, TEXT("image/jpeg")) || StrStrI(szMimeType, TEXT("image/jpg")))
	{
		if(setjmp(sSetjmpBuffer))
		{
			//jpeglib-turbo error, check if its likely a png file!
			if(strcmp(szErrorMessage, "Not a JPEG file: starts with 0x89 0x50") == 0)
				return SetSource(pCompressedData, ulDataLength, L"image/png");
			else
				return false;
		}

		jpeg_mem_src(&cinfo, (unsigned char *)pCompressedData, ulDataLength);
		jpeg_read_header(&cinfo, TRUE);

		cinfo.out_color_space = JCS_EXT_BGRX;
		cinfo.dct_method = JDCT_FLOAT;

		jpeg_start_decompress(&cinfo);

		m_ulBitmapWidth		= cinfo.output_width;
		m_ulBitmapHeight	= cinfo.output_height;

		if(m_pBitmapData)
		{
			free(m_pBitmapData);
			m_pBitmapData = NULL;
		}

		m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * cinfo.output_components);
		if(!m_pBitmapData)
			return false;

		unsigned char * pOutData = (unsigned char *)m_pBitmapData;

		int row_stride = cinfo.output_width * cinfo.output_components;

		while (cinfo.output_scanline < m_ulBitmapHeight)
		{
			jpeg_read_scanlines(&cinfo, &pOutData, 1);
			pOutData += row_stride;
		}

		jpeg_finish_decompress(&cinfo);
		return true;
	}

	if( StrStrI(szMimeType, TEXT("image/png")) )
	{

		memset(&image, 0, (sizeof image));
		image.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_memory(&image, pCompressedData, ulDataLength))
		{
			image.format = PNG_FORMAT_BGRA;

			m_ulBitmapWidth		= image.width;
			m_ulBitmapHeight	= image.height;

			if(m_pBitmapData)
			{
				free(m_pBitmapData);
				m_pBitmapData = NULL;
			}

			m_pBitmapData = malloc(PNG_IMAGE_SIZE(image));
			if(!m_pBitmapData)
				return false;

			if(png_image_finish_read(&image, NULL, m_pBitmapData, 0, NULL))
			{
				png_image_free(&image);
				return true;
			}

			png_image_free(&image);
		}

		return false;
	}

	return false;
}

bool CAlbumArt::SetSource(LPTSTR szFilename)
{
	CAutoLock lock(&m_ArtLock);

	std::vector<unsigned char> buffer;
	std::ifstream file(szFilename, std::ios::in|std::ios::binary|std::ios::ate);
	if(!file.is_open())
		return false;

	//get filesize
	std::streamsize size = 0;
	if(file.seekg(0, std::ios::end).good()) 
		size = file.tellg();
	if(file.seekg(0, std::ios::beg).good()) 
		size -= file.tellg();

	//read contents of the file into the vector

	buffer.resize(size_t(size));
	if(size > 0) 
		file.read((char*)(&buffer[0]), size);

	if(StrStrI(szFilename, TEXT(".jpeg")) || StrStrI(szFilename, TEXT(".jpg")) || StrStrI(szFilename, TEXT(".jpe")))
		return SetSource(&buffer[0], size, TEXT("image/jpeg"));

	if( StrStrI(szFilename, TEXT(".png")) )
		return SetSource(&buffer[0], size, TEXT("image/png"));

	return false;
}

bool CAlbumArt::Draw(HDC hDC, long x, long y, long lWidth, long lHeight)
{
	CAutoLock lock(&m_ArtLock);

	if(!m_pBitmapData)
		return false;


	BITMAPINFO		bmi = {0,};

	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= m_ulBitmapWidth;
	bmi.bmiHeader.biHeight		= -((long)m_ulBitmapHeight);
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biSizeImage	= 0;

	//SetStretchBltMode(hDC, COLORONCOLOR);
	SetStretchBltMode(hDC, HALFTONE);
	SetBrushOrgEx(hDC, 0, 0, NULL);
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
