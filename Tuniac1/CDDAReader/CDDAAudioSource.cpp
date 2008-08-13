#include "StdAfx.h"
#include ".\cddaaudiosource.h"

#define RAW_SECTOR_SIZE 2352
#define MSF2UINT(hgs) ((hgs[1]*4500)+(hgs[2]*75)+(hgs[3]))

CCDDAAudioSource::CCDDAAudioSource(void)
{
	m_hDrive = INVALID_HANDLE_VALUE;


	memset(&m_TOC, 0, sizeof(m_TOC));
	m_nStartSector = m_nStopSector = 0;

}

CCDDAAudioSource::~CCDDAAudioSource(void)
{
	if(m_hDrive != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDrive);
		m_hDrive = INVALID_HANDLE_VALUE;
	}
}

bool		CCDDAAudioSource::Open(LPTSTR szStream)
{
	TCHAR drive[32] = {0};
	int iTrackIndex;

	StrCpy(	drive, TEXT("\\\\.\\"));
	StrCatN(drive, &szStream[8], 2);
	StrCat(	drive, TEXT(":"));

	m_hDrive = CreateFile(	drive, 
							GENERIC_READ, 
							FILE_SHARE_READ, 
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_READONLY|FILE_FLAG_SEQUENTIAL_SCAN, 
							(HANDLE)NULL);

	if(m_hDrive == INVALID_HANDLE_VALUE)
	{
		return(false);
	}

	DWORD cbReturned;//discard results
	PREVENT_MEDIA_REMOVAL pmr;
	pmr.PreventMediaRemoval = TRUE;
	DeviceIoControl(m_hDrive, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &cbReturned, NULL);
	
	iTrackIndex = _wtoi(&szStream[10]);

	DWORD BytesReturned;
	if(!DeviceIoControl(	m_hDrive, 
							IOCTL_CDROM_READ_TOC, 
							NULL, 
							0, 
							&m_TOC, 
							sizeof(m_TOC), 
							&BytesReturned, 
							0)
	|| !(m_TOC.FirstTrack <= iTrackIndex && iTrackIndex <= m_TOC.LastTrack))
	{
		CloseHandle(m_hDrive);
		m_hDrive = INVALID_HANDLE_VALUE;
		return(false);
	}

	// MMC-3 Draft Revision 10g: Table 222 – Q Sub-channel control field
	m_TOC.TrackData[iTrackIndex-1].Control &= 5;
	if(!(m_TOC.TrackData[iTrackIndex-1].Control == 0 || m_TOC.TrackData[iTrackIndex-1].Control == 1))
	{
		CloseHandle(m_hDrive);
		m_hDrive = INVALID_HANDLE_VALUE;
		return(false);
	}

	m_Channels = 2;
	if(m_TOC.TrackData[iTrackIndex-1].Control&8) 
		m_Channels = 4;

	m_nStartSector		= MSF2UINT(m_TOC.TrackData[iTrackIndex-1].Address) - 150;//MSF2UINT(m_TOC.TrackData[0].Address);
	m_nStopSector		= MSF2UINT(m_TOC.TrackData[iTrackIndex].Address) - 150;//MSF2UINT(m_TOC.TrackData[0].Address);
	m_nCurrentSector	= 0;

	return true;
}


void		CCDDAAudioSource::Destroy(void)
{
	DWORD cbReturned;//discard results
	PREVENT_MEDIA_REMOVAL pmr;
	pmr.PreventMediaRemoval = false;
	DeviceIoControl(m_hDrive, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &cbReturned, NULL);
	delete this;
}

bool		CCDDAAudioSource::GetLength(unsigned long * MS)
{
	*MS = (unsigned long)(((float)((m_nStopSector - m_nStartSector) * RAW_SECTOR_SIZE)/ (float)((44100 * 2) * m_Channels)) * 1000.0f);
	return true;
}

bool		CCDDAAudioSource::SetPosition(unsigned long * MS)
{
	/*
	m_llPosition = (((float)((44100 * 2) * m_Channels)) / 1000.0f) * (*MS);

	while(m_llPosition % (2 * m_Channels))
		m_llPosition ++;
	return true;
	*/

	return false;
}

bool		CCDDAAudioSource::SetState(unsigned long State)
{
	return true;
}

bool		CCDDAAudioSource::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate		= 44100;
	*Channels		= m_Channels;
	return true;
}

bool CCDDAAudioSource::Read(PBYTE pbBuffer, LPDWORD pdwBytesRead)
{
	BYTE buff[RAW_SECTOR_SIZE]	= {0};
	RAW_READ_INFO rawreadinfo	= {0};
	unsigned long BytesReturned = 0;

	rawreadinfo.SectorCount			= 1;
	rawreadinfo.TrackMode			= CDDA;
	rawreadinfo.DiskOffset.QuadPart = (m_nStartSector + m_nCurrentSector)*2048;

	BOOL b = DeviceIoControl(	m_hDrive, 
								IOCTL_CDROM_RAW_READ,
								&rawreadinfo, 
								sizeof(rawreadinfo),
								pbBuffer, 
								RAW_SECTOR_SIZE,
								&BytesReturned, 
								0);

	if(b)
	{
		m_nCurrentSector++;
		*pdwBytesRead = BytesReturned;
		return true;
	}

	return false;
}

bool		CCDDAAudioSource::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	unsigned long	ulNumIO = 0;
	BYTE			buffer[BUF_SIZE];

	if((m_nStartSector + m_nCurrentSector) > m_nStopSector)
		return false;

	if(Read(buffer, &ulNumIO))
	{
		TCHAR	tstr[1024];
		wsprintf(tstr, TEXT("Read worked (%d bytes)\r\n"), ulNumIO);
		OutputDebugString(tstr);

		short * pData = (short*)buffer;

		for(int x=0; x<(ulNumIO/2); x++)
		{
			m_Buffer[x] = (float)pData[x] / 16384.0f;
		}
		*ppBuffer	= m_Buffer;
		*NumSamples = (ulNumIO/2);

		return true;
	}

	return false;

}

/*
	PBYTE pbBufferOrg = pbBuffer;
	LONGLONG pos = m_llPosition;
	size_t len = RAW_SECTOR_SIZE;

	RAW_READ_INFO rawreadinfo = {0};

	rawreadinfo.SectorCount = 1;
	rawreadinfo.TrackMode = CDDA;

	UINT sector = m_nStartSector + int(pos/RAW_SECTOR_SIZE);
	__int64 offset = pos%RAW_SECTOR_SIZE;
	rawreadinfo.DiskOffset.QuadPart = sector*2048;

	DWORD BytesReturned = 0;

	bool b = DeviceIoControl(	m_hDrive, 
								IOCTL_CDROM_RAW_READ,
								&rawreadinfo, 
								sizeof(rawreadinfo),
								buff, 
								RAW_SECTOR_SIZE,
								&BytesReturned, 
								0);
	if(!b)
	{
		TCHAR	tstr[1024];
		wsprintf(tstr, TEXT("DeviceIoControl FAILED\r\n"));
		OutputDebugString(tstr);
		return (E_FAIL);
	}

	size_t l = (size_t)min(min(len, RAW_SECTOR_SIZE - offset), m_llLength - pos);
	memcpy(pbBuffer, &buff[offset], l);


	if(pdwBytesRead) 
		*pdwBytesRead = pbBuffer - pbBufferOrg;
	m_llPosition += pbBuffer - pbBufferOrg;


	return S_OK;

	*/