/*
 *  header.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "header.h"

#define HDRMAIN_BITS	0xfffffc00			// 1111 1111 1111 1111 1111 1100 0000 0000
#define MPEG_BITS		0x00180000			// 0000 0000 0001 1000 0000 0000 0000 0000
#define LAYER_BITS		0x00060000			// 0000 0000 0000 0110 0000 0000 0000 0000
#define CRC_BIT			0x00010000			// 0000 0000 0000 0001 0000 0000 0000 0000
#define BITRATE_BITS	0x0000f000			// 0000 0000 0000 0000 ffff 0000 0000 0000
#define SAMP_BITS		0x00000C00			// 0000 0000 0000 0000 0000 1100 0000 0000 
#define PADING_BIT		0x00000200			// 0000 0000 0000 0000 0000 0010 0000 0000 
#define PRIVATE_BIT		0x00000100			// 0000 0000 0000 0000 0000 0001 0000 0000 
#define MODE_BITS		0x000000C0			// 0000 0000 0000 0000 0000 0000 1100 0000 
#define MODEEXT_BITS	0x00000030			// 0000 0000 0000 0000 0000 0000 0011 0000 
#define COPYRIGHT_BIT	0x00000008			// 0000 0000 0000 0000 0000 0000 0000 1000 
#define ORIGINAL_BIT	0x00000004			// 0000 0000 0000 0000 0000 0000 0000 0100 
#define EMPHASIS_BITS	0x00000003			// 0000 0000 0000 0000 0000 0000 0000 0011 

#define CHECK_BITS		0xfffe0c00

#define MPEG_BITS_1		0x00180000
#define MPEG_BITS_2		0x00100000
#define MPEG_BITS_25	0x00000000

#define LAYER_BITS_1	0x00060000
#define LAYER_BITS_2	0x00040000
#define LAYER_BITS_3	0x00020000

#define MPEGVERSION_BITSHIFT	19
#define LAYER_BITSHIFT			17
#define BITRATE_BITSHIFT		12
#define SAMPLERATE_BITSHIFT		10
#define MODE_BITSHIFT			6
#define MODE_EXT_BITSHIFT		4

const uint32_t BitRates[2][3][16] = 
{
	{
		{0 /*free format*/, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000, 0},
		{0 /*free format*/, 32000, 48000, 56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000, 0},
		{0 /*free format*/, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 0}
	},
	{
		{0 /*free format*/, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000, 176000, 192000, 224000, 256000, 0},
		{0 /*free format*/,  8000, 16000, 24000, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000, 0},
		{0 /*free format*/,  8000, 16000, 24000, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000, 0}
	},
};


const uint32_t SampleRates[3][3] = 
{
	{
		44100, 48000, 32000
	},
	{
		22050, 24000, 16000
	},
	{
		11025, 12000, 8000
	}
};

// mpeg version, channels -2
const uint32_t SideInfoSize[3][2] = 
{
	{
		32, 17
	},
	{
		9, 17
	},
	{
		9, 17
	},
};

int Header::decodeHeader(uint8_t * bytes)
{
	SideInfoSize		= 0;
	AudioDataSize		= 0;
	TotalFrameSize		= 0;
	
	
	bs.load(bytes, 4);
	
	
	// ultimately we want to real 11 bits 
	// the next bit defines MPEG2.5
	// but lets ignore that for the moment eh?
	if((bs.getbits(12) & 0xfff) != 0xfff)
		return 0;
	
	// read the MPEG version
	if(bs.getbits(1))
	{
		MpegVersion = MPEG1;
	}
	else
	{
		MpegVersion = MPEG2;
	}
	
	// get layer, anything other than layer3 is invalid
	int layer = bs.getbits(2);
	switch(layer)
	{
		case 1:
			{
				Layer = LAYER3;
				
				if(MpegVersion == MPEG2) 
				{
					SamplesPerFrame = 576;
				}
				else
				{
					SamplesPerFrame = 1152;
				}
			}
			break;
		default:
			return 0;
	}
	
	// read the CRC bit (we use this later
	CRCPresent = 1 - bs.getbits(1);
	
	
	// read the bitrate index, and get the bitrate out of the array
	BitrateIndex = bs.getbits(4);
	if((BitrateIndex == 0xff) || (BitrateIndex == 0))
	{
		// 16 and 0 are invalid
		// technically 0 isn't invalid, but we can't do it atm
		return 0;
	}
	Bitrate = BitRates[MpegVersion][Layer][BitrateIndex];
	
	
	// read the sample rate index, and grab it
	SampleRateIndex = bs.getbits(2);
	if(SampleRateIndex == 0x03)
	{
		// 3 is invalid and is used as a check
		return 0;
	}
	SampleRate = SampleRates[MpegVersion][SampleRateIndex];
	
	// this bit means there is an extra byte on the end of the frame (you'll see later)
	PaddingBit = bs.getbits(1);
	
	// lame copyright bit....
	PrivateBit = bs.getbits(1);
	
	Mode			= bs.getbits(2);
	Mode_Extension	= bs.getbits(2);
	
	if(Mode == MODE_MONO)
	{
		Channels = 1;
	}
	else
	{
		Channels = 2;
	}
	
	Copyright			= bs.getbits(1);
	Original			= bs.getbits(1);
	
	Emphasis			= bs.getbits(2);
	if(Emphasis == EMPH_RESERVED)
		return 0;
	if(Emphasis != EMPH_NONE)
	{
		DLog("Holy crap this track has emphasis!!!\n");
	}
	
	//TODO: load value from sideinfo thing
	if(MpegVersion == MPEG1)
	{
		if(Channels==1)
			SideInfoSize = 17;
		else
			SideInfoSize = 32;
	}
	else
	{
		if(Channels==1)
			SideInfoSize = 9;
		else
			SideInfoSize = 17;
	}
	
	int base = 144;   
	if(MpegVersion != MPEG1)
		base = 72;
	
	TotalFrameSize = ((base * Bitrate) / SampleRate);
	if(PaddingBit)
		TotalFrameSize++;
	
	//TODO: we should do some tests here to make sure TotalFrameSize is in acceptable limits!
	if(TotalFrameSize > 1441)
		return 0; //(size too big possibly)
	
	//TODO work out smallest possible frame size!
	if (TotalFrameSize < 14) {
		return 0; //(size too small possibly)
	}
	
	AudioDataSize	=  TotalFrameSize;
	
	AudioDataSize -= 4;		// for the header
	
	if(CRCPresent)
		AudioDataSize -= 2;
	
	AudioDataSize -= SideInfoSize;
		
	return 1;
}
