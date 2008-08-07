#pragma once
#include "iaudiosource.h"

#include "ntddcdrm.h"
#include "devioctl.h"

#define BUF_SIZE 4096

class CCDDAAudioSource :
	public IAudioSource
{
protected:
	LONGLONG		m_llPosition, 
					m_llLength;

	HANDLE			m_hDrive;
	CDROM_TOC		m_TOC;
	UINT			m_nFirstSector, 
					m_nStartSector, 
					m_nStopSector;

	unsigned long	m_Channels;

	float			m_Buffer[BUF_SIZE];
	BYTE			buffer[BUF_SIZE];

	HRESULT Read(PBYTE pbBuffer, DWORD dwBytesToRead, BOOL bAlign, LPDWORD pdwBytesRead);


public:
	CCDDAAudioSource(void);
	~CCDDAAudioSource(void);

public:
	bool		Open(LPTSTR szStream);

public:	
	void		Destroy(void);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
