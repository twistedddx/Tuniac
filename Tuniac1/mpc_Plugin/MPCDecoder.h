#pragma once

#include "iaudiosource.h"
#include "mpc\mpcdec.h"

class CMPCDecoder :
	public IAudioSource
{
protected:
	float			*	m_Buffer;

    mpc_reader reader;
	mpc_demux* demux;
	mpc_streaminfo si;
	mpc_status err;

	FILE			*	m_file;

	unsigned int		m_currentsection;


public:
	CMPCDecoder(void);
	~CMPCDecoder(void);

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
