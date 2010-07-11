/*
 *  layer3decoder.cpp
 *  audioenginetest
 *
 *  Created by Tony Million on 01/12/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "generaldefs.h"

#include "layer3decoder.h"
#include "huffman.h"

#define PI		(3.1415926535897932384626433832795f)
#define PI12	(0.2617993877991494365385536152732f)
#define PI18	(0.17453292519943295769236907684886f)
#define PI36	(0.0872664625997164788461845384244f)

#define SQRT2	(1.4142135623730950488016887242097f)

typedef struct _SBI_
{
	unsigned short Long[23];
	unsigned short Short[14];
} SBI;

const SBI sfBandIndex[3][3] = 
{
	{ // MPEG1
		{	// 44.4 Khz
			{0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 52,  62,  74,  90, 110, 134, 162, 196, 238, 288, 342, 418, 576},
			{0, 4, 8, 12, 16, 22, 30, 40, 52, 66, 84, 106, 136, 192 }
		},
		{	// 48 Khz
			{0, 4, 8, 12, 16, 20, 24, 30, 36, 42, 50,  60,  72,  88, 106, 128, 156, 190, 230, 276, 330, 384, 576 },
			{0, 4, 8, 12, 16, 22, 28, 38, 50, 64, 80, 100, 126, 192 }
		},
		{	// 32Khz
			{0, 4, 8, 12, 16, 20, 24, 30, 36, 44,  54,  66,  82, 102, 126, 156, 194, 240, 296, 364, 448, 550, 576 },
			{0, 4, 8, 12, 16, 22, 30, 42, 58, 78, 104, 138, 180, 192}
		}
	},
	{ // MPEG2
		{	// 22.5khz
			{0, 6, 12, 18, 24, 30, 36, 44, 54, 66,  80,  96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576},
			{0, 4, 8,  12, 18, 24, 32, 42, 56, 74, 100, 132, 174, 192}
		},
		{	// 24Khz
			{0, 6, 12, 18, 24, 30, 36, 44, 54, 66,  80,  96, 114, 136, 162, 194, 232, 278, 332, 394, 464, 540, 576},
			{0, 4, 8,  12, 18, 26, 36, 48, 62, 80, 104, 136, 180, 192}
		},
		{	// 16khz
			{0, 6, 12, 18, 24, 30, 36, 44, 54, 66,  80,  96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576},
			{0, 4, 8,  12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192}
		},
	},
	{ // MPEG2.5
		{	// 11.25Khz
			{0, 6, 12, 18, 24, 30, 36, 44, 54, 66,  80,  96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576},
			{0, 4, 8,  12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192},
		},
		{	// 12Khz
			{0, 6, 12, 18, 24, 30, 36, 44, 54, 66,  80,  96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576},
			{0, 4, 8,  12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192},
		},
		{	// 8Khz
			{0, 12, 24, 36, 48, 60, 72, 88, 108, 132, 160, 192, 232, 280, 336, 400, 476, 566, 568, 570, 572, 574, 576},
			{0, 8,  16, 24, 36, 52, 72, 96, 124, 160, 162, 164, 166, 192},
		},
	}
};

const float ShortTwiddles[] = 
{
    0.866025403f, 0.500000000f, 1.931851653f, 
	0.707106781f, 0.517638090f, 0.504314480f, 
	0.541196100f, 0.630236207f, 0.821339815f, 
	1.306562965f, 3.830648788f, 0.793353340f, 
	0.608761429f, 0.923879532f, 0.382683432f, 
	0.991444861f, 0.130526192f, 0.382683432f, 
	0.608761429f, 0.793353340f, 0.923879532f, 
	0.991444861f, 0.130526192f
};

const float NormalTwiddles[] = 
{
    5.736856623f,   1.931851653f,  1.183100792f,
    0.871723397f,   0.707106781f,  0.610387294f, 
    0.551688959f,   0.517638090f,  0.501909918f,
    -0.500476342f, -0.504314480f, -0.512139757f,
    -0.524264562f, -0.541196100f, -0.563690973f, 
    -0.592844523f, -0.630236207f, -0.678170852f, 
    -0.740093616f, -0.821339815f, -0.930579498f,
    -1.082840285f, -1.306562965f, -1.662754762f, 
    -2.310113158f, -3.830648788f, -11.46279281f
};


layer3decoder::layer3decoder()
{
	int		i;
	float	ci[8] = { -0.6f,   -0.535f, -0.33f,   -0.185f,  -0.095f, -0.041f, -0.0142f, -0.0037f };
	
	for (i = 0; i < 8; i++) 
	{
		Cs[i] = 1.0f / (float)sqrt(1.0 + ci[i]*ci[i]);
		Ca[i] = ci[i] / (float)sqrt(1.0 + ci[i]*ci[i]);
	}
	
    for(i = 0; i < 64; i++) 
	{
		PowerTableMinus2[i]  = (float)pow(2.0, -2.0 * i);
		PowerTableMinus05[i] = (float)pow(2.0, -0.5 * i);
    }
	
    for(i = 0; i < 256; i++)
		GainTable[i] = (float)pow(2.0 , (0.25 * (i - 210.0)));
	
	// table for the MPEG1 intensity stereo position
	// 7 == INVALID_POS so....
	for(i=0; i<16; i++)
	{
		TanPi12Table[i] = (float)tan(i * PI/12);
	}
	
	// magic for the IMDCT stuff
	int odd_i, two_odd_i, four_odd_i, eight_odd_i;
	int j = 0;
	for(i = 0; i < 9; i++) 
	{
		odd_i = (i << 1) + 1;
		two_odd_i = odd_i << 1;
		four_odd_i = odd_i << 2;
		IMDCT9x8Table[j++] = (float)cos(PI18 * odd_i);
		IMDCT9x8Table[j++] = (float)cos(PI18 * two_odd_i);
		
		eight_odd_i = two_odd_i << 2;
		IMDCT9x8Table[j++] = (float)cos(PI18 * (four_odd_i - odd_i));
		IMDCT9x8Table[j++] = (float)cos(PI18 * four_odd_i);
		IMDCT9x8Table[j++] = (float)cos(PI18 * (four_odd_i + odd_i));
		IMDCT9x8Table[j++] = (float)cos(PI18 * (four_odd_i + two_odd_i));
		IMDCT9x8Table[j++] = (float)cos(PI18 * (eight_odd_i - odd_i));
		IMDCT9x8Table[j++] = (float)cos(PI18 * eight_odd_i);
    }
	
	/* block_type 0 (normal window) */
	for(i = 0; i < 36; i++)
		IMDCTwin[0][i] = (float)sin(PI36 * (i + 0.5));
	
	/* block_type 1 (start block) */
	for(i = 0; i < 18; i++)
		IMDCTwin[1][i] = (float)sin(PI36 * (i + 0.5));
	for(i = 18; i < 24; i++)
		IMDCTwin[1][i] = 1.0f;
	for(i = 24; i < 30; i++)
		IMDCTwin[1][i] = (float)sin(PI12 * (i - 18 + 0.5));
	for(i = 30; i < 36; i++)
		IMDCTwin[1][i] = 0.0f;
	
	/* block_type 2 (short block) */
	for(i = 0; i < 12; i++)
		IMDCTwin[2][i] = (float)sin(PI12 * (i + 0.5));
	for(i = 12; i < 36; i++)
		IMDCTwin[2][i] = 0.0f;
	
	/* block_type 3 (stop block) */
	for(i = 0; i < 6; i++)
		IMDCTwin[3][i] = 0.0f;
	for(i = 6; i < 12; i++)
		IMDCTwin[3][i] = (float)sin(PI12 * (i - 6 + 0.5));
	for(i = 12; i < 18; i++)
		IMDCTwin[3][i] = 1.0f;
	for(i = 18; i < 36; i++)
		IMDCTwin[3][i] = (float)sin(PI36 * (i + 0.5));	
	
	reset();
}

