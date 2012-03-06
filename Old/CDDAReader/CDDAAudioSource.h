#pragma once
#include "iaudiosource.h"

#include "ntddcdrm.h"
#include "devioctl.h"

#define BUF_SIZE 4096

class CCDDAAudioSource :
	public IAudioSource
{
protected:

	HANDLE			m_hDrive;
	CDROM_TOC		m_TOC;
	UINT			m_nFirstSector, 
					m_nStartSector, 
					m_nStopSector;

	UINT			m_nCurrentSector;

	unsigned long	m_Channels;

	float			m_Buffer[BUF_SIZE];

	bool Read(PBYTE pbBuffer, LPDWORD pdwBytesRead);


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
