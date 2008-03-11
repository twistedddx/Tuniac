#include "StdAfx.h"
#include "FLACAudioSource.h"

static FLAC__uint64 total_samples = 0;
static unsigned sample_rate = 0;
static unsigned channels = 0;
static unsigned bps = 0;

const char * const FLAC__StreamDecoderErrorStatusString[] = 
{
	"FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC",
	"FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER",
	"FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH",
	"FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM"
};


void OurDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
	fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

::FLAC__StreamDecoderWriteStatus OurDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
	const FLAC__uint32 total_size = (FLAC__uint32)(total_samples * channels * (bps/8));
	size_t i;

	if(total_samples == 0) 
	{
		fprintf(stderr, "ERROR: this example only works for FLAC files that have a total_samples count in STREAMINFO\n");
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}
	if(channels != 2 || bps != 16) 
	{
		fprintf(stderr, "ERROR: this example only supports 16bit stereo streams\n");
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	/* write WAVE header before we write the first frame */
	if(frame->header.number.sample_number == 0) 
	{
		/*
		if(
			fwrite("RIFF", 1, 4, f) < 4 ||
			!write_little_endian_uint32(f, total_size + 36) ||
			fwrite("WAVEfmt ", 1, 8, f) < 8 ||
			!write_little_endian_uint32(f, 16) ||
			!write_little_endian_uint16(f, 1) ||
			!write_little_endian_uint16(f, (FLAC__uint16)channels) ||
			!write_little_endian_uint32(f, sample_rate) ||
			!write_little_endian_uint32(f, sample_rate * channels * (bps/8)) ||
			!write_little_endian_uint16(f, (FLAC__uint16)(channels * (bps/8))) || // block align
			!write_little_endian_uint16(f, (FLAC__uint16)bps) ||
			fwrite("data", 1, 4, f) < 4 ||
			!write_little_endian_uint32(f, total_size)
		) 
		{
			fprintf(stderr, "ERROR: write error\n");
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
			*/
	}

	/* write decoded PCM samples */
	for(i = 0; i < frame->header.blocksize; i++) 
	{
//		if(
//			!write_little_endian_int16(f, (FLAC__int16)buffer[0][i]) ||  /* left channel */
//			!write_little_endian_int16(f, (FLAC__int16)buffer[1][i])     /* right channel */
//		) 
		{
			fprintf(stderr, "ERROR: write error\n");
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}



CFLACAudioSource::CFLACAudioSource(void) : 
	m_ulLastDecodedBufferSize(0)
{
}

CFLACAudioSource::~CFLACAudioSource(void)
{
}

bool		CFLACAudioSource::Open(LPTSTR szStream)
{
	char tempname[_MAX_PATH];

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8,
						0,
						szStream,
						-1,
						tempname,
						_MAX_PATH,
						NULL,
						NULL);
#endif

	if(m_FileDecoder.init(tempname) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return false;

	return true;
}

void		CFLACAudioSource::Destroy(void)
{
	m_FileDecoder.reset();
	delete this;
}

bool		CFLACAudioSource::GetLength(unsigned long * MS)
{
	unsigned long samplesperms = 	m_FileDecoder.get_sample_rate() / 1000;

	unsigned __int64 length = m_FileDecoder.get_total_samples() / m_FileDecoder.get_channels();

	*MS = length / samplesperms;

	return true;
}

bool		CFLACAudioSource::SetPosition(unsigned long * MS)
{
	unsigned long samplesperms = 	m_FileDecoder.get_sample_rate() / 1000;

	unsigned __int64 pos = *MS * (samplesperms * m_FileDecoder.get_channels());

	m_FileDecoder.seek_absolute(pos);

	return true;
}

bool		CFLACAudioSource::SetState(unsigned long State)
{
	return false;
}

bool		CFLACAudioSource::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*Channels	= m_FileDecoder.get_channels();
	*SampleRate = m_FileDecoder.get_sample_rate();
	return true;
}

bool		CFLACAudioSource::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	m_FileDecoder.process_single();

	*ppBuffer		= m_AudioBuffer;
	*NumSamples		= m_ulLastDecodedBufferSize;

	return true;
}



/*
void OurDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) 
	{
		total_samples = metadata->data.stream_info.total_samples;
		sample_rate = metadata->data.stream_info.sample_rate;
		channels = metadata->data.stream_info.channels;
		bps = metadata->data.stream_info.bits_per_sample;

		fprintf(stderr, "sample rate    : %u Hz\n", sample_rate);
		fprintf(stderr, "channels       : %u\n", channels);
		fprintf(stderr, "bits per sample: %u\n", bps);
#ifdef _MSC_VER
		fprintf(stderr, "total samples  : %I64u\n", total_samples);
#else
		fprintf(stderr, "total samples  : %llu\n", total_samples);
#endif
	}
}


*/