void layer3decoder::DecodeScaleFactorsMPEG1(SideInfo & si, int ch, int gr)
{
	const uint32_t slentab1[16] = {0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4};
	const uint32_t slentab2[16] = {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3};
	
	int sfb, window;
	
	for(sfb=0; sfb<21; sfb++)
	{
		si.grinf[gr][ch].Scalefac_Long[sfb] = 0;
	}
	
	for(sfb=0; sfb<13; sfb++)
	{
		si.grinf[gr][ch].Scalefac_Short[sfb][0] = 0;
		si.grinf[gr][ch].Scalefac_Short[sfb][1] = 0;
		si.grinf[gr][ch].Scalefac_Short[sfb][2] = 0;
	}

	uint32_t slen1 = slentab1[si.grinf[gr][ch].ScalefacCompress];
	uint32_t slen2 = slentab2[si.grinf[gr][ch].ScalefacCompress];
	
	if( (si.grinf[gr][ch].WindowSwitchingFlag == 1) && (si.grinf[gr][ch].BlockType == 2))
	{
		if(si.grinf[gr][ch].MixedBlockFlag)
		{
			// this is the long block
			for (sfb = 0; sfb < 8; sfb++)
				si.grinf[gr][ch].Scalefac_Long[sfb]					= br.getbits(slen1);
			
			for (sfb = 3; sfb < 6; sfb++)
				for (window=0; window<3; window++)
					si.grinf[gr][ch].Scalefac_Short[sfb][window]	= br.getbits(slen1);
			
			for (sfb = 6; sfb < 12; sfb++)
				for (window=0; window<3; window++)
					si.grinf[gr][ch].Scalefac_Short[sfb][window]	= br.getbits(slen2);
			
			for (sfb=12,window=0; window<3; window++)
				si.grinf[gr][ch].Scalefac_Short[12][window] = 0;
		} 
		else 
		{  // SHORT
			// this is a short block...
			for(sfb=0; sfb<6; sfb++)
			{
				for(window=0; window<3; window++)
				{
					si.grinf[gr][ch].Scalefac_Short[sfb][window]	= br.getbits(slen1);
				}
			}
			
			for(sfb=6; sfb<12; sfb++)
			{
				for(window=0; window<3; window++)
				{
					si.grinf[gr][ch].Scalefac_Short[sfb][window]	= br.getbits(slen2);
				}
			}
			
			for(window=0; window<3; window++)
				si.grinf[gr][ch].Scalefac_Short[12][window] = 0;
		}
	}
	else
	{
		for(sfb=0; sfb<6; sfb++)
		{
			if((si.ScfSi[ch][0] == 0) || (gr == 0))
			{
				si.grinf[gr][ch].Scalefac_Long[sfb] = br.getbits(slen1);
			}
			else
				si.grinf[1][ch].Scalefac_Long[sfb] = si.grinf[0][ch].Scalefac_Long[sfb];
		}
		
		for(sfb=6; sfb<11; sfb++)
		{
			if((si.ScfSi[ch][1] == 0) || (gr == 0))
			{
				si.grinf[gr][ch].Scalefac_Long[sfb] = br.getbits(slen1);
			}
			else
				si.grinf[1][ch].Scalefac_Long[sfb] = si.grinf[0][ch].Scalefac_Long[sfb];
		}
		
		for(sfb=11; sfb<16; sfb++)
		{
			if((si.ScfSi[ch][2] == 0) || (gr == 0))
			{
				si.grinf[gr][ch].Scalefac_Long[sfb] = br.getbits(slen2);
			}
			else
				si.grinf[1][ch].Scalefac_Long[sfb] = si.grinf[0][ch].Scalefac_Long[sfb];
		}
		
		for(sfb=16; sfb<21; sfb++)
		{
			if((si.ScfSi[ch][3] == 0) || (gr == 0))
			{
				si.grinf[gr][ch].Scalefac_Long[sfb] = br.getbits(slen2);
			}
			else
				si.grinf[1][ch].Scalefac_Long[sfb] = si.grinf[0][ch].Scalefac_Long[sfb];
		}
		
		si.grinf[gr][ch].Scalefac_Long[21] = 0;
	}
	
}

