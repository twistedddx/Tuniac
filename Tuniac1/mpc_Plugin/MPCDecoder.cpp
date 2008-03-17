#include "StdAfx.h"
#include "mpcdecoder.h"

CMPCDecoder::CMPCDecoder(void)
{
	m_currentsection = 0;
}

CMPCDecoder::~CMPCDecoder(void)
{
}

bool CMPCDecoder::Open(LPTSTR szSource)
{
	m_file = _wfopen(szSource, TEXT("rbS"));

	if(m_file == NULL)
		return false;

	err = mpc_reader_init_stdio_stream(&reader, m_file);
    if(err < 0) return false;

    demux = mpc_demux_init(&reader);
    if(!demux) return false;

    mpc_demux_get_info(demux,  &si);
	m_Buffer = (float*)malloc(MPC_DECODER_BUFFER_LENGTH);

	return(true);
}

bool CMPCDecoder::Close()
{
	free(m_Buffer);
    mpc_demux_exit(demux);
    mpc_reader_exit_stdio(&reader);
	return(true);
}

void		CMPCDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CMPCDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = si.samples;
	*Channels	= si.channels;

	return(true);
}

bool		CMPCDecoder::GetLength(unsigned long * MS)
{

	double time = mpc_streaminfo_get_length(&si);

	*MS = (unsigned long)(time * 1000.0);

	return(true);
}

bool		CMPCDecoder::SetPosition(unsigned long * MS)
{
	
	double pos = *MS / 1000.0;
	mpc_demux_seek_second(demux, pos);

	return(false);
}

bool		CMPCDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CMPCDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	MPC_SAMPLE_FORMAT sample_buffer[MPC_DECODER_BUFFER_LENGTH];
	mpc_frame_info frame;
	*NumSamples =0;

	frame.buffer = sample_buffer;
	mpc_demux_decode(demux, &frame);
	if(frame.bits == -1) return(false);

	float FloatSampleBuffer[MPC_DECODER_BUFFER_LENGTH];
	int SampleLoc = 0;
	for(int x=0; x<frame.samples; x++)
	{
		for(int ch=0; ch<si.channels; ch++)
		{
			FloatSampleBuffer[SampleLoc] = (float)sample_buffer[SampleLoc] / 32768.0f;
			SampleLoc++;
		}
	}

	*NumSamples = frame.samples;
	*ppBuffer = FloatSampleBuffer;

	return(true);
}