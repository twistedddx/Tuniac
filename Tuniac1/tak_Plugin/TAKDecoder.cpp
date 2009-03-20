#include "StdAfx.h"
#include "takdecoder.h"


CTAKDecoder::CTAKDecoder(void)
{
}

CTAKDecoder::~CTAKDecoder(void)
{
}

bool CTAKDecoder::Open(LPTSTR szSource)
{
	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

	Options.Cpu   = tak_Cpu_Any;
	Options.Flags = NULL;

	Decoder = tak_SSD_Create_FromFile (tempname, &Options, NULL, NULL);
	if (Decoder == NULL)
		return false;
	if (tak_SSD_Valid (Decoder) != tak_True)
		return false;

	if (tak_SSD_GetStreamInfo (Decoder, &StreamInfo) != tak_res_Ok) 
		return false;

	//SamplesPerBuf = StreamInfo.Sizes.FrameSizeInSamples;
	//SampleSize    = StreamInfo.Audio.BlockSize;
	/* Frame / Sample size.
	*/
	//BufSize = SamplesPerBuf * SampleSize;
	/* Enough space to hold a decoded frame.
	*/
	buffer = new char [4096 * StreamInfo.Audio.BlockSize];
	m_Buffer = new float [4096 * StreamInfo.Audio.BlockSize];

	if(StreamInfo.Audio.SampleBits == 8)
	{
		m_divider = 128.0f;
	}
	else if(StreamInfo.Audio.SampleBits == 16)
	{
		m_divider = 32767.0f;
	}
	else if(StreamInfo.Audio.SampleBits == 24)
	{
		m_divider = 8388608.0f;
	}
	else if(StreamInfo.Audio.SampleBits == 32)
	{
		m_divider = 2147483648.0f;
	}
	return(true);
}

void		CTAKDecoder::Destroy(void)
{
	tak_SSD_Destroy (Decoder);
	if(buffer)
	{
		delete [] buffer;
		buffer = NULL;
	}
	if(m_Buffer)
	{
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	delete this;
}

bool		CTAKDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = StreamInfo.Audio.SampleRate;
	*Channels	= StreamInfo.Audio.ChannelNum;

	return(true);
}

bool		CTAKDecoder::GetLength(unsigned long * MS)
{
	*MS		= StreamInfo.Sizes.SampleNum/StreamInfo.Audio.SampleRate*1000;
	return true;
}

bool		CTAKDecoder::SetPosition(unsigned long * MS)
{
	unsigned long sample = (*MS * StreamInfo.Audio.SampleRate)/1000;
	tak_SSD_Seek(Decoder, sample);
    return true;
}

bool		CTAKDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CTAKDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	OpResult = tak_SSD_ReadAudio (Decoder, buffer, 4096, &ReadNum);
	if ((OpResult != tak_res_Ok) && (OpResult != tak_res_ssd_FrameDamaged))
	{
		//char ErrorMsg[tak_ErrorStringSizeMax];
		//tak_SSD_GetErrorString (tak_SSD_State (Decoder), ErrorMsg, tak_ErrorStringSizeMax);
		return false;
	}

    if (ReadNum > 0)
	{
     	short * pData = (short*)buffer;
		float * pBuffer = m_Buffer;

		for(int x=0; x<ReadNum * StreamInfo.Audio.ChannelNum; x++)
		{
			*pBuffer = (*pData) / m_divider;	
			pData ++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;


	}
	else
		return false;

	*NumSamples = ReadNum * StreamInfo.Audio.ChannelNum;
	return true;
}