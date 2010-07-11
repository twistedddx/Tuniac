/*
 *  bitstream.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "bitstream.h"

void bitstream::load(const uint8_t * pData, uint32_t length)
{
	data			= pData;
	datalength		= length;
	datalengthbits	= length * 8;
	
	bitoffset		= 0;
}

uint32_t bitstream::getbits(uint32_t numbits)
{
	uint32_t rval = 0;
	int pos;
	
	if(numbits == 0)
		return 0;
	

	if(bitoffset + numbits > datalengthbits)
		return 0;

	// replaced with code above which makes sure we can read that many bits eh
	// prevents overrunning the buffer
	//if(pos > datalength)
	//	return 0;
	
	pos = (uint32_t)(bitoffset >> 3);
	rval =	(uint32_t)data[pos]	<< 24 |
			(uint32_t)data[pos+1]	<< 16 |
			(uint32_t)data[pos+2]	<< 8 |
			(uint32_t)data[pos+3];
	
	rval <<= bitoffset & 7;
	rval >>= 32 - numbits;
	
	bitoffset += numbits;
	
	return rval;	
}

uint32_t bitstream::dismiss(uint32_t numbits)
{
	bitoffset += numbits;
	return bitoffset;
}

void bitstream::rewind(uint32_t numbits)
{
	// make sure we cant rewind past the beginning
	bitoffset -= MIN(numbits, bitoffset);
}

uint32_t bitstream::getBitOffset(void)
{
	return bitoffset;
}

void bitstream::setBitOffset(uint32_t offset)
{
	bitoffset = offset;
}


uint32_t bitstream::getByteOffset(void)
{
	return (uint32_t)(bitoffset >> 3);
}

bool bitstream::EOS(void)
{
	uint32_t pos = (uint32_t)(bitoffset >> 3);
	
	// prevents overrunning the buffer
	if(pos > datalength)
		return true;
	
	return false;
}
