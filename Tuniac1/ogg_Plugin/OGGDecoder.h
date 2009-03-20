#pragma once

#include "iaudiosource.h"
#include "vorbis\codec.h"
#include "vorbis\vorbisfile.h"

class COGGDecoder :
	public IAudioSource
{
protected:
	float			*	m_Buffer;

	OggVorbis_File		m_vf;
	FILE			*	m_file;

	unsigned int		m_currentsection;


public:
	COGGDecoder(void);
	~COGGDecoder(void);

	bool Open(LPTSTR szSource);

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
