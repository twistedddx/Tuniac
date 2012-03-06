/*
 *  mpegdecoder.h
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#pragma once

// positive values are always good
// negative errors represent a failure in the stream sync/processing
// its up to the caller what to do..

enum decodeFrameReturn 
{
	decodeFrame_NotEnoughBRV = 4,
	decodeFrame_Xing = 3,
	decodeFrame_Done = 2,
	decodeFrame_Success = 1,
	decodeFrame_NeedMoreData = 0,
	decodeFrame_Failure = -1,
	decodeFrame_NoFollowingHeader = -2,
};

#define OE6GUARDBUFFER	(4+2+32)
#define MIN_MPEG_DATA_SIZE	((1442*2)+OE6GUARDBUFFER)

#include "header.h"
#include "sideinfo.h"
#include "maindatabuffer.h"
#include "layer3decoder.h"

class mpegdecoder
{
public:
	mpegdecoder();
public:
	uint32_t		bufferlength;
	uint8_t		*	pbufStart;
	uint8_t		*	pBuf;
	uint8_t		*	pBufEnd;
	
	bool			bEndOfStream;
	
	Header			header;
	Header			nextheader;
	
	SideInfo		si;
	
	MainDataBuffer	mdb;
	
	layer3decoder	l3dec;
	
	uint32_t		mp3datasize;
	uint8_t			mp3data[BITRESERVOIR_SIZE];
	
	
	bool			XingFound;
	uint8_t			XingData[MIN_MPEG_DATA_SIZE];
	
	
public:
	void Reset();
	bool setData(uint8_t * pData, uint32_t length);

	uint32_t getBytesUsed(void);
	uint32_t getBytesLeft(void);
	
	// lets the decoder know there will be no more data coming in and to make the best of
	// what it has in its buffer!
	void setEndOfStream(void);
	
	const uint8_t * getXingData(void);
	
	// when sync next frame returns with decodeFrame_Success or decodeFrame_Done
	// you can bet your bottom dollar that all variables are set up properly!
	// if it returns with decodeFrame_Xing then getXingData will return a pointer
	// to the Xing/Info header which has been copied out of the byteStream for posterity
	// and can be accessed at any time as long as this decoder object exists...
	// I've not tried two Xing frames in a row but I dont see why it wouldn't work
	// unless the dark ones dont want that...
	int syncNextFrame(void);
	
	int synthFrame(float * tohere);
};