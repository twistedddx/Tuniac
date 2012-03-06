#pragma once
#include "iaudiosource.h"

class Cm4aAudioSource : public IAudioSource
{
protected:

	NeAACDecHandle				m_hDecoder;
    mp4ff_t				*		m_mp4file;
    FILE				*		m_mp4FileHandle;
	mp4ff_callback_t			m_mp4cb;
    int							m_mp4track;
    long						m_numSamples;
    long						m_sampleId;

	unsigned long				m_SampleRate;
	unsigned char				m_NumChannels;

public:
	Cm4aAudioSource(void);
	~Cm4aAudioSource(void);

public:
	bool Open(LPTSTR szSource);

public:
	void		Destroy(void);

	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);

	bool		SetState(unsigned long State);

	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);

	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
