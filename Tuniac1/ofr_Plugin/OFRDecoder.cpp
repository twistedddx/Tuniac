#include "StdAfx.h"

#include "ofrdecoder.h"

extern "C" int host_bigendian = 0;

COFRDecoder::COFRDecoder(void)
{
}

COFRDecoder::~COFRDecoder(void)
{
}

bool COFRDecoder::Open(LPTSTR szSource)
{
	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

    decoderInstance = OptimFROG_createInstance();
    if (decoderInstance == NULL)
        return 0;
    if (!OptimFROG_open(decoderInstance, tempname))
    {
        OptimFROG_destroyInstance(decoderInstance);
        decoderInstance = NULL;
        return 0;
    }

    OptimFROG_getInfo(decoderInstance, &iInfo);


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

	buffer = new char [1024*(iInfo.bitspersample/8)*iInfo.channels];

	return(true);
}

bool COFRDecoder::Close()
{
	if (decoderInstance != NULL)
		OptimFROG_destroyInstance(decoderInstance);
	delete [] buffer;
	buffer = NULL;
	return(true);
}

void		COFRDecoder::Destroy(void)
{
	Close();
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
        return 0;

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