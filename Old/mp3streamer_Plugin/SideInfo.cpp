/*
 *  sideinfo.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "sideinfo.h"

// TODO: more checks in here please!

int SideInfo::decodeSideInfo(const uint8_t * data, Header & head)
{
	uint32_t ch, gr, scfsi_band, region, window;
	
	bs.load(data, 32);
	
	// reset the part23length here 
	// its easier to just do it all in one go than to mess about
	grinf[0][0].Part23Length = 0;
	grinf[0][1].Part23Length = 0;
	grinf[1][0].Part23Length = 0;
	grinf[1][1].Part23Length = 0;
	
	if(head.MpegVersion == MPEG1)
	{
		MainDataBegin = bs.getbits(9);
		// maximum of 511 (2^9 - 1)
		if(MainDataBegin > 511)
			return 0;
		
		if(head.Channels == 1)
		{
			PrivateBits = bs.getbits(5);
		}
		else
			PrivateBits = bs.getbits(3);
		
		for(ch=0; ch<head.Channels; ch++)
			for(scfsi_band=0; scfsi_band<4; scfsi_band++)
				ScfSi[ch][scfsi_band] = bs.getbits(1);
		
		for(gr=0; gr<2; gr++)
		{
			for(ch=0; ch<head.Channels; ch++)
			{
				grinf[gr][ch].Part23Length					= bs.getbits(12);
				grinf[gr][ch].BigValues						= bs.getbits(9);
				
				// BigValues can't be more than 288 apparently
				if(grinf[gr][ch].BigValues > 288)
					return 0;//
				
				grinf[gr][ch].GlobalGain					= bs.getbits(8);
				
				grinf[gr][ch].ScalefacCompress				= bs.getbits(4);
				
				grinf[gr][ch].WindowSwitchingFlag			= bs.getbits(1);
				
				if(grinf[gr][ch].WindowSwitchingFlag == 1)
				{
					grinf[gr][ch].BlockType					= bs.getbits(2);
					// Block type 0 is invalid
					if(grinf[gr][ch].BlockType == 0)
						return 0;
					
					grinf[gr][ch].MixedBlockFlag			= bs.getbits(1);
					
					grinf[gr][ch].TableSelect[0]			= bs.getbits(5);
					grinf[gr][ch].TableSelect[1]			= bs.getbits(5);
					grinf[gr][ch].TableSelect[2]			= 0;
					
					grinf[gr][ch].SubblockGain[0]			= bs.getbits(3);
					grinf[gr][ch].SubblockGain[1]			= bs.getbits(3);
					grinf[gr][ch].SubblockGain[2]			= bs.getbits(3);
					
					// not used in short blocks
					if(	(grinf[gr][ch].BlockType == BLOCKTYPE_3WIN) && 
					   (grinf[gr][ch].MixedBlockFlag == 0) )
					{
						grinf[gr][ch].Region0Count			= 8;
					}
					else
						grinf[gr][ch].Region0Count			= 7;
					
					grinf[gr][ch].Region1Count				= 20 - grinf[gr][ch].Region0Count;
				}
				else
				{
					grinf[gr][ch].BlockType					= 0;
					grinf[gr][ch].MixedBlockFlag			= 0;
					
					grinf[gr][ch].TableSelect[0]			= bs.getbits(5);
					grinf[gr][ch].TableSelect[1]			= bs.getbits(5);
					grinf[gr][ch].TableSelect[2]			= bs.getbits(5);
					
					grinf[gr][ch].SubblockGain[0]			= 0;
					grinf[gr][ch].SubblockGain[1]			= 0;
					grinf[gr][ch].SubblockGain[2]			= 0;
					
					grinf[gr][ch].Region0Count				= bs.getbits(4);
					grinf[gr][ch].Region1Count				= bs.getbits(3);
				}
				
				grinf[gr][ch].PreFlag						= bs.getbits(1);
				grinf[gr][ch].ScalefacScale					= bs.getbits(1);
				grinf[gr][ch].Count1Table_Select			= bs.getbits(1);
			}
		}
	}
	else
	{
		MainDataBegin = bs.getbits(8);
		
		if(head.Channels == 1)
			PrivateBits = bs.getbits(1);
		else
			PrivateBits = bs.getbits(2);
		
		gr=0;
		
		for(ch=0; ch<head.Channels; ch++)
		{
			grinf[gr][ch].Part23Length					= bs.getbits(12);
			grinf[gr][ch].BigValues						= bs.getbits(9);
			grinf[gr][ch].GlobalGain					= bs.getbits(8);
			
			grinf[gr][ch].ScalefacCompress				= bs.getbits(9);
			
			grinf[gr][ch].WindowSwitchingFlag			= bs.getbits(1);
			
			if(grinf[gr][ch].WindowSwitchingFlag == 1)
			{
				grinf[gr][ch].BlockType					= bs.getbits(2);
				grinf[gr][ch].MixedBlockFlag			= bs.getbits(1);
				
				for(region = 0; region < 2; region++)
				{
					grinf[gr][ch].TableSelect[region]	= bs.getbits(5);
				}
				grinf[gr][ch].TableSelect[2]			= 0;
				
				for(window = 0; window < 3; window++)
				{
					grinf[gr][ch].SubblockGain[window]	= bs.getbits(3);
				}
				
				// not used in short blocks
				if(	(grinf[gr][ch].BlockType == BLOCKTYPE_3WIN) && 
				   (grinf[gr][ch].MixedBlockFlag == 0) )
				{
					grinf[gr][ch].Region0Count			= 8;
				}
				else
					grinf[gr][ch].Region0Count			= 7;
				
				grinf[gr][ch].Region1Count				= 20 - grinf[gr][ch].Region0Count;
			}
			else
			{
				grinf[gr][ch].BlockType					= 0;
				
				for(region = 0; region < 3; region++)
				{
					grinf[gr][ch].TableSelect[region]	= bs.getbits(5);
				}
				
				grinf[gr][ch].Region0Count				= bs.getbits(4);
				grinf[gr][ch].Region1Count				= bs.getbits(3);
			}
			
			grinf[gr][ch].ScalefacScale					= bs.getbits(1);
			grinf[gr][ch].Count1Table_Select			= bs.getbits(1);
		}
	}
	
	return 1;
}
