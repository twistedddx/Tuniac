#pragma once

#include "iaudiosource.h"

class CFLACAudioSource :
	public IAudioSource
{
public:
	CFLACAudioSource(void);
	~CFLACAudioSource(void);

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
