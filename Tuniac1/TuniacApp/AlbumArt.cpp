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


class jpeg_decoder_memory_stream : public jpeg_decoder_stream
{
  LPVOID			m_jPegData;
  unsigned long		m_Size;
  unsigned long		m_Offset;
  bool eof_flag, error_flag;

public:

  jpeg_decoder_memory_stream(LPVOID pMemory, unsigned long ulSize)
  {
    m_jPegData	= pMemory;
	m_Size		= ulSize;
	m_Offset	= 0;
    eof_flag	= false;
    error_flag	= false;
  }

  virtual ~jpeg_decoder_memory_stream()
  {
  }
 
  virtual int read(uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag)
  {
		if (eof_flag)
		{
		  *Peof_flag = true;
		  return (0);
		}

		if (error_flag)
			return (-1);

		unsigned long bytesthistime = min(max_bytes_to_read, m_Size - m_Offset);

		CopyMemory(Pbuf, ((unsigned char*)m_jPegData)+m_Offset, bytesthistime);
		m_Offset += bytesthistime;

		if(m_Offset >= m_Size)
		{
			eof_flag = true;
			*Peof_flag = true;
		}

		return (bytesthistime);
	}
};

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

bool	CAlbumArt::LoadJpegData(jpeg_decoder_stream & input_stream)
{
	//even with use_mmx set to true, under x64 it will be ignored and set to false due to asm requirement
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

	return true;
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

bool CAlbumArt::SetSource(LPVOID pData, unsigned long ulDataLength, LPTSTR szMimeType)
{
	CAutoLock lock(&m_ArtLock);

	if( StrStrI(szMimeType, TEXT("image/jpeg")) )
	{
		jpeg_decoder_memory_stream	input_stream(pData, ulDataLength);

		if(!LoadJpegData(input_stream))
			return false;

		return true;
	}

	if( StrStrI(szMimeType, TEXT("image/png")) )
	{
		//TODO: PNG HERE PLSKTHX
		std::vector<unsigned char> imageData;

		// fills out our std::vector as a 32bit ARGB data stream
		unsigned int w, h;
		if(decodePNG(	imageData,
					w,
					h,
					(const unsigned char *)pData,
					ulDataLength) == 0)
		{
			m_ulBitmapWidth		= w;
			m_ulBitmapHeight	= h;
			m_ulBytesPerPixel	= 4;
			m_ulComponents		= 4;

			if(m_pBitmapData)
			{
				free(m_pBitmapData);
				m_pBitmapData = NULL;
			}

			m_pBitmapData = malloc(m_ulBitmapWidth * m_ulBitmapHeight * m_ulBytesPerPixel);
			if(!m_pBitmapData)
				return false;

			//CopyMemory(m_pBitmapData, &imageData[0], imageData.size());
			unsigned char * pData	= (unsigned char *)m_pBitmapData;
			unsigned char * pSrc	= (unsigned char *)&imageData[0];
			for(int x=0; x<imageData.size(); x+= m_ulBytesPerPixel)
			{
				pData[x+0]	= pSrc[x+2];
				pData[x+1]	= pSrc[x+1];
				pData[x+2]	= pSrc[x+0];
				pData[x+3]	= pSrc[x+3];
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
		bool bRet = false;

		jpeg_decoder_file_stream input_stream;
		if (input_stream.open(szFilename))
		{
			return (false);
		}

		bRet = LoadJpegData(input_stream);

		input_stream.close();

		return bRet;
	}

	if( StrStrI(szFilename, TEXT(".png")) )
	{
		//TODO: PNG HERE PLSKTHX
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
