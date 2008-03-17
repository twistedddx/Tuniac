#include "StdAfx.h"

#include "mpc/reader.h"
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
//	m_file = _wfopen(szSource, TEXT("rbS"));

//	if(m_file == NULL)
//		return false;

//	mpc_reader_setup_file_reader(&reader, m_file);


	char tempname[_MAX_PATH];
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

	mpc_status err = mpc_reader_init_stdio(&reader, tempname);
    if(err < 0) 
		return false;




    demux = mpc_demux_init(&reader);
    if(!demux) return false;

    mpc_demux_get_info(demux,  &si);

	m_MPCTime = mpc_streaminfo_get_length(&si);

	return(true);
}

bool CMPCDecoder::Close()
{
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
	*MS = (unsigned long)(m_MPCTime * 1000.0);

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
	mpc_frame_info frame;
	*NumSamples =0;

	frame.buffer = m_Buffer;
	mpc_status stat = mpc_demux_decode(demux, &frame);

	if(stat != MPC_STATUS_OK)
	{
		return false;
	}

	if(frame.bits == -1) 
		return(false);
/*
	int SampleLoc = 0;
	for(int x=0; x<frame.samples; x++)
	{
		for(int ch=0; ch<si.channels; ch++)
		{
			m_Buffer[SampleLoc] = (float)sample_buffer[SampleLoc] / 32768.0f;
			SampleLoc++;
		}
	}
*/
	*NumSamples = frame.samples;
	*ppBuffer	= m_Buffer;

	return(true);
}