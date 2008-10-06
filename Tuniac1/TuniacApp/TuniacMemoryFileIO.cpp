#include "stdafx.h"

#include "TuniacMemoryFileIO.h"

CTuniacMemoryFileIO::CTuniacMemoryFileIO(void) : m_hFile(NULL), m_hMappingObject(NULL), m_pMemory(NULL), m_ullOffset(0), m_bEOF(false)
{
}

CTuniacMemoryFileIO::~CTuniacMemoryFileIO(void)
{
}

bool CTuniacMemoryFileIO::Open(LPTSTR szFilename)
{
	m_hFile = CreateFile(	szFilename, 
							GENERIC_READ, 
							FILE_SHARE_READ, NULL, 
							OPEN_EXISTING, 
							FILE_FLAG_SEQUENTIAL_SCAN,
							NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	LARGE_INTEGER	li;
	GetFileSizeEx(m_hFile, &li);
	m_ullFileSize = li.QuadPart;

	m_hMappingObject = CreateFileMapping(	m_hFile,
											NULL,
											PAGE_READONLY,
											0,
											0,
											NULL);

	if(!m_hMappingObject)
		return false;

	m_pMemory = (LPBYTE)MapViewOfFileEx(m_hMappingObject,
										FILE_MAP_READ,
										0,
										0,
										0,
										NULL);

	if(!m_pMemory)
		return false;


	return true;
}

void CTuniacMemoryFileIO::Destroy(void)
{
	if(m_pMemory)
	{
		UnmapViewOfFile(m_pMemory);
	}

	if(m_hMappingObject)
	{
		CloseHandle(m_hMappingObject);
	}

	if(m_hFile)
	{
		CloseHandle(m_hFile);
	}

	delete this;
}

bool CTuniacMemoryFileIO::Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead)
{
	__int64 ullBytesThisTime = 0;

	if(m_bEOF)
	{
		*pBytesRead = 0;
		return true;
	}

	if(m_ullOffset + numberofbytes > m_ullFileSize)
	{
		numberofbytes = m_ullFileSize - m_ullOffset;
	}

	if(numberofbytes)
	{
		CopyMemory(pData, m_pMemory + m_ullOffset, numberofbytes);

		m_ullOffset +=	numberofbytes;
	}

	*pBytesRead	=	numberofbytes;

	if(m_ullOffset == m_ullFileSize)
	{
		m_bEOF = true;
	}

	return true;
}

bool CTuniacMemoryFileIO::Seek(unsigned __int64 Offset, bool bFromEnd)
{
	if(bFromEnd)
	{
		m_ullOffset = m_ullFileSize - Offset;
	}
	else
	{
		m_ullOffset = Offset;
	}

	return true;
}

void CTuniacMemoryFileIO::Tell(unsigned __int64 * pCurrentOffset)
{
	*pCurrentOffset = m_ullOffset;
}

void CTuniacMemoryFileIO::Size(unsigned __int64 * pSize)
{
	*pSize = m_ullFileSize;
}

bool CTuniacMemoryFileIO::IsEOF(void)
{
	return m_bEOF;
}