void layer3decoder::DecodeScaleFactorsMPEG2(SideInfo & si, int ch, int gr)
{
	int sfb;
	
	for(sfb=0; sfb<21; sfb++)
	{
		si.grinf[gr][ch].Scalefac_Long[sfb] = 0;
	}
	
	for(sfb=0; sfb<13; sfb++)
	{
		si.grinf[gr][ch].Scalefac_Short[sfb][0] = 0;
		si.grinf[gr][ch].Scalefac_Short[sfb][1] = 0;
		si.grinf[gr][ch].Scalefac_Short[sfb][2] = 0;
	}
	
	int nfsbtable [2][3][3][4] =
	{
		{
			{
				{6, 5, 5, 5},
				{9, 9, 9, 9},
				{6, 9, 9, 9}
			},
			{
				{6, 5, 7,  3},
				{9, 9, 12, 6},
				{6, 9, 12, 6}
			},
			{
				{11, 10, 0, 0},
				{18, 18, 0, 0},
				{15, 18, 0, 0}
			},
		},
		{
			{
				{ 7,  7,  7, 0},
				{12, 12, 12, 0},
				{ 6, 15, 12, 0}
			},
			{
				{ 6,  6, 6, 3},
				{12,  9, 9, 6},
				{ 6, 12, 9, 6}
			},
			{
				{ 8, 8,  5, 0},
				{15, 12, 9, 0},
				{ 6, 18, 9, 0}
			},
		},
	};
	
	uint32_t scalefac_comp;
	
	uint32_t slen[4] = {0,0,0,0};
	uint32_t index1, index2, index3;
	
	if( (si.grinf[gr][ch].WindowSwitchingFlag == 1) && (si.grinf[gr][ch].BlockType == 2))
	{
		if(si.grinf[gr][ch].MixedBlockFlag)
		{
			index3 = 2;
		}
		else
		{
			index3 = 1;
		}
	}
	else
	{
		index3 = 0;
	}
	
	scalefac_comp =  si.grinf[gr][ch].ScalefacCompress;
	
	if(!( ((m_ModeExt == 1) || (m_ModeExt==3)) && (ch == 1)))
	{
		index1 = 0;
		si.grinf[gr][ch].PreFlag = 0;
		
		
		if (scalefac_comp>=500)
		{
			slen[0] = ((scalefac_comp-500)/ 3)%4;
			slen[1] = ((scalefac_comp-500)/ 1)%3;
			slen[2] = ((scalefac_comp-500)/ 1)%1;
			slen[3] = ((scalefac_comp-500)/ 1)%1;
			index2 = 2;
			si.grinf[gr][ch].PreFlag = 1;
		}
		else if (scalefac_comp>=400)
		{
			slen[0] = ((scalefac_comp-400)/20)%5;
			slen[1] = ((scalefac_comp-400)/ 4)%5;
			slen[2] = ((scalefac_comp-400)/ 1)%4;
			slen[3] = ((scalefac_comp-400)/ 1)%1;
			index2 = 1;
		}
		else
		{
			slen[0] = ((scalefac_comp-  0)/80)%5;
			slen[1] = ((scalefac_comp-  0)/16)%5;
			slen[2] = ((scalefac_comp-  0)/ 4)%4;
			slen[3] = ((scalefac_comp-  0)/ 1)%4;
			index2 = 0;
		}
	}
	else
	{
		index1 = 1;
		
		si.grinf[gr][ch].PreFlag = 0;
		scalefac_comp>>=1;
		
		if (scalefac_comp>=244)
		{
			slen[0] = ((scalefac_comp-244)/ 3)%4;
			slen[1] = ((scalefac_comp-244)/ 1)%3;
			slen[2] = ((scalefac_comp-244)/ 1)%1;
			slen[3] = ((scalefac_comp-244)/ 1)%1;
			index2 = 2;
		}
		else if (scalefac_comp>=180)
		{
			slen[0] = ((scalefac_comp-180)/16)%4;
			slen[1] = ((scalefac_comp-180)/ 4)%4;
			slen[2] = ((scalefac_comp-180)/ 1)%4;
			slen[3] = ((scalefac_comp-180)/ 1)%1;
			index2 = 1;
		}
		else
		{
			slen[0] = ((scalefac_comp-  0)/36)%5;
			slen[1] = ((scalefac_comp-  0)/ 6)%6;
			slen[2] = ((scalefac_comp-  0)/ 1)%6;
			slen[3] = ((scalefac_comp-  0)/ 1)%1;
			index2 = 0;
		}
	}
	
	if( (si.grinf[gr][ch].WindowSwitchingFlag == 1) && (si.grinf[gr][ch].BlockType == 2))
	{
		if(si.grinf[gr][ch].MixedBlockFlag)
		{
		}
		else
		{
			int sfb = 0;
			int window = 0;
			
			for(int j=0; j<4; j++)
			{
				for(int i=0; i<nfsbtable[index1][index2][index3][j]; i++)
				{
					if(slen[j] > 0)
						si.grinf[gr][ch].Scalefac_Short[sfb][window] = br.getbits(slen[j]);
					else
						si.grinf[gr][ch].Scalefac_Short[sfb][window] = 0;
					
					window++;
					if(window > 2)
					{
						if( (m_MpegVer != MPEG1) && ((m_ModeExt == 1) || (m_ModeExt==3)) )
							si.grinf[gr][ch].is_max[sfb] = (1<<slen[j])-1;
						
						window = 0;
						sfb++;
					}
					
				}
			}
			
			for(window=0; window<3; window++)
				si.grinf[gr][ch].Scalefac_Short[12][window] = 0;
		}
	}
	else
	{
		int sfb = 0;
		
		for(int j=0; j<4; j++)
		{
			for(int i=0; i<nfsbtable[index1][index2][index3][j]; i++)
			{
				if(slen[j] > 0)
					si.grinf[gr][ch].Scalefac_Long[sfb] = br.getbits(slen[j]);
				else
					si.grinf[gr][ch].Scalefac_Long[sfb] = 0;
				
				if( (m_MpegVer != MPEG1) && ((m_ModeExt == 1) || (m_ModeExt==3)) )
					si.grinf[gr][ch].is_max[sfb] = (1<<slen[j])-1;
				
				sfb++;
			}
		}
		si.grinf[gr][ch].Scalefac_Long[21] = 0;
		si.grinf[gr][ch].Scalefac_Long[22] = 0;
	}
}

