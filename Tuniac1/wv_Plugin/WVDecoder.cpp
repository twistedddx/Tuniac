#include "StdAfx.h"
#include "wvdecoder.h"

#define NUM_SAMPLES_UNPACK 576L

CWVDecoder::CWVDecoder(void) : 
	m_bFloatMode(false)
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
    wpc = WavpackOpenFileInput(tempname, error, OPEN_WVC | OPEN_NORMALIZE, 23);
    if (!wpc)
        return(false);

	ulSampleRate = WavpackGetSampleRate(wpc);
	ulChannels = WavpackGetNumChannels(wpc);
	ulSamples = WavpackGetNumSamples(wpc);
	ulBitsPerSample = WavpackGetBitsPerSample(wpc);
	ulBytesPerSample = WavpackGetBytesPerSample(wpc);

	pRawData = (long*)malloc(NUM_SAMPLES_UNPACK*ulChannels*4);

	if(WavpackGetMode(wpc) & MODE_FLOAT)
		m_bFloatMode = true;

	return(true);
}

bool CWVDecoder::Close()
{
	free(pRawData);
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
	double pos = *MS;
	WavpackSeekSample(wpc, (int)(ulSampleRate / 1000 * pos));

	return(false);
}

bool		CWVDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CWVDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	unsigned status = WavpackUnpackSamples(wpc, (int32_t *)pRawData, NUM_SAMPLES_UNPACK);
    if (!status)
        return false;

	if(m_bFloatMode)
	{
		*ppBuffer = (float*)pRawData;
	}
	else
	{
		int * pData = (int *)pRawData;
		float * pBuffer = m_Buffer;
		
		float divider = 0.0;

		if(ulBitsPerSample == 8)
		{
			divider = 128.0f;
		}
		else if(ulBitsPerSample == 16)
		{
			divider = 32767.0f;
		}
		else if(ulBitsPerSample == 24)
		{
			divider = 8388608.0f;
		}
		else if(ulBitsPerSample == 32)
		{
			divider = 2147483648.0;
		}
		
		for(int x=0; x<status*ulChannels;x++)
		{
			*pBuffer = (float) ((double)(*pData) / divider);	

			pData ++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;
	}

	*NumSamples = status * ulChannels;

	return(true);
}