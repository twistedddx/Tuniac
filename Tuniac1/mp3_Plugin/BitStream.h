/*
 *  bitstream.h
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#pragma once

#include "stdint.h"

// TODO: maybe make this capable of reading up to 63bits at a time rather than 31

class bitstream
{
protected:
	const uint8_t *		data;
	uint32_t		datalength;
	uint32_t		datalengthbits;
	
	uint32_t		bitoffset;
	
public:
	
	void		load(const uint8_t * pData, uint32_t length);
	
	uint32_t	getbits(uint32_t numbits);
	
	uint32_t	dismiss(uint32_t numbits);
	void		rewind(uint32_t numbits);
	
	uint32_t	getBitOffset(void);
	void		setBitOffset(uint32_t offset);
	
	
	uint32_t	getByteOffset(void);
	
	bool		EOS(void);
};