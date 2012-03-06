/*
 *  sideinfo.h
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */
#pragma once

/* used in Granule.block_type */
#define BLOCKTYPE_NORMAL	0
#define BLOCKTYPE_START		1
#define BLOCKTYPE_3WIN		2
#define BLOCKTYPE_STOP		3

#define MAXCH				2
#define MAXGR				2
#define MAXSCFCI			4

#include "header.h"

typedef struct _GranuleInfo_
{
	unsigned int	Part23Length;	// number of bits used for scalefactors and huffmancode data
	unsigned int	BigValues;
	unsigned int	GlobalGain;
	
	unsigned int	ScalefacCompress;
	unsigned int	WindowSwitchingFlag;
	
	unsigned int	BlockType;
	unsigned int	MixedBlockFlag;
	
	unsigned int	TableSelect[4];
	unsigned int	SubblockGain[4];
	
	unsigned int	Region0Count;
	unsigned int	Region1Count;
	
	unsigned int	PreFlag;
	
	unsigned int	ScalefacScale;
	unsigned int	Count1Table_Select;
	
	unsigned int	Scalefac_Long[23];     /* actual scalefactors for this granule */
	unsigned int	Scalefac_Short[13][3];  /* scalefactors used in short windows */
	
	unsigned int	IntensityScale;
	unsigned int	is_max[21];
} GranuleInfo;

class SideInfo
{
public:
	unsigned int	MainDataBegin;
	unsigned int	PrivateBits;
	
	unsigned int	ScfSi[MAXCH][MAXSCFCI];
	
	GranuleInfo		grinf[MAXGR][MAXCH];
	
	bitstream	bs;	

	
	int decodeSideInfo(const uint8_t * data, Header & head);		// we need this
};

