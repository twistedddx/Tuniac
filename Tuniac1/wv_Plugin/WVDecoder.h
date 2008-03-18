#pragma once

#include "iaudiosource.h"
#include "wavpack.h"

class CWVDecoder :
	public IAudioSource
{
protected:
	float			*	m_Buffer;

	WavpackContext *wpc;
	int SAMPRATE;


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
