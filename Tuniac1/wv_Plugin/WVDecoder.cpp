#include "StdAfx.h"
#include "wvdecoder.h"

#define NUM_SAMPLES_UNPACK 576L

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

	ulSampleRate = WavpackGetSampleRate(wpc);
	ulChannels = WavpackGetNumChannels(wpc);
	ulSamples = WavpackGetNumSamples(wpc);
	ulBitsPerSample = WavpackGetBitsPerSample(wpc);
	ulBytesPerSample = WavpackGetBytesPerSample(wpc);

	pRawData = (char*)malloc(NUM_SAMPLES_UNPACK*ulChannels*4);

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

	if(ulBytesPerSample == 2)  //16bit
	{
		short * pData = (short *)pRawData;
		float * pBuffer = m_Buffer;
		
		for(int x=0; x<status*ulChannels; x++)
		{
			*pBuffer = (*pData) / 32767.0f;	
			pData ++;
			pBuffer++;
		}
	}
	else if (ulBytesPerSample == 3) //24bit
	{
		/*
		char * pData = (char *)pRawData;
		float *pBuffer = m_Buffer;
		int sourceIndex = 0;
		int padding = nBlockAlign - ulChannels * (ulBitsPerSample >> 3);
		int32 workingValue;

		for ( unsigned int i = 0; i < 1024; i++ ) {
			for ( unsigned int j = 0; j < nChannels; j++ ) {
				workingValue = 0;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 8 & 0xFF00;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 16 & 0xFF0000;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 24 & 0xFF000000;

				*pBuffer = ((float)workingValue) * QUANTFACTOR;

				pBuffer ++;
				pData += 3;
			}
			sourceIndex += padding;
		}
		*/
	}
	else if(ulBytesPerSample == 4)  //32bit?
	{
		int * pData = (int *)pRawData;
		float * pBuffer = m_Buffer;
		
		float divider = (float)((1<<ulBitsPerSample)-1);
		
		for(int x=0; x<status*ulChannels;x++)
		{
			*pBuffer = (*pData) / divider;	
			pData ++;
			pBuffer++;
		}
	}

	*ppBuffer = m_Buffer;
	*NumSamples = status * ulChannels;

	return(true);
}