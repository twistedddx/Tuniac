/*
 *  CMpegAudioDecoder.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 24/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "cmpegaudiodecoder.h"
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
	uint64_t newMS = *MS;
	float percent = 100.00f * ((float)newMS / (float)m_TotalMS);
	uint64_t offset;
	
	if(m_bXingValid)
	{
		/* interpolate in TOC to get file seek point in bytes */ 
		int a;
		float fa, fb, fx;
		
		a = MIN(percent, 99);
		
		fa = m_TOC[a];
		
		if (a < 99)
			fb = m_TOC[a + 1];
		else
			fb = 256;
		
		fx = fa + (fb - fa) * (percent - a);
		offset = (1.0f / 256.0f) * fx * m_StreamDataLength;
	}
	else
	{
		offset = (float)m_StreamDataLength * (float)(percent/ 100.0f) ;
	}
	
	offset += m_datastart;
	m_pSource->Seek(offset, false);
	
	// TODO: REMEMBER TO CALL RESET YOU FUCKING IDIOT
	decoder.Reset();

	return true;
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

	if(!AnalyseStream())
		return false;

	return true;
}

bool		CMpegAudioDecoder::AnalyseStream(void)
{
	unsigned char buffer[MIN_MPEG_DATA_SIZE];

	/*
	if(m_pSource->Peek(buffer, 10))
	{
		int id3_length = 0;
		
		if(0x49 == buffer[0] && 0x44 == buffer[1] && 0x33 == buffer[2]) 
		{
			id3_length = (((buffer[6] & 0x7F) << (3 * 7)) | ((buffer[7] & 0x7F) << (2 * 7)) |
						  ((buffer[8] & 0x7F) << (1 * 7)) | ((buffer[9] & 0x7F) << (0 * 7)));
			
			// Add 10 bytes for ID3 header
			id3_length += 10;
			
			m_datastart = id3_length;
		}
	}
	*/
	
	if(!m_pSource->Seek(m_datastart, false))
		return false;
	
	uint32_t ulBytesIn = 0;
	
	int ret = decodeFrame_NeedMoreData;

	while(true)
	{
		if((ret == decodeFrame_NeedMoreData) || (ret == decodeFrame_NotEnoughBRV))
		{
			uint32_t left = decoder.getBytesLeft();
			
			uint32_t bytesthistime = MIN_MPEG_DATA_SIZE - left;
			
			memcpy(buffer, buffer + bytesthistime, left);
			
			unsigned __int64 bytesread;
			m_pSource->Read(bytesthistime, buffer + left, &bytesread);

			if(bytesread < bytesthistime)
				decoder.setEndOfStream();
			
			decoder.setData(buffer, left + bytesread);
			
					
			// scan as far as 64k pls
			if(ulBytesIn+=bytesread > 65536)
			{
				DLog("no headers found giving up after %d bytes\n", ulBytesIn);
				return false;
			}
		}
		else if((ret == decodeFrame_Success) || (ret == decodeFrame_Xing))
		{
			// found our first frame!
			break;
		}
		else 
		{
			// something else happened!
			return false;
		}
		
		ret = decoder.syncNextFrame();
	}

	
	if(ret == decodeFrame_Xing)
	{
		//we found a xing header
		const unsigned char * pXing = decoder.getXingData();
		if(pXing)
		{
			int i;
			bitstream	bs;
			bs.load((unsigned char *)pXing, decoder.header.AudioDataSize);
			
			uint32_t ID = bs.getbits(32);
			if(ID != 'Info' && ID != 'Xing')
			{
				// then how the hell did we get here lets crash for good measure
				uint32_t *p = 0; *p=0;
			}
			
			uint32_t Flags = bs.getbits(32);
			
			m_NumFrames = 0;
			if(Flags & FRAMES_FLAG)
			{
				m_NumFrames = bs.getbits(32);
			}
			
			m_StreamDataLength = 0;
			if(Flags & BYTES_FLAG)
			{
				m_StreamDataLength = bs.getbits(32);
			}
			
			if(Flags & TOC_FLAG)
			{
				for (i = 0; i < 100; i++)
					m_TOC[i] = bs.getbits(8);
			}
			
			m_VbrScale = -1;
			if(Flags & VBR_SCALE_FLAG)
			{
				m_VbrScale = bs.getbits(32);
			}
					
			ID = bs.getbits(32);
			if('LAME' == ID) 
			{
			}
			
			
			// todo - this might not be right :o
			// skip past version number
			for(i = 0; i < 5; ++i)
				bs.getbits(8);

			uint8_t infoTagRevision = bs.getbits(4);
			uint8_t vbrMethod = bs.getbits(4);

			uint8_t lowpassFilterValue = bs.getbits(8);

			float peakSignalAmplitude = bs.getbits(32);
			uint16_t radioReplayGain = bs.getbits(16);
			uint16_t audiophileReplayGain = bs.getbits(16);

			uint8_t encodingFlags = bs.getbits(4);
			uint8_t athType = bs.getbits(4);

			uint8_t lameBitrate = bs.getbits(8);

			uint32_t _startPadding = bs.getbits(12);
			uint32_t _endPadding = bs.getbits(12);

			_startPadding += 528 + 1; //MDCT/filterbank delay
			_endPadding -= 528 + 1;

			uint8_t misc = bs.getbits(8);

			uint8_t mp3Gain = bs.getbits(8);
			//printf("Gain: %i\n", mp3Gain);

			uint8_t unused =bs.getbits(2);
			uint8_t surroundInfo = bs.getbits(3);
			uint16_t presetInfo = bs.getbits(11);

			uint32_t musicGain = bs.getbits(32);

			uint32_t musicCRC = bs.getbits(32);

			uint32_t tagCRC = bs.getbits(32);
					   
		}
		
		m_bXingValid = true;
		
		// we want to skip past the xing thing
		m_datastart		+= decoder.header.TotalFrameSize;
	}
	else
	{
		//there was no xing header
		// TODO: seek to the end of the file - 128 bytes
		// see if there is a TAG
		// if so use the TELL point
		// else use TELL + 128 as file size
		
		// ASSUME CBR
		// calc num frames from there
		
		m_bXingValid = false;

		
		int sub = 0;
		
		unsigned __int64 stream_length;
		m_pSource->Size(&stream_length);

		if(!m_pSource->Seek(stream_length-128, false))
			return false;
		char tbuf[3];
/*		if(m_pSource->Peek(tbuf, 3))
		{
			if (tbuf[0] == 'T' && 
				tbuf[1] == 'A' &&
				tbuf[2] == 'G') 
			{
				//its a TAG header!!!
				sub = 128;
			}
		}
*/
		
		m_StreamDataLength	= stream_length - sub - m_datastart;
		// lets hope to merry god that this is a CBR file!!!
		m_NumFrames			= m_StreamDataLength / decoder.header.TotalFrameSize;
	}
	
	m_SampleRate	= decoder.header.SampleRate;
	m_Channels		= decoder.header.Channels;
	
	m_TotalSamples	= m_NumFrames * decoder.header.SamplesPerFrame;
	m_TotalMS		= (uint32_t)((float)m_TotalSamples / ((float)m_SampleRate / 1000.0f));

	decoder.Reset();
	
	// get us back to the start of the mp3 data thanks
	if(!m_pSource->Seek(m_datastart, false))
		return false;

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
