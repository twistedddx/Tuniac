#include "StdAfx.h"
#include "wvdecoder.h"

CWVDecoder::CWVDecoder(void) : 
	m_bFloatMode(false),
		m_divider(0)
{
}

CWVDecoder::~CWVDecoder(void)
{
}

bool CWVDecoder::Open(LPTSTR szSource)
{
	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

    char error [128];
    wpc = WavpackOpenFileInput(tempname, error, OPEN_WVC, 23);
    if (!wpc)
        return false;

	ulSampleRate = WavpackGetSampleRate(wpc);
	ulChannels = WavpackGetNumChannels(wpc);
	ulSamples = WavpackGetNumSamples(wpc);
	//ulBitsPerSample = WavpackGetBitsPerSample(wpc);
	//ulBytesPerSample = WavpackGetBytesPerSample(wpc);

	if(WavpackGetMode(wpc) & MODE_FLOAT)
		m_bFloatMode = true;


	//work out divider for unsigned integer signed: (float)(1<<wfmex.wBitsPerSample)-1;
	//m_divider = pow(2, (wfmex.wBitsPerSample-1))-1;
	m_divider = (float)(1<<(WavpackGetBitsPerSample(wpc)-1))-1;

	/*
	if(ulBitsPerSample == 8)
	{
		m_divider = 128.0f;
	}
	else if(ulBitsPerSample == 12)
	{
		m_divider = 4095.0f;
	}
	else if(ulBitsPerSample == 16)
	{
		m_divider = 32767.0f;
	}
	else if(ulBitsPerSample == 20)
	{
		m_divider = 1048575.0f;
	}
	else if(ulBitsPerSample == 24)
	{
		m_divider = 8388608.0f;
	}
	else if(ulBitsPerSample == 32)
	{
		m_divider = 2147483648.0;
	}
	*/
	return(true);
}

bool CWVDecoder::Close()
{
	wpc = WavpackCloseFile(wpc);
	wpc = NULL;
	return(true);
}

void		CWVDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CWVDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = ulSampleRate;
	*Channels	= ulChannels;

	return(true);
}

bool		CWVDecoder::GetLength(unsigned long * MS)
{

	*MS =  ulSamples * 1000 / ulSampleRate;

	return(true);
}

bool		CWVDecoder::SetPosition(unsigned long * MS)
{
	WavpackSeekSample(wpc, (int)(ulSampleRate / 1000 * (unsigned long)*MS));
	return(true);
}

bool		CWVDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CWVDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	unsigned status = WavpackUnpackSamples(wpc, (int32_t *)m_RawData, 576);
    if (!status)
        return false;

	if(m_bFloatMode)
	{
		*ppBuffer = (float*)m_RawData;
	}
	else
	{
		int		* pData		= (int *)m_RawData;
		float	* pBuffer	= m_Buffer;
		
		for(int x=0; x<status*ulChannels;x++)
		{
			*pBuffer = (*pData) / m_divider;	
			pData ++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;
	}

	*NumSamples = status * ulChannels;

	return(true);
}
