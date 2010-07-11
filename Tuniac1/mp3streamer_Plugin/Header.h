/*
 *  header.h
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#pragma once


#define LAYER1				0
#define LAYER2				1
#define LAYER3				2

#define MPEG1				0
#define MPEG2				1
#define MPEG25				2

#define MODE_STEREO			0
#define MODE_JOINT_STEREO	1	// see MODE_EXT defines
#define MODE_DUAL_CHANNEL	2
#define MODE_MONO			3

//mode extensions
// for LAYER3
#define MODE_EXT_STEREO		0	// stream is actually MODE_STEREO
#define MODE_EXT_IS			1	// Intensity stereo coding is used
#define MODE_EXT_MS			2	// MS Stereo is used
#define MODE_EXT_ISMS		3	// both Intensity and MS stereo are used

#define EMPH_NONE			0	// no deemphasis
#define EMPH_5015			1	// 50/15 microseconds
#define EMPH_RESERVED		2	// should never happen
#define EMPH_CCITT			3	// CCITT j.17

#include "stdint.h"
#include "bitstream.h"

class Header
{
protected:
	bitstream bs;

	
public:
	uint32_t	MpegVersion;
	uint32_t	Layer;
	
	uint32_t	SampleRateIndex;
	
	uint32_t	ProtectionBit;
	
	uint32_t	BitrateIndex;
	
	uint32_t	PaddingBit;
	uint32_t	PrivateBit;
	
	uint32_t	Mode;
	uint32_t	Mode_Extension;
	
	uint32_t	Copyright;
	uint32_t	Original;
	uint32_t	Emphasis;
	
	// Calculated stuff
	uint32_t	SideInfoSize;
	uint32_t	AudioDataSize;
	uint32_t	TotalFrameSize;
	
	uint32_t	CRCPresent;
	uint16_t	CRC;	
	
	uint32_t	Bitrate;
	uint32_t	SampleRate;
	uint32_t	Channels;
	
	uint32_t	SamplesPerFrame;
	
	
public:
	int decodeHeader(uint8_t * bytes);

};