#include "StdAfx.h"
#include "wvdecoder.h"

CWVDecoder::CWVDecoder(void)
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
    wpc = WavpackOpenFileInput(tempname, error, (OPEN_WVC), 23);
    if (!wpc)
        return(false);

	SAMPRATE = WavpackGetSampleRate(wpc);


	return(true);
}

bool CWVDecoder::Close()
{
	free(m_Buffer);
	wpc = WavpackCloseFile(wpc);
	return(true);
}

void		CWVDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CWVDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = WavpackGetSampleRate(wpc);
	*Channels	= WavpackGetNumChannels(wpc);

	return(true);
}

bool		CWVDecoder::GetLength(unsigned long * MS)
{

	double time = WavpackGetNumSamples(wpc) * 1000.0 / SAMPRATE;

	*MS = (unsigned long)(time * 1000.0);

	return(true);
}

bool		CWVDecoder::SetPosition(unsigned long * MS)
{
	
	double pos = *MS / 1000.0;
	WavpackSeekSample(wpc, (int)(SAMPRATE / 1000.0 * pos));

	return(false);
}

bool		CWVDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CWVDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	//WavpackGetBitsPerSample (wpc)
	*NumSamples =0;

	float * pDataBuffer = m_Buffer;
	for(unsigned long x=0; x<WavpackGetNumSamples(wpc)*WavpackGetNumChannels(wpc); x++)
	{
		//*pDataBuffer = (float)frame.buffer[x] / 32767.0f;
		pDataBuffer++;
	}

	*NumSamples = WavpackGetNumSamples(wpc)*WavpackGetNumChannels(wpc);
	*ppBuffer = m_Buffer;

	return(true);
}