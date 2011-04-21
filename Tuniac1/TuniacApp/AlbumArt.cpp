#include "stdafx.h"

#include <vector>
#include <iostream>
#include <fstream>


#include "AlbumArt.h"

extern unsigned decodePNG(	std::vector<unsigned char>& out_image_32bit, 
							unsigned& image_width, 
							unsigned& image_height, 
							const unsigned char* in_png, 
							unsigned in_size);

CAlbumArt::CAlbumArt(void)
{
}

CAlbumArt::~CAlbumArt(void)
{
	if(m_pBitmapData)
	{
		free(m_pBitmapData);
		m_pBitmapData = NULL;
	}

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

bool CAlbumArt::SetSource(LPVOID pCompressedData, unsigned long ulDataLength, LPTSTR szMimeType)
{
	CAutoLock lock(&m_ArtLock);

	if(StrStrI(szMimeType, TEXT("image/jpeg")) || StrStrI(szMimeType, TEXT("image/jpg")))
	{
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo, (unsigned char *)pCompressedData, ulDataLength);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		m_ulBitmapWidth		= cinfo.output_width;
		m_ulBitmapHeight	= cinfo.output_height;

		if(m_pBitmapData)
		{
			free(m_pBitmapData);
			m_pBitmapData = NULL;
		}

		m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * 4);
		if(!m_pBitmapData)
			return false;

		unsigned char * pOutData = (unsigned char *)m_pBitmapData;

		JSAMPLE *buffer;
		int row_stride = cinfo.output_width * cinfo.output_components;
		if ((buffer = (JSAMPLE *)malloc(row_stride * sizeof(JSAMPLE))) == NULL) 
			return false;


		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo,&buffer,1);
			if (cinfo.output_components == 3)
			{
				int y = 0;
				for (int i=0; i<row_stride; i+=3)
				{
					pOutData[y] = buffer[i+2];
					pOutData[y+1] = buffer[i+1];
					pOutData[y+2] = buffer[i];
					pOutData[y+3] = 255;
					y += 4;
				}
			}
			else
			{
				int y = 0;
				for (int i=0; i<row_stride; i++)
				{
					pOutData[y] = buffer[i];
					pOutData[y+1] = buffer[i];
					pOutData[y+2] = buffer[i];
					pOutData[y+3] = 255;
					y += 4;
				}
			}
			pOutData += m_ulBitmapWidth * 4;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		free(buffer);

		return true;

	}

	if( StrStrI(szMimeType, TEXT("image/png")) )
	{
		std::vector<unsigned char> decompressedData;

		// fills out our std::vector as a 32bit ARGB data stream
		unsigned int w, h;
		if(decodePNG(	decompressedData,
					w,
					h,
					(const unsigned char *)pCompressedData,
					ulDataLength) == 0)
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

	if(StrStrI(szFilename, TEXT(".jpeg")) || StrStrI(szFilename, TEXT(".jpg")) || StrStrI(szFilename, TEXT(".jpe")))
	{
		FILE * imageFile = _wfopen(szFilename, TEXT("rbS"));
		if(imageFile == NULL)
			return false;

		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, imageFile);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		m_ulBitmapWidth		= cinfo.output_width;
		m_ulBitmapHeight	= cinfo.output_height;

		if(m_pBitmapData)
		{
			free(m_pBitmapData);
			m_pBitmapData = NULL;
		}

		m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * 4);
		if(!m_pBitmapData)
			return false;

		unsigned char * pOutData = (unsigned char *)m_pBitmapData;

		JSAMPLE *buffer;
		int row_stride = cinfo.output_width * cinfo.output_components;
		if ((buffer = (JSAMPLE *)malloc(row_stride * sizeof(JSAMPLE))) == NULL) 
			return false;


		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo,&buffer,1);
			if (cinfo.output_components == 3)
			{
				int y = 0;
				for (int i=0; i<row_stride; i+=3)
				{
					pOutData[y] = buffer[i+2];
					pOutData[y+1] = buffer[i+1];
					pOutData[y+2] = buffer[i];
					pOutData[y+3] = 255;
					y += 4;
				}
			}
			else
			{
				int y = 0;
				for (int i=0; i<row_stride; i++)
				{
					pOutData[y] = buffer[i];
					pOutData[y+1] = buffer[i];
					pOutData[y+2] = buffer[i];
					pOutData[y+3] = 255;
					y += 4;
				}
			}
			pOutData += m_ulBitmapWidth * 4;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		free(buffer);
		fclose(imageFile);
		imageFile = NULL;

		return true;
	}

	if( StrStrI(szFilename, TEXT(".png")) )
	{
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

		return SetSource(&buffer[0], size, TEXT("image/png"));
	}

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
