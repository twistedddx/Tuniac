#include "stdafx.h"

#include <vector>
#include <iostream>
#include <fstream>


#include "AlbumArt.h"

extern int decodePNG(	std::vector<unsigned char>& out_image_32bit, 
							unsigned long& image_width, 
							unsigned long& image_height, 
							const unsigned char* in_png, 
							size_t in_size,
							bool convert_to_rgba32);


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
		StrCpyN(szCurrentArtSource, szNewArtSource, 511);
	}
}

LPTSTR CAlbumArt::GetCurrentArtSource(void)
{
	return szCurrentArtSource;
}

void CAlbumArt::SetJPEGErrorMessage(char * szError)
{
	strcpy(szErrorMessage, szError);
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

		JSAMPLE *buffer;
		int row_stride = cinfo.output_width * cinfo.output_components;
		if ((buffer = (JSAMPLE *)malloc(row_stride)) == NULL) 
			return false;

		while (cinfo.output_scanline < m_ulBitmapHeight)
		{
			jpeg_read_scanlines(&cinfo, &buffer, 1);
			CopyMemory(pOutData, buffer, row_stride);
			pOutData += row_stride;
		}

		jpeg_finish_decompress(&cinfo);
		free(buffer);

		return true;
	}

	if( StrStrI(szMimeType, TEXT("image/png")) )
	{
		std::vector<unsigned char> decompressedData;

		// fills out our std::vector as a 32bit ARGB data stream
		unsigned long w, h;
		if(decodePNG(	decompressedData,
					w,
					h,
					(const unsigned char *)pCompressedData,
					ulDataLength,
					true) == 0)
		{
			m_ulBitmapWidth		= w;
			m_ulBitmapHeight	= h;

			if(m_pBitmapData)
			{
				free(m_pBitmapData);
				m_pBitmapData = NULL;
			}

			m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * 4);
			if(!m_pBitmapData)
				return false;

			//CopyMemory(m_pBitmapData, &imageData[0], imageData.size());
			unsigned char * pOutData	= (unsigned char *)m_pBitmapData;
			unsigned char * pSrc	= (unsigned char *)&decompressedData[0];
			for(int x=0; x<decompressedData.size(); x+= 4)
			{
				pOutData[x]		= pSrc[x+2];
				pOutData[x+1]	= pSrc[x+1];
				pOutData[x+2]	= pSrc[x];
				pOutData[x+3]	= pSrc[x+3];
			}

			return true;
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
