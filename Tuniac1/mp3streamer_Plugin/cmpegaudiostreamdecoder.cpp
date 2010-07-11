/*
 *  CMpegAudioDecoder.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 24/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "cmpegaudiostreamdecoder.h"
#include "stdlib.h"
#include "stdio.h"

#include "mpegdecoder.h"

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

void		CMpegAudioDecoder::Destroy(void)
{
#ifdef DEBUG_DESTROY
	printf("CMpegAudioDecoder::Destroy(this) == %p\n", this);
#endif
	delete this;
}

bool		CMpegAudioDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CMpegAudioDecoder::GetLength(unsigned long * MS)
{
	*MS = ((float)m_TotalSamples / ((float)m_SampleRate / 1000.0f));
	return true;
}

bool		CMpegAudioDecoder::SetPosition(unsigned long * MS)
{
	return false;
}

bool		CMpegAudioDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate		= m_SampleRate;
	*Channels		= m_Channels;
	return true;
}

bool		CMpegAudioDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	int ret = 0;
		
	while(true)
	{
		ret = decoder.syncNextFrame();	
		
		if(ret == decodeFrame_NeedMoreData)
		{
			uint32_t left = decoder.getBytesLeft();
			
			uint32_t bytesthistime = MIN_MPEG_DATA_SIZE - left;
			
			memcpy(m_databuffer, m_databuffer + bytesthistime, left);
			
			unsigned __int64 bytesread;
			m_pSource->Read(bytesthistime, m_databuffer + left, &bytesread);
			if(bytesread < bytesthistime)
				decoder.setEndOfStream();
			
			decoder.setData(m_databuffer, left + bytesread);
		}
		else if(ret == decodeFrame_Success)
		{
			// found our first frame!
			break;
		}
		else if((ret == decodeFrame_NotEnoughBRV)  || (ret == decodeFrame_Xing) || ret == decodeFrame_NoFollowingHeader)
		{
			// also skip ID3 frames please
			// need to go around again or we're gonna 'whoop'
			continue;
		}
		else 
		{
			// something else happened!
			return false;
		}
	}
	
	if((ret == decodeFrame_Success) || (ret == decodeFrame_Xing))
	{
		// we got a frame - now lets decode it innit

		
		//memset(m_audiobuffer, 0, 4096 * 4);
		decoder.synthFrame(m_audiobuffer);

		*ppBuffer	= m_audiobuffer;
		*NumSamples = 2304;
		return true;
	}
	
	
	return false;
}

bool		CMpegAudioDecoder::Open(IAudioFileIO * pFileIO)
{

	m_pSource = pFileIO;

	if(!m_pSource)
		return NULL;

	//Lets find a frame so we can get sample rate and channels!
	float * temp;
	unsigned long	blah;
	
	if(GetBuffer(&temp, &blah))
	{
		m_SampleRate	= decoder.header.SampleRate;
		m_Channels		= decoder.header.Channels;
		
		decoder.Reset();
	
	}

	return true;
}

#include <math.h>
void deemphasis5015(void)
{
	int num_channels = 2;
	float frequency = 44100;
	short * outbuf;
	int numsamples[2];
	double llastin[2], llastout[2];
	
	
	int i, ch;
	 /* this implements an attenuation treble shelving filter
	 to undo the effect of pre-emphasis. The filter is of
	 a recursive first order */
	short lastin;
	double lastout;
	short *pmm;
	/* See deemphasis.gnuplot */
	double V0     = 0.3365;
	double OMEGAG = (1./19e-6);
	double T  = (1./frequency);
	double H0 = (V0-1.);
	double B  = (V0*tan((OMEGAG * T)/2.0));
	double a1 = ((B - 1.)/(B + 1.));
	double b0 = (1.0 + (1.0 - a1) * H0/2.0);
	double b1 = (a1 + (a1 - 1.0) * H0/2.0);

    for (ch=0; ch< num_channels; ch++) 
	{
		/* ---------------------------------------------------------------------
		 * For 48Khz:   a1=-0.659065
		 *              b0= 0.449605
		 *              b1=-0.108670
		 * For 44.1Khz: a1=-0.62786881719628784282
		 *              b0= 0.45995451989513153057
		 *              b1=-0.08782333709141937339
		 * For 32kHZ ?
		 */
		lastin = llastin [ch];
		lastout = llastout [ch];
		for (pmm = (short *)outbuf [ch], i=0;
			 i < numsamples[0]; /* TODO: check for second channel */
			 i++) 
		{
			lastout = *pmm * b0 + lastin * b1 - lastout * a1;
			lastin = *pmm;
			//*pmm++ = (lastout > 0.0) ? lastout + 0.5 : lastout - 0.5;
			*pmm++ = lastout;
		} // for (pmn = .. */
		llastout [ch] = lastout;
		llastin [ch] = lastin;
    } // or (ch = .. */

}
