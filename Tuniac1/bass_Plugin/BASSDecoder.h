#pragma once

#include "iaudiosource.h"
#include <stdio.h>
#include "bass.h"

#define BUFFERSIZE (1024 * sizeof(float))

class CBASSDecoder :
	public IAudioSource
{
protected:

	FILE			*	m_file;

	float			*	m_Buffer;

	bool				bIsStream;
	double				dTime;
	bool				bModFile;
	BASS_CHANNELINFO	info;

	HSTREAM				decodehandle;

public:
	CBASSDecoder(void);
	~CBASSDecoder(void);

	bool Open(LPTSTR szSource, IAudioSourceHelper * pHelper);
	bool Close();

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
