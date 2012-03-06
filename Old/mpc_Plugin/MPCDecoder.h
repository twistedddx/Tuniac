#pragma once

#include "iaudiosource.h"
#include "mpcdec/mpcdec.h"

class CMPCDecoder :
	public IAudioSource
{
protected:
	float				m_Buffer[MPC_DECODER_BUFFER_LENGTH];

	FILE			*	m_file;

	mpc_streaminfo		si;

    mpc_reader_file		reader;
    mpc_decoder			decoder;

	mpc_int64_t			m_LengthSamples;


public:
	CMPCDecoder(void);
	~CMPCDecoder(void);

	bool Open(LPTSTR szSource);

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