bool layer3decoder::ReadHuffman(uint32_t ch, uint32_t gr, SideInfo & si, Header & head)
{
	uint32_t index = 0;
	uint32_t Part23End;
	uint32_t Region1;
	uint32_t Region2;
	uint32_t TableNumber;
	
	int x, y, v, w;
	
	// quick return if we have an empty frame!
	if(si.grinf[gr][ch].Part23Length == 0) 
	{
		for(index = 0; index < 576; index++) 
		{
			is[ch][index] = 0;
		}
		
		m_NonZero[ch] = 0;
		
		return true;
	}
	
	Part23End = m_Part2Start[ch] + si.grinf[gr][ch].Part23Length;
	
	if(m_MpegVer == MPEG1)
	{
		m_MixedBandLimit[ch] = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[8];
	}
	else
	{
		m_MixedBandLimit[ch] = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[6];
	}
	
	if(	(si.grinf[gr][ch].WindowSwitchingFlag) && (si.grinf[gr][ch].BlockType == BLOCKTYPE_3WIN) )
	{
		Region1 = m_MixedBandLimit[ch];
		Region2 = 576;
	} 
	else 
	{
		Region1 = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[si.grinf[gr][ch].Region0Count + 1];
		Region2 = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[si.grinf[gr][ch].Region0Count + si.grinf[gr][ch].Region1Count + 2];
	}
	
	
	while(index < (si.grinf[gr][ch].BigValues * 2)) 
	{
		if(index < Region1) 
		{
			TableNumber = si.grinf[gr][ch].TableSelect[0];
		} 
		else if(index < Region2) 
		{
			TableNumber = si.grinf[gr][ch].TableSelect[1];
		} 
		else 
		{
			TableNumber = si.grinf[gr][ch].TableSelect[2];
		}
		
		if(!HuffmanDecode2(TableNumber, &x, &y, br))
			return false;
		
		is[ch][index++]	= x;
		is[ch][index++]	= y;
	}
	
	uint32_t pos = br.getBitOffset();
	
	TableNumber = si.grinf[gr][ch].Count1Table_Select + 32;
	while((index < 576) && (pos < Part23End)) 
	{
		if(!HuffmanDecode4(TableNumber, &x, &y, &v, &w, br))
			return false;
		
		is[ch][index++] = v;
		is[ch][index++] = w;
		is[ch][index++] = x;   
		is[ch][index++] = y;
		
		pos = br.getBitOffset();
	}
	
	m_NonZero[ch] = index;
	
	
	for(; index < 576; index++) 
	{
		is[ch][index] = 0;
	}
	
	return true;
}

void layer3decoder::DequantizeSample(uint32_t ch, uint32_t gr, SideInfo & si)
{
 	static const int pretab[22] = { 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0 };
	
	uint32_t index		= 0;
	int window		= 0;
	int windowsize	= 0;
	int sfb			= 0;
	int startband	= 0;
	float txr		= 0.0f;
    float globscale = GainTable[ si.grinf[gr][ch].GlobalGain ];
	
	
    if(si.grinf[gr][ch].WindowSwitchingFlag && si.grinf[gr][ch].BlockType == BLOCKTYPE_3WIN) 
	{
		if(si.grinf[gr][ch].MixedBlockFlag) 
		{
			int endblock = 0;
			// Mixed Block
			
			if(m_MpegVer == MPEG1)
			{
				endblock = 8;
			}
			else
			{
				endblock = 6;
			}
			
			for(sfb=0; sfb<endblock; sfb++)
			{
				windowsize = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb+1] - sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb];
				
				for(int i=0; i<windowsize; i++)
				{
					txr = globscale * PowerTableMinus05[(1 + si.grinf[gr][ch].ScalefacScale) * (si.grinf[gr][ch].Scalefac_Long[sfb] + (si.grinf[gr][ch].PreFlag * pretab[sfb]) )];
					xr[ch][index] = txr * ( pow( (float)abs(is[ch][index]), (float)1.333333333333333) * ( is[ch][index] > 0 ? 1 : -1 ) );
					
					index ++;	// should be 36 at the end of all this!
				}
			}
			
			startband = 3;
		}
		
		// Short Block
		// xr[i] = sign(is[i]) * |is[i]|^(4/3) * 2 ^ 0.25 * (global_gain - 210 - 8 * subblock_gain[window]) * 2 ^ -(scalefac_multiplier * scalefac_s[band][window])
		
		// short blocks are arranged as 3 windowsize windows per scalefactorband
		// like this: 111 111 111  22 22 22  3 3 3
		//				SFB1		SFB2		SFB3 ... etc
		
		for(sfb=startband; sfb < 13; sfb++)
		{
			windowsize = sfBandIndex[m_MpegVer][m_SampleFrequency].Short[sfb+1] - sfBandIndex[m_MpegVer][m_SampleFrequency].Short[sfb];
			
			for(window = 0; window<3; window++)
			{
				for(int i=0; i<windowsize; i++)
				{
					txr = globscale * PowerTableMinus2[si.grinf[gr][ch].SubblockGain[window]] * PowerTableMinus05[(1 + si.grinf[gr][ch].ScalefacScale) * si.grinf[gr][ch].Scalefac_Short[sfb][window]];
					/* apply the sign(is[i]) * |is[i]| ^ (4/3) formula */
					xr[ch][index] = txr * ( pow( (float)abs(is[ch][index]), (float)1.333333333333333) * ( is[ch][index] > 0 ? 1 : -1 ) );
					index ++;
					if(index > m_NonZero[ch])
						break;
				}
			}
		}
	}
	else
	{
		// Long Block
		// xr[i] = sign(is[i]) * | is[i] |^(4/3) * 2 ^ 0.25 * (global_gain - 210) * 2 ^ -(scalefac_multiplier * (scalefac_l[band] + preflag * pretab[band]))
		
		
		for(sfb=0; sfb<22; sfb++)
		{
			windowsize = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb+1] - sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb];
			
			for(int i=0; i<windowsize; i++)
			{
				txr = globscale * PowerTableMinus05[(1 + si.grinf[gr][ch].ScalefacScale) * (si.grinf[gr][ch].Scalefac_Long[sfb] + (si.grinf[gr][ch].PreFlag * pretab[sfb]) )];
				
				float tpow = pow( (float)abs(is[ch][index]), (float)1.333333333333333);
				if(is[ch][index] < 0)
					tpow = -tpow;
				
				xr[ch][index] = txr * tpow ;
				
				index ++;
				if(index > m_NonZero[ch])
					break;
			}
		}
	}
	
	for(; index<576; index++)
	{
		xr[ch][index] = 0.0;
	}
}

