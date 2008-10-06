#pragma once

#include "iaudiosource.h"
#include "FLAC++/decoder.h"

class OurDecoder: public FLAC::Decoder::Stream
{
public:
	OurDecoder() : 
	  FLAC::Decoder::Stream()
	{ 
		m_pAudioIO = NULL;
	}

	unsigned long		ulSampleRate;
	unsigned long		ulChannels;
	unsigned long		ulBitsPerSample;

	unsigned __int64	ulTotalSamples;

	float				m_AudioBuffer[FLAC__MAX_BLOCK_SIZE];
	unsigned long		m_ulLastDecodedBufferSize;

	void				SetSource(IAudioFileIO * pIO);


protected:
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);

	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes);
	virtual ::FLAC__StreamDecoderSeekStatus seek_callback(FLAC__uint64 absolute_byte_offset);
	virtual ::FLAC__StreamDecoderTellStatus tell_callback(FLAC__uint64 *absolute_byte_offset);
	virtual ::FLAC__StreamDecoderLengthStatus length_callback(FLAC__uint64 *stream_length);
	virtual bool eof_callback();

	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);

	IAudioFileIO * m_pAudioIO;
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
	bool		Open(IAudioFileIO * pFileIO);

public:	
	void		Destroy(void);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
