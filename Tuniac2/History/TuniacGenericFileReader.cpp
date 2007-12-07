#include "stdafx.h"
#include ".\tuniacgenericfilereader.h"

CTuniacGenericFileReader::CTuniacGenericFileReader(void)
{
	m_hFile = NULL;
}

CTuniacGenericFileReader::~CTuniacGenericFileReader(void)
{
}

void			CTuniacGenericFileReader::Destroy(void)
{
}

LPTSTR			CTuniacGenericFileReader::GetName(void)
{
	return TEXT("Tuniac Generic File Reader");
}

GUID			CTuniacGenericFileReader::GetPluginID(void)
{
// {F03DCD41-4948-4fab-99C5-78288CCA89BC}
static const GUID GUID_GENERICFILEREADER = { 0xf03dcd41, 0x4948, 0x4fab, { 0x99, 0xc5, 0x78, 0x28, 0x8c, 0xca, 0x89, 0xbc } };

	return GUID_GENERICFILEREADER;
}

unsigned long	CTuniacGenericFileReader::GetFlags(void)
{
	return PLUGINFLAGS_ISUNICODE;
}

void			CTuniacGenericFileReader::SetHelper(ITuniacPluginHelper * pHelper)
{
}

bool			CTuniacGenericFileReader::CanHandle(LPTSTR szFilename, unsigned long * Merit)
{
	*Merit = 0;

	HANDLE hTmp = CreateFile(	szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hTmp != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hTmp);
		return true;
	}

	return false;
}

bool			CTuniacGenericFileReader::Open(LPTSTR szFilename)
{
	m_hFile = CreateFile(	szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		return true;
	}

	return false;
}

bool			CTuniacGenericFileReader::Close(void)
{
	if(m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;

		return true;
	}

	return false;
}

bool			CTuniacGenericFileReader::Read(LPVOID pData, unsigned long ulBytesToRead, unsigned long * ulBytesRead)
{
	if(m_hFile)
	{	
		unsigned long ulRead;

		BOOL Worked = ReadFile(m_hFile, pData, ulBytesToRead, &ulRead, NULL);
		if(Worked)
		{
			if(ulBytesRead)
			{
				*ulBytesRead = ulRead;
			}

			return true;
		}
	}
	return false;
}

bool			CTuniacGenericFileReader::SetPosition(unsigned long ulNewPosition, unsigned long * ulOldPosition)
{
	if(m_hFile)
	{
		unsigned long OldPos = SetFilePointer(m_hFile, ulNewPosition, NULL, FILE_BEGIN);
		if(OldPos != INVALID_SET_FILE_POINTER)
		{
			if(ulOldPosition)
				*ulOldPosition = OldPos;

			return true;
		}
	}
	return false;
}

bool			CTuniacGenericFileReader::GetLength(unsigned long * ulLength)
{
	if(m_hFile)
	{
		unsigned long Length = GetFileSize(m_hFile, NULL);

		if(ulLength)
		{
			*ulLength = Length;
			return true;
		}

	}
	return false;
}
