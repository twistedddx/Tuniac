#include "stdafx.h"

#include "TuniacStandardFileIO.h"

CTuniacStandardFileIO::CTuniacStandardFileIO(void) : m_hFile(NULL), m_ullOffset(0), m_bEOF(false)
{
}

CTuniacStandardFileIO::~CTuniacStandardFileIO(void)
{
}

bool CTuniacStandardFileIO::Open(LPTSTR szFilename)
{
    m_hFile = _wfopen(szFilename, TEXT("rbS"));
    if(m_hFile == NULL)
		return false;

	fseek(m_hFile, 0, SEEK_END);
	m_ullFileSize = ftell(m_hFile);
	fseek(m_hFile, 0, SEEK_SET);

	return true;
}

void CTuniacStandardFileIO::Destroy(void)
{
	delete this;
}

bool CTuniacStandardFileIO::Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead)
{
	int read = fread(pData, 1, numberofbytes, m_hFile);

	m_ullOffset +=	read;
	*pBytesRead	=	read;

	if(m_ullOffset >= m_ullFileSize)
	{
		m_bEOF = true;
	}

	return true;
}

bool CTuniacStandardFileIO::Seek(unsigned __int64 Offset, bool bFromEnd)
{
	if(bFromEnd)
	{
		fseek(m_hFile, m_ullFileSize - Offset, SEEK_SET);
	}
	else
	{
		fseek(m_hFile, Offset, SEEK_SET);
	}

	return true;
}

void CTuniacStandardFileIO::Tell(unsigned __int64 * pCurrentOffset)
{
	*pCurrentOffset = m_ullOffset;
}

void CTuniacStandardFileIO::Size(unsigned __int64 * pSize)
{
	*pSize = m_ullFileSize;
}

bool CTuniacStandardFileIO::IsEOF(void)
{
	return m_bEOF;
}
