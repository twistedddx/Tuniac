#pragma once

#include "iaudiosource.h"
#include "FLAC++/decoder.h"

class OurDecoder: public FLAC::Decoder::File 
{
public:
	OurDecoder() : 
		FLAC::Decoder::File()
	{ 
	}

	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	unsigned long		ulBitsPerSample;

	unsigned __int64	ulTotalSamples;

	float				m_AudioBuffer[FLAC__MAX_BLOCK_SIZE];
	unsigned long		m_ulLastDecodedBufferSize;


protected:
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
};


class CFLACAudioSource :
	public IAudioSource
{
private:
	OurDecoder		m_FileDecoder;


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
