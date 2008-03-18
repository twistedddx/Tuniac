#pragma once

#include "iaudiosource.h"
#include "wavpack.h"

class CWVDecoder :
	public IAudioSource
{
protected:

	long				m_RawData[4096];
	float				m_Buffer[4096];

	bool				m_bFloatMode;

	WavpackContext *wpc;
	unsigned long ulSampleRate;
	unsigned long ulChannels;
	unsigned long ulSamples;
	unsigned long ulBitsPerSample;
	unsigned long ulBytesPerSample;



public:
	CWVDecoder(void);
	~CWVDecoder(void);

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
