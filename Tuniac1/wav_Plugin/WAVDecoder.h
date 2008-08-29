#pragma once

#include "iaudiosource.h"
#include <stdio.h>
#include <mmsystem.h>

#define BUF_SIZE 4096


class CWAVDecoder :
	public IAudioSource
{
protected:
	FILE			*	m_file;
	float				m_Buffer[BUF_SIZE];
	float				m_divider;

	HMMIO hWav;
	WAVEFORMATEX wfmex;
	MMCKINFO child, parent;

	unsigned long Read;
	unsigned long ulChunkSize;
	unsigned long ulLenthMS;

public:
	CWAVDecoder(void);
	~CWAVDecoder(void);

	bool Open(LPTSTR szSource);
	bool Close();

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};