void layer3decoder::Reorder(uint32_t ch, uint32_t gr, SideInfo & si)
{
    if(si.grinf[gr][ch].WindowSwitchingFlag && si.grinf[gr][ch].BlockType == BLOCKTYPE_3WIN) 
	{
		int ScaleFactorBand = 0;
		int BandStart;
		int BandSize;
		int Window, src, dst;
		int i;
		
		if(si.grinf[gr][ch].MixedBlockFlag) 
		{
			for(uint32_t index=0; index < m_MixedBandLimit[ch]; index++)
			{
		  		xrr[ch][index] = xr[ch][index];
			}
			
			
			ScaleFactorBand = 4;
		}
		
		/* short block, reorder everything  */
		// this is a 3window block, we need to reorder it.... in something like this
		// 111 222 333 444 555 666 777 888 999 101010 111111 121212
		// to
		// 123123123 456456456 789789789 101112101112101112
		do
		{
			BandStart = sfBandIndex[m_MpegVer][m_SampleFrequency].Short[ScaleFactorBand];
			BandSize  = sfBandIndex[m_MpegVer][m_SampleFrequency].Short[ScaleFactorBand+1] - BandStart;
			
			src = BandStart * 3;
			
			for(Window = 0; Window < 3; Window++)
			{
				dst = (BandStart * 3) + Window;
				
				for(i = 0; i < BandSize; i++) 
				{
					xrr[ch][dst] = xr[ch][src];
					src++;
					dst += 3;
				}
			}
			
			ScaleFactorBand++;
			
		} while(ScaleFactorBand < 13);
	}
	else
	{   
		// long blocks
		for(uint32_t index=0; index < 576; index++)
		{
			xrr[ch][index] = xr[ch][index];
		}
	}
}

