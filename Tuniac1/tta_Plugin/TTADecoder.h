#pragma once

#include "iaudiosource.h"
#include <stdio.h>
extern "C" {
#include ".\TrueAudio\ttalib.h"
}
class CTTADecoder :
	public IAudioSource
{
protected:
	unsigned char		*	buffer;
	float			m_Buffer[4096*4*6];
	tta_info info;
	long samples_read;
	float			m_divider;

public:
	CTTADecoder(void);
	~CTTADecoder(void);

;
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