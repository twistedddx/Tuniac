#include "StdAfx.h"

#include "ttadecoder.h"

CTTADecoder::CTTADecoder(void)
{
}

CTTADecoder::~CTTADecoder(void)
{
}

bool CTTADecoder::Open(LPTSTR szSource)
{
	int ret = 0;

	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

	ret = open_tta_file(tempname, &info, 0);
	if(ret < 0)
		return false;

	ret = player_init(&info);
	if(ret < 0)
		return false;

	if(info.BPS == 8)
	{
		m_divider = 128.0f;
	}
	else if(info.BPS == 16)
	{
		m_divider = 32767.0f;
	}
	else if(info.BPS == 24)
	{
		m_divider = 8388608.0f;
	}
	else if(info.BPS == 32)
	{
		m_divider = 2147483648.0f;
	}

	samples_read = 0;
	buffer = new unsigned char [PCM_BUFFER_LENGTH * info.BSIZE * info.NCH];

	return(true);
}

void		CTTADecoder::Destroy(void)
{
	close_tta_file(&info);
	delete [] buffer;
	buffer = NULL;
	delete this;
}

bool		CTTADecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = info.SAMPLERATE;
	*Channels	= info.NCH;

	return(true);
}

bool		CTTADecoder::GetLength(unsigned long * MS)
{
	*MS = info.LENGTH*1000;
	return true;
}

bool		CTTADecoder::SetPosition(unsigned long * MS)
{
	set_position(*MS);
    return true;
}

bool		CTTADecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CTTADecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

    samples_read = get_samples(buffer);
	if(!samples_read)
        return 0;

	short * pData = (short *)buffer;
	float * pBuffer = m_Buffer;
		
	for(int x=0; x<samples_read*info.NCH; x++)
	{
		*pBuffer = (*pData) / m_divider;	
		pData ++;
		pBuffer++;
	}
	*ppBuffer = m_Buffer;


	*NumSamples = samples_read*info.NCH;

	return true;
}