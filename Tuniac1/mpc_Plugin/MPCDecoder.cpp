#include "StdAfx.h"

#include "mpcdecoder.h"
#include "mpcdec/mpcdec.h"


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

	mpc_reader_setup_file_reader(&reader, m_file);

    mpc_streaminfo_init(&si);

    if(mpc_streaminfo_read(&si, &reader.reader) != ERROR_CODE_OK) 
	{
        return false;
    }

    /* instantiate a decoder with our file reader */
    mpc_decoder_setup(&decoder, &reader.reader);
    if(!mpc_decoder_initialize(&decoder, &si)) 
	{
        return false;
    }

	m_LengthSamples  = mpc_streaminfo_get_length_samples(&si);

	return(true);
}

bool CMPCDecoder::Close()
{
	return(true);
}

void		CMPCDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CMPCDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = si.sample_freq;
	*Channels	= si.channels;

	return(true);
}

bool		CMPCDecoder::GetLength(unsigned long * MS)
{
	*MS = (unsigned long)(m_LengthSamples / (si.sample_freq / 1000));

	return(true);
}

bool		CMPCDecoder::SetPosition(unsigned long * MS)
{
	mpc_int64_t pos = *MS  * (si.sample_freq / 1000);

	if(mpc_decoder_seek_sample(&decoder, pos) == FALSE)
	{
		return false;
	}

	return(true);
}

bool		CMPCDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CMPCDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples = 0;

	unsigned status = mpc_decoder_decode(&decoder, m_Buffer, 0, 0);
    if (status == (unsigned)(-1)) 
	{
        return false;
    }
    else if (status == 0)   //EOF
    {
        return false;
    }
    else                    //status>0
    {
		*NumSamples = status * si.channels;
		*ppBuffer	= m_Buffer;
    }

	return(true);
}