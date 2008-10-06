#include "StdAfx.h"
#include "FLACAudioSource.h"


//#define 8BitToFloat(char x) 

void OurDecoder::SetSource(IAudioFileIO	* pIO)
{
	if(m_pAudioIO)
	{
		m_pAudioIO->Destroy();
		m_pAudioIO = NULL;
	}

	m_pAudioIO = pIO;
}

void OurDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
	fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

void OurDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) 
	{
		ulTotalSamples	= metadata->data.stream_info.total_samples;
		ulSampleRate	= metadata->data.stream_info.sample_rate;
		ulChannels		= metadata->data.stream_info.channels;
		ulBitsPerSample	= metadata->data.stream_info.bits_per_sample;
	}
}

::FLAC__StreamDecoderReadStatus OurDecoder::read_callback(FLAC__byte buffer[], size_t *bytes)
{
	unsigned __int64 bytesRead;
	if(!m_pAudioIO->Read(*bytes, buffer, &bytesRead))
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;

	if(bytesRead == 0)
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

	*bytes = bytesRead;

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

::FLAC__StreamDecoderSeekStatus OurDecoder::seek_callback(FLAC__uint64 absolute_byte_offset)
{
	m_pAudioIO->Seek(absolute_byte_offset, false);
	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

::FLAC__StreamDecoderTellStatus OurDecoder::tell_callback(FLAC__uint64 *absolute_byte_offset)
{
	m_pAudioIO->Tell(absolute_byte_offset);
	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

::FLAC__StreamDecoderLengthStatus OurDecoder::length_callback(FLAC__uint64 *stream_length)
{
	m_pAudioIO->Size(stream_length);
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

bool OurDecoder::eof_callback()
{
	return m_pAudioIO->IsEOF();
}


::FLAC__StreamDecoderWriteStatus OurDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
	if(ulTotalSamples == 0) 
	{
		//fprintf(stderr, "ERROR: this example only works for FLAC files that have a total_samples count in STREAMINFO\n");
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	/* write decoded PCM samples */
	for(int i = 0; i < frame->header.blocksize; i++) 
	{
		for(int channel=0; channel<ulChannels; channel++)
		{
			if(ulBitsPerSample == 8)
			{
				m_AudioBuffer[(i*ulChannels)+channel]	= (float)(FLAC__int8)buffer[channel][i] / 128.0f;
			}
			else if(ulBitsPerSample == 16)
			{
				m_AudioBuffer[(i*ulChannels)+channel]	= (float)(FLAC__int16)buffer[channel][i] / 32767.0f;
			}
			else if(ulBitsPerSample == 24)
			{
				m_AudioBuffer[(i*ulChannels)+channel]	= (float)(FLAC__int32)buffer[channel][i] / 8388608.0f;
			}
			else if(ulBitsPerSample == 32)
			{
				m_AudioBuffer[(i*ulChannels)+channel]	= (float)((double)(FLAC__int32)buffer[channel][i] / 2147483648.0);
			}

		}
	}

	m_ulLastDecodedBufferSize = frame->header.blocksize * ulChannels;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}



CFLACAudioSource::CFLACAudioSource(void)
{
}

CFLACAudioSource::~CFLACAudioSource(void)
{
}

bool		CFLACAudioSource::Open(IAudioFileIO * pFileIO)
{
//TODO: replace this with FILE * fp = fopen() code, which supports unicode, cos wcstomb isn't going to cut it!


	m_FileDecoder.SetSource(pFileIO);

	::FLAC__StreamDecoderInitStatus		flacStat;

	flacStat = m_FileDecoder.init();
	if(flacStat != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return false;

	m_FileDecoder.process_until_end_of_metadata();

	return true;
}

void		CFLACAudioSource::Destroy(void)
{
	m_FileDecoder.reset();
	if(m_FileDecoder.get_state() != FLAC__STREAM_DECODER_UNINITIALIZED)
	{
		m_FileDecoder.finish();
	}

	m_FileDecoder.SetSource(NULL);

	delete this;
}

bool		CFLACAudioSource::GetLength(unsigned long * MS)
{
	unsigned long samplesperms = 	m_FileDecoder.ulSampleRate / 1000;

	*MS = (unsigned long)((double)m_FileDecoder.ulTotalSamples / (double)samplesperms);

	return true;
}

bool		CFLACAudioSource::SetPosition(unsigned long * MS)
{
	unsigned long samplesperms = 	m_FileDecoder.ulSampleRate / 1000;

	unsigned __int64 pos = *MS * samplesperms;

	m_FileDecoder.seek_absolute(pos);

	return true;
}

bool		CFLACAudioSource::SetState(unsigned long State)
{
	return false;
}

bool		CFLACAudioSource::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*Channels	= m_FileDecoder.ulChannels;
	*SampleRate = m_FileDecoder.ulSampleRate;
	return true;
}

bool		CFLACAudioSource::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	if(m_FileDecoder.get_state() == FLAC__STREAM_DECODER_END_OF_STREAM)
	{
		*NumSamples = 0;
		return false;
	}


	if(!m_FileDecoder.process_single())
		return false;

	*ppBuffer		= m_FileDecoder.m_AudioBuffer;
	*NumSamples		= m_FileDecoder.m_ulLastDecodedBufferSize;

	return true;
}



/*


*/