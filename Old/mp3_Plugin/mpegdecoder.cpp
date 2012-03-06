/*
 *  mpegdecoder.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "mpegdecoder.h"

#define PTRDIFF(x,y) (x - y)

mpegdecoder::mpegdecoder()
{
	Reset();
}

void mpegdecoder::Reset()
{
	pbufStart		= NULL;
	bufferlength	= 0;
	pBuf			= NULL;
	pBufEnd			= NULL;
	
	bEndOfStream	= false;
	
	// we could also clear out the synthesis buffer here and other stuff
	
	mp3datasize	= 0;
	mdb.Reset();
	
	l3dec.reset();

}

bool mpegdecoder::setData(uint8_t * pData, uint32_t length)
{
	if(length < 4)
		return false;
	
	pbufStart		= pData;
	bufferlength	= length;
	
	pBuf			= pData;
	pBufEnd			= pData+length;
	
	
	return true;
}

uint32_t mpegdecoder::getBytesUsed(void)
{
	return pBuf - pbufStart;
}

uint32_t mpegdecoder::getBytesLeft(void)
{
	return pBufEnd - pBuf;
}


void mpegdecoder::setEndOfStream(void)
{
	bEndOfStream = true;
}

const unsigned char * mpegdecoder::getXingData(void)
{
	if(XingFound)
		return XingData;
	
	return NULL;
}

//TODO: should we actually REWIND the stream on decode failure?
// if so back to the point of failure and allow the caller to decide what to do, or back to startpoint + 1 and then expect the caller to call us again
// oh what decisions eh?

int mpegdecoder::syncNextFrame(void)
{
	uint8_t	*	pThisTime = pBuf;
	for(;;)
	{
		if(PTRDIFF(pBufEnd, pBuf) < 4)
		{
			if(bEndOfStream)
				return decodeFrame_Done;
			
			return decodeFrame_NeedMoreData;
		}
		
		if(header.decodeHeader(pBuf))
		{	
			// ok we got what looks like a valid header - lets check theres another one after it eh?
			if(PTRDIFF(pBufEnd, pBuf) < header.TotalFrameSize+4+2+32 )
			{
				// ok we have less than header.TotalFrameSize+4+2+32 bytes left
				
				// if we're not at the end of the stream then lets ask for more data then?
				if(!bEndOfStream)
					return decodeFrame_NeedMoreData;
				
				// or lets just drop out and assume this last frame is falid
				break;
			}
			else
			{
				// set our advance pointer to the next header
				unsigned char * nexthdrptr = pBuf + header.TotalFrameSize;
				
				// check the next header follows this one (prevents bad sync)
				if(nextheader.decodeHeader(nexthdrptr))
				{
					// TODO: we should check the current header against the next one to make sure things match up!
					// but in the mean time lets just assume its ok....
					
					break;
				}
			}
		}
		
		pBuf++;
	}

	// this line of code is probably irrelevant now as we should have checked this above!
	// lets make sure we have enough data for this stream and if so, decode it
	if(PTRDIFF(pBufEnd, pBuf) < header.TotalFrameSize)
		return decodeFrame_NeedMoreData;
	
	pBuf += 4;

	if(header.CRCPresent)
	{
		// we really should load up the CRC data
		// and actually do some CRC processing innit
		pBuf += 2;
	}
	
	if(!si.decodeSideInfo(pBuf, header))
	{
		// TODO: handle error here
		DLog("side info error!!!");
		pBuf = pThisTime+1;
		return decodeFrame_NoFollowingHeader;
	}
	pBuf += header.SideInfoSize;
	
	// now we're at MAIN DATA!!!
	if( (strncmp((char*)pBuf, "Info", 4) == 0) || (strncmp((char*)pBuf, "Xing", 4) == 0) )
	{
		DLog("XING HEADER FOUND!!!\n");
		XingFound = true;
		memcpy(XingData, pBuf, header.AudioDataSize);
		return decodeFrame_Xing;
	}
	
	// add audio data to the bitresevoir
	if(!mdb.adddata(pBuf, header.AudioDataSize, si.MainDataBegin))
	{
		DLog("Not enough BitReservoir to step back MainDataBegin!\n");
		return decodeFrame_NotEnoughBRV;
	}

	pBuf += header.AudioDataSize;
	
	if(PTRDIFF(pBufEnd, pBuf) < 36)
	{
		DLog("No more Data\n");
		return decodeFrame_NeedMoreData;
	}
	
	if( (pBuf[0] == 'T') && (pBuf[1] == 'A') && (pBuf[2] == 'G') )
	{
		// TAG FOUND ITS CRAP!!
		// TODO: should we classify this as 'Done' or not?
		return decodeFrame_Success;
	}

	/*
	int adv = 4;
	if (nextheader.CRCPresent) 
	{
		adv+=2;
	}

	SideInfo nextsi;
	nextsi.decodeSideInfo(pBuf+adv , nextheader);
	*/
	
	// here we calculate the number of bits that this frame uses
	// we need to check 'si' is zeroed out in all cases or we'll blow up!
	// we could stick a load of IF statements in here checking for stereo
	// or mpeg1 vs mpeg2, but its easier to just demand that the structure
	// is set to 0!
	uint32_t		p23total = 0;
	p23total += si.grinf[0][0].Part23Length;
	p23total += si.grinf[0][1].Part23Length;
	p23total += si.grinf[1][0].Part23Length;	
	p23total += si.grinf[1][1].Part23Length;
	
	mp3datasize = p23total / 8;
	if(p23total % 8)
		mp3datasize++;
	
	mdb.ExtractBytes(mp3data, mp3datasize);
	
	
	// at this point Header is valid
	// side info is extracted
	// mp3data should contain enough data to decompress scalefactors and huffman stuff
	// at some point we should move mdb and side info stuff to a separate layer 3 processor
	// or we only deal with mp3 and write separate decoders for layer 1 and 2 (which is perfectly accepable too)
	
	return decodeFrame_Success;
}

int mpegdecoder::synthFrame(float * tohere)
{
	l3dec.processFrame(tohere, mp3data, mp3datasize, header, si);
	return 0;
}
