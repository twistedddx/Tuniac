#include "StdAfx.h"

#include "ofrdecoder.h"

//#include <intrin.h>

COFRDecoder::COFRDecoder(void)
{
}

COFRDecoder::~COFRDecoder(void)
{
}

bool COFRDecoder::Open(LPTSTR szSource)
{
	char tempname[MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, MAX_PATH, 0, 0);

    decoderInstance = OptimFROG_createInstance();
    if (decoderInstance == NULL)
        return false;
    if (!OptimFROG_open(decoderInstance, tempname))
    {
        OptimFROG_destroyInstance(decoderInstance);
        decoderInstance = NULL;
        return false;
    }

    OptimFROG_getInfo(decoderInstance, &iInfo);

	//work out divider for unsigned integer signed: (float)(1<<wfmex.wBitsPerSample)-1;
	//m_divider = pow(2, (wfmex.wBitsPerSample-1))-1;
	m_divider = (float)(1<<(iInfo.bitspersample-1))-1;

	/*
	if(iInfo.bitspersample == 8)
	{
		m_divider = 128.0f;
	}
	else if(iInfo.bitspersample == 12)
	{
		m_divider = 4095.0f;
	}
	else if(iInfo.bitspersample == 16)
	{
		m_divider = 32767.0f;
	}
	else if(iInfo.bitspersample == 20)
	{
		m_divider = 1048575.0f;
	}
	else if(iInfo.bitspersample == 24)
	{
		m_divider = 8388608.0f;
	}
	else if(iInfo.bitspersample == 32)
	{
		m_divider = 2147483648.0f;
	}
	*/

	buffer = new char [1024*(iInfo.bitspersample/8)*iInfo.channels];
	m_Buffer = new float [1024*(iInfo.bitspersample/8)*iInfo.channels];

	return(true);
}

void		COFRDecoder::Destroy(void)
{
	if (decoderInstance != NULL)
		OptimFROG_destroyInstance(decoderInstance);
	delete [] buffer;
	buffer = NULL;
	delete [] m_Buffer;
	m_Buffer = NULL;
	delete this;
}

bool		COFRDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = iInfo.samplerate;
	*Channels	= iInfo.channels;

	return(true);
}

bool		COFRDecoder::GetLength(unsigned long * MS)
{

	*MS = iInfo.length_ms;

	return true;
}

bool		COFRDecoder::SetPosition(unsigned long * MS)
{
	if(!OptimFROG_seekable(decoderInstance))
		return false;

	OptimFROG_seekPoint(decoderInstance, (sInt64_t) ((*MS/1000) * iInfo.samplerate));
    return true;
}

bool		COFRDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		COFRDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

    sInt32_t pointsRetrieved = OptimFROG_read(decoderInstance, buffer, 1024);
    if (pointsRetrieved <= 0)
        return false;

/*
	*NumSamples = pointsRetrieved*iInfo.channels;

	int len = pointsRetrieved*iInfo.channels;
	int index = 0;

	__m64 * psrc = (__m64 *)buffer;
	__m128 XMM0, XMM1;

	XMM1 = _mm_load1_ps(&m_divider);

	while(len >= 4)
	{
		XMM0 = _mm_cvtpi16_ps(psrc[index]);
		XMM0 = _mm_div_ps(XMM0, XMM1);

		_mm_store_ps(&m_Buffer[index], XMM0);

		index+=4;			
		len -= 4;
	}
	while(len)
	{
		//XMM0 = _mm_cvtpi16_ss(psrc[index]); //doesnt exist :(
		float * pSample = (float *)buffer[index];
		XMM0 = _mm_load1_ps(&pSample[index]);
		XMM0 = _mm_div_ss(XMM0, XMM1);

		_mm_store_ss(&m_Buffer[index], XMM0);

		index++;			
		len--;
	}

_mm_cvtpu8_ps
_mm_cvtpi8_ps
_mm_cvtpu16_ps
_mm_cvtpi16_ps
_mm_cvtpi32_ps
_mm_div_ps

*/
	short * pData = (short *)buffer;
	float * pBuffer = m_Buffer;

	for(int x=0; x<pointsRetrieved*iInfo.channels; x++)
	{
		*pBuffer = (*pData) / m_divider;	
		pData ++;
		pBuffer++;
	}
	*ppBuffer = m_Buffer;

	*NumSamples = pointsRetrieved*iInfo.channels;


	return true;
}
