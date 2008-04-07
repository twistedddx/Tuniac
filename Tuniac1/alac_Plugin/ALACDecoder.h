#pragma once

#include "iaudiosource.h"

extern "C" {

#include ".\alac_decoder\demux.h"
#include ".\alac_decoder\decomp.h"
#include ".\alac_decoder\stream.h"

}
class CALACDecoder :
	public IAudioSource
{
protected:
	FILE			*	m_file;
	float				m_Buffer[4096*4];
	float				m_divider;
	unsigned char		buffer[4096*4];
	unsigned char		stream_buffer[4096*4*4];

	/* all things for alac decoder */
	int decoded_frames;
	int outputBytes;
	alac_file *alac;
	demux_res_t demux;
	stream_t *stream;


public:
	CALACDecoder(void);
	~CALACDecoder(void);

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