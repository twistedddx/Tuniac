#pragma once

#include "iaudiosource.h"
#include "./TAK/tak_deco_lib.h"


class CTAKDecoder :
	public IAudioSource
{
protected:
	char			buffer[4096*4*4];
	float			m_Buffer[4096*4*4];
	float			m_divider;

	TtakSSDOptions				Options;
	TtakSeekableStreamDecoder	Decoder;
	Ttak_str_StreamInfo			StreamInfo;
	TtakInt32					ReadNum;
	TtakResult					OpResult;
	TtakInt32					SamplesPerBuf;

public:
	CTAKDecoder(void);
	~CTAKDecoder(void);

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