void layer3decoder::Stereo(uint32_t gr, SideInfo & si)
{
	uint32_t index;
	
	if(m_Channels == 1)		// dont bother with stereo processing unless its STEREO duuh
	{
		return;
	}
	
	int MS_Stereo	= (m_Mode == MODE_JOINT_STEREO) && (m_ModeExt & MODE_EXT_MS);
	int I_Stereo	= (m_Mode == MODE_JOINT_STEREO) && (m_ModeExt & MODE_EXT_IS);
	
	int	is_pos;	// intensity stereo positions
	int is_valid[576];
	float	is_ratio[576];	// intensity stereo left / right ratio
	
	int		intensity_scale = (si.grinf[gr][0].ScalefacCompress % 2);
	
	if(I_Stereo)
	{
		DLog("istereo\n");
		for(index=0; index<576; index++)
			is_valid[index] = 0;
		
		
		if(si.grinf[gr][0].WindowSwitchingFlag && (si.grinf[gr][0].BlockType == BLOCKTYPE_3WIN)) 
		{
			if(si.grinf[gr][0].MixedBlockFlag) 
			{
				DLog("Mixed Block - Arrgh\n");
			}
			else
			{
				uint32_t startband = 0;
				while((uint32_t)(sfBandIndex[m_MpegVer][m_SampleFrequency].Short[startband]*3) < m_NonZero[1])
					startband ++;
				
				for(int sfb=startband; sfb<13; sfb++)
				{
					int windowsize = sfBandIndex[m_MpegVer][m_SampleFrequency].Short[sfb+1] - sfBandIndex[m_MpegVer][m_SampleFrequency].Short[sfb];
					
					for(int window=0; window<3; window++)
					{
						int sfb_start = sfBandIndex[m_MpegVer][m_SampleFrequency].Short[sfb]*3 + windowsize*window;
						int sfb_stop = sfb_start + windowsize;
						
						for(int i = sfb_start; i < sfb_stop; i++) 
						{
							is_pos = si.grinf[gr][1].Scalefac_Short[ sfb ][window];
							
							if(m_MpegVer == MPEG1)
							{
								if (is_pos != 7)
								{
									is_ratio[ i ] = TanPi12Table[ is_pos ];
									is_valid[ i ] = 1;
								}
							}
							else
							{
								if(is_pos != si.grinf[gr][1].is_max[ sfb ] )
								{
									//CalculateK(i, is_pos, intensity_scale);
									is_valid[ i ] = 1;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			int i;
			
			int startband = 0;
			
			while(sfBandIndex[m_MpegVer][m_SampleFrequency].Long[startband] < m_NonZero[1])
				startband ++;
			
			for(int sfb = startband; sfb<22; sfb++)
			{
				i = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb];
				
				int windowsize = sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb+1] - sfBandIndex[m_MpegVer][m_SampleFrequency].Long[sfb];
				
				for(int w=0; w<windowsize; w++)
				{
					is_pos = si.grinf[gr][1].Scalefac_Long[ sfb ];
					
					if(m_MpegVer == MPEG1)
					{
						if (is_pos != 7)
						{
							is_ratio[ i ] = TanPi12Table[ is_pos ];
							is_valid[ i ] = 1;
						}
					}
					else
					{
						if(is_pos != si.grinf[gr][1].is_max[ sfb ] )
						{
							//CalculateK(i, is_pos, intensity_scale);
							is_valid[ i ] = 1;
						}
					}
					
					i++;
				}
			}
		}
		
		// process the intensity stere stuff
		for(index=0; index<576; index++)
		{
			float temp = xrr[0][index];
			
			if( is_valid[index] == 0 )
			{	
				// NOT intensity Stereo Mode
				if(MS_Stereo) 
				{
					// MSStereo mode. we have to undo it...
					xrr[0][index] = (temp + xrr[1][index]) / SQRT2;
					xrr[1][index] = (temp - xrr[1][index]) / SQRT2;
				} 
			}
			else
			{
				// this IS an intensity stereo value
				if(m_MpegVer == MPEG1)
				{
					xrr[0][index] = temp * (is_ratio[index] / (1.0f + is_ratio[index]));
					xrr[1][index] = temp * (1.0f / (1.0f + is_ratio[index])); 
				}
				else
				{
					xrr[0][index] = temp * kl[index];
					xrr[1][index] = temp * kr[index];
				}
			}
		}
	}
	else if(MS_Stereo) 
	{
		// MSStereo mode. we have to undo it...
		for(index=0; index<576; index++)
		{
			float temp = xrr[0][index];
			xrr[0][index] = (temp + xrr[1][index]) / SQRT2;
			xrr[1][index] = (temp - xrr[1][index]) / SQRT2;
		}
	} 
}

void layer3decoder::AntiAlias(uint32_t ch, uint32_t gr, SideInfo & si)
{
	uint32_t sb_amount;

	if(si.grinf[gr][ch].BlockType != BLOCKTYPE_3WIN)
	{
		sb_amount = 558;	// antialias the full stectrum
	}
	else
	{
		if(si.grinf[gr][ch].MixedBlockFlag)
		{	// this is a long block, then a short block
			sb_amount = m_MixedBandLimit[ch];
		}
		else	// No antialiasing for short blocks
		{
			return;
		}
	}
	
    uint32_t src_idx1;
	uint32_t src_idx2;
	
	register float temp;
	
	for(int sb = 18; sb < sb_amount; sb+=18)
	{
		for(int i=0; i<8; i++)
		{
			src_idx1 = sb - 1 - i;
			src_idx2 = sb + i;
			
			temp = xrr[ch][src_idx1];
			
			xrr[ch][src_idx1] = (temp * Cs[i])				- (xrr[ch][src_idx2] * Ca[i]);
			xrr[ch][src_idx2] = (xrr[ch][src_idx2] * Cs[i]) + (temp * Ca[i]);
		}
	}
	
}

void layer3decoder::FreqencyInverse(uint32_t gr, uint32_t ch)
{
    uint32_t sb, ss;
	
	uint32_t offset;
	
	for(sb = 1; sb < 32; sb+=2)
	{
		offset = 32;
		for(ss=1; ss<18; ss+=2)
		{
			xir[ch][offset+sb] = -xir[ch][offset+sb];
			offset += 64;
		}
	}
}

void layer3decoder::IMDCT(float *in, float *out, uint32_t block_type)
{
	if(block_type == 2)
	{
		float	tmp[18];
		register float	sum;
		register float save;
		register float	pp1;
		
		uint32_t		six_i = 6;
		uint32_t		window;
		
		for(int i=0; i<36; i++)
		{
			out[i] = 0.0;
		}
		
		for(window=0; window<3; window++)
   		{
			in[15+window] += in[12+window];
			in[12+window] += in[9+window];
			in[9+window]  += in[6+window];
			in[6+window]  += in[3+window];
			in[3+window]  += in[window];
			
			in[15+window] += in[9+window];
			in[9+window]  += in[3+window];
			
			pp1 = in[6+window] * ShortTwiddles[0];
			sum = in[window]   + in[12+window] * ShortTwiddles[1];
			
			tmp[1] = in[window] - in[12+window];
			tmp[0] = sum + pp1;
			tmp[2] = sum - pp1;
			
			pp1 = in[9+window] * ShortTwiddles[0];
			sum = in[3+window] + in[15+window] * ShortTwiddles[1];
			
			tmp[4] = in[3+window] - in[15+window];
			tmp[5] = sum + pp1;
			tmp[3] = sum - pp1;
			
			tmp[3] *= ShortTwiddles[2];
			tmp[4] *= ShortTwiddles[3];
			tmp[5] *= ShortTwiddles[4];
			
			save = tmp[0];
			tmp[0] += tmp[5];
			tmp[5]  = save - tmp[5];
			
			save = tmp[1];
			tmp[1] += tmp[4];
			tmp[4]  = save - tmp[4];
			
			save = tmp[2];
			tmp[2] += tmp[3];
			tmp[3]  = save - tmp[3];
			
			tmp[0] *= ShortTwiddles[5];
			tmp[1] *= ShortTwiddles[6];
			tmp[2] *= ShortTwiddles[7];
			tmp[3] *= ShortTwiddles[8];
			tmp[4] *= ShortTwiddles[9];
			tmp[5] *= ShortTwiddles[10];
			
			tmp[6]  = -tmp[2] * ShortTwiddles[15];
			tmp[7]  = -tmp[1] * ShortTwiddles[13];
			tmp[8]  = -tmp[0] * ShortTwiddles[11];
			tmp[9]  = -tmp[0] * ShortTwiddles[12];
			tmp[10] = -tmp[1] * ShortTwiddles[14];
			tmp[11] = -tmp[2] * ShortTwiddles[16];
			
			tmp[0]  =  tmp[3];
			tmp[1]  =  tmp[4] * ShortTwiddles[17];
			tmp[2]  =  tmp[5] * ShortTwiddles[18];
			
			tmp[3]  = -tmp[5] * ShortTwiddles[19];
			tmp[4]  = -tmp[4] * ShortTwiddles[20];
			tmp[5]  = -tmp[0] * ShortTwiddles[21];
			
			tmp[0] *= ShortTwiddles[22];
			
   			out[six_i]		+= tmp[0];
			out[six_i + 1]  += tmp[1];
	   		out[six_i + 2]  += tmp[2];
			out[six_i + 3]  += tmp[3];
   			out[six_i + 4]  += tmp[4];
			out[six_i + 5]  += tmp[5];
	   		out[six_i + 6]  += tmp[6];
			out[six_i + 7]  += tmp[7];
	   		out[six_i + 8]  += tmp[8];
			out[six_i + 9]  += tmp[9];
	   		out[six_i + 10] += tmp[10];
			out[six_i + 11] += tmp[11];
			
   			six_i += 6;
   		}
	} 
	else 
	{
		float	tmp[18];
		
		uint32_t i, j;
		register float sum;
		register float sum2;
		register float save;
		
	    in[17] += in[16];
	    in[16] += in[15];
	    in[15] += in[14];
	    in[14] += in[13];
	    in[13] += in[12];
	    in[12] += in[11];
	    in[11] += in[10];
	    in[10] += in[9];
	    in[9]  += in[8];
	    in[8]  += in[7];
	    in[7]  += in[6];
	    in[6]  += in[5];
	    in[5]  += in[4];
	    in[4]  += in[3];
	    in[3]  += in[2];
	    in[2]  += in[1];
	    in[1]  += in[0];
		
		in[17] += in[15];
	    in[15] += in[13];
	    in[13] += in[11];
	    in[11] += in[9];
	    in[9]  += in[7];
	    in[7]  += in[5];
	    in[5]  += in[3];
	    in[3]  += in[1];
		
		j = 0;
		i = 0;
		
		int b = 9;
		do
		{
			sum = in[0];
			sum2 = in[1];
			
			sum  += in[2]  * IMDCT9x8Table[j];
			sum2 += in[3]  * IMDCT9x8Table[j];
			sum  += in[4]  * IMDCT9x8Table[j+1];
			sum2 += in[5]  * IMDCT9x8Table[j+1];
			sum  += in[6]  * IMDCT9x8Table[j+2];
			sum2 += in[7]  * IMDCT9x8Table[j+2];
			sum  += in[8]  * IMDCT9x8Table[j+3];
			sum2 += in[9]  * IMDCT9x8Table[j+3];
			sum  += in[10] * IMDCT9x8Table[j+4];
			sum2 += in[11] * IMDCT9x8Table[j+4];
			sum  += in[12] * IMDCT9x8Table[j+5];
			sum2 += in[13] * IMDCT9x8Table[j+5];
			sum  += in[14] * IMDCT9x8Table[j+6];
			sum2 += in[15] * IMDCT9x8Table[j+6];
			sum  += in[16] * IMDCT9x8Table[j+7];
			sum2 += in[17] * IMDCT9x8Table[j+7];
			
			tmp[i]		= sum;
			tmp[17-i]	= sum2;
			
			j += 8;
			i++;
			
		} while(--b);
		
		tmp[9]  *= NormalTwiddles[0];
		tmp[10] *= NormalTwiddles[1];
		tmp[11] *= NormalTwiddles[2];
		tmp[12] *= NormalTwiddles[3];
		tmp[13] *= NormalTwiddles[4];
		tmp[14] *= NormalTwiddles[5];
		tmp[15] *= NormalTwiddles[6];
		tmp[16] *= NormalTwiddles[7];
		tmp[17] *= NormalTwiddles[8];
		
	    for(i = 0; i < 9; i++) 
		{
			save		 = tmp[i];
			tmp[i]		+= tmp[17-i];
			tmp[17-i]	 = save - tmp[17-i];
	    }
		
		tmp[0]  *= NormalTwiddles[9];
		tmp[1]  *= NormalTwiddles[10];
		tmp[2]  *= NormalTwiddles[11];
		tmp[3]  *= NormalTwiddles[12];
		tmp[4]  *= NormalTwiddles[13];
		tmp[5]  *= NormalTwiddles[14];
		tmp[6]  *= NormalTwiddles[15];
		tmp[7]  *= NormalTwiddles[16];
		tmp[8]  *= NormalTwiddles[17];
		tmp[9]  *= NormalTwiddles[18];
		tmp[10] *= NormalTwiddles[19];
		tmp[11] *= NormalTwiddles[20];
		tmp[12] *= NormalTwiddles[21];
		tmp[13] *= NormalTwiddles[22];
		tmp[14] *= NormalTwiddles[23];
		tmp[15] *= NormalTwiddles[24];
		tmp[16] *= NormalTwiddles[25];
		tmp[17] *= NormalTwiddles[26];
		
	    for(i = 0; i < 9; i++) 
		{
			out[i]		= -tmp[i+9]	* IMDCTwin[block_type][i];
			out[i+9]	= tmp[17-i]	* IMDCTwin[block_type][i+9];
			out[i+18]	= tmp[8-i]	* IMDCTwin[block_type][i+18];
			out[i+27]	= tmp[i]	* IMDCTwin[block_type][i+27];
	    }
	}
}

void layer3decoder::Hybrid(uint32_t ch, float *xfrom, float *xto, uint32_t blocktype, uint32_t windowswitching, uint32_t mixedblock)
{
	float rawout[36];
	uint32_t bt;
	uint32_t sb18 = 0;
	uint32_t sb = 0;
	
	uint32_t x = 32;
	
	do
	{
		bt = (windowswitching && mixedblock && (sb18 < 36)) ? 0 : blocktype;
		
		IMDCT(&xfrom[sb18], rawout, bt);
		
		xto[sb]		= rawout[ 0] + prevblck[ch][sb18+0];
		xto[sb+32]	= rawout[ 1] + prevblck[ch][sb18+1];
		xto[sb+64]	= rawout[ 2] + prevblck[ch][sb18+2];
		xto[sb+96]	= rawout[ 3] + prevblck[ch][sb18+3];
		xto[sb+128]	= rawout[ 4] + prevblck[ch][sb18+4];
		xto[sb+160]	= rawout[ 5] + prevblck[ch][sb18+5];
		xto[sb+192]	= rawout[ 6] + prevblck[ch][sb18+6];
		xto[sb+224]	= rawout[ 7] + prevblck[ch][sb18+7];
		xto[sb+256]	= rawout[ 8] + prevblck[ch][sb18+8];
		xto[sb+288]	= rawout[ 9] + prevblck[ch][sb18+9];
		xto[sb+320]	= rawout[10] + prevblck[ch][sb18+10];
		xto[sb+352]	= rawout[11] + prevblck[ch][sb18+11];
		xto[sb+384]	= rawout[12] + prevblck[ch][sb18+12];
		xto[sb+416]	= rawout[13] + prevblck[ch][sb18+13];
		xto[sb+448]	= rawout[14] + prevblck[ch][sb18+14];
		xto[sb+480]	= rawout[15] + prevblck[ch][sb18+15];
		xto[sb+512]	= rawout[16] + prevblck[ch][sb18+16];
		xto[sb+544]	= rawout[17] + prevblck[ch][sb18+17];
		
		prevblck[ch][sb18+0]	= rawout[18];
		prevblck[ch][sb18+1]	= rawout[19];
		prevblck[ch][sb18+2]	= rawout[20];
		prevblck[ch][sb18+3]	= rawout[21];
		prevblck[ch][sb18+4]	= rawout[22];
		prevblck[ch][sb18+5]	= rawout[23];
		prevblck[ch][sb18+6]	= rawout[24];
		prevblck[ch][sb18+7]	= rawout[25];
		prevblck[ch][sb18+8]	= rawout[26];
		prevblck[ch][sb18+9]	= rawout[27];
		prevblck[ch][sb18+10]	= rawout[28];
		prevblck[ch][sb18+11]	= rawout[29];
		prevblck[ch][sb18+12]	= rawout[30];
		prevblck[ch][sb18+13]	= rawout[31];
		prevblck[ch][sb18+14]	= rawout[32];
		prevblck[ch][sb18+15]	= rawout[33];
		prevblck[ch][sb18+16]	= rawout[34];
		prevblck[ch][sb18+17]	= rawout[35];
		
		sb++;
		sb18+=18;
		
	} while(--x);
}

void layer3decoder::reset(void)
{
	filter[0].Reset();
	filter[1].Reset();
	
	for (uint32_t i=0; i<576; i++) 
	{
		prevblck[0][i] = 0.0f;
		prevblck[1][i] = 0.0f;
	}
	
}


bool layer3decoder::processFrame(float * tohere, uint8_t * audiodata, uint32_t audiodatalength, Header & head, SideInfo & si)
{
	uint32_t ch, gr;
	
	uint32_t toff = 0;
	
	// set us up the bomb!
	m_MpegVer			= head.MpegVersion;
	m_SampleFrequency	= head.SampleRateIndex;
	m_Channels			= head.Channels;
	m_Mode				= head.Mode;
	m_ModeExt			= head.Mode_Extension;
	
	if(m_MpegVer == MPEG1)
		m_Granules			= 2;
	else
		m_Granules			= 1;
	

	// br is already backstepped and shit!
	br.load(audiodata, audiodatalength);
	
	
	for(gr=0; gr<m_Granules; gr++)
	{
		for(ch = 0; ch<m_Channels; ch++)
		{
			br.setBitOffset(toff);
			m_Part2Start[ch] = br.getBitOffset();

			DecodeScaleFactorsMPEG1(si, ch, gr);
			if(!ReadHuffman(ch, gr, si, head))
				DLog("huffman decode error!!!!\n");
			DequantizeSample(ch, gr, si);
         	Reorder(ch, gr, si);
			
			toff+= si.grinf[gr][ch].Part23Length;
		}
		
		Stereo(gr, si);
		
		for(ch = 0; ch<m_Channels; ch++)
		{
			AntiAlias(ch, gr, si);
			Hybrid(ch, xrr[ch], xir[ch], si.grinf[gr][ch].BlockType, si.grinf[gr][ch].WindowSwitchingFlag, si.grinf[gr][ch].MixedBlockFlag);
			FreqencyInverse(gr, ch);
		}
		
		if(m_Channels == 1)
		{
			for(uint32_t ss=0; ss<576; ss+=32)
			{
				filter[0].PerformSynthesis(&xir[0][ss], tohere, 1);
			}
		}
		else
		{
			for(uint32_t ss=0; ss<576; ss+=32)
			{
				filter[0].PerformSynthesis(&xir[0][ss], tohere, 2);
				filter[1].PerformSynthesis(&xir[1][ss], tohere+1, 2);
				tohere+=64;
			}
		}
	}

	return true;
}


