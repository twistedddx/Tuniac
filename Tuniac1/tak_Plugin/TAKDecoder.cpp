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
	Options.Flags = tak_ssd_opt_SequentialRead;

	Decoder = tak_SSD_Create_FromFile (tempname, &Options, NULL, NULL);
	if (Decoder == NULL)
		return false;
	if (tak_SSD_Valid (Decoder) != tak_True)
		return false;

	if (tak_SSD_GetStreamInfo (Decoder, &StreamInfo) != tak_res_Ok) 
		return false;

	return(true);
}

bool CTAKDecoder::Close()
{
	tak_SSD_Destroy (Decoder);
	return(true);
}

void		CTAKDecoder::Destroy(void)
{
	Close();
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
	*MS		= StreamInfo.Sizes.SampleNum/StreamInfo.Audio.SampleRate/StreamInfo.Audio.ChannelNum*1000;
	return true;
}

bool		CTAKDecoder::SetPosition(unsigned long * MS)
{
	//tak_SSD_Seek(Decoder, SamplePos);
    return true;
}

bool		CTAKDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CTAKDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	OpResult = tak_SSD_ReadAudio (Decoder, buffer, SamplesPerBuf, &ReadNum);
	if (   (OpResult != tak_res_Ok) && (OpResult != tak_res_ssd_FrameDamaged)) 
		return false;

    if (ReadNum > 0)
	{
     	short * pData = (short*)buffer;
		float * pBuffer = m_Buffer;
		
		for(int x=0; x<ReadNum*StreamInfo.Audio.ChannelNum; x++)
		{
			*pBuffer = (*pData) / m_divider;	
			pData ++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;

		*NumSamples = ReadNum*StreamInfo.Audio.ChannelNum;
	}
	else
		return false;

	return true;
}