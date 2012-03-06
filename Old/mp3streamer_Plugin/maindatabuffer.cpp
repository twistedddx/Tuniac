/*
 *  maindatabuffer.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "maindatabuffer.h"

bool MainDataBuffer::adddata(uint8_t * inbuf, uint32_t nSlots, uint32_t	mainDataBegin)
{
	if(BitReservoirBytes >= mainDataBegin) 
	{
		// adequate "old" main data available (i.e. bit reservoir)
		memmove(BitReservoir, BitReservoir + BitReservoirBytes - mainDataBegin, mainDataBegin);
		memcpy(BitReservoir + mainDataBegin, inbuf, nSlots);
		BitReservoirBytes = mainDataBegin + nSlots;
	} 
	else 
	{
		// not enough data in bit reservoir from previous frames (perhaps starting in middle of file)
		memcpy(BitReservoir + BitReservoirBytes, inbuf, nSlots);
		BitReservoirBytes += nSlots;

		return false;
	}
	
	return true;
}

uint32_t	MainDataBuffer::getMainDataBytes(void)
{
	return BitReservoirBytes;
}

bool		MainDataBuffer::ExtractBytes(uint8_t	* pBuffer, uint32_t bytes)
{
	if(bytes > BitReservoirBytes)
	{
		// more than is in the buffer ZOMG
		// what we do in this situation is actually copy to
		// (pBuffer + bytes) - mainDataBytes
		// and zeromem(pBuffer, bytes - mainDataBytes;
		
		// in reality what this will do is set the mp3 data 
		// to 0 untill its at the point where it can be decoded
		
		// which means scalefactors and stuff will be 0 along 
		// with huffman data - which hopefully wont explode
		// we will return false to let the caller know it wasn't
		// a complete success!
		return false;
	}
	
	memcpy(pBuffer, BitReservoir, bytes);
	return true;
}


void MainDataBuffer::Reset()
{
	memset(BitReservoir, 0, BITRESERVOIR_SIZE);
	BitReservoirBytes = 0;
}
