/*
 *  layer3decoder.h
 *  audioenginetest
 *
 *  Created by Tony Million on 01/12/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "sideinfo.h"
#include "synthesisfilter.h"

class layer3decoder
{
protected:
	uint32_t	m_MpegVer;
	uint32_t	m_SampleFrequency;
	uint32_t	m_Channels;
	uint32_t	m_Granules;
	uint32_t	m_Mode;
	uint32_t	m_ModeExt;
	
	uint32_t	m_Part2Start[2];
	uint32_t	m_MixedBandLimit[2];
	uint32_t	m_NonZero[2];
	
	int32_t		is[2][576];
	float		xr[2][576];	// Dequantized
	float		xrr[2][576];	// reordered
	float		xir[2][576];	// IMDCT'd
	
	float		kl[576];
	float		kr[576];
	float		prevblck[2][576];	// overlapped
	float		IMDCTwin[4][36];	// used for windowing the IMDCT samples
	
	
	// calculated at runtime
	float PowerTableMinus2[64]; /* (2^(-2))^i */
	float PowerTableMinus05[64]; /* (2^(-0.5))^i */
	float GainTable[256];
	float TanPi12Table[16];	// mpeg1 tan(is * PI/12);
	float Cs[8], Ca[8];
	float IMDCT9x8Table[72];
	
	bitstream br;
	
	synthesisfilter filter[2];
	
	void DecodeScaleFactorsMPEG1(SideInfo & si, int ch, int gr);
	void DecodeScaleFactorsMPEG2(SideInfo & si, int ch, int gr);
	
	bool ReadHuffman(uint32_t ch, uint32_t gr, SideInfo & si, Header & head);
	
	void DequantizeSample(uint32_t ch, uint32_t gr, SideInfo & si);
	void Reorder(uint32_t ch, uint32_t gr, SideInfo & si);

	void Stereo(uint32_t gr, SideInfo & si);
	
	void AntiAlias(uint32_t ch, uint32_t gr, SideInfo & si);
	void FreqencyInverse(uint32_t gr, uint32_t ch);
	void IMDCT(float *in, float *out, uint32_t block_type);
	void Hybrid(uint32_t ch, float *xfrom, float *xto, uint32_t blocktype, uint32_t windowswitching, uint32_t mixedblock);

public:
	
	layer3decoder();
	
	void reset(void);
	
	bool processFrame(float * tohere, uint8_t * audiodata, uint32_t audiodatalength, Header & head, SideInfo & si);

};