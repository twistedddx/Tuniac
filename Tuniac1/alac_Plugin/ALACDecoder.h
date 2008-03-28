#pragma once

#include "iaudiosource.h"
#include <stdio.h>

#include ".\alac_decoder\demux.h"
#include ".\alac_decoder\decomp.h"
#include ".\alac_decoder\stream.h"

class CALACDecoder :
	public IAudioSource
{
protected:
	FILE			*	m_file;
	float				m_Buffer[4096];
	float				m_divider;


	/* all things for alac decoder */
	int decoded_frames, outputBytes;
	unsigned long song_length;
	alac_file *alac;
	demux_res_t demux;
	FILE *file;
	stream_t *stream;
	unsigned char *buffer;
	unsigned char *stream_buffer